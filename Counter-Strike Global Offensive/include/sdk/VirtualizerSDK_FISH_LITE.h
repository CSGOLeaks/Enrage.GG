/******************************************************************************
 * Header: VirtualizerSDK_FISH_LITE.h
 * Description:  Definitions for Private VM in SecureEngine
 *
 * Author/s: Oreans Technologies 
 * (c) 2014 Oreans Technologies
 *
 * --- File generated automatically from Oreans VM Generator (2/4/2014) ---
 ******************************************************************************/

// ***********************************************
// Definition of macros as function names
// ***********************************************

 #ifdef __cplusplus
  extern "C" {
 #endif

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000128_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000128_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000129_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000129_End(void);

#ifdef __cplusplus
}
#endif


// ***********************************************
// x64 definition as function names
// ***********************************************

#if defined(PLATFORM_X64) && !defined(CV_X64_INSERT_VIA_INLINE)

#define VIRTUALIZER_FISH_LITE_START CustomVM00000129_Start();
#define VIRTUALIZER_FISH_LITE_END CustomVM00000129_End();

#define CV_CUSTOM_VMS_DEFINED

#endif 


// ***********************************************
// x32 definition as function names
// ***********************************************

#if defined(PLATFORM_X32) && !defined(CV_X32_INSERT_VIA_INLINE)

#define VIRTUALIZER_FISH_LITE_START CustomVM00000128_Start();
#define VIRTUALIZER_FISH_LITE_END CustomVM00000128_End();

#define CV_CUSTOM_VMS_DEFINED

#endif 


// ***********************************************
// x32/x64 definition as inline assembly
// ***********************************************

#ifndef CV_CUSTOM_VMS_DEFINED

#ifdef __BORLANDC__
  #include "VirtualizerSDK_BorlandC_inline_FISH_LITE.h"
#endif

#ifdef __GNUC__
  #include "VirtualizerSDK_GNU_inline_FISH_LITE.h"
#endif

#ifdef __ICL
  #include "VirtualizerSDK_ICL_inline_FISH_LITE.h"
#endif

#ifdef __LCC__
  #include "VirtualizerSDK_LCC_inline_FISH_LITE.h"
#endif

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
  #include "VirtualizerSDK_VC_inline_FISH_LITE.h"
#endif

#endif
