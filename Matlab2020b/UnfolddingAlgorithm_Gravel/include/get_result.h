//
// MATLAB Compiler: 8.1 (R2020b)
// Date: Tue Jul 15 11:17:59 2025
// Arguments:
// "-B""macro_default""-W""cpplib:get_result,all,version=1.0""-T""link:lib""-d""
// F:\????????\??????????\3-??????????\?????????\get_result\for_testing""-v""F:\
// ????????\??????????\3-??????????\?????????\get_result.m""F:\????????\????????
// ??\3-??????????\?????????\ReadData.m"
//

#ifndef get_result_h
#define get_result_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_get_result_C_API 
#define LIB_get_result_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_get_result_C_API 
bool MW_CALL_CONV get_resultInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_get_result_C_API 
bool MW_CALL_CONV get_resultInitialize(void);

extern LIB_get_result_C_API 
void MW_CALL_CONV get_resultTerminate(void);

extern LIB_get_result_C_API 
void MW_CALL_CONV get_resultPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_get_result_C_API 
bool MW_CALL_CONV mlxGet_result(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_get_result_C_API 
bool MW_CALL_CONV mlxReadData(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_get_result
#define PUBLIC_get_result_CPP_API __declspec(dllexport)
#else
#define PUBLIC_get_result_CPP_API __declspec(dllimport)
#endif

#define LIB_get_result_CPP_API PUBLIC_get_result_CPP_API

#else

#if !defined(LIB_get_result_CPP_API)
#if defined(LIB_get_result_C_API)
#define LIB_get_result_CPP_API LIB_get_result_C_API
#else
#define LIB_get_result_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_get_result_CPP_API void MW_CALL_CONV get_result(int nargout, mwArray& N, mwArray& Time_fit, mwArray& Peak_Count, mwArray& R, mwArray& residuals, \
    const mwArray& SpecData, const mwArray& SpecTime, const mwArray& Time_Bin, const mwArray& start_time, const mwArray& end_time, const mwArray& Energy_Scale);

extern LIB_get_result_CPP_API void MW_CALL_CONV ReadData(int nargout, mwArray& spec_data, mwArray& spec_time, const mwArray& file_name, const mwArray& start_time, const mwArray& num);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
