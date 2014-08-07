#define ERROR_GENERIC -1
#define RETURN_SUCCESS "0"
#define RETURN_FAILURE "1"

#define FN_GET_INFO_RTN_COUNT 2
#define FN_INSTALL_PLUG_RTN_COUNT 2
#define DO_RECURSIVE_COPY 1
#define FILE_NAME_MAX_SIZE  128
#define GENERIC_STRING_SIZE 128
#define FN_GET_INFO "getInfo"
#define TAG_VERSION "Version"
#define TAG_VENDOR_NAME "VendorName"
#define TAG_PRODUCT_NAME "ProductName"
#define TAG_APP_ID "AppId"
#define TAG_PLUG_NODE "Plug"
#define MANIFEST_FILE_NAME "manifest.xml"
#define XPATH_PLUGIN_CATEGORY "//Manifest:Manifest/Manifest:Apps/Manifest:UiApp/Manifest:Categories/Manifest:Category"
#define PLUGIN_ANTI_VIRUS_TAG "http://tizen.org/category/antivirus"
#define PLUGIN_SYSTEM_PATH "/usr/bin"
#define PLUGIN_DEFAULT_DIR_NAME "/lib/plugin"
#define XPATH_PLUGINS "//TPCSConfig/Plugins"
#define XPATH_PLUGINS_PLUG "//TPCSConfig/Plugins/Plug/AppId"
#define XPATH_ACTIVE_PLUGIN "//TPCSConfig/Active/AppId"
#define XPATH_ACTIVE "//TPCSConfig/Active"
#define XPATH_APP_PATHS "//TPCSConfig/AppPaths/Path"
#define CONFIG_ENCODING "UTF-8"
#define CONFIG_DTD_FILE_W_PATH "/usr/bin/tpcs_config.dtd"
#define CONFIG_FILE_W_PATH "/usr/bin/tpcs_config.xml"
#define CONFIG_FILE_NEW_W_PATH "/usr/bin/tpcs_config_new.xml"
#define SYSLINK_PLUG_PATH "/lib/plugin"
#define ACTIVE_NONE "None"
#define CONFIG_DEFAULT_STRING "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
    <TPCSConfig>\n\
    <AppPaths>\n\
    <Path>/opt/usr/apps</Path>\n\
    <Path>/sdcard</Path>\n \
    </AppPaths>\n\
    <Active>\n\
        <AppId>None</AppId>\n\
    </Active>\n\
    <Plugins>\n\
    </Plugins>\n\
</TPCSConfig>"


#define CONFIG_DEFAULT_DTD_STRING "<!ELEMENT TPCSConfig (AppPaths, Active, Plugins)>\n\
<!ELEMENT AppPaths (Path+)>\n\
<!ELEMENT Active (AppId)>\n\
<!ELEMENT Plugins (Plug*)>\n\
<!ELEMENT Plug (Version|VendorName|ProductName|AppId)*>\n\
<!ELEMENT Path (#PCDATA)>\n\
<!ELEMENT Version (#PCDATA)>\n\
<!ELEMENT VendorName (#PCDATA)>\n\
<!ELEMENT ProductName (#PCDATA)>\n\
<!ELEMENT AppId (#PCDATA)>"
#define CONFIG_FILE_NAME "tpcs_config.xml"

