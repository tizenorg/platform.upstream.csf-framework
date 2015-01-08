#ifndef TCSPIMPL_H
#define TCSPIMPL_H

#ifdef __cplusplus 
extern "C" {
#endif

#define TCSP_PLUGIN_VERSION "2.0.2"
#define TCSP_PLUGIN_INFO "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <Root><Plug>\n\
        <Version>2.0.2</Version>\n\
        <VendorName>McAfee</VendorName>\n\
        <ProductName>TPCS</ProductName>\n\
        <AppId>EmbkcJFK7q</AppId>\n\
        </Plug></Root>\n"
/**
 * \file TCSPImpl.h
 * \brief TCS Plug-in Header File
 *  
 * This file provides the Tizen Content Screen Plug-in API functions.
 */


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * \brief Version number of plugin
 *
 * This is a synchronous API
 *
 * \return Return Type (const char *) \n
 * Version number of plugin. \n
 */
const char *TCSPGetVersion(void);

/**
 * \brief Gets Meta info about the plugin
 *
 * This is a synchronous API
 *
 * \return Return Type (const char *) \n
 * Meta information of plugin. \n
 * Size should less than TCS_META_MAX. \n
 */
const char *TCSPGetInfo(void);

/**
 * \brief Initializes and returns a Tizen Content Screening library
 * interface handle.
 *
 * A Content Screening library interface handle (or TCS library handle) is
 * obtained using the TCSLibraryOpen() function. The library handle is required for
 * subsequent TCS API calls. The TCSLibraryClose() function releases/closes the library
 * handle. Multiple library handles can be obtained using TCSLibraryOpen().
 *
 * This is a synchronous API.
 *
 * \return Return Type (TCSLIB_HANDLE) \n
 * TCS library interface handle - on success. \n
 * INVALID_TCSLIB_HANDLE - on failure. \n
 */
TCSLIB_HANDLE TCSPLibraryOpen(void);

/**
 * \brief Releases system resources associated with an TCS API library
 * handle returned by the TCSLibraryOpen() function.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib TCS library handle returned by TCSLibraryOpen().
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure. \n
 */
int TCSPLibraryClose(TCSLIB_HANDLE hLib);

/**
 * \brief Returns the last error code associated with the given
 * TCS library handle.
 *
 * Once the TCS library handle has been successfully obtained from TCSLibraryOpen(),
 * TCSGetLastError() can be used to retrieve the last TCS error that occurred. All TCS
 * API functions return zero (= 0) or a valid object pointer if successful, and -1
 * or a null object handle (e.g. INVALID_TCSSCAN_HANDLE) in case of an error. The
 * TCSGetLastError() function is used to retrieve error information when a TCS
 * function fails.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib TCS library handle returned by TCSLibraryOpen().
 *
 * \return Return Type (TCSErrorCode) \n
 * Last error code set by the TCS library. The TCSErrorCode data type is defined as a
 * 32-bit unsigned integer which contains both component and an error code (see
 * Figure about TCS Error Code Format). Two macros are available to extract the error
 * module and the error code. Call TCS_ERRMODULE(error-code) to get the error module,
 * and TCS_ERRCODE(error-code) to get the error code (where error-code is the value
 * returned by TCSGetLastError()).
 *
 * TCS library call sequence with a call to the TCSGetLastError() function:
 *
 */
TCSErrorCode TCSPGetLastError(TCSLIB_HANDLE hLib);

/**
 * \brief TCSScanData() is used to scan a data buffer for malware. The caller
 * specifies a scanner action, scan target data type, set I/O functions to access
 * the data, and an optional callback function for information retrieval. The result
 * of the data scanning is returned in a caller provided data structure.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the TCSLibraryOpen()
 * function.
 * \param[in] pParam Pointer to a structure containing data scan parameters.
 * \param[out] pResult Pointer to a structure containing data scan
 * results.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 *
 */
int TCSPScanData(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult);

/**
 * \brief TCSScanFile() is used to scan a file for malware. The caller specifies a
 * file name, a scanner action, and scan target data type. The scan result is
 * returned in a caller provided data structure.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the
 * TCSLibraryOpen() function.
 * \param[in] pszFileName Name of file to scan. The file name must include the
 * absolute path.
 * \param[in] iDataType Type of data contained in the file. This is used to
 * perform data type specific scans on files.
 * \param[in] iAction Type of scanning to perform on file.
 * \param[in] iCompressFlag Enables or disables decompression of archive files.
 * \param[out] pResult Pointer to a structure containing data scan results.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSPScanFile(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                 int iAction, int iCompressFlag, TCSScanResult *pResult);

/**
 * \brief TCSScanFile() is used to scan a file for malware. The caller specifies a
 * file name, a scanner action, and scan target data type. The scan result is
 * returned in a caller provided data structure. Extra parameters pPrivate and
 * pfCallBack are for Callback.
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the
 * TCSLibraryOpen() function.
 * \param[in] pszFileName Name of file to scan. The file name must include the
 * absolute path.
 * \param[in] iDataType Type of data contained in the file. This is used to
 * perform data type specific scans on files.
 * \param[in] iAction Type of scanning to perform on file.
 * \param[in] iCompressFlag Enables or disables decompression of archive files.
 * \param[in] pPrivate Pointer (or handle) to an application object being scanned.
 * \param[in] pfCallBack Callback function used to notify caller for specific
 * events via this callback function.
 * \param[out] pResult Pointer to a structure containing data scan results.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSPScanFileEx(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                 int iAction, int iCompressFlag, void *pPrivate,
                 int (*pfCallBack)(void *pPrivate, int iReason, void *pParam),
                 TCSScanResult *pResult);


#ifdef __cplusplus
}
#endif 

#endif  /* TCSIMPL_H */
