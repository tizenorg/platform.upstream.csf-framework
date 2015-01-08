/*
    Copyright (c) 2014, McAfee, Inc.
    
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    
    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.
    
    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
    
    Neither the name of McAfee, Inc. nor the names of its contributors may be used
    to endorse or promote products derived from this software without specific prior
    written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TWPIMPL_H
#define TWPIMPL_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * \file TWPImpl.h
 * \brief TWP Header File
 *
 * This file provides the Tizen Web Protection API functions.
 */

typedef long unsigned int TWPMallocSizeT; /* Size unit */

struct TWPLibHandle_struct {int iDummy;};

typedef struct TWPLibHandle_struct *TWPLIB_HANDLE;

#define TWP_VER_MAX 16

#define TWP_META_MAX 1000

#define TWP_FRAMEWORK_VERSION "2.0.2"

#define TWPAPI_VERSION 1 /* SDK version */

#define TWPCONFIG_VERSION 1 /* Configure version */

#define TWPREQUEST_VERSION 1 /* Request version */

#define INVALID_TWPLIB_HANDLE ((TWPLIB_HANDLE) 0) /* Invalid web protection library interface handle. */

/**
 * Structure for version info of TWP Framework and Plugin
 */
typedef struct TWPVerInfo_struct
{
    char szFrameworkVer[TWP_VER_MAX];
    char szPluginVer[TWP_VER_MAX];
} TWPVerInfo;

/**
 * Result code used by TWP_RESULT
 */
typedef enum
{
    TWP_SUCCESS = 0,
    TWP_ERROR = 1,
    TWP_NOMEM = 2,
    TWP_INVALID_HANDLE = 3,
    TWP_INVALID_PARAMETER = 4,
    TWP_INVALID_VERSION = 5,
    TWP_INVALID_RESPONSE = 6,
    TWP_NO_DATA = 7,
    TWP_NOT_IMPLEMENTED = 500
} TWP_RESULT;

/**
 * Web site category definitions
 */
typedef enum 
{
    TWP_Artcultureheritage,
    TWP_Alcohol,
    TWP_Anonymizers,
    TWP_Anonymizingutilities,
    TWP_Business,
    TWP_Chat,
    TWP_Publicinformation,
    TWP_Potentialcriminalactivities,
    TWP_Drugs,
    TWP_Educationreference,
    TWP_Entertainment,
    TWP_Extreme,
    TWP_Financebanking,
    TWP_Gambling,
    TWP_Games,
    TWP_Governmentmilitary,
    TWP_Potentialhackingcomputercrime,
    TWP_Health,
    TWP_Humorcomics,
    TWP_Discrimination,
    TWP_Instantmessaging,
    TWP_Stocktrading,
    TWP_Internetradiotv,
    TWP_Jobsearch,
    TWP_Informationsecurity,
    TWP_E_RESERVED_1,
    TWP_Mobilephone,
    TWP_Mediadownloads,
    TWP_Malicioussites,
    TWP_E_RESERVED_2,
    TWP_Nudity,
    TWP_Nonprofitadvocacyngo,
    TWP_Generalnews,
    TWP_Onlineshopping,
    TWP_Provocativeattire,
    TWP_P2pfilesharing,
    TWP_Politicsopinion,
    TWP_Personalpages,
    TWP_Portalsites,
    TWP_Remoteaccess,
    TWP_Religionideology,
    TWP_Resourcesharing,
    TWP_Searchengines,
    TWP_Sports,
    TWP_Streamingmedia,
    TWP_Sharewarefreeware,
    TWP_Pornography,
    TWP_Spywareadwarekeyloggers,
    TWP_Tobacco,
    TWP_Travel,
    TWP_Violence,
    TWP_Webads,
    TWP_Weapons,
    TWP_Webmail,
    TWP_Webphone,
    TWP_Auctionsclassifieds,
    TWP_Forumbulletinboards,
    TWP_Profanity,
    TWP_Schoolcheatinginformation,
    TWP_Sexualmaterials,
    TWP_Gruesomecontent,
    TWP_Visualsearchengine,
    TWP_Technicalbusinessforums,
    TWP_Gamblingrelated,
    TWP_Messaging,
    TWP_Gamecartoonviolence,
    TWP_Phishing,
    TWP_Personalnetworkstorage,
    TWP_Spamurls,
    TWP_Interactivewebapplications,
    TWP_Fashionbeauty,
    TWP_Softwarehardware,
    TWP_Potentialillegalsoftware,
    TWP_Contentserver,
    TWP_Internetservices,
    TWP_Mediasharing,
    TWP_Incidentalnudity,
    TWP_Marketingmerchandising,
    TWP_Parkeddomain,
    TWP_Pharmacy,
    TWP_Restaurants,
    TWP_Realestate,
    TWP_Recreationhobbies,
    TWP_Blogswiki,
    TWP_Digitalpostcards,
    TWP_Historicalrevisionism,
    TWP_Technicalinformation,
    TWP_Datingpersonals,
    TWP_Motorvehicles,
    TWP_Professionalnetworking,
    TWP_Socialnetworking,
    TWP_Texttranslators,
    TWP_Webmeetings,
    TWP_Forkids,
    TWP_E_RESERVED_3,
    TWP_Moderated,
    TWP_Textspokenonly,
    TWP_Controversialopinions,
    TWP_Residentialipaddresses,
    TWP_Browserexploits,
    TWP_Consumerprotection,
    TWP_Illegaluk,
    TWP_Majorglobalreligions,
    TWP_Maliciousdownloads,
    TWP_Potentiallyunwantedprograms,

    TWP_LastCategoryPlaceholder = 128,
    TWP_OverallPhishing = 129,
    TWP_OverallRiskHigh = 130,
    TWP_OverallRiskMedium = 131,
    TWP_OverallRiskMinimal = 132,
    TWP_OverallRiskUnverified = 137,
    TWP_OverallMcAfeeSecure = 138,
    TWP_LastAttributePlaceholder = 160,
} TWPCategories;

/**
 * Risk level
 */
typedef enum
{
    TWP_Minimal,
    TWP_Unverified,
    TWP_Medium,
    TWP_High,
} TWPRiskLevel;

/**
 * Score range
 */
typedef enum 
{
    TWP_MinimalLow = 0,
    TWP_MinimalHigh = 14,
    TWP_UnverifiedLow = 15,
    TWP_UnverifiedHigh = 29,
    TWP_MediumLow = 30,
    TWP_MediumHigh = 49,
    TWP_HighLow= 50,
    TWP_HighHigh = 127
} TWPScoreRange;

/**
 * HTTP submit method
 */
typedef enum
{
    TWPPOST,
} TWPSubmitMethod;

/* forward declaration */
struct TWPRequest;
typedef struct TWPConfiguration *TWPConfigurationHandle;
typedef struct TWPResponse *TWPResponseHandle;
typedef struct TWPUrlRating *TWPUrlRatingHandle;
typedef struct TWPPolicy *TWPPolicyHandle;
typedef void *(*TWPFnMemAlloc)(TWPMallocSizeT size);
typedef void (*TWPFnMemFree)(void *address);
typedef long (*TWPFnRandom)(void);
typedef TWP_RESULT (*TWPFnRequestSetUrl)(struct TWPRequest *request, const char *pUrl,
                                         unsigned int length);
typedef TWP_RESULT (*TWPFnRequestSetMethod)(struct TWPRequest *request, TWPSubmitMethod method);
typedef TWP_RESULT (*TWPFnRequestSend)(struct TWPRequest *request, TWPResponseHandle response,
                                       const void *data, unsigned int length);
typedef TWP_RESULT (*TWPFnRequestReceive)(struct TWPRequest *request, void *buffer,
                                          unsigned int buffer_length, unsigned int *length);

/**
 * Initialize data requested by SDK initialization
 */
typedef struct TWPAPIInit
{
    int api_version;
    TWPFnMemAlloc memallocfunc;
    TWPFnMemFree memfreefunc;
} TWPAPIInit;

/**
 * Configuration which enable caller to customize the SDK
 */
typedef struct TWPConfiguration
{
    int config_version; /* Configuration version */
    const char *client_id; /* Client id for cloud to qualify */
    const char *client_key; /* Corresponding key for specific client for validation from cloud */
    const char *host; /* Host name for cloud where SDK send request to, set to NULL for SDK to use default settings in plug-in */
    int secure_connection; /* 1 - use secured connection (HTTPS), 0 - not secured connection. */
    int skip_dla; /* 1 - disable DLA lookup, 0 - enable DLA lookup */
    int obfuscate_request; /* 1 - obfuscate request data, 0 - do not obfuscate request data */
    TWPFnRandom randomfunc; /* Caller customized random function */
} TWPConfiguration;

/**
 * Request for SDK to check URL against cloud database
 */
typedef struct TWPRequest
{
    int request_version; /* Request version */
    TWPFnRequestSetUrl seturlfunc; /* Callback for SDK to set URL from SDK */
    TWPFnRequestSetMethod setmethodfunc; /* Callback for SDK to set HTTP request method */
    TWPFnRequestSend sendfunc; /* Callback for SDK to send HTTP request */
    TWPFnRequestReceive receivefunc; /* Callback for SDK to receive HTTP response, if caller set it to be NULL,
                                        SDK will assume the HTTP request will be handled in a-synchronized manner */
} TWPRequest;

/**
 * \brief
 * Initialize SDK
 *
 * This is a synchronized API
 *
 * \param[in] pApiInit API initialization data structure.
 *
 * \return TWP_RESULT
 */
TWPLIB_HANDLE TWPInitLibrary(TWPAPIInit *pApiInit);

/**
 * \brief Gets the version number of TWP Framework and Plugin.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the TWPInitLibrary().
 * \param[out] pszVersion Pointer to a structure containing version info.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPGetVersion(TWPLIB_HANDLE hLib, TWPVerInfo *pVerInfo);

/**
 * \brief Gets the meta information about the plugin
 *
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the TWPInitLibrary().
 * \param[out] pszInfo string containing meta info, pszInfo allocated by caller of size TWP_META_MAX.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPGetInfo(TWPLIB_HANDLE hLib, char *pszInfo);

/**
 * \brief
 * Uninitialize SDK
 *
 * This is a synchronized API
 *
 */
void TWPUninitLibrary(TWPLIB_HANDLE hLib);

/**
 * \brief
 * Create TWP configuration to customize SDK
 * 
 * This is a synchronized API
 *
 * \param[in] pConfigure caller configurations
 * \param[out] phConfigure created configuration for SDK
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPConfigurationCreate(TWPLIB_HANDLE hLib, TWPConfiguration *pConfigure, TWPConfigurationHandle *phConfigure);

/**
 * \brief
 * Release the configuration resources allocated by TWPConfigurationCreate
 *
 * This is a synchronized API
 *
 * \param[in] hConfigure configuration to be destroyed
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPConfigurationDestroy(TWPLIB_HANDLE hLib, TWPConfigurationHandle *hConfigure);

/**
 * \brief
 * Main function for caller to check URL reputation against the cloud database
 *
 * This can be a synchronized API or a-synchronized API depends on the configuration from caller
 *
 * Synchronous mode
 *      In this synchronous operation mode, the function invokes TWPRequest::sendfunc and
 *      TWPRequest::receivefunc, one right after the other, expecting the entire HTTP
 *      transaction to be completed between the calls. Upon successful completion, the
 *      phResponse will point to a valid response handle that can be used to analyze results.
 *
 * Asynchronous mode
 *      In the asynchronous mode, the function invokes TWPRequest::sendfunc and returns
 *      immediately with TWP_SUCCESS. Upon completion, phResponse is NULL. The application
 *      is supposed to complete the HTTP transaction while calling TWPResponseWrite as
 *      response data becomes available. When all data was read, TWPResponseWrite must
 *      be called again with zero data length to signal the end transaction.
 *
 * \param[in] hConfigure Configuration of caller
 * \param[in] pRequest Request data structure for SDK to check with cloud
 * \param[in] iRedirUrl 1 indicating instruct the cloud server to provide a landing page
 *            URL to which blocked URLs can be redirected.
 * \param[in] ppUrls An array of 7 bit ASCII character strings representing URLs to obtain
 *            the rating for.
 *
 *            Note: All URLs have to be normalized before submission (see RFC 3986) and
 *            pynicoded if required.
 * \param[in] uCount Length of the ppUrls array.
 * \param[out] phResponse For synchronous requests, a pointer to the location where the
 *             response object handle will be stored upon completion. It can be NULL for
 *             asynchronous requests.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPLookupUrls(TWPLIB_HANDLE hLib, TWPConfigurationHandle hConfigure, TWPRequest *pRequest,
                         int iRedirUrl, const char **ppUrls, unsigned int uCount, TWPResponseHandle *phResponse);

/**
 * \brief
 * In asynchronous mode, caller will call this API to write received HTTP response data
 * to SDK. Writing with zero data length will be taken as end of HTTP transaction for
 * SDK.
 *
 * This is a synchronized API
 *
 * \param[in] hResponse Response handle for SDK to keep track on HTTP transaction.
 * \param[in] pData Received HTTP response data chunk.
 * \param[in] uLength Length of the HTTP response data.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseWrite(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const void *pData, unsigned uLength);

/**
 * \brief
 * Get web site rating by its index in URL list in the response which comply to
 * the URL list order passed by caller in TWPLookupUrls().
 *
 * This is a synchronized API
 *
 * \param[in] hResponse Response handle created based on cloud response.
 * \param[in] iIndex Index of the web site in request list.
 * \param[out] phRating Rating of the specified web site.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseGetUrlRatingByIndex(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int uIndex,
                                          TWPUrlRatingHandle *phRating);

/**
 * \brief
 * Get web site rating by its URL string.
 *
 * This is a synchronized API
 *
 * \param[in] hResponse Response handle created based on cloud response.
 * \param[in] pUrl URL string
 * \param[in] iUrlLength URL string length
 * \param[out] hRating Rating of the specified web site
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseGetUrlRatingByUrl(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const char *pUrl,
                                        unsigned int uUrlLength, TWPUrlRatingHandle *hRating);

/**
 * \brief
 * Get the redirection URL for blocked URL to display to user.
 *
 * Blocking pages can be used by application that want to block users
 * from navigating to a URL that violates one of the defined policies.
 * The returned string must be deallocated by the application using
 * TWPAPIInit::TWPFnMemFree function.
 *
 * \param[in] hResponse Response handle created based on cloud response.
 * \param[in] hRating Rating handle resolved from cloud response.
 * \param[in] hPolicy Policy handle created by caller.
 * \param[out] ppUrl Redirection URL.
 * \param[ou] puLength Length of redirection URL
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseGetRedirUrlFor(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, TWPUrlRatingHandle hRating,
                                     TWPPolicyHandle hPolicy, char **ppUrl, unsigned int *puLength);

/**
 * \brief
 * Get the rating count of specified response.
 *
 * \param[in] hResponse Response handle created based on cloud response.
 * \param[out] puCount Rating count.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseGetUrlRatingsCount(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int *puCount);

/**
 * \brief
 * Release resource for response handle.
 *
 * \param[in] hResponse Response handle created based on cloud response.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPResponseDestroy(TWPLIB_HANDLE hLib, TWPResponseHandle *hResponse);

/**
 * \brief
 * Create the policy (set of web site categories) caller want to check.
 *
 * \param[in] hCfg configuration handle
 * \param[in] pCategories Web site category list
 * \param[in] uCount Category list length.
 * \param[out] phPolicy Policy handle.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPPolicyCreate(TWPLIB_HANDLE hLib, TWPConfigurationHandle hCfg, TWPCategories *pCategories, unsigned int uCount, TWPPolicyHandle *hPolicy);

/**
 * \brief
 * Compare the categories assigned by security vendor to the URL represented
 * by hRating with the categories assigned to the policy handle.
 *
 * \param[in] hPolicy Polcy handle
 * \param[in] hRating Rating for specific URL
 * \param[out] piVialated non-zero if intersection found between the policy and URL rating categories.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPPolicyValidate(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating, int *piViolated);

/**
 * \brief
 * Retrieves all categories common between the policy and URL rating.
 *
 * \param[in] hPolicy Policy handle.
 * \param[in] hRating URL rating handle.
 * \param[out] ppViolated An array of all common categories. This array is allocated by using
 *             TWPAPIInit::memallocfunc and has to be deallocated by the caller.
 * \param[out] puLength Length of violation array.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPPolicyGetViolations(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating,
                                  TWPCategories **ppViolated, unsigned *puLength);

/**
 * \brief
 * Release resource for policy handle.
 *
 * \param[in] phPolicy Pointer to policy handle.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPPolicyDestroy(TWPLIB_HANDLE hLib, TWPPolicyHandle *hPolicy);

/**
 * \brief
 * Get score from URL rating data structure which is assigned by security vendor.
 *
 * \param[in] hRating Rating handle.
 * \param[out] piScore URL score.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPUrlRatingGetScore(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, int *piScore);

/**
 * \brief
 * Get corresponding URL from rating handle.
 *
 * \param[in] hRating Rating handle.
 * \param[out] ppUrl A pointer to a NULL terminated string representing
 *             the URL. The string is valid as long as the URL rating
 *             handle is valid.
 * \param[out] puLength An optional pointer to the length of URL string.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPUrlRatingGetUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppUrl,
                              unsigned int *puLength);

/**
 * \brief
 * Get DLA (Deep Link Analysis) URL
 *
 * \param[in] hRating Rating handle.
 * \param[out] ppDlaUrl A ponit to a NULL terminated string representing
 *             the DLA URL. This string is valid as long as the URL rating
 *             handle is valid.
 * \param[out] puLength Length of DLA URL string.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPUrlRatingGetDLAUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppDlaUrl,
                                 unsigned int *puLength);

/**
 * \brief
 * Determine whether the URL rating object has the specified category.
 *
 * \param[in] hRating Rating handle.
 * \param[in] Category Category enum value.
 * \param[out] piPresent Non-zero value indicating exists.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPUrlRatingHasCategory(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories Category,
                                   int *piPresent);

/**
 * \brief
 * Retrives categories assigned by security vendor for the rated URL.
 *
 * \param[in] hRating Rating handle.
 * \param[out] ppCategories The pointer to a variable that contains the address
 *             of the category list.
 * \param[out] puLength Length of category list.
 *
 * \return TWP_RESULT
 */
TWP_RESULT TWPUrlRatingGetCategories(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories **ppCategories,
                                     unsigned int *puLength);
/**
 * \brief
 * Checks whether the URL violates the default policy and configurations.
 * Retrieves the Risk level for all URLs and the Redirection URL is retrieved
 * if level is greater than or equal to TWP_MediumLow score.
 *
 * \param[in] pUrl URL to check.
 * \param[out] ppBlkUrl Redirection URL.
 * \param[out] puBlkUrlLen Length of redirection URL.
 * \param[out] pRiskLevel Risk level of the URL.
 * 
 * \return TWP_RESULT
 */
TWP_RESULT TWPCheckURL(TWPLIB_HANDLE hLib, const char *pUrl, char **ppBlkUrl, unsigned int *puBlkUrlLen,
                      int *pRiskLevel);

#ifdef __cplusplus
}
#endif
#endif

