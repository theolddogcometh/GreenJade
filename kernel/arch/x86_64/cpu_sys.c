/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL/SYSRET bring-up (STAR/LSTAR/SFMASK/EFER.SCE) and first-entry
 * helpers into ring-3 long mode or 32-bit compat (iretq).
 *
 * Soft SYSCALL MSR observability: program path snapshots STAR/LSTAR/
 * CSTAR/SFMASK/EFER, readback-verifies SCE + STAR layout, and exposes
 * greppable counters for product/smoke inventory (no hot-path locks).
 */
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/string.h>
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

/* Soft SYSCALL MSR observability (monotonic; no locks — BSP soft). */
static volatile u32 g_u32SoftInits;
static volatile u32 g_u32SoftVerifyOk;
static volatile u32 g_u32SoftVerifyBad;
static volatile u32 g_u32SoftEnter64;
static volatile u32 g_u32SoftEnter32;
static volatile u32 g_u32SoftEnterBad;
static struct gj_cpu_syscall_soft g_SoftSnap;
static int g_fSoftSnapLive;

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

/**
 * Soft: snapshot programmed MSR values + STAR decode. Pure observability.
 */
static void
cpu_syscall_soft_note_program(u64 u64Star, u64 u64Lstar, u64 u64Cstar,
                              u64 u64Sfmask, u64 u64Efer)
{
    g_SoftSnap.u64Star = u64Star;
    g_SoftSnap.u64Lstar = u64Lstar;
    g_SoftSnap.u64Cstar = u64Cstar;
    g_SoftSnap.u64Sfmask = u64Sfmask;
    g_SoftSnap.u64Efer = u64Efer;
    g_SoftSnap.u16StarKernCs = (u16)((u64Star >> 32) & 0xffffu);
    g_SoftSnap.u16StarUserBase = (u16)((u64Star >> 48) & 0xffffu);
    g_SoftSnap.u8Sce = (u8)((u64Efer & EFER_SCE) != 0);
    g_SoftSnap.u8Nxe = (u8)((u64Efer & EFER_NXE) != 0);
    g_SoftSnap.u8Pad = 0;
    g_fSoftSnapLive = 1;
    g_u32SoftInits++;
}

/**
 * Soft: rdmsr verify against last program snapshot + expected layout.
 * Does not reprogram MSRs.
 */
static int
cpu_syscall_soft_verify_inner(void)
{
    u64 u64Star;
    u64 u64Lstar;
    u64 u64Cstar;
    u64 u64Sfmask;
    u64 u64Efer;
    u16 u16KernCs;
    u16 u16UserBase;
    int fOk = 1;

    if (!g_fSoftSnapLive || !g_fSyscallReady) {
        return 0;
    }

    u64Star = rdmsr(MSR_STAR);
    u64Lstar = rdmsr(MSR_LSTAR);
    u64Cstar = rdmsr(MSR_CSTAR);
    u64Sfmask = rdmsr(MSR_SFMASK);
    u64Efer = rdmsr(MSR_EFER);

    if (u64Star != g_SoftSnap.u64Star) {
        fOk = 0;
    }
    if (u64Lstar != g_SoftSnap.u64Lstar) {
        fOk = 0;
    }
    if (u64Cstar != g_SoftSnap.u64Cstar) {
        fOk = 0;
    }
    if (u64Sfmask != g_SoftSnap.u64Sfmask) {
        fOk = 0;
    }
    if ((u64Efer & EFER_SCE) == 0) {
        fOk = 0;
    }

    u16KernCs = (u16)((u64Star >> 32) & 0xffffu);
    u16UserBase = (u16)((u64Star >> 48) & 0xffffu);
    if (u16KernCs != (u16)GJ_CPU_SYSCALL_STAR_KERNEL_CS) {
        fOk = 0;
    }
    if (u16UserBase != (u16)GJ_CPU_SYSCALL_STAR_USER_BASE) {
        fOk = 0;
    }
    /* SYSRETQ CS = user_base+16 → 0x28 raw; RPL applied by SYSRET. */
    if ((u16)(u16UserBase + 16u) != (u16)(GJ_GDT_USER_CS & 0xf8u)) {
        fOk = 0;
    }

    /* Refresh soft EFER bits from live readback. */
    g_SoftSnap.u8Sce = (u8)((u64Efer & EFER_SCE) != 0);
    g_SoftSnap.u8Nxe = (u8)((u64Efer & EFER_NXE) != 0);
    g_SoftSnap.u64Efer = u64Efer;
    g_SoftSnap.u8VerifyOk = fOk ? 1u : 0u;

    if (fOk) {
        g_u32SoftVerifyOk++;
    } else {
        g_u32SoftVerifyBad++;
    }
    return fOk;
}

void
cpu_syscall_init(void)
{
    u64 u64Efer;
    u64 u64Star;
    u64 u64Lstar;
    u64 u64Cstar;
    u64 u64Sfmask;

    /*
     * STAR: kernel CS in [47:32], user selector base in [63:48].
     * SYSRETQ: CS = base+16 = 0x28 (user long code), SS = base+8 = 0x20.
     * GDT layout: index5=0x28 code, index4=0x20 data → base 0x18.
     */
    u64Star = ((u64)GJ_CPU_SYSCALL_STAR_USER_BASE << 48) |
              ((u64)GJ_GDT_KERNEL_CS << 32);
    u64Lstar = (u64)(gj_vaddr_t)gj_syscall_entry;
    u64Cstar = u64Lstar; /* CSTAR unused (no 32-bit SYSCALL path) */
    u64Sfmask = SFMASK_DEFAULT;

    wrmsr(MSR_STAR, u64Star);
    wrmsr(MSR_LSTAR, u64Lstar);
    wrmsr(MSR_CSTAR, u64Cstar);
    wrmsr(MSR_SFMASK, u64Sfmask);

    u64Efer = rdmsr(MSR_EFER);
    u64Efer |= EFER_SCE | EFER_NXE; /* NXE backs W^X mprotect */
    wrmsr(MSR_EFER, u64Efer);
    u64Efer = rdmsr(MSR_EFER); /* soft: post-write live value */

    g_fSyscallReady = 1;
    cpu_syscall_soft_note_program(u64Star, u64Lstar, u64Cstar, u64Sfmask,
                                  u64Efer);

    kprintf("cpu: SYSCALL ready LSTAR=0x%lx STAR=0x%lx\n",
            (unsigned long)u64Lstar, (unsigned long)u64Star);
    /* Soft: rate-limited bring-up line (first program path). */
    kprintf("cpu: syscall soft program STAR=0x%lx LSTAR=0x%lx "
            "SFMASK=0x%lx EFER=0x%lx SCE=%u NXE=%u\n",
            (unsigned long)u64Star, (unsigned long)u64Lstar,
            (unsigned long)u64Sfmask, (unsigned long)u64Efer,
            (unsigned)g_SoftSnap.u8Sce, (unsigned)g_SoftSnap.u8Nxe);
    kprintf("cpu: syscall soft decode kern_cs=0x%x user_base=0x%x "
            "sysret_cs=0x%x sysret_ss=0x%x\n",
            (unsigned)g_SoftSnap.u16StarKernCs,
            (unsigned)g_SoftSnap.u16StarUserBase,
            (unsigned)(g_SoftSnap.u16StarUserBase + 16u),
            (unsigned)(g_SoftSnap.u16StarUserBase + 8u));

    if (cpu_syscall_soft_verify_inner()) {
        kprintf("cpu: syscall soft verify PASS inits=%u\n",
                g_u32SoftInits);
    } else {
        kprintf("cpu: syscall soft verify FAIL inits=%u\n",
                g_u32SoftInits);
    }

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
        g_u32SoftEnterBad++;
        kprintf("cpu: enter_user invalid (ready=%d)\n", g_fSyscallReady);
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    g_u32SoftEnter64++;
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
        g_u32SoftEnterBad++;
        kprintf("cpu: enter_user32 invalid\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    g_u32SoftEnter32++;
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
        g_u32SoftEnterBad++;
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

/* ---- Soft SYSCALL MSR observability API ---------------------------- */

u32
cpu_syscall_soft_inits(void)
{
    return g_u32SoftInits;
}

u32
cpu_syscall_soft_verify_ok(void)
{
    return g_u32SoftVerifyOk;
}

u32
cpu_syscall_soft_verify_bad(void)
{
    return g_u32SoftVerifyBad;
}

u32
cpu_syscall_soft_enter64(void)
{
    return g_u32SoftEnter64;
}

u32
cpu_syscall_soft_enter32(void)
{
    return g_u32SoftEnter32;
}

u32
cpu_syscall_soft_enter_bad(void)
{
    return g_u32SoftEnterBad;
}

u64
cpu_syscall_soft_star(void)
{
    return g_SoftSnap.u64Star;
}

u64
cpu_syscall_soft_lstar(void)
{
    return g_SoftSnap.u64Lstar;
}

u64
cpu_syscall_soft_sfmask(void)
{
    return g_SoftSnap.u64Sfmask;
}

u64
cpu_syscall_soft_efer(void)
{
    return g_SoftSnap.u64Efer;
}

int
cpu_syscall_soft_info_get(struct gj_cpu_syscall_soft *pOut)
{
    if (pOut == NULL) {
        return g_fSoftSnapLive ? 1 : 0;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fSoftSnapLive) {
        return 0;
    }
    *pOut = g_SoftSnap;
    return 1;
}

int
cpu_syscall_soft_verify(void)
{
    return cpu_syscall_soft_verify_inner();
}

void
cpu_syscall_soft_log(void)
{
    /*
     * Greppable soft summary (product / smoke inventory):
     *   cpu: syscall soft inits=… verify_ok=… verify_bad=… enter64=… …
     *   cpu: syscall soft STAR=0x… LSTAR=0x… SFMASK=0x… EFER=0x… SCE=… NXE=…
     *   cpu: syscall soft decode kern_cs=0x… user_base=0x… (CS64=base+16)
     */
    kprintf("cpu: syscall soft inits=%u verify_ok=%u verify_bad=%u "
            "enter64=%u enter32=%u bad=%u\n",
            g_u32SoftInits, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_u32SoftEnter64, g_u32SoftEnter32, g_u32SoftEnterBad);
    kprintf("cpu: syscall soft STAR=0x%lx LSTAR=0x%lx SFMASK=0x%lx "
            "EFER=0x%lx SCE=%u NXE=%u\n",
            (unsigned long)g_SoftSnap.u64Star,
            (unsigned long)g_SoftSnap.u64Lstar,
            (unsigned long)g_SoftSnap.u64Sfmask,
            (unsigned long)g_SoftSnap.u64Efer,
            (unsigned)g_SoftSnap.u8Sce, (unsigned)g_SoftSnap.u8Nxe);
    kprintf("cpu: syscall soft decode kern_cs=0x%x user_base=0x%x "
            "(CS64=base+16=0x%x)\n",
            (unsigned)g_SoftSnap.u16StarKernCs,
            (unsigned)g_SoftSnap.u16StarUserBase,
            (unsigned)(g_SoftSnap.u16StarUserBase + 16u));
    if (!g_fSoftSnapLive) {
        kprintf("cpu: syscall soft idle (MSRs not programmed)\n");
    } else if (g_SoftSnap.u8VerifyOk) {
        kprintf("cpu: syscall soft verify PASS\n");
    } else if (g_u32SoftVerifyBad != 0) {
        kprintf("cpu: syscall soft verify FAIL\n");
    } else {
        kprintf("cpu: syscall soft armed (no re-verify yet)\n");
    }
}
