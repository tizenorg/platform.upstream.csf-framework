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

#ifndef TCSIMPL_H
#define TCSIMPL_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * \file TCSImpl.h
 * \brief TCS Header File
 *  
 * This file provides the Tizen Content Screen API functions.
 */
#define TCS_VER_MAX 16

#define TCS_META_MAX 1000

#define TCS_FRAMEWORK_VERSION "2.0.1"

#define TCS_SA_SCANONLY 1 /* Instructs the scan functions to perform scanning only. */

#define TCS_SA_SCANREPAIR 2 /* Instructs the scan functions to carry out both
                                                                    scanning and repair/removal of detected malware. */

#define TCS_CB_SCANFINISH 0 /* Informs the caller about completion of async file or data scan. The callback data
                                                                            argument pParam is set to point to a TCSScanResult
                                                                            structure. if pParam is null then it means Scan did
                                                                            not succeed*/

#define TCS_CB_DETECTED 1 /* Informs the caller a malicious code has been
                                                              detected in the scan target. The callback data
                                                              argument pParam is set to point to a TCSDetected
                                                              structure. */

#define TCS_DTYPE_UNKNOWN 0 /* Scan for malicious content in an unknown data
                                                                    type. This data type should be used when the
                                                                    other types are not appropriate. */

#define TCS_DTYPE_HTML 1 /* Scan for malicious content in HTML. */

#define TCS_VTYPE_PUP 3 /* Potential unwanted program. */

#define TCS_DTYPE_URL 2 /* Scan for URL with malicious content.
                                                            url-string format should conform to the
                                                            Uniform Resource Locators (RFC 1738)
                                                            specification. */

#define TCS_DTYPE_EMAIL 3 /* Scan for email-address with malicious intent.
                                                                  email-string format should conform with
                                                                  the Internet E-mail address format
                                                                  (RFC 822) specification. */

#define TCS_DTYPE_PHONE 4 /* Scan for phone number with malicious intent.
                                                              phone-number string consists of the numeric
                                                              characters '0' through '9', and the
                                                              '#' and '*' characters. */

#define TCS_DTYPE_JAVA 5 /* Scan for malicious Java code. */

#define TCS_DTYPE_JAVAS 6 /* Scan for malicious Java code. */

#define TCS_DTYPE_TEXT 7 /* Scan text data for malicious content. */

#define TCS_VTYPE_MALWARE 1 /* Malware type. */

#define TCS_SC_USER 1 /* Detected malware is harmful to the user. */

#define TCS_SC_TERMINAL 2 /* Detected malware is harmful to the terminal. */

#define TCS_BC_LEVEL0 0 /* Process with a warning. This severity level may
                                                          be assigned to data previously considered malicious. */

#define TCS_BC_LEVEL1 1 /* Prompt the user before processing. Ask the user
                                                            if they want the application to process the data. */

#define TCS_BC_LEVEL2 2 /* Do not process the data. */

#define TCS_BC_LEVEL3 3 /* Do not process the data and prompt user for removal.
                                                        If the content is stored on the terminal,
                                                        prompt the user for permission before removal. */

#define TCS_BC_LEVEL4 4 /* Do not process the data and automatically remove if stored. */


/*==================================================================================================
                                            MACROS
==================================================================================================*/

/**
 * Helper macro to get error module.
 */
#define TCS_ERRMODULE(e) (((e) >> 24) & 0xff)

/**
 * Helper macro to get error code.
 */
#define TCS_ERRCODE(e) ((e) & 0x00ffffff)

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/**
 * Structure for version info of TCS Framework and Plugin
 */
typedef struct TCSVerInfo_struct
{
    char szFrameworkVer[TCS_VER_MAX];
    char szPluginVer[TCS_VER_MAX];
} TCSVerInfo;

/**
 * Dummy data structure to avoid unexpected data type casting.
 */
struct TCSLibHandle_struct {int iDummy;};

/**
 * TCS library handle type.
 */
typedef struct TCSLibHandle_struct *TCSLIB_HANDLE;

#define INVALID_TCSLIB_HANDLE ((TCSLIB_HANDLE) 0) /* Invalid Content Screening library interface handle. */

/**
 * error code type.
 */
typedef unsigned long TCSErrorCode;

/**
 * Support 64 bits data / file locating
 */
typedef long long TCSOffset;

/**
 * The calling application specifies scan
 * parameters using the TCSScanParam structure. The information
 * contained in the structure provides the scan functions with:
 * - scan action type (iAction)
 * - the scan data type (iDataType)
 * - data pointer to the scan target (pPrivate)
 * - callback function to retrieve the data size in bytes (pfGetSize)
 * - callback function to resize the scan data (pfSetSize)
 * - callback function used by the scan functions to retrieve a
 *     block of scan data (pfRead)
 * - callback function used to write to the scan data (pfWrite)
 * - callback function for status/progress reporting (pfCallBack)
 */
typedef struct TCSScanParam_struct
{
    int iAction; /* The scan-action specifies the type of scanning to be performed on supplied scan data. */

    int iDataType; /* The calling application specifies the data type/format of the data to be scanned using this variable. */

    int iCompressFlag; /* 0 - decompression disabled, 1 - decompression enabled. */

    void *pPrivate; /* Pointer (or handle) to an application object being scanned.
                       The scan functions do not perform direct memory I/O using this data
                       pointer/handle. The data pointer/handle is simply passed back to the caller when
                       performing data read/write using caller specified I/O functions. Also the private
                       data is passed back to the caller using the pfCallback function if it is set. */

    TCSOffset (*pfGetSize)(void *pPrivate); /* Used by the scan functions
                                               to obtain the scan target data size (in bytes) from the caller.
                                                                                                            
                                               This is a synchronous API.
                                               [in] pPrivate Pointer (or handle) to an application object being scanned.

                                               return - Return Type (int)
                                               The size (in bytes) of the data to be scanned.
                                               */

    int (*pfSetSize)(void *pPrivate, TCSOffset uSize); /* Called by the scan
                                                          functions to resize the scanned data to a given size (in bytes) during
                                                          repair/clean. The resize function pointer needs to be set if the scan-action
                                                          (iAction) is set to TCS_SA_SCANREPAIR.

                                                          This is a synchronous API.

                                                          @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                          @param[in] uSize The size (in bytes) of the repaired data.

                                                          @return Return Type (int)
                                                          The size (in bytes) of the application data.
                                                          Not equal to the value of uSize indicating this call fails.
                                                          */

    unsigned int (*pfRead)(void *pPrivate, TCSOffset uOffset, void *pBuffer,
                           unsigned int uCount); /* Used for reading a specified
                                                    amount of application data during scanning/analysis.
                                                                                                                    
                                                    This is a synchronous API.

                                                    @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                    @param[in] uOffset Read from the offset in the application data.
                                                    @param[out] pBuffer The buffer used to store the read data.
                                                    @param[in] uCount The size (in bytes) of the data to be read.

                                                    @return Return Type (int) 
                                                    The size (in bytes) of the read data. 
                                                    Not equal to the value of uCount indicating this call fails.
                                                    */

    unsigned int (*pfWrite)(void *pPrivate, TCSOffset uOffset, void const *pBuffer,
                            unsigned int uCount); /* The scan functions use the
                                                     given function to write a specified amount of data to the scanned object as a part
                                                     of the repair process. The function pointer needs to be set if the scan action
                                                     (iAction) is set to TCS_SA_SCANREPAIR.
                                                                                                                    
                                                     This is a synchronous API.

                                                     @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                     @param[in] uOffset Write data from the offset in the application data.
                                                     @param[in] pBuffer The buffer hold the data to be written.
                                                     @param[in] uCount The size (in bytes) of the data to be written.

                                                     @return Return Type (int)
                                                     The size (in bytes) of the written data.
                                                     Not equal to the value of uCount indicating this call fails.
                                                     */

    int (*pfCallBack)(void *pPrivate, int iReason, void *pParam); /* This callback
                                                                     function is set by the caller to be notified to each detected malware while
                                                                     scanning is in process. If specified (not NULL), the scan functions call the
                                                                     specified function with the information (e.g. TCS_CB_DETECTED) for each malware
                                                                     detected in the content/data during scanning.
                                                                                                                                                        
                                                                     This is a synchronous API.
                                                                                                                                                    
                                                                     @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                                     @param[in] iReason Reason of this callback.
                                                                     @param[in] pParam The data for specified callback reason respectively.
                                                                                                                                                    
                                                                     @return Return Type (int) 
                                                                     The scanning process continues if the callback function returns 0. If a negative
                                                                     value (e.g. -1) is returned, the scanning process is aborted and control is
                                                                     returned to the caller.
                                                                     */
} TCSScanParam;

/**
 * Detected malicious code/content information structure.
 */
typedef struct TCSDetected_struct
{
    struct TCSDetected_struct *pNext; /* Pointer to next malware found, NULL if at the end of list. */

    char const *pszName; /* Detected malware name. */
    char const *pszVariant; /* Detected malware's variant name. pszName and
                               pszVariant report detected malicious code/content and variant names. The maximum
                               string length for both strings is 64 characters and each is terminated by a null
                               character ('\\0') - the maximum buffer size for both strings is 65 bytes.

                               pszVariant is set to an empty string ("\0") if the detected malware is not a
                               variant. */

    unsigned int uType; /* Detected malware type. \see TCS_VTYPE_MALWARE */
    unsigned int uAction; /* Bit-field specifying severity, class and behavior level.

                             Included in the TCSDetected structure is a bit-field variable containing malware
                             severity flags and client application behavior levels.

                             The scan functions set the TCS_SC_USER flag if the scanned object/data contains
                             malware harmful to the user. TCS_SC_TERMINAL flag is set if the malware is
                             harmful to the terminal itself. Both TCS_SC_USER and TCS_SC_TERMINAL flags are
                             set if the malware is harmful to both the user and the terminal.

                             The application behavior level specifies what to do with the data/object
                             containing the detected malware.

                             When multiple behavior level codes are found in a scanned data/object, the
                             calling application would be expected to act with the highest behavior level.
                             For example, if both TCS_BC_LEVEL0 and TCS_BC_LEVEL3 were reported, the application
                             would need to take on TCS_BC_LEVEL3 action. */

    char const *pszFileName; /* Path of the infected file. The pszFileName field
                                report, if not NULL, the complete file path of the infected content. If the scan
                                functions have the ability to scan/analyze inside archives, then the path
                                reported in pszFileName would be composed of multiple paths separated by the '|'
                                character. The first path of the sequence is the real file system path of the
                                currently scanned file, for TCSScanFile(), or empty for TCSScanData(). No
                                assumption should be made on the path name separator used for the archive
                                components of the path (the ones following the first). Only the first component,
                                if not empty, is the real file path of the currently scanned content. */
} TCSDetected;

/**
 * Detected malware information is returned to the caller in the TCSScanResult 
 * structure provided by the caller. The TCSScanResult structure contains a pointer 
 * to a structure that contains scan result information and a pointer to a function 
 * used to remove the scan result resource. The memory used to hold the scan result 
 * is allocated by the scan functions and freed by calling the function pointed by the 
 * pfFreeResult pointer. The detected malware information includes the malware
 * information which had been reported via the callback (pfCallback) function during
 * scanning.
 *
 * \code
 * int ScanAppData( ... )
 * {
 *     TCSScanResult scanResult;
 *     .
 *     .
 *     if (TCSScanData(hScanner, &scanParam, &scanResult) == 0)
 *     {
 *         .
 *         .
 *         scanResult.pfFreeResult( &scanResult );
 *     }
 *     .
 *     .
 * }
 * \endcode
 */
typedef struct TCSScanResult_struct
{
    int iNumDetected; /* Number of malware found. */
    TCSDetected *pDList; /* Detected malware list. */
    void (*pfFreeResult)(struct TCSScanResult_struct *pResult); /* Function pointer
                                                                   used to free reported scan result.
                                                                                                                                                
                                                                   This is a synchronous API.
                                                                   \param[in] pResult Pointer to data structure in which detected scan result
                                                                   information is stored.

                                                                   \return None
                                                                   */
} TCSScanResult;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

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
TCSLIB_HANDLE TCSLibraryOpen(void);

/**
 * \brief Gets the version number of TCS Framework and Plugin.
 *
 * This is a synchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the TCSLibraryOpen().
 * \param[out] pVerInfo Pointer to a structure containing version info.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure. \n
 */
int TCSGetVersion(TCSLIB_HANDLE hLib, TCSVerInfo *pVerInfo);

/**
 * \brief Gets the meta information about the plugin
 * \param[in] hLib instance handle obtained from a call to the TCSLibraryOpen().
 * \param[out] pszInfo string containing meta info
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure. \n
 */
int TCSGetInfo(TCSLIB_HANDLE hLib, char *pszInfo);

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
int TCSLibraryClose(TCSLIB_HANDLE hLib);

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
TCSErrorCode TCSGetLastError(TCSLIB_HANDLE hLib);

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
int TCSScanData(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult);

/**
 * \brief TCSScanDataAsync() is used to scan a data buffer for malware. The caller
 * specifies a scanner action, scan target data type, set I/O functions to access
 * the data, and a callback function for information retrieval. The result
 * of the data scanning is returned to the callback function.
 *
 * This is a Asynchronous API.
 * \param[in] hLib instance handle obtained from a call to the TCSLibraryOpen()
 * \param[in] pParam Pointer to a structure containing data scan parameters.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSScanDataAsync(TCSLIB_HANDLE hLib, TCSScanParam *pParam);

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
 * \param[in] iCompressFlag Enables or disables compression.
 * \param[out] pResult Pointer to a structure containing data scan results.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSScanFile(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                int iAction, int iCompressFlag, TCSScanResult *pResult);

/**
 * \brief TCSScanFileEx() is used to scan a file for malware. The caller specifies a
 * file name, a scanner action, and scan target data type. The scan result is
 * returned in a caller provided data structure. Extra parameters pPrivate and
 * pfCallBack are for Callback.
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
 * \param[in] iCompressFlag Enables or disables compression.
 * \param[in] pPrivate Pointer (or handle) to an application object being scanned.
 * \param[in] pfCallBack Callback function used to notify the report of file scan and also during scan in progress.
 * \param[out] pResult Pointer to a structure containing data scan results.
 *
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSScanFileEx(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                int iAction, int iCompressFlag, void *pPrivate,
                int (*pfCallBack)(void *pPrivate, int iReason, void *pParam),
                /* This callback
                 function is set by the caller to be notified to each detected malware while
                 scanning is in process. If specified (not NULL), the scan functions call the
                 specified function with the information (e.g. TCS_CB_DETECTED) for each malware
                 detected in the content/data during scanning.
                                                                                                    
                 This is a synchronous API.
                                                                                                
                 @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                 @param[in] iReason Reason of this callback.
                 @param[in] pParam The data for specified callback reason respectively.
                                                                                                
                 @return Return Type (int) 
                 The scanning process continues if the callback function returns 0. If a negative
                 value (e.g. -1) is returned, the scanning process is aborted and control is
                 returned to the caller.
                 */
                TCSScanResult *pResult);

/**
 * \brief TCSScanFileAsync() is used to scan a file for malware. The caller specifies a
 * file name, a scanner action, and scan target data type. The scan result is
 * returned to the callback function
 *
 * This is an Asynchronous API.
 *
 * \param[in] hLib instance handle obtained from a call to the TCSLibraryOpen()
 * \param[in] pszFileName Name of file to scan. The file name must include the
 * absolute path.
 * \param[in] pParam Pointer to a structure containing data scan parameters.
 * \param[in] iDataType Type of data contained in the file. This is used to
 * perform data type specific scans on files.
 * \param[in] iAction Type of scanning to perform on file.
 * \param[in] iCompressFlag enables or disables decompression of archive files.
 * \param[in] pPrivate Pointer (or handle) to an application object being scanned.
 * \param[in] pfCallBack Callback function used to notify the report of file scan and also during scan in progress.
 * \return Return Type (int) \n
 * 0 - on success. \n
 * -1 - on failure and error code is set. \n
 */
int TCSScanFileAsync(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                     int iAction, int iCompressFlag, void *pPrivate,
				     int (*pfCallBack)(void *pPrivate, int iReason, void *pParam)
				     /* This callback
				      function is set by the caller to be notified to each detected malware while
				      scanning is in process. If specified (not NULL), the scan functions call the
				      specified function with the information (e.g. TCS_CB_DETECTED) for each malware
				      detected in the content/data during scanning.
				                                                                                         
				      This is a synchronous API.
				                                                                                     
				      @param[in] pPrivate Pointer (or handle) to an application object being scanned.
				      @param[in] iReason Reason of this callback.
				      @param[in] pParam The data for specified callback reason respectively.
				                                                                                     
				      @return Return Type (int) 
				      The scanning process continues if the callback function returns 0. If a negative
				      value (e.g. -1) is returned, the scanning process is aborted and control is
				      returned to the caller.
				      */ );

#ifdef __cplusplus
}
#endif 

#endif  /* TCSIMPL_H */
