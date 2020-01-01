/*
 * Copyright (c) 2020, Devin Nakamura
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

OUTPUT_FORMAT("elf32-i386")
ENTRY(_multibootEntry)

KERNEL_VIRTUAL_BASE = 0xC0000000;
KERNEL_LOAD_ADDRESS = 0x00100000;
KERNEL_OFFSET = (KERNEL_VIRTUAL_BASE - KERNEL_LOAD_ADDRESS);
KERNEL_UNPAGED_BASE = 0xFFFF << 4;

SECTIONS
{
    . = KERNEL_LOAD_ADDRESS;

	.unpaged_seg : {
        *(.header)
         ASSERT((. > 1), "Malformed multiboot header")
        *(.unpaged_text);
        *(.unpaged_data);
        _unpaged_bss_start = .;
        *(.unpaged_bss);
        _unpaged_bss_end = .;
    }

    /*. += KERNEL_OFFSET;*/
    . += KERNEL_VIRTUAL_BASE;

    .text ALIGN(0x1000): AT(ADDR(.text) - KERNEL_VIRTUAL_BASE)
	{

        *(.text);
        *(.gnu.linkonce.t*);
        /*. = ALIGN(0x1000);*/
    }

    .rodata ALIGN(0x1000) : /*AT(ADDR(.rodata) - KERNEL_OFFSET)*/
    {
        start_ctors = .;
        *(SORT(.ctors*))  /* Note the "SORT" */
        end_ctors = .;

        start_dtors = .;
        *(SORT(.dtors*))
        end_dtors = .;

        *(.rodata*);
        *(.gnu.linkonce.r*);

    }
    .data ALIGN(0x1000) :
    {
        *(.data);
        *(.gnu.linkonce.d*);
        . = ALIGN(0x1000);
    }
    end_load = LOADADDR(.bss);
    .bss :
    {
        sbss = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        ebss = . - KERNEL_VIRTUAL_BASE;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame) /* c++ exception stuff  */
    }
}
