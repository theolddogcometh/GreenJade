/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Real-mode AP trampoline — copied to physical 0x8000 (SIPI vector 8).
 * Bootstrap info at 0x7000 (struct gj_ap_boot_info, see smp.c).
 *
 * Soft deepen (ABI-stable bring-up contract — smp_start_aps must match):
 *
 *   Layout of copied block (phys 0x8000 + offset):
 *     +0x00  16-bit entry (SIPI target, vector 8 → 0x8000)
 *     private GDT / GDTR embedded via labels (not BSP GDT — low identity)
 *     long64 parks with hlt if entry returns
 *
 *   gj_ap_boot_info @ 0x7000 (packed; offsets must match smp.c):
 *     +0   gdtr placeholder (unused when trampoline embeds GDT)
 *     +16  u64 cr3     — BSP CR3 (PML4 must be < 4 GiB for 32-bit load)
 *     +24  u64 entry   — 64-bit C entry (e.g. ap long-mode continue)
 *     +32  u64 stack   — stack top for this AP
 *     +40  u32 cpu     — cpu index (passed in EDI, System V first arg)
 *
 *   Mode ladder: real → prot32 (CS=0x08) → long64 (CS=0x18).
 *   Temporary GDT selectors:
 *     0x08  32-bit code    0x10  32-bit data
 *     0x18  64-bit code    0x20  64-bit data
 *
 *   EFER: LME + NXE. APs load the BSP page tables which may already have
 *   NX leaves after vmm_harden / mprotect; NXE=0 would #PF reserved-bit
 *   on those PTEs (Intel SDM). Soft-safe alignment with BSP cpu_syscall_init.
 *
 * Grep: ap_trampoline, smp: bringup soft
 */
.section .rodata
.global ap_trampoline_start
.global ap_trampoline_end
.global ap_trampoline_prot32
.global ap_trampoline_long64

/* Offsets into gj_ap_boot_info at 0x7000 — must match smp.c */
.set AP_BOOT_BASE,   0x7000
.set AP_OFF_GDTR,    0      /* gdtr (10 bytes) filled by BSP; unused here */
.set AP_OFF_CR3,     16     /* u64 cr3 */
.set AP_OFF_ENTRY,   24     /* u64 64-bit entry */
.set AP_OFF_STACK,   32     /* u64 stack top */
.set AP_OFF_CPU,     40     /* u32 cpu index */

/* Phys load address of this trampoline (SIPI vector << 12). */
.set AP_TRAMP_PA,    0x8000

#define EFER_MSR     0xC0000080
#define EFER_LME     (1 << 8)
#define EFER_NXE     (1 << 11)
#define CR4_PAE      (1 << 5)
#define CR0_PG       (1 << 31)
#define CR0_PE       (1 << 0)

ap_trampoline_start:
    .code16
    cli
    cld
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    /* Tiny real-mode stack in the boot-info page (below trampoline). */
    mov $0x7000, %sp

    /* Load temporary GDT (32-bit + 64-bit CS) embedded in trampoline. */
    lgdtl ap_tmp_gdtr - ap_trampoline_start + AP_TRAMP_PA

    /* Enter protected mode (PE only; paging still off). */
    mov %cr0, %eax
    or $CR0_PE, %eax
    mov %eax, %cr0
    ljmpl $0x08, $ap_trampoline_prot32 - ap_trampoline_start + AP_TRAMP_PA

    .code32
ap_trampoline_prot32:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    /* CR4.PAE — required before long mode. */
    mov %cr4, %eax
    or $CR4_PAE, %eax
    mov %eax, %cr4

    /*
     * CR3 from boot info (low 32 bits). Early kernel PML4 lives in identity
     * low RAM; if CR3 ever moves above 4 GiB this load must widen carefully
     * (still 32-bit protected mode here).
     */
    mov AP_BOOT_BASE + AP_OFF_CR3, %eax
    mov %eax, %cr3

    /* EFER.LME | EFER.NXE — match BSP W^X page tables (see file header). */
    mov $EFER_MSR, %ecx
    rdmsr
    or $(EFER_LME | EFER_NXE), %eax
    wrmsr

    /* CR0.PG + PE → activate long mode (compatibility, then far jump). */
    mov %cr0, %eax
    or $(CR0_PG | CR0_PE), %eax
    mov %eax, %cr0

    ljmpl $0x18, $ap_trampoline_long64 - ap_trampoline_start + AP_TRAMP_PA

    .code64
ap_trampoline_long64:
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    movabs $AP_BOOT_BASE, %rbx
    mov AP_OFF_STACK(%rbx), %rsp   /* stack top */
    mov AP_OFF_ENTRY(%rbx), %rax   /* 64-bit entry */
    mov AP_OFF_CPU(%rbx), %edi     /* cpu index arg (System V) */
    and $~0xfull, %rsp             /* 16-byte align before call */
    xor %rbp, %rbp                 /* SysV frame terminator */
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
    .long ap_tmp_gdt - ap_trampoline_start + AP_TRAMP_PA

ap_trampoline_end:

.section .note.GNU-stack,"",@progbits
