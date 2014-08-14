#ifndef TWPPIMPL_H
#define TWPPIMPL_H

#ifdef __cplusplus 
extern "C" {
#endif

#define TWPP_PLUGIN_VERSION "2.0.2"

#define TWPP_PLUGIN_INFO "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <Root><Plug>\n\
        <Version>2.0.2</Version>\n\
        <VendorName>McAfee</VendorName>\n\
        <ProductName>TWP</ProductName>\n\
        <AppId>EmbkcJFK7q</AppId>\n\
        </Plug></Root>\n"
/**
 * \file TWPPImpl.h
 * \brief TWP Plug-in Header File
 *  
 * This file provides the Tizen Web Protection Plug-in API functions.
 */


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * \brief Initializes.
 *
 * This is a synchronous API.
 *
 */
TWP_RESULT TWPPInitLibrary(TWPAPIInit *pApiInit);

void TWPPUninitLibrary(void);

TWP_RESULT TWPPConfigurationCreate(TWPConfiguration *pConfigure, TWPConfigurationHandle *phConfigure);

TWP_RESULT TWPPConfigurationDestroy(TWPConfigurationHandle *phConfigure);

TWP_RESULT TWPPLookupUrls(TWPConfigurationHandle hConfigure, TWPRequest *pRequest, int iRedirUrl,
                          const char **ppUrls, unsigned int uCount, TWPResponseHandle *phResponse);

TWP_RESULT TWPPResponseWrite(TWPResponseHandle hResponse, const void *pData, unsigned uLength);

TWP_RESULT TWPPResponseGetUrlRatingByIndex(TWPResponseHandle hResponse, unsigned int uIndex,
                                           TWPUrlRatingHandle *phRating);

TWP_RESULT TWPPResponseGetUrlRatingByUrl(TWPResponseHandle hResponse, const char *pUrl,
                                         unsigned int uUrlLength, TWPUrlRatingHandle *phRating);

TWP_RESULT TWPPResponseGetRedirUrlFor(TWPResponseHandle hResponse, TWPUrlRatingHandle hRating,
                                      TWPPolicyHandle hPolicy, char **ppUrl, unsigned int *puLength);

TWP_RESULT TWPPResponseGetUrlRatingsCount(TWPResponseHandle hResponse, unsigned int *puCount);

TWP_RESULT TWPPResponseDestroy(TWPResponseHandle *phResponse);

TWP_RESULT TWPPPolicyCreate(TWPConfigurationHandle hCfg, TWPCategories *pCategories, unsigned int uCount,
                            TWPPolicyHandle *phPolicy);

TWP_RESULT TWPPPolicyValidate(TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating, int *piViolated);

TWP_RESULT TWPPPolicyGetViolations(TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating,
                                   TWPCategories **ppViolated, unsigned *puLength);

TWP_RESULT TWPPPolicyDestroy(TWPPolicyHandle *phPolicy);

TWP_RESULT TWPPUrlRatingGetScore(TWPUrlRatingHandle hRating, int *piScore);

TWP_RESULT TWPPUrlRatingGetUrl(TWPUrlRatingHandle hRating, const char **ppUrl,
                              unsigned int *puLength);

TWP_RESULT TWPPUrlRatingGetDLAUrl(TWPUrlRatingHandle hRating, const char **ppDlaUrl,
                                  unsigned int *puLength);

TWP_RESULT TWPPUrlRatingHasCategory(TWPUrlRatingHandle hRating, TWPCategories Category,
                                    int *piPresent);

TWP_RESULT TWPPUrlRatingGetCategories(TWPUrlRatingHandle hRating, TWPCategories **ppCategories,
                                      unsigned int *puLength);

TWP_RESULT TWPPCheckURL(const char *pUrl, char **ppBlkUrl, unsigned int *puBlkUrlLen,
                        int *pRiskLevel);

const char *TWPPGetVersion(void);

#ifdef __cplusplus
}
#endif 

#endif  /* TWPPIMPL_H */

