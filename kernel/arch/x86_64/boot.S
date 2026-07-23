/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Multiboot2 header + protected-mode entry + long-mode bootstrap.
 */
#define MB2_MAGIC    0xE85250D6
#define MB2_ARCH     0
#define MB2_HDRLEN   (mb2_header_end - mb2_header)
#define MB2_CHECKSUM -(MB2_MAGIC + MB2_ARCH + MB2_HDRLEN)

#define CR0_PG       (1 << 31)
#define CR0_PE       (1 << 0)
#define CR4_PAE      (1 << 5)
#define EFER_MSR     0xC0000080
#define EFER_LME     (1 << 8)
#define EFER_NXE     (1 << 11)

.section .multiboot2, "a"
.align 8
mb2_header:
    .long MB2_MAGIC
    .long MB2_ARCH
    .long MB2_HDRLEN
    .long MB2_CHECKSUM
    /* end tag */
    .word 0
    .word 0
    .long 8
mb2_header_end:

.section .boot, "ax"
.code32
.global _start
.type _start, @function
_start:
    cli
    /* Multiboot2: EAX = magic, EBX = info pointer */
    mov $stack_top, %esp

    /* Save MB2 info pointer */
    mov %ebx, mb2_info32

    /* Clear BSS */
    mov $__bss_start, %edi
    mov $__bss_end, %ecx
    sub %edi, %ecx
    xor %eax, %eax
    rep stosb

    /* Identity-map first 4GiB with 2MiB pages: PML4[0] -> PDPT -> PD */
    /* pml4, pdpt, pd0..pd3 in bss, page-aligned */

    /* zero page tables */
    mov $pml4, %edi
    mov $(8 * 4096), %ecx   /* pml4+pdpt+4*pd */
    xor %eax, %eax
    rep stosb

    /* pml4[0] = pdpt | PRESENT | RW */
    mov $pdpt, %eax
    or  $0x3, %eax
    mov %eax, pml4

    /* pdpt[0..3] = pdN | PRESENT | RW */
    mov $0, %ecx
1:
    mov $pd0, %eax
    mov %ecx, %edx
    shl $12, %edx          /* edx = ecx * 4096 */
    add %edx, %eax
    or  $0x3, %eax
    mov %eax, pdpt(,%ecx,8)
    inc %ecx
    cmp $4, %ecx
    jb  1b

    /* Each PD: 512 entries of 2MiB pages */
    mov $0, %ecx           /* pd index 0..3 */
2:
    mov $pd0, %edi
    mov %ecx, %edx
    shl $12, %edx
    add %edx, %edi         /* edi = &pdN */
    mov $0, %ebx           /* entry index */
    mov %ecx, %eax
    shl $30, %eax          /* phys base = ecx * 1GiB */
3:
    mov %eax, %edx
    or  $0x83, %edx        /* PRESENT | RW | PS(2MiB) */
    mov %edx, (%edi,%ebx,8)
    add $0x200000, %eax    /* +2MiB */
    inc %ebx
    cmp $512, %ebx
    jb  3b
    inc %ecx
    cmp $4, %ecx
    jb  2b

    /* Load CR3 */
    mov $pml4, %eax
    mov %eax, %cr3

    /* Enable PAE */
    mov %cr4, %eax
    or  $CR4_PAE, %eax
    mov %eax, %cr4

    /* Enable LME in EFER */
    mov $EFER_MSR, %ecx
    rdmsr
    or  $EFER_LME, %eax
    wrmsr

    /* Enable paging */
    mov %cr0, %eax
    or  $(CR0_PG | CR0_PE), %eax
    mov %eax, %cr0

    /* Load 64-bit GDT and far jump */
    lgdt gdt64_ptr
    ljmp $0x08, $start64

.code64
start64:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    mov $stack_top, %rsp

    /* System V AMD64: first arg in RDI = mb2 info */
    mov mb2_info32, %edi
    xor %rbp, %rbp
    call kmain

1:
    hlt
    jmp 1b

.section .bss
/* 64 KiB boot stack — deep kmain/smoke used to sit near 16 KiB. */
.align 16
.skip 65536
stack_top:

.align 4096
pml4:
    .skip 4096
pdpt:
    .skip 4096
pd0:
    .skip 4096
pd1:
    .skip 4096
pd2:
    .skip 4096
pd3:
    .skip 4096

.section .data
.align 4
mb2_info32:
    .long 0

.align 8
gdt64:
    .quad 0
    /* kernel code 0x08: 64-bit */
    .quad 0x00AF9A000000FFFF
    /* kernel data 0x10 */
    .quad 0x00AF92000000FFFF
gdt64_end:

gdt64_ptr:
    .word gdt64_end - gdt64 - 1
    .quad gdt64
