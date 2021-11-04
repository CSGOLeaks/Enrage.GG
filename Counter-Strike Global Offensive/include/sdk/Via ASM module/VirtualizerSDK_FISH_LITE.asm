; ****************************************************************************
; Module: VirtualizerSDK_FISH_LITE.asm
; Description: Another way to link with the SecureEngine SDK via an ASM module
;
; Author/s: Oreans Technologies 
; (c) 2014 Oreans Technologies
;
; --- File generated automatically from Oreans VM Generator (2/4/2014) ---
; ****************************************************************************


IFDEF RAX

ELSE

.586
.model flat,stdcall
option casemap:none

ENDIF


; ****************************************************************************
;                                 Constants
; ****************************************************************************

.CONST


; ****************************************************************************
;                               Data Segment
; ****************************************************************************

.DATA


; ****************************************************************************
;                               Code Segment
; ****************************************************************************

.CODE

IFDEF RAX

; ****************************************************************************
; VIRTUALIZER_FISH_LITE definition
; ****************************************************************************

VIRTUALIZER_FISH_LITE_START_ASM64 PROC

    push    rax
    push    rbx
    push    rcx

    mov     eax, 'CV'
    mov     ebx, 129
    mov     ecx, 'CV'
    add     ebx, eax
    add     ecx, eax

    pop     rcx
    pop     rbx
    pop     rax
    ret

VIRTUALIZER_FISH_LITE_START_ASM64 ENDP

VIRTUALIZER_FISH_LITE_END_ASM64 PROC

    push    rax
    push    rbx
    push    rcx

    mov     eax, 'CV'
    mov     ebx, 529
    mov     ecx, 'CV'
    add     ebx, eax
    add     ecx, eax

    pop     rcx
    pop     rbx
    pop     rax
    ret

VIRTUALIZER_FISH_LITE_END_ASM64 ENDP

ELSE

; ****************************************************************************
; VIRTUALIZER_FISH_LITE definition
; ****************************************************************************

VIRTUALIZER_FISH_LITE_START_ASM32 PROC

    push    eax
    push    ebx
    push    ecx

    mov     eax, 'CV'
    mov     ebx, 128
    mov     ecx, 'CV'
    add     ebx, eax
    add     ecx, eax

    pop     ecx
    pop     ebx
    pop     eax
    ret

VIRTUALIZER_FISH_LITE_START_ASM32 ENDP

VIRTUALIZER_FISH_LITE_END_ASM32 PROC

    push    eax
    push    ebx
    push    ecx

    mov     eax, 'CV'
    mov     ebx, 528
    mov     ecx, 'CV'
    add     ebx, eax
    add     ecx, eax

    pop     ecx
    pop     ebx
    pop     eax
    ret

VIRTUALIZER_FISH_LITE_END_ASM32 ENDP

ENDIF

END
