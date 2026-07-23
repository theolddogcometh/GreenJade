/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Real-mode AP trampoline — copied to physical 0x8000 (SIPI vector 8).
 * Bootstrap info at 0x7000 (struct gj_ap_boot_info, see smp.c).
 *
 * Layout of copied block:
 *   +0x00  16-bit entry (SIPI target)
 *   private GDT / GDTR embedded via labels
 */
.section .rodata
.global ap_trampoline_start
.global ap_trampoline_end
.global ap_trampoline_prot32
.global ap_trampoline_long64

/* Offsets into gj_ap_boot_info at 0x7000 — must match smp.c */
.set AP_BOOT_BASE,   0x7000
.set AP_OFF_GDTR,    0      /* gdtr (10 bytes) filled by BSP */
.set AP_OFF_CR3,     16     /* u64 cr3 */
.set AP_OFF_ENTRY,   24     /* u64 64-bit entry */
.set AP_OFF_STACK,   32     /* u64 stack top */
.set AP_OFF_CPU,     40     /* u32 cpu index */

ap_trampoline_start:
    .code16
    cli
    cld
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov $0x7000, %sp

    /* Load temporary GDT (32-bit + 64-bit CS) embedded in trampoline */
    lgdtl ap_tmp_gdtr - ap_trampoline_start + 0x8000

    /* Enter protected mode */
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0
    ljmpl $0x08, $ap_trampoline_prot32 - ap_trampoline_start + 0x8000

    .code32
ap_trampoline_prot32:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    /* CR4.PAE */
    mov %cr4, %eax
    or $(1 << 5), %eax
    mov %eax, %cr4

    /* CR3 from boot info */
    mov AP_BOOT_BASE + AP_OFF_CR3, %eax
    mov %eax, %cr3

    /* EFER.LME */
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr

    /* CR0.PG + PE */
    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    ljmpl $0x18, $ap_trampoline_long64 - ap_trampoline_start + 0x8000

    .code64
ap_trampoline_long64:
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    movabs $AP_BOOT_BASE, %rbx
    mov 32(%rbx), %rsp          /* stack */
    mov 24(%rbx), %rax          /* entry */
    mov 40(%rbx), %edi          /* cpu index arg */
    and $~0xfull, %rsp
    call *%rax
1:
    hlt
    jmp 1b

    .align 16
ap_tmp_gdt:
    .quad 0x0000000000000000          /* 0: null */
    .quad 0x00cf9a000000ffff          /* 1: 32-bit code 0x08 */
    .quad 0x00cf92000000ffff          /* 2: 32-bit data 0x10 */
    .quad 0x00af9a000000ffff          /* 3: 64-bit code 0x18 */
    .quad 0x00cf92000000ffff          /* 4: 64-bit data 0x20 */
ap_tmp_gdtr:
    .word ap_tmp_gdtr - ap_tmp_gdt - 1
    .long ap_tmp_gdt - ap_trampoline_start + 0x8000

ap_trampoline_end:

.section .note.GNU-stack,"",@progbits
