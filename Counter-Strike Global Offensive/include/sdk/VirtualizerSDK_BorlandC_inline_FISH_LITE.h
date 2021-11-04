/******************************************************************************
 * Header: VirtualizerSDK_BorlandC_inline_FISH_LITE.h
 * Description: Borland C++ inline assembly macros definitions
 *
 * Author/s: Oreans Technologies 
 * (c) 2014 Oreans Technologies
 *
 * --- File generated automatically from Oreans VM Generator (2/4/2014) ---
 ******************************************************************************/

/***********************************************
 * Definition as inline assembly
 ***********************************************/

#ifdef PLATFORM_X32

#ifndef VIRTUALIZER_FISH_LITE_START
#define VIRTUALIZER_FISH_LITE_START                        __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x80, 0x00, 0x00, 0x00, \
                                                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);
#endif

#ifndef VIRTUALIZER_FISH_LITE_END
#define VIRTUALIZER_FISH_LITE_END                          __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x10, 0x02, 0x00, 0x00, \
                                                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);
#endif

#endif

#ifdef PLATFORM_X64

#ifndef VIRTUALIZER_FISH_LITE_START
#define VIRTUALIZER_FISH_LITE_START                        __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x81, 0x00, 0x00, 0x00, \
                                                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);
#endif

#ifndef VIRTUALIZER_FISH_LITE_END
#define VIRTUALIZER_FISH_LITE_END                          __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x11, 0x02, 0x00, 0x00, \
                                                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);
#endif

#endif

