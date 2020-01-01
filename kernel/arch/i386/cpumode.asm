;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (c) 2018, Devin Nakamura
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice, this
;    list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; SPDX-License-Identifier: BSD-2-Clause
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

REAL_MODE_DATA_SEG equ 4 << 3
REAL_MODE_CODE_SEG equ 3 << 3

REAL_MODE_STACK_BASE equ 0x2000
extern KERNEL_UNPAGED_BASE

SEGMENT .unpaged_text

global protToReal

protToReal:
    cli
    ;get return address and put on real mode stack

    sidt [prot_idt]
    o16 lidt [real_idt]

    sgdt [saved_gdtr]

    mov eax, cr3
    mov [saved_page_dir], eax

    ;set segment registers
    mov ax, REAL_MODE_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp REAL_MODE_CODE_SEG:.temp_real_code

.temp_real_code:
    [bits 16]
    ;clear p-mode and paging flags
    mov eax, cr0
    and eax, 0x7FFFFFFF ; disable paging bit
    mov cr0, eax
    and al, 0xFE ;PE flag is bit 0
    mov cr0, eax
    jmp dword 0xFFFF:(.real_mode - (0xFFFF * 16))

.real_mode:
    ; Calculate the return address
    ; Note: we will fail horribly if it is outside the addressable range
    mov eax, [esp]
    add esp, 4
    sub eax, KERNEL_UNPAGED_BASE
    jmp ax

global realToProt
realToProt:
    cli
    mov ecx, KERNEL_UNPAGED_BASE
    mov eax, saved_gdtr
    sub eax, ecx
    shr ecx, 4
    mov ds, ecx
    ; Note lgdt [X] is implictly lgdt [DS:x]
    lgdt [eax]

    ; Enable protected mode
    mov eax, cr0
    or al, 1 ;set p-mode bit
    mov cr0, eax

    ;save the old cs value, needed later to calculate return address
    mov ecx, cs

    ;we need to replace the CS descriptor cached value with one from
    ;the GDT
    jmp dword 08h:.pmode_start


.pmode_start:
    [Bits 32]

    ;lets update all the data segment descriptors
    mov AX, 0x10 ;ie the 3rd gdt entry
    mov DS, AX
    mov ES, AX
    mov FS, AX
    mov GS, AX

    ; recalculate stack pointer
    mov eax, ss
    shl eax, 4
    add esp, eax

    ; restore stack segment
    mov ax, 0x10
    mov ss, ax

    ;Load the page directory
    mov eax, [saved_page_dir]
    mov cr3, eax

    ;enable paging
    mov eax, cr0
    or eax, 0x80000000 ; set paging bit
    mov cr0, eax

    ;restore idt
    lidt [prot_idt]

    ;calculate the return address
    xor eax,eax
    pop ax
    shl ecx, 4
    add eax, ecx
    jmp eax ; return

section .unpaged_data
saved_page_dir: dd 0

real_idt:
    dw 0x3FF
    dd 0

prot_idt:
    dw 0
    dd 0

saved_gdtr:
    dw 0
    dd 0
