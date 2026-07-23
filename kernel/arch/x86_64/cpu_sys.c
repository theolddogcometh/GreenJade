/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL/SYSRET bring-up (STAR/LSTAR/SFMASK/EFER.SCE) and first-entry
 * helpers into ring-3 long mode or 32-bit compat (iretq).
 */
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/thread.h>

#define MSR_EFER     0xC0000080u
#define MSR_STAR     0xC0000081u
#define MSR_LSTAR    0xC0000082u
#define MSR_CSTAR    0xC0000083u
#define MSR_SFMASK   0xC0000084u

#define EFER_SCE     (1ull << 0)
#define EFER_LME     (1ull << 8)
#define EFER_NXE     (1ull << 11)

/* RFLAGS bits cleared on SYSCALL (IF, TF, DF, AC, … — SDM SFMASK). */
#define SFMASK_DEFAULT 0x257fdull

static int g_fSyscallReady;

extern void gj_syscall_entry(void);

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo;
    u32 u32Hi;

    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);

    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
}

void
cpu_syscall_init(void)
{
    u64 u64Efer;
    u64 u64Star;
    u64 u64Lstar;

    /*
     * STAR: kernel CS in [47:32], user selector base in [63:48].
     * SYSRETQ: CS = base+16 = 0x28 (user long code), SS = base+8 = 0x20.
     * GDT layout: index5=0x28 code, index4=0x20 data → base 0x18.
     */
    u64Star = ((u64)0x18 << 48) | ((u64)GJ_GDT_KERNEL_CS << 32);
    u64Lstar = (u64)(gj_vaddr_t)gj_syscall_entry;

    wrmsr(MSR_STAR, u64Star);
    wrmsr(MSR_LSTAR, u64Lstar);
    wrmsr(MSR_CSTAR, u64Lstar); /* CSTAR unused (no 32-bit SYSCALL path) */
    wrmsr(MSR_SFMASK, SFMASK_DEFAULT);

    u64Efer = rdmsr(MSR_EFER);
    u64Efer |= EFER_SCE | EFER_NXE; /* NXE backs W^X mprotect */
    wrmsr(MSR_EFER, u64Efer);

    g_fSyscallReady = 1;
    kprintf("cpu: SYSCALL ready LSTAR=0x%lx STAR=0x%lx\n",
            (unsigned long)u64Lstar, (unsigned long)u64Star);
    (void)EFER_LME;
    (void)EFER_NXE;
}

int
cpu_syscall_ready(void)
{
    return g_fSyscallReady;
}

void
cpu_enter_user(u64 u64Entry, u64 u64Stack)
{
    struct gj_thread *pThr;

    if (!g_fSyscallReady || u64Entry == 0 || u64Stack == 0) {
        kprintf("cpu: enter_user invalid (ready=%d)\n", g_fSyscallReady);
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    pThr = thread_current();
    if (pThr != NULL) {
        thread_install_kstack(pThr);
    } else {
        tss_use_irq_rsp0();
    }
    /*
     * Kernel GSBASE holds per-CPU state. swapgs → GS=user(0), KERNEL_GS=percpu.
     * sysretq: rcx=rip, r11=rflags, rsp=user stack.
     */
    __asm__ volatile (
        "swapgs\n\t"
        "mov %0, %%rsp\n\t"
        "mov $0x200, %%r11\n\t" /* IF */
        "mov %1, %%rcx\n\t"
        "sysretq\n\t"
        :
        : "r"(u64Stack), "r"(u64Entry)
        : "rcx", "r11", "memory"
    );
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void
cpu_enter_user32(u64 u64Entry, u64 u64Stack)
{
    struct gj_thread *pThr;
    u64 u64Rsp0;

    /*
     * Long mode → 32-bit compat user via iretq:
     *   SS=USER_DS, RSP=stack, RFLAGS=IF, CS=USER_CS32, RIP=entry
     * Interrupt-return path only; SYSCALL MSRs are not required.
     */
    if (u64Entry == 0 || u64Stack == 0) {
        kprintf("cpu: enter_user32 invalid\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    pThr = thread_current();
    if (pThr != NULL && pThr->u64KstackTop != 0) {
        /* Per-thread kstack for SYSCALL; ring-3 IRQs keep dedicated RSP0. */
        thread_install_kstack(pThr);
    }
    /*
     * Never pin TSS.RSP0 to a thread kstack: if that thread parks mid-syscall,
     * a timer IRQ from another thread smashes its saved frames → kernel #UD.
     */
    tss_use_irq_rsp0();
    u64Rsp0 = tss_irq_rsp0();
    if (u64Rsp0 == 0) {
        u64Rsp0 = tss_get_rsp0();
    }
    if (u64Rsp0 == 0) {
        kprintf("cpu: enter_user32 no RSP0\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    kprintf("cpu: enter_user32 rip=0x%lx rsp=0x%lx cs32=0x1b rsp0=0x%lx\n",
            (unsigned long)u64Entry, (unsigned long)u64Stack,
            (unsigned long)u64Rsp0);
    __asm__ volatile(
        "cli\n\t"
        "mov $0x23, %%ax\n\t" /* USER_DS */
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "pushq $0x23\n\t"     /* SS */
        "pushq %[rsp]\n\t"    /* RSP */
        "pushq $0x200\n\t"    /* RFLAGS IF */
        "pushq $0x1b\n\t"     /* CS32 */
        "pushq %[rip]\n\t"    /* RIP */
        "iretq\n\t"
        :
        : [rsp] "r"(u64Stack), [rip] "r"(u64Entry)
        : "rax", "memory");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
