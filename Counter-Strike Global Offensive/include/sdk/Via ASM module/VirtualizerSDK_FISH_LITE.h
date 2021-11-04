/******************************************************************************
 Header: VirtualizerSDK_FISH_LITE.h
 Description: Definition of CustomVM macros

 Author/s: Oreans Technologies 
 (c) 2014 Oreans Technologies

 --- File generated automatically from Oreans VM Generator (2/4/2014) ---
******************************************************************************/


// ****************************************************************************
// Declaration of Custom VM macros
// ****************************************************************************

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(PLATFORM_X32)

void __stdcall VIRTUALIZER_FISH_LITE_START_ASM32();
void __stdcall VIRTUALIZER_FISH_LITE_END_ASM32();

#define VIRTUALIZER_FISH_LITE_START VIRTUALIZER_FISH_LITE_START_ASM32();
#define VIRTUALIZER_FISH_LITE_END VIRTUALIZER_FISH_LITE_END_ASM32();

#endif

#if defined(PLATFORM_X64)

void __stdcall VIRTUALIZER_FISH_LITE_START_ASM64();
void __stdcall VIRTUALIZER_FISH_LITE_END_ASM64();

#define VIRTUALIZER_FISH_LITE_START VIRTUALIZER_FISH_LITE_START_ASM64();
#define VIRTUALIZER_FISH_LITE_END VIRTUALIZER_FISH_LITE_END_ASM64();

#endif

#ifdef __cplusplus
}
#endif

