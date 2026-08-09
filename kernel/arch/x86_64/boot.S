/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Multiboot2 header + protected-mode entry + long-mode bootstrap.
 *
 * Soft deepen (ABI-stable edge docs / safe EFER):
 *   Entry contract (Multiboot2 handoff):
 *     EAX = 0x36d76289 (MB2 magic; not re-checked here - QEMU/loader path)
 *     EBX = physical pointer to Multiboot2 info structure
 *     ESP is undefined -> we install a private stack immediately
 *   Exit contract into C:
 *     System V AMD64: RDI = mb2 info phys (zero-extended from EBX)
 *     RBP = 0 (SysV frame-chain terminator for kmain)
 *     RSP = stack_top (16-byte aligned BSS region)
 *     CS/DS/ES/SS/FS/GS = kernel long-mode selectors (0x08 / 0x10)
 *   Early paging: identity-map first 4 GiB with 2 MiB PS pages so the
 *     kernel image, Multiboot tags, and low firmware tables remain reachable
 *     before higher-half / HHDM work in C. PML4/PDPT/PD live in .bss.
 *   EFER: LME + NXE. NXE early matches P-MEM-6 spirit; identity leaves do
 *     not set NX yet, so execute remains valid until vmm_harden / mprotect.
 *
 * C3 residual deepen (arch entry; security/isolation; Soft!=product):
 *   Behavior > soft lamp (serial not live until C; no stamp storm here):
 *     - cli before stack / BSS / page tables (H1: no IRQ residual at entry)
 *     - cld before rep stosb (DF=1 loader residue must not reverse wipe)
 *     - private 64 KiB stack before any clobber (H1 early stack class)
 *     - EFER.NXE early; identity leaves still RWX until vmm_harden (honesty)
 *     - mb2_info32 in .data (survives BSS zero; EBX saved before clobber)
 *   H1: IF masked; no net_eth_poll / soft inventory on this path (pre-C).
 *   H2: static asm only - no ret*angle / multi-line soft flood at entry.
 *   H3: kmain return parks hlt (entry park != thr death residual).
 *   G-AC-1: no Linux .ko load / product AC at Multiboot2 entry (never).
 *   Dual DoD A/B OPEN: long-mode land != UDX USB/NIC product close.
 *   Soft residual docs/contract only - Soft!=product; never hard-gate;
 *   no version stamp; dual MIT OR Apache-2.0; no GPL.
 *
 * Symbols consumed by the linker script / Multiboot loaders:
 *   _start              - Multiboot2 entry (32-bit)
 *   __bss_start/end     - provided by linker.ld
 * greppable: boot long-mode | Multiboot2 identity map
 * greppable: Soft!=product | G-AC-1 | C3 residual | Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 * greppable: boot: soft residual lean | arch entry residual Soft!=product
 */
#define MB2_MAGIC    0xE85250D6
#define MB2_ARCH     0 /* i386 protected mode */
#define MB2_HDRLEN   (mb2_header_end - mb2_header)
#define MB2_CHECKSUM -(MB2_MAGIC + MB2_ARCH + MB2_HDRLEN)

#define CR0_PG       (1 << 31)
#define CR0_PE       (1 << 0)
#define CR4_PAE      (1 << 5)
#define EFER_MSR     0xC0000080
#define EFER_LME     (1 << 8)
#define EFER_NXE     (1 << 11)

/* Early GDT selectors (must match gdt64 layout below). */
#define BOOT_KERN_CS 0x08
#define BOOT_KERN_DS 0x10

/* PTE flags for 2 MiB identity leaves: Present | RW | PS. */
#define PTE_P_RW_PS  0x83

.section .multiboot2, "a"
.align 8
mb2_header:
    .long MB2_MAGIC
    .long MB2_ARCH
    .long MB2_HDRLEN
    .long MB2_CHECKSUM
    /* end tag (type=0, flags=0, size=8) — Multiboot2 requires this terminator */
    .word 0
    .word 0
    .long 8
mb2_header_end:

.section .boot, "ax"
.code32
.global _start
.type _start, @function
_start:
    /* H1 residual: mask IF before stack/BSS/PT work (Soft!=product pre-C). */
    cli
    /* Multiboot2: EAX = magic, EBX = info pointer (phys). */
    mov $stack_top, %esp

    /* Save MB2 info pointer before any clobber (stosb / page-table build). */
    mov %ebx, mb2_info32

    /*
     * Clear BSS (includes page tables + boot stack body).
     * C3 residual: cld so DF=1 loader residue cannot reverse the wipe.
     */
    cld
    mov $__bss_start, %edi
    mov $__bss_end, %ecx
    sub %edi, %ecx
    xor %eax, %eax
    rep stosb

    /*
     * Identity-map first 4 GiB with 2 MiB pages:
     *   pml4[0] -> pdpt -> pd0..pd3 (each PD covers 1 GiB via 512x2 MiB).
     * Tables are page-aligned BSS; phys == virt in this early window.
     * Soft residual: leaves PTE_P|RW|PS only (no NX) until vmm_harden.
     */

    /* Zero the six page-table pages (pml4 + pdpt + 4xpd). DF still clear. */
    mov $pml4, %edi
    mov $(8 * 4096), %ecx
    xor %eax, %eax
    rep stosb

    /* pml4[0] = pdpt | PRESENT | RW */
    mov $pdpt, %eax
    or  $0x3, %eax
    mov %eax, pml4

    /* pdpt[i] = pd_i | PRESENT | RW  for i = 0..3 */
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

    /* Each PD: 512 entries of 2 MiB pages, phys base = pd_index * 1 GiB. */
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
    or  $PTE_P_RW_PS, %edx /* PRESENT | RW | PS(2MiB) */
    mov %edx, (%edi,%ebx,8)
    add $0x200000, %eax    /* +2MiB */
    inc %ebx
    cmp $512, %ebx
    jb  3b
    inc %ecx
    cmp $4, %ecx
    jb  2b

    /* Load CR3 with PML4 physical address (identity). */
    mov $pml4, %eax
    mov %eax, %cr3

    /* Enable PAE (required before long mode). */
    mov %cr4, %eax
    or  $CR4_PAE, %eax
    mov %eax, %cr4

    /*
     * IA32_EFER: LME (long mode) + NXE (execute-disable).
     * C3 residual: NXE early matches P-MEM-6; identity leaves still lack
     * NX bit so execute stays valid until vmm_harden / mprotect.
     * NXE is safe with current identity leaves (no NX bit set); later C
     * paths (cpu_syscall_init) re-assert SCE|NXE on the BSP.
     * Soft!=product: early NXE != full G-MAP / SMEP product close.
     */
    mov $EFER_MSR, %ecx
    rdmsr
    or  $(EFER_LME | EFER_NXE), %eax
    wrmsr

    /* Enable paging (+ PE already set by Multiboot environment). */
    mov %cr0, %eax
    or  $(CR0_PG | CR0_PE), %eax
    mov %eax, %cr0

    /* Load 64-bit GDT and far jump into 64-bit CS. */
    lgdt gdt64_ptr
    ljmp $BOOT_KERN_CS, $start64

.code64
start64:
    /* Long-mode data segs; IF still masked from entry cli (H1 residual). */
    mov $BOOT_KERN_DS, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    mov $stack_top, %rsp

    /* System V AMD64: first arg in RDI = mb2 info; RBP = frame terminator. */
    mov mb2_info32, %edi
    xor %rbp, %rbp
    call kmain

    /*
     * kmain is not expected to return; park safely if it does.
     * H3 residual: entry park only - not thr death / AS destroy residual.
     * Soft!=product; Dual_DoD_A=OPEN Dual_DoD_B=OPEN unchanged by land.
     */
1:
    hlt
    jmp 1b

.size _start, . - _start

.section .bss
/* 64 KiB boot stack — deep kmain/smoke used to sit near 16 KiB. */
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
    /* kernel code 0x08: 64-bit (L=1, D=0, P=1, S=1, exec/read) */
    .quad 0x00AF9A000000FFFF
    /* kernel data 0x10: 64-bit data (P=1, S=1, RW) */
    .quad 0x00AF92000000FFFF
gdt64_end:

gdt64_ptr:
    .word gdt64_end - gdt64 - 1
    .quad gdt64

.section .note.GNU-stack,"",@progbits
