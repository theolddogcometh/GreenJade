/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared ELF for objcopy → pei-x86-64 EFI (gnu-efi style). Pure linker script.
 */
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(efi_main)
SECTIONS
{
    . = 0;
    ImageBase = .;
    .hash : { *(.hash) }
    . = ALIGN(4096);
    .text : {
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    }
    . = ALIGN(4096);
    .sdata : {
        *(.got.plt)
        *(.got)
        *(.srodata)
        *(.sdata .sdata.*)
        *(.sbss .sbss.*)
    }
    . = ALIGN(4096);
    .data : {
        *(.rodata .rodata.*)
        *(.data .data.*)
    }
    . = ALIGN(4096);
    .dynamic : { *(.dynamic) }
    . = ALIGN(4096);
    .rela : { *(.rela .rela.*) }
    . = ALIGN(4096);
    .reloc : { *(.reloc) }
    . = ALIGN(4096);
    .dynsym : { *(.dynsym) }
    . = ALIGN(4096);
    .dynstr : { *(.dynstr) }
    . = ALIGN(4096);
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
    /DISCARD/ : {
        *(.note .note.*)
        *(.comment)
        *(.eh_frame*)
        *(.debug*)
    }
}
