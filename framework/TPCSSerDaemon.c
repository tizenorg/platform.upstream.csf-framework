/**
 * Methods supported in TPCS (Tizen Plugin Control Service)
 */

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <ftw.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Debug.h"
#include "IpcServer.h"
#include "IpcForkDaemon.h"
#include "TPCSSerDaemon.h"

/**
 * This daemon provides four services to manage plugins.
 * InstallPlugin, UninstallPlugin, SetActivePlugin, GetInfoPlugin
 *
 * It maintains config.xml that has the plugin info.
 * It will recover the config.xml if it is corrupted.
 *
 * During the services, the update config.xml and set up the symbolic link of the library is an
 * atomic action.
 * Daemon will store the config.xml in the buffer. When running each method, the buffer config.xml
 * is copied to pData and being update inside the method.
 * Except the GetInfoPlugin(), all other methods serializing when updating the config.xml.
 */

/**
 * Common Utils, TODO: move to Utils.c
 *
 */

typedef struct _ConfigBuf
{
    xmlDoc *pConfigBuffer;
    pthread_mutex_t configLock;
} ConfigBuf;

static void GetNSURI(xmlDoc *pXmlDoc, xmlChar **pns, xmlChar **uri);
static void GetConfigElements(xmlDoc **pXmlDoc, xmlChar *pXPath, int *size, char ***content);
static int WriteXmlToFile(xmlDoc *pXmlDoc, const char* filename);
static int UpdatePluglibList(const char *appPath, xmlDoc **pXmlDoc, const char *appId);
static int ActivePlugin(ConfigBuf *pBuf, xmlDoc **pXmlCp, const xmlChar *appId);
static void SetSuccessReturn(int *argc, char ***argv);
static void SetFailureReturn(int *argc, char ***argv);
static void CleanupArray(char*** pArr, int len);
static int EndsWith(char const *hay, char const *needle);

/**
 * Local functions declaration.
 */
static int HasTag(const char *pTag, const char *pFileName, const char *xpath);
int UpdateNode(const xmlChar* content, xmlDoc **pDoc, const xmlChar *xpathNodeParent,
               const char* appID);

static void SetReturn(int *res_argc, char ***res_argv, char *val)
{
    if (val)
    {
        *res_argv = calloc(1, sizeof(char*));
        if (*res_argv)
        {
            (*res_argv)[0] = strdup((const char*) val);
            if ((*res_argv)[0])
            {
                *res_argc = 1;
                return;
            }
        }

        free(*res_argv);
        *res_argc = 0;
    }
}

static void SetSuccessReturn(int *res_argc, char ***res_argv)
{
    SetReturn(res_argc, res_argv, RETURN_SUCCESS);
}

static void SetFailureReturn(int *res_argc, char ***res_argv)
{
    SetReturn(res_argc, res_argv, RETURN_FAILURE);
}

int SearchNodeN(const xmlChar *xpath, xmlDoc *pXmlDoc, xmlXPathObject **pXPathObj)
{
    DDBG("searchNode :%s\n", xpath);
    int result = -1;
    xmlXPathContextPtr pXPathCtx;

    pXPathCtx = xmlXPathNewContext(pXmlDoc);
    if (pXPathCtx == NULL)
        return result;

    xmlChar *pns = NULL;
    xmlChar *puri = NULL;
    GetNSURI(pXmlDoc, &pns, &puri);

    if (pns != NULL && puri != NULL)
    {
        if (xmlXPathRegisterNs(pXPathCtx, pns, puri) != 0)
            return result;
    }

    //Evaluate xpath expression
    *pXPathObj = xmlXPathEvalExpression(xpath, pXPathCtx);
    DDBG("============== XPATHobj address:%p\n", *pXPathObj);

    if (*pXPathObj == NULL)
    {
        DDBG("Cannot find node:%s\n", xpath);
        free(pns);
        free(puri);
        return result;
    }

    result = 0;

    free(pns);
    free(puri);

    xmlXPathFreeContext(pXPathCtx);
    return result;

}

// Dynamically load the library, get the product name, vendor name and version number, then
// update the xml file.
int InsertNode(xmlDoc **pDoc, const xmlChar *xpathNodeParent, const char *content)
{
    DDBG("insert node :%s\n", "==============");
    int result = -1;
    xmlXPathContext *pXPathCtx;
    xmlXPathObject *pXPathObj;

    pXPathCtx = xmlXPathNewContext(*pDoc);
    if (pXPathCtx != NULL)
    {
        xmlChar *pns = NULL;
        xmlChar *puri = NULL;
        GetNSURI(*pDoc, &pns, &puri);
        if (pns != NULL && puri != NULL)
        {
            if (xmlXPathRegisterNs(pXPathCtx, pns, puri) != 0)
            {
                DDBG("Cannot register xpath :%s\n", "");
                return result;
            }
        }

        // Evaluate xpath expression
        pXPathObj = xmlXPathEvalExpression(xpathNodeParent, pXPathCtx);
        if (pXPathObj != NULL)
        {
            xmlNodeSet *pNodeSet = pXPathObj->nodesetval;
            int size;
            size = pNodeSet ? pNodeSet->nodeNr : 0;
            if (size == 1) // Should find only one node
            {
                // Get xmlnode
                xmlDoc *pPlugDoc = xmlReadMemory(content, strlen(content), NULL, NULL, 0);
                DDBG("plugdoc :%s\n", content);

                if (pPlugDoc != NULL)
                {
                    xmlNode *newnode = xmlDocCopyNode(xmlDocGetRootElement(pPlugDoc),
                                                      pNodeSet->nodeTab[0]->doc,1);
                    DDBG("=========== pplugdoc address:%p\n", pPlugDoc);

                    xmlFreeDoc(pPlugDoc);
                    //xmlNode *addNode = xmlAddChildList(pNodeSet->nodeTab[0], newnode->children);
                    xmlNode *addNode = xmlAddChildList(pNodeSet->nodeTab[0], newnode->children);
                    DDBG("=========== new node addr:%p\n", newnode);

                    if (addNode != NULL)
                    {
                        result = 0;
                        DDBG("FREE new node:%p\n", newnode);
                        newnode->children = NULL;
                        xmlFreeNode(newnode);
                    }
                }
            }
        }
        free(pns);
        free(puri);
        pns = NULL;
        puri = NULL;

        xmlXPathFreeObject(pXPathObj);
        xmlXPathFreeContext(pXPathCtx);
    }

    return result;
}

int RemoveNodeParent(xmlDoc **pDoc, const xmlChar *path, const char* value)
{
    int result = -1;
    if (*pDoc != NULL)
    {
        xmlXPathContext *pXPathCtx;
        xmlXPathObject *pXPathObj;

        pXPathCtx = xmlXPathNewContext(*pDoc);

        if (pXPathCtx != NULL)
        {
            xmlChar *pns = NULL;
            xmlChar *puri = NULL;
            GetNSURI(*pDoc, &pns, &puri);
            if (pns != NULL && puri != NULL)
            {
                if (xmlXPathRegisterNs(pXPathCtx, pns, puri) != 0)
                {
                    return result;
                }
            }
            pXPathObj = xmlXPathEvalExpression(path, pXPathCtx);
            if (pXPathObj != NULL)
            {
                xmlNodeSet *pNodeSet = pXPathObj->nodesetval;

                int size;
                size = pNodeSet ? pNodeSet->nodeNr : 0;
                int i = 0;
                xmlNode *cur;
                xmlNode *parent;
                for (; i < size; i++)
                {
                    cur = pNodeSet->nodeTab[i]->children;
                    if (strcmp((const char*)cur->content, value) == 0)
                    {
                        parent = cur->parent->parent;
                        xmlUnlinkNode(parent);
                        break;
                    }
                    else
                        DDBG("THEY ARE NOT EQUAL:%s\n", value);

                }
                result = 0;
                xmlXPathFreeObject(pXPathObj);
            }
            free(pns);
            free(puri);
            pns = NULL;
            puri = NULL;
            xmlXPathFreeContext(pXPathCtx);
        }
    }
    return result;
}

int PrintXmlDoc(xmlDocPtr pDoc)
{
#ifdef DEBUG
    int result = 0;
    int xmlSize;
    xmlChar *xmlMem;
    xmlDocDumpFormatMemory(pDoc, &xmlMem, &xmlSize, 0);
    DDBG("pdoc content:%s\n", xmlMem);
    free(xmlMem);
    return result;
#else
    return 0;
#endif
}

int UpdateConfigFile(ConfigBuf **pData, xmlDoc **pXmlCp)
{
    int result = -1;

    if (pData && *pData)
    {
        pthread_mutex_lock(&((*pData)->configLock));
        if (pXmlCp && *pXmlCp)
        {
            result = WriteXmlToFile(*pXmlCp, CONFIG_FILE_NEW_W_PATH);
            if (result == 0)
            {
                DDBG("==============update config file%s\n", " ");
                PrintXmlDoc(*pXmlCp);
                result = WriteXmlToFile(*pXmlCp, CONFIG_FILE_W_PATH);
                if (result == 0)
                {
                    //delete config new file
                    DDBG("REMOVE file:%s\n", CONFIG_FILE_NEW_W_PATH);
                    result = remove(CONFIG_FILE_NEW_W_PATH);

                     if ((*pData)->pConfigBuffer != NULL)
                         xmlFreeDoc((*pData)->pConfigBuffer);

                    (*pData)->pConfigBuffer = xmlCopyDoc(*pXmlCp, DO_RECURSIVE_COPY);
                }
            }
        }
        pthread_mutex_unlock(&((*pData)->configLock));
    }
    return result;
}

static int UpdatePlugin(const char *libname, xmlDoc **pXmlDoc, const char* appId)
{
    DDBG("~~~~~ UPDATE plugin: libname:%s, appID:%s\n", libname, appId);
    int result = -1;
    void *pHandle = NULL;

    pHandle = dlopen(libname, RTLD_LAZY);
    if (pHandle == NULL)
    {
        char *error;
        if ((error = dlerror()) != NULL)
            DDBG("failed to open lib: %s, error:%s\n", libname, error);
    }
    else
    {
        char *pInfo = NULL;
        char* (*getInfo)(void);

        DDBG("open library:%s\n", libname);
        getInfo = dlsym(pHandle, "TCSGetInfo");

        if (getInfo == NULL)
        {
            char *error;
            if ((error = dlerror()) != NULL)
                DDBG("SOMETHID :%s\n", error);
        }
        else
        {
            pInfo = strdup(getInfo());
            if (pInfo != NULL)
            {
                // Update node tree
                result = UpdateNode((const xmlChar*) pInfo, pXmlDoc, (const xmlChar*) XPATH_PLUGINS,
                                    appId);
                free(pInfo);
                pInfo = NULL;
            }
        }

        dlclose(pHandle);
    }
    return result;
}

/**
 * Get plugins node, should only have one node. After that, Check if has the node with appId, if so,
 * remove this node. otherwise, add the new node.
 */
int UpdateNode(const xmlChar* content, xmlDoc **pDoc, const xmlChar *xpathNodeParent,
               const char* appId)
{
    int result = -1;

    // Remove the plugin first if its appId same
    char appidpath[GENERIC_STRING_SIZE];
    strncpy(appidpath, (const char*) XPATH_PLUGINS_PLUG, sizeof(appidpath));
    result = RemoveNodeParent(pDoc, (const xmlChar *)appidpath, appId);

    result = InsertNode(pDoc, xpathNodeParent, (const char*)content);
    return result;
}

static int ActivePlugin(ConfigBuf *pBuf, xmlDoc **pXmlCp, const xmlChar *appId)
{
    int result = -1;

    xmlXPathObject *pObj = NULL;
    result = SearchNodeN((const xmlChar *)XPATH_ACTIVE_PLUGIN, *pXmlCp, &pObj);

    PrintXmlDoc(*pXmlCp);

    if (result == -1)
        return result;

    result = -1;
    xmlNodeSet *pNodeSet = NULL;
    if (pObj)
    {
    	pNodeSet = pObj->nodesetval;
    }

    // If appId is empty, disable the active plugin, update activePlugin Node to None
    if (strcmp((const char*)appId, APP_ID_NULL) == 0)
    {
        pthread_mutex_lock(&(pBuf->configLock));
        result = unlink(SYSLINK_PLUG_PATH);
        pthread_mutex_unlock(&(pBuf->configLock));

		if (pNodeSet)
		{
			if (pNodeSet->nodeTab[0])
			{
				xmlNode *cur = pNodeSet->nodeTab[0];
				xmlNodeSetContent(cur, (xmlChar*)ACTIVE_NONE);
				DDBG("GET node content :%s\n", cur->content);
			}
		}
		result = 0;
    }
    else
    {
        // Get the plugin path from the appPath, set symbolic link for the plugin, update the xmlDoc
        int count;
        char **paths = NULL;
        GetConfigElements(pXmlCp, (xmlChar *) XPATH_APP_PATHS, &count, &paths);
        int i = 0;
        for (; i < count; i++)
        {
            char appPath[FILE_NAME_MAX_SIZE];
            snprintf(appPath, FILE_NAME_MAX_SIZE, "%s%s%s%s", paths[i], "/", appId,
                     PLUGIN_DEFAULT_DIR_NAME);

            //Found the app path
            struct stat info;
            if (stat(appPath, &info) != 0)
                continue;

            pthread_mutex_lock(&(pBuf->configLock));
            unlink(SYSLINK_PLUG_PATH);
            result = symlink(appPath, SYSLINK_PLUG_PATH);
            pthread_mutex_unlock(&(pBuf->configLock));

            DDBG("set symbolic link result:from :%s to %s result:%d\n", appPath, SYSLINK_PLUG_PATH, result);
            if (result == 0 && pNodeSet)
            {
                //update active Id node
                xmlNode *cur = pNodeSet->nodeTab[0];
                xmlNodeSetContent(cur, appId);
                DDBG("GET NOde conten :%s\n", cur->content);

                //xmlFreeNode(cur);
                DDBG("RETURn from active plu g  0000:%d\n", result);
                PrintXmlDoc(*pXmlCp);

            }
            DDBG("RETURn from active plu g  0000  000000:%d\n", result);
            //xmlXPathFreeNodeSet(pNodeSet);
            break;
        }
        DDBG("RETURn from active plu g  0000  0111 :%d\n", result);
        CleanupArray(&paths, count);
        xmlXPathFreeObject(pObj);

    }

    DDBG("RETURn from active plug :%d\n", result);
    return result;
}

/**
 * @appPath: /opt/usr/app/appId
 */
static int UpdatePluglibList(const char *appPath, xmlDoc **pXmlDoc, const char* appId)
{
    DDBG("=========================update plugin lib path:%s\n", appPath);

    int result = -1;
    char plugPath[FILE_NAME_MAX_SIZE];

    sprintf(plugPath, "%s%s", appPath, PLUGIN_DEFAULT_DIR_NAME);
    DIR *pDir;
    struct dirent *pDirEnt;

    pDir = opendir(plugPath);
    DDBG("OPEN DIR:%s\n", plugPath);
    if (pDir)
    {
        //open directories
        while ((pDirEnt = readdir(pDir)) != NULL)
        {
            if (strcmp(pDirEnt->d_name, ".") == 0 || strcmp(pDirEnt->d_name, "..") == 0)
                continue;

            if (pDirEnt->d_type == DT_REG)
            {
                // load the library, update xml node
                char libPath[FILE_NAME_MAX_SIZE];
                sprintf(libPath, "%s%s%s", plugPath, "/", pDirEnt->d_name);
                result = UpdatePlugin(libPath, pXmlDoc, appId);
                break;
            }
            else
            {
                DDBG("IT IS NO regular file:%s\n", pDirEnt->d_name);
                continue;
            }
        }
        closedir(pDir);

    }
    DDBG("RESTURN  from update pluglib list :%d\n", result);
    return result;
}

/**
 *
 */
int GetInfoPlugin(void *pData, int req_argc, char **req_argv, char ***res_argv, int *res_argc,
                  CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    // check data pass is valid xml
    xmlDoc *pXml = (xmlDoc*) *((xmlDoc**) pData);
    *res_argc = FN_GET_INFO_RTN_COUNT;
    *res_argv = calloc(1, sizeof(char*) * FN_GET_INFO_RTN_COUNT);
    if (*res_argv == NULL)
        goto no_memory;

    (*res_argv)[0] = strdup((const char*) RETURN_SUCCESS);
    if ((*res_argv)[0] != NULL)
    {
        xmlChar *xmlMem;
        int xmlSize;
        xmlDocDumpFormatMemory(pXml, &xmlMem, &xmlSize, 0);
        if (xmlMem != NULL)
        {
            DDBG("size is: %d, xmlmem is ** :%s\n", xmlSize, xmlMem);
            (*res_argv)[1] = strdup((const char*) xmlMem);
            if ((*res_argv)[1] == NULL)
            {
                DDBG("Not enough memory in :%s\n", "set xmlmem in return");
                free((*res_argv)[0]);
                (*res_argv)[0] = NULL;
                goto invalid_xml;
            }
            xmlFree(xmlMem);
            xmlMem = NULL;
        }
        else
            goto invalid_xml;
    }
    else
    {
        DDBG("failed to copy return_successS:%s\n", "");
        goto no_memory;
    }
    DDBG("successfully finished:%s\n", "GetInfoPlugin");
    return 0;

    invalid_xml:
    // get recoverred config.xml and return
    no_memory:
    *res_argc = 0;
    return 0;
}

/**
 * pData is the buffer of config.xml,
 * ** apply lock
 *   if copy pData to pData.cp success,
 *     if update pData.cp with the newly installed info
 *       if successfully update the symbolic link,
 *         if write pData.cp to config.xml.new
 *           if switch config.xml.new to config.xml
 *           else do nothing, wait for next time update
 *         else do nothing, wait for next time udpate
 *         copy pData.cp to pData, free pData.cp
 *       else return failure update
 *     ** unlock
 *
 *     @req_argc: 1
 *     @req_argv: app installation path, i.e. /opt/usr/apps/appID
 */
int InstallPlugin(void *pData, int req_argc, char **req_argv, char ***res_argv, int *res_argc,
                  CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    DDBG("InstallPlugin for serverstub:%s\n", req_argv[0]);
    int result = -1;
    ConfigBuf *pBuf = *((ConfigBuf **) pData);
    pthread_mutex_lock(&(pBuf->configLock));
    xmlDoc *pXml = (*((ConfigBuf **) pData))->pConfigBuffer;
    xmlDoc *pXmlCp = xmlCopyDoc(pXml, DO_RECURSIVE_COPY);
    pthread_mutex_unlock(&(pBuf->configLock));

    if (pXmlCp != NULL)
    {
        if (req_argc == 1 && req_argv != NULL)
        {
            // Get apppath list
            int count;
            char **paths = NULL;
            GetConfigElements(&pXmlCp, (xmlChar *) XPATH_APP_PATHS, &count, &paths);
            int i = 0;
            for (; i < count; i++)
            {
                char appPath[FILE_NAME_MAX_SIZE];
                sprintf(appPath, "%s%s%s", paths[i], "/", req_argv[0]);

                PrintXmlDoc(pXmlCp);
                if (UpdatePluglibList(appPath, &pXmlCp, req_argv[0]) == 0)
                {
                    if (ActivePlugin(*((ConfigBuf**) pData), &pXmlCp, (const xmlChar *)req_argv[0]) == 0)
                    {
                        // Write to new file
                        DDBG("Success cpy xml 2:%s\n", appPath);

                        *res_argc = FN_INSTALL_PLUG_RTN_COUNT;
                        *res_argv = calloc(1, sizeof(char*) * FN_INSTALL_PLUG_RTN_COUNT);
                        if (*res_argv == NULL)
                            goto no_memory;

                        (*res_argv)[0] = strdup((const char*) RETURN_SUCCESS);
                        if ((*res_argv)[0] == NULL)
                            goto no_memory;

                        xmlChar *xmlMem = NULL;
                        int xmlSize;

                        xmlDocDumpFormatMemory(pXmlCp, &xmlMem, &xmlSize, 0);
                        DDBG("XMLmEM address: %p\n", xmlMem);
                        if (xmlMem != NULL)
                        {
                            (*res_argv)[1] = strdup((const char*) xmlMem);
                            DDBG("return from install plug:%s\n", (*res_argv)[1]);
                            free(xmlMem);
                            xmlMem = NULL;

                            if ((*res_argv)[1] != NULL)
                            {
                                result = UpdateConfigFile(pData, &pXmlCp);
                                break;
                            }
                        }
                    }
                }
            }
            CleanupArray(&paths, count);
        }
    }
    xmlFreeDoc(pXmlCp);

    if (result == -1)
        goto failure;
    else
    {
        return result;
    }

no_memory:
    xmlFreeDoc(pXmlCp);

failure:
    *res_argc = 1;
    *res_argv = calloc(1, sizeof(char*));
    (*res_argv)[0] = strdup((const char*) RETURN_FAILURE);
    return result;
}

/**
 * If it is active plugin, unlink the syslink, and update active element with AppId to None.
 * If the request plugin is not active one, remove it from the plugin list
 */
int UninstallPlugin(void *pData, int req_argc, char **req_argv, char ***res_argv, int *res_argc,
                    CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    int result = -1;
    xmlDoc *pXml = NULL;
    xmlDoc *pXmlCp = NULL;
    ConfigBuf *pBuf = NULL;
    xmlXPathObject *pObj = NULL;

    if (req_argc != 1 || req_argv == NULL)
        goto failure;

    pBuf = *((ConfigBuf **) pData);

    pthread_mutex_lock(&(pBuf->configLock));
    pXml = pBuf->pConfigBuffer;
    pXmlCp = xmlCopyDoc(pXml, DO_RECURSIVE_COPY);
    pthread_mutex_unlock(&(pBuf->configLock));

    if (pXmlCp == NULL)
        goto failure;

    result = SearchNodeN((const xmlChar *)XPATH_ACTIVE_PLUGIN, pXmlCp, &pObj);
    if (result != 0)
        goto failure;

    xmlNodeSet *pNodeSet = pObj->nodesetval;
    xmlNode *pNode = pNodeSet->nodeTab[0];
    if (pNode == NULL)
    {
        DDBG("not such node %s\n", XPATH_ACTIVE_PLUGIN);
        goto failure;
    }

    if (pNode->type != XML_ELEMENT_NODE)
        goto failure;

    // Search for plugin. If found, unlink the plugin. Otherwise, the plugin was already
    // unlinked and updated in the config file.
    DDBG("node name:%s, activenode value: %s\n", pNode->name, pNode->children->content);
    if (strcmp((const char *)pNode->children->content, req_argv[0]) == 0)
    {
        pthread_mutex_lock(&(pBuf->configLock));
        result = unlink(SYSLINK_PLUG_PATH);
        pthread_mutex_unlock(&(pBuf->configLock));

        if (result != 0 && result != ENOENT)
            goto failure;

        xmlNodeSetContent(pNode, (const xmlChar *)ACTIVE_NONE);
    }

    // Remove the plugin node if it is in the plugin list
    result = RemoveNodeParent(&pXmlCp, (const xmlChar *)XPATH_PLUGINS_PLUG, req_argv[0]);
    result = UpdateConfigFile((ConfigBuf**) pData, &pXmlCp);
    if (result != 0)
    {
        DDBG("Unable to update config file with uninstall info\n");
        goto failure;
    }
    SetSuccessReturn(res_argc, res_argv);
    goto cleanup;

failure:
    SetFailureReturn(res_argc, res_argv);

cleanup:
    if (pObj)
        xmlXPathFreeObject(pObj);
    if (pXmlCp)
        xmlFreeDoc(pXmlCp);

    return result;
}

/**
 * Copy the xmlDoc
 * Update the symbolic link if it is not the active plug
 * Update xmlDoc, save to the tpcs_config.xml
 */
int SetActivePlugin(void *pData, int req_argc, char **req_argv, char ***res_argv, int *res_argc,
                    CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    DDBG("SetActivePlugin :%s\n", req_argv[0]);

    ConfigBuf *pBuf = NULL;
    xmlDoc *pXml = NULL;
    xmlDoc *pXmlCp = NULL;
    int result = -1;

    if (req_argc != 1 || req_argv == NULL)
        return result;

    pBuf = *((ConfigBuf **) pData);

    pthread_mutex_lock(&(pBuf->configLock));
    pXml = pBuf->pConfigBuffer;
    pXmlCp = xmlCopyDoc(pXml, DO_RECURSIVE_COPY);
    pthread_mutex_unlock(&(pBuf->configLock));

    if (pXmlCp == NULL)
        return result;

    PrintXmlDoc(pXmlCp);
    if ((ActivePlugin(pBuf, &pXmlCp, (const xmlChar *)req_argv[0]) == 0)
            && UpdateConfigFile((ConfigBuf**) pData, &pXmlCp) == 0)
    {
        SetSuccessReturn(res_argc, res_argv);
        DDBG("==== FINd tha ppid %s\n", req_argv[0]);
    }
    else
    {
        DDBG("==== **** FINd tha ppid %s\n", req_argv[0]);
        SetFailureReturn(res_argc, res_argv);
    }

    xmlFreeDoc(pXmlCp);
    return result;
}

static int WriteXmlToFile(xmlDoc *pXmlDoc, const char* filename)
{

    DDBG("Write xml to file: %s\n", filename);
    int result = -1;
    FILE *pfConfig = fopen(filename, "w");
    if (pfConfig != NULL)
    {
        xmlDocDump(pfConfig, pXmlDoc);
        fclose(pfConfig);
        result = 0;
    }
    return result;
}

static int InitConfigFile(xmlDoc **pXmlDoc)
{
    int result = -1;
    DDBG("Size of xml doc in memory :%d\n", sizeof(CONFIG_DEFAULT_STRING));

    *pXmlDoc = xmlReadMemory(CONFIG_DEFAULT_STRING, sizeof(CONFIG_DEFAULT_STRING) + 1,
                             CONFIG_FILE_NAME,
                             NULL, 0);

    if (*pXmlDoc != NULL)
    {
        result = 0;
    }

    return result;
}

static bool IsValidTPCSConfig(const char *pFileName, xmlDoc **ppXmlDoc)
{
    // Create a parse context
    bool ret = FALSE;
    xmlParserCtxt *pParserCtxt = xmlNewParserCtxt();
    xmlDoc *pXmlDoc;
    if (pParserCtxt == NULL)
    {
        DDBG("%s\n", "======Failed to allocate parser context");
        return ret;
    }
    else
    {
        // valid this config file
            // Parse the file, activating the DTD validation option */
		DDBG("------------------ IS VALID config file :%s\n", "---------------------");
		pXmlDoc = xmlCtxtReadFile(pParserCtxt, pFileName, NULL, XML_PARSE_DTDVALID);
		DDBG("--- 1 pxmldoc address: %p\n", pXmlDoc);
		DDBG("file to read:%s\n", pFileName);
		PrintXmlDoc(pXmlDoc);
		if (pXmlDoc != NULL)
		{
			// Check if validation succeeded
			if (pParserCtxt->valid)
			{
				*ppXmlDoc = pXmlDoc;
				DDBG("address of ppxmldoc: %p\n", *ppXmlDoc);
				ret = TRUE;
			}
		}
		else
		{
			DDBG("NOT VALID file: %s\n", pFileName);
		}

        xmlFreeParserCtxt(pParserCtxt);
        return ret;
    }
}

/**
 * Return directory list that has the specified directory within
 */
static void GetDirsHasPath(const char *dir, const char *search_path, int *pCount,
                           char ***name_w_path, char ***appId)
{
    DIR *pDir = NULL;
    struct dirent *pDirEnt = NULL;

    if (!dir || !search_path || !pCount || !appId || !name_w_path)
        return;

    pDir = opendir(dir);
    *pCount = 0;
    if (pDir)
    {
        char plugPath[FILE_NAME_MAX_SIZE];
        char appPath[FILE_NAME_MAX_SIZE];

        struct stat info;
        while ((pDirEnt = readdir(pDir)) != NULL)
        {
            if (strcmp(pDirEnt->d_name, ".") == 0 || strcmp(pDirEnt->d_name, "..") == 0)
            {
                continue;
            }
            if (pDirEnt->d_type == DT_DIR)
            {
                sprintf(appPath, "%s%s%s", dir, "/", pDirEnt->d_name);

                sprintf(plugPath, "%s%s", appPath, PLUGIN_DEFAULT_DIR_NAME);

                if (stat(plugPath, &info) == 0)
                {
                    if (*pCount == 0)
                    {
                        *name_w_path = calloc(1, sizeof(char*));

                        if (*name_w_path)
                        {
                            (*pCount)++;
                            (*name_w_path)[0] = strdup((const char*) appPath);

                            if ((*name_w_path)[0] == NULL)
                                goto clean_up;
                        }
                        else
                            goto clean_up;

                        *appId = calloc(1, sizeof(char*));
                        if (*appId)
                        {
                            (*appId)[0] = strdup((const char*) pDirEnt->d_name);
                            DDBG("COUNT 0, name: %s\n", pDirEnt->d_name);
                            if ((*appId)[0] == NULL)
                                goto clean_up;
                        }
                    }
                    else
                    {
                        //TODO: all the realloc * size, use chunk
                        *name_w_path = realloc(*name_w_path, sizeof(char*) * (*pCount + 1));
                        if (*name_w_path)
                        {
                            (*pCount)++;
                            (*name_w_path)[(*pCount) - 1] = strdup((const char*) appPath);
                            if ((*name_w_path)[(*pCount) - 1] == NULL)
                                goto clean_up;

                            *appId = realloc(*appId, sizeof(char*) * (*pCount + 1));
                            if (*appId)
                            {
                                (*appId)[*pCount - 1] = strdup((const char*) pDirEnt->d_name);
                                if (((*appId)[*pCount - 1]) == NULL)
                                {
                                    goto clean_up;
                                }
                            }
                            else
                            {
                                goto clean_up;
                            }
                        }
                        else
                        {
                            goto clean_up;
                        }
                    }
                }
            }
        }

        goto success;
    }

clean_up:
    CleanupArray(name_w_path, *pCount);

success:
    DDBG("CLOSE dir: %s\n", " " );
    if (pDir)
        closedir(pDir);

    return;
}

/**
 *
 */
static void CleanupArray(char*** pArr, int len)
{
    if (pArr)
    {
        while (len > 0)
        {
            len--;
            if ((*pArr)[len])
            {
                free((*pArr)[len]);
                (*pArr)[len] = NULL;
            }
        }

        if (*pArr)
        {
            free(*pArr);
            *pArr = NULL;
        }
    }
}

/*
 * int ftw(const char *dir, int (*fn)(const char *file, const struct stat *sb, int flag), int nopenfd);
 */
int FileTreeCallback(const char *pFile, const struct stat *pStat, int flag)
{
    int result = 0;
    if (pStat->st_mode & S_IFREG)
    {
        //DDBG(">>>>>file tree walk :%s, such substring:%s\n", pFile, MANIFEST_FILE_NAME);
        if (EndsWith(pFile, MANIFEST_FILE_NAME) == 0)
        {
            DDBG("FOUNd the file: %s\n", pFile);
            // check if it has the plugin tag
            if (HasTag(PLUGIN_ANTI_VIRUS_TAG, pFile, XPATH_PLUGIN_CATEGORY) == 0)
            {
                // found the library
                DDBG("this app is plugin app:%s\n", pFile);
                result = 1;
            }
        }
    }
    return result;
}

/**
 * Checks if str ends with needle. Return zero on success otherwise non-zero.
 */
static int EndsWith(const char *hay, const char *needle)
{
    size_t haylen, needlelen;

    if (!hay || !needle)
        return -1;

    needlelen = strlen(needle);
    haylen = strlen(hay);

    if (needlelen > haylen)
        return -1;

    return strncmp(hay + haylen - needlelen, needle, needlelen);
}

static void GetNSURI(xmlDoc *pXmlDoc, xmlChar **pns, xmlChar **uri)
{
    xmlNode *root_node = xmlDocGetRootElement(pXmlDoc);

    if (root_node)
    {
        xmlNs *pXmlNS = root_node->ns;
        xmlNs *pXmlNSDef = root_node->nsDef;

        if (pXmlNS != NULL)
        {
            if (pXmlNS->prefix != NULL)
            {
                *pns = (xmlChar*) strdup((const char*) pXmlNS->prefix);
            }
            else
            {
                DDBG("root node name:%s\n", root_node->name);
                *pns = (xmlChar*) strdup((const char*) root_node->name);
            }
        }

        if (pXmlNSDef != NULL)
        {
            if (pXmlNSDef->href != NULL)
            {
                DDBG("nsdef :%s\n", pXmlNSDef->href);
                *uri = (xmlChar*) strdup((const char*) pXmlNSDef->href);
            }
        }
    }
    return;
}

/**
 * Get elements from xmlDoc, and output to the count and content
 */
static void GetConfigElements(xmlDoc **pXmlDoc, xmlChar *pXPath, int *size, char ***content)
{
    DDBG("GetConfigElements in xpath: %s\n", (char*)pXPath);
    // Scan all the directories that has av category, update its plugin element in config.xml
    xmlXPathContext *pXPathCtx;
    xmlXPathObject *pXPathObj;

    pXPathCtx = xmlXPathNewContext(*pXmlDoc);
    if (pXPathCtx != NULL)
    {
        xmlChar *pns = NULL;
        xmlChar *puri = NULL;
        GetNSURI(*pXmlDoc, &pns, &puri);
        if (pns != NULL && puri != NULL)
        {
            if (xmlXPathRegisterNs(pXPathCtx, pns, puri) != 0)
            {
                DDBG("Cannot register xpath :%s\n", "");
                return;
            }
        }
        // Evaluate xpath expression
        pXPathObj = xmlXPathEvalExpression(pXPath, pXPathCtx);
        if (pXPathObj != NULL)
        {
            xmlNode *cur;
            xmlNodeSet *pNodeSet = pXPathObj->nodesetval;

            *size = pNodeSet ? pNodeSet->nodeNr : 0;
            if (*size > 0)
            {
                int i = 0;
                *content = calloc(1, sizeof(char*) * (*size));
                DDBG("XAPTH OBJCT FOUND:%s, size:%d\n", "", *size);
                if (*content)
                {
                    for (; i < *size; i++)
                    {
                        if (pNodeSet->nodeTab[i]->type == XML_ELEMENT_NODE)
                        {
                            cur = pNodeSet->nodeTab[i];

                            if (*content)
                            {
                                DDBG("content; %s\n", cur->children->content);
                                (*content)[i] = strdup((const char*) cur->children->content);
                            }
                        }
                    }
                }
                else
                {
                    DDBG("CANNOT FOUND content:%s\n", "");
                }

            }
            xmlXPathFreeObject(pXPathObj);
        }
        if (pns)
        {
            free(pns);
            pns = NULL;
        }

        if (puri)
        {
            free(puri);
            puri = NULL;
        }

        xmlXPathFreeContext(pXPathCtx);
    }
}

static int HasTag(const char *pTag, const char *pFileName, const char *xpath)
{
    int result = 1;
    xmlDoc *pConfigXml = NULL;

    if (!pTag)
        return result;

    pConfigXml = xmlReadFile(pFileName, NULL, 0);
    if (pConfigXml != NULL)
    {
        int size;
        char **pCategory = NULL;
        GetConfigElements(&pConfigXml, (xmlChar*) xpath, &size, &pCategory);
        if (size > 0)
        {
            DDBG("size is: %d, category:%s\n", size, pCategory[0]);
            int i = 0;
            for (; i < size; i++)
            {
                if (strcmp(pCategory[i], pTag) == 0)
                {
                    result = 0;
                    break;
                }
            }
        }
        CleanupArray(&pCategory, size);
    }
    xmlFreeDoc(pConfigXml);
    return result;
}

/*
 * config.xml validation
 * If config.xml.new exists, and it is valid,
 *  if it is consistent with symbolic link, switch this to config.xml
 *  else if there is config.xml, and
 *   if it is valid
 *    if it is consistent with symbolic link, buffer the config.xml
 *    else based on the symbolic link, update config.xml, then buffer the config.xml
 *   else
 *    recover the config.xml by scan all the applications, checking symbolic link, create the
 *    config.xml, buffer it.
 * Pass the data address to the pData
 */
int main(int argc, char **argv)
{
    //TODO: make it a daemon

#ifndef DEBUG
    fork_daemon();
#endif

    //Add methods
    //Start TPCS daemon
    TSC_SERVER_HANDLE hServer = NULL;
    int ret = -1;


    fprintf(stderr, "TPCS Service Daemon Started%s\n", "");
    //TODO: rename TSC_DBUS_SERVER_PLUGIN_CHANNEL to TSC_DBUS_SERVER_PLUGIN_CHANNEL
    if ((hServer = IpcServerOpen(TSC_DBUS_SERVER_PLUGIN_CHANNEL)) != NULL)
    {
        DDBG("%s", "**** successfully opened server ****\n");
        // Write out the DTD validation file
        FILE *pDTD = fopen(CONFIG_DTD_FILE_W_PATH, "r");
        if (pDTD == NULL)
        {
            pDTD = fopen(CONFIG_DTD_FILE_W_PATH, "w");
            if (pDTD)
            {
                fwrite(CONFIG_DEFAULT_DTD_STRING, strlen(CONFIG_DEFAULT_DTD_STRING), 1, pDTD);
                fclose(pDTD);
            }
            else
            {
                goto done;
            }
        }
        else
        {
            //TODO: close the file, check if the file exists lstat, signature of the file
        }

        ConfigBuf *pConfigBuf = calloc(1, sizeof(ConfigBuf));
        if (pConfigBuf == NULL)
            return ret;
        ret = pthread_mutex_init(&(pConfigBuf->configLock), NULL);
        if (ret == -1)
        {
            //TODO: memory clean up
            free(pConfigBuf);
            pConfigBuf = NULL;
            return ret;
        }

        if (IsValidTPCSConfig(CONFIG_FILE_NEW_W_PATH, &(pConfigBuf->pConfigBuffer)))
        {
            // switch to config.xml
            if (rename(CONFIG_FILE_NEW_W_PATH, CONFIG_FILE_W_PATH) == 0)
            {
                DDBG("%s has been renamed to :%s\n", CONFIG_FILE_NEW_W_PATH, CONFIG_FILE_W_PATH);
            }
            //TODO: if failed, it will exist,
        }
        else
        {
            DDBG("=== check config.xml not valid config file: %s\n", CONFIG_FILE_NEW_W_PATH);


            // Buffer config.xml
            if (!IsValidTPCSConfig(CONFIG_FILE_W_PATH, &(pConfigBuf->pConfigBuffer)))
            {
                //Recover config.xml from memory
                InitConfigFile(&(pConfigBuf->pConfigBuffer));
                DDBG("***************** initconfigfile: %p\n", pConfigBuf->pConfigBuffer);

                DDBG("Failed to parse file : %s\n", CONFIG_FILE_W_PATH);

                // Add plugins by scanning appPath directory
                int count;
                char **paths = NULL;
                GetConfigElements(&(pConfigBuf->pConfigBuffer), (xmlChar*) XPATH_APP_PATHS, &count,
                                  &paths);

                int i;
                int dirCount = 0;
                char **app_path = NULL;
                char **appId = NULL;
                for (i = 0; i < count; i++)
                {
                    // Get list of directories under the app path, if it has $appPath/lib/plugin/,
                    // search its manifest.xml for av tag, then recompose config.xml

                    //TODO: if better way to get list of directory
                    GetDirsHasPath(paths[i], PLUGIN_DEFAULT_DIR_NAME, &dirCount, &app_path, &appId);
                    DDBG("fater get dirshas path:%s\n", PLUGIN_DEFAULT_DIR_NAME);
                    if (dirCount > 0)
                    {
                        // search manifest.xml within the app_path, if found, copy the appid to plugin element
                        int j;
                        for (j = 0; j < dirCount; j++)
                        {
                            //ftw return 0: tree exhausted, -1: error, others whatever the FileTreeCallback return
                            int result = ftw(app_path[j], FileTreeCallback, 12);
                            if (result == 1)
                            {
                                DDBG("it found the plugin app dir:%s, appID:%s\n", app_path[j], appId[j]);
                                //Update plugin element node
                                UpdatePluglibList(app_path[j], &(pConfigBuf->pConfigBuffer),
                                                  appId[j]);
                            }
                        }
                    }
                    CleanupArray(&appId, dirCount);
                    CleanupArray(&app_path, dirCount);
                }

                CleanupArray(&paths, count);

                // Recover config.xml from memory
                WriteXmlToFile(pConfigBuf->pConfigBuffer, CONFIG_FILE_W_PATH);
                DDBG("before clean up:%s, dircoutn:%d, pathCount:%d, \n", CONFIG_FILE_W_PATH, dirCount, count);

            }
            else
            {
                DDBG("Success in paring file: %s\n", CONFIG_FILE_W_PATH);
            }
        }

        // Register methods for GetInfoPlugin
        IpcServerMethod method_getPlugin;
        snprintf(method_getPlugin.szMethod, sizeof(method_getPlugin.szMethod), "%s",
                 "TPCSGetInfoPlugin");
        method_getPlugin.method = (METHODFUNC) GetInfoPlugin;
        method_getPlugin.pData = &(pConfigBuf->pConfigBuffer);

        if (IpcServerAddMethod(hServer, &method_getPlugin) != 0)
        {
            DDBG("%s", "unable to add method GetInfoPlugin\n");
            goto close_server;
        }

        // Register methods for InstallPlugin
        IpcServerMethod method_installPlugin;
        snprintf(method_installPlugin.szMethod, sizeof(method_installPlugin.szMethod), "%s",
                 "TPCSInstallPlugin");
        method_installPlugin.method = (METHODFUNC) InstallPlugin;
        method_installPlugin.pData = &pConfigBuf;

        if (IpcServerAddMethod(hServer, &method_installPlugin) != 0)
        {
            DDBG("%s", "unable to add method InstallPlugin\n");
            goto close_server;
        }

        // Register methods for SetActivePlugin
        IpcServerMethod method_setActivePlugin;
        snprintf(method_setActivePlugin.szMethod, sizeof(method_setActivePlugin.szMethod), "%s",
                 "TPCSSetActivePlugin");
        method_setActivePlugin.method = (METHODFUNC) SetActivePlugin;
        method_setActivePlugin.pData = &pConfigBuf;

        if (IpcServerAddMethod(hServer, &method_setActivePlugin) != 0)
        {
            DDBG("%s", "unable to add method SetActivePlugin\n");
            goto close_server;
        }

        // Register methods for UnInstallPlugin
        IpcServerMethod method_uninstallPlugin;
        snprintf(method_uninstallPlugin.szMethod, sizeof(method_uninstallPlugin.szMethod), "%s",
                 "TPCSUninstallPlugin");
        method_uninstallPlugin.method = (METHODFUNC) UninstallPlugin;
        method_uninstallPlugin.pData = &pConfigBuf;

        if (IpcServerAddMethod(hServer, &method_uninstallPlugin) != 0)
        {
            DDBG("%s", "unable to add method UninstallPlugin\n");
            goto close_server;
        }

        DDBG("----------------- START method loop----------%s", "\n");

        // Daemon waits here for request from clients.
        IpcServerMainLoop(hServer);
        DDBG("----------------- END method loop----------%s", "\n");
        // Clean up
        DDBG("======================= free xmldoc for isvalidConfig %s\n", "==============");
        DDBG("pconfigbuf address:%p\n", pConfigBuf->pConfigBuffer);
        xmlFreeDoc(pConfigBuf->pConfigBuffer);
        free(pConfigBuf);

        IpcServerClose(&hServer);
    }
    else
    {
        DDBG("%s", "unable to open server connection \n");
        ret = -1;
        goto done;
    }
    xmlCleanupParser(); // clean up library for xml library for valgrind
    close_server:
    IpcServerClose(&hServer);

    done:
    DDBG("%s", "Unable to start the Daemon \n");

    return ret;

}
