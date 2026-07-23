/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Percpu pool + GS_BASE. Layout of struct gj_cpu is an asm contract —
 * see cpu.h; do not reorder leading fields (sched / SYSCALL / swapgs).
 */
#include <gj/cpu.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

#define MSR_GS_BASE        0xC0000101u
#define MSR_KERNEL_GS_BASE 0xC0000102u

/*
 * Static BSS pool for BSP + low AP slots (keeps kernel image under user maps).
 * Slots >= GJ_CPU_STATIC_MAX grow via PMM into g_apCpuDyn[] (HHDM/identity VA).
 * g_apCpuDyn is sparse: only indices in [GJ_CPU_STATIC_MAX, GJ_MAX_CPUS) publish.
 */
static struct gj_cpu g_aCpus[GJ_CPU_STATIC_MAX];
/* Sparse; only indices >= GJ_CPU_STATIC_MAX are published. BSS starts NULL. */
static struct gj_cpu *g_apCpuDyn[GJ_MAX_CPUS];
static u32 g_u32NOnline = 1;
/* Successful PMM-backed percpu allocations (boot telemetry / soft inventory). */
static u32 g_u32DynPercpu;

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo, u32Hi;
    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);
    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
    (void)rdmsr;
}

void
cpu_gs_init(struct gj_cpu *pCpu)
{
    /*
     * Kernel: GS_BASE = percpu.
     * KERNEL_GS_BASE = 0 (user GS).
     * enter_user: swapgs → GS=0, KERNEL_GS=percpu; SYSCALL: swapgs → GS=percpu.
     */
    if (pCpu == NULL) {
        return;
    }
    wrmsr(MSR_GS_BASE, (u64)(gj_vaddr_t)pCpu);
    wrmsr(MSR_KERNEL_GS_BASE, 0);
}

void
cpu_init_bsp(void)
{
    struct gj_cpu *pCpu = &g_aCpus[0];

    memset(pCpu, 0, sizeof(*pCpu));
    pCpu->u32CpuId = 0;
    pCpu->u32Online = 1;
    pCpu->u64KernelRsp =
        (u64)(gj_vaddr_t)&pCpu->aSyscallStack[GJ_SYSCALL_STACK_SIZE];
    pCpu->u64Cr3 = cpu_read_cr3();
    pCpu->pCurrent = NULL;
    g_u32NOnline = 1;
    g_u32DynPercpu = 0;
    cpu_gs_init(pCpu);
    /* Greppable: cpu: BSP ... */
    kprintf("cpu: BSP id=0 percpu=%p kstack=%lx cr3=%lx static_max=%u max=%u\n",
            (void *)pCpu, (unsigned long)pCpu->u64KernelRsp,
            (unsigned long)pCpu->u64Cr3, GJ_CPU_STATIC_MAX, GJ_MAX_CPUS);
}

void
cpu_init_ap(u32 u32CpuId)
{
    struct gj_cpu *pCpu;
    u32 cPages;
    gj_paddr_t paBase;

    /* Reject BSP (handled by cpu_init_bsp) and out-of-range design ceiling. */
    if (u32CpuId == 0 || u32CpuId >= GJ_MAX_CPUS) {
        return;
    }
    if (u32CpuId < GJ_CPU_STATIC_MAX) {
        pCpu = &g_aCpus[u32CpuId];
        /* Idempotent: never re-memset a live online percpu (pCurrent/GS). */
        if (pCpu->u32Online != 0) {
            return;
        }
    } else {
        /*
         * PMM-backed percpu growth past the static BSS pool.
         * Round sizeof(gj_cpu) up to whole pages; PMM frames are already
         * reachable via low identity map or HHDM (once hhdm_ready()).
         * Reuse a published dyn slot on re-entry; never install a null VA.
         */
        pCpu = g_apCpuDyn[u32CpuId];
        if (pCpu != NULL && pCpu->u32Online != 0) {
            return; /* already published */
        }
        if (pCpu == NULL) {
            cPages = (u32)((sizeof(struct gj_cpu) + GJ_PAGE_SIZE - 1) /
                           GJ_PAGE_SIZE);
            if (cPages == 0) {
                return; /* defensive: sizeof edge */
            }
            paBase = pmm_alloc_pages(cPages);
            /* pmm_alloc_pages → 0 on OOM: leave AP without GS percpu. */
            if (paBase == 0) {
                /* Greppable: cpu: ... PMM percpu alloc fail */
                kprintf("cpu: AP id=%u PMM percpu alloc fail pages=%u\n",
                        u32CpuId, cPages);
                return;
            }
            if (hhdm_ready() || paBase >= 0x100000000ull) {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)(GJ_HHDM_BASE +
                                                     (u64)paBase);
            } else {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)paBase;
            }
            if (pCpu == NULL) {
                return; /* unreachable; keeps null-VA guard explicit */
            }
            g_apCpuDyn[u32CpuId] = pCpu;
            if (g_u32DynPercpu < GJ_MAX_CPUS) {
                g_u32DynPercpu++;
            }
            /* Greppable: cpu: ... PMM percpu pa= */
            kprintf("cpu: AP id=%u PMM percpu pa=0x%lx pages=%u dyn=%u\n",
                    u32CpuId, (unsigned long)paBase, cPages, g_u32DynPercpu);
        }
    }
    /* pCpu non-null: static address or a published dyn slot. */
    memset(pCpu, 0, sizeof(*pCpu));
    pCpu->u32CpuId = u32CpuId;
    pCpu->u32Online = 1;
    pCpu->u64KernelRsp =
        (u64)(gj_vaddr_t)&pCpu->aSyscallStack[GJ_SYSCALL_STACK_SIZE];
    pCpu->u64Cr3 = cpu_read_cr3();
    pCpu->pCurrent = NULL;
    cpu_gs_init(pCpu);
    if (g_u32NOnline < GJ_MAX_CPUS) {
        g_u32NOnline++;
    }
    /* Greppable: cpu: AP id=... online= */
    kprintf("cpu: AP id=%u percpu=%p online=%u dyn=%u\n", u32CpuId, (void *)pCpu,
            g_u32NOnline, g_u32DynPercpu);
}

u32
cpu_online_count(void)
{
    return g_u32NOnline;
}

struct gj_cpu *
cpu_for_id(u32 u32CpuId)
{
    struct gj_cpu *pCpu;

    if (u32CpuId >= GJ_MAX_CPUS) {
        return NULL;
    }
    if (u32CpuId < GJ_CPU_STATIC_MAX) {
        pCpu = &g_aCpus[u32CpuId];
        return (pCpu->u32Online != 0) ? pCpu : NULL;
    }
    pCpu = g_apCpuDyn[u32CpuId];
    if (pCpu == NULL || pCpu->u32Online == 0) {
        return NULL;
    }
    return pCpu;
}

int
cpu_slot_online(u32 u32CpuId)
{
    return cpu_for_id(u32CpuId) != NULL ? 1 : 0;
}

u32
cpu_dyn_percpu_count(void)
{
    return g_u32DynPercpu;
}

struct gj_cpu *
cpu_current(void)
{
    u64 u64Gs;

    __asm__ volatile ("mov %%gs:0, %0" : "=r"(u64Gs));
    /*
     * First field is u64KernelRsp — not a self pointer.
     * Use MSR_GS_BASE value instead.
     */
    u64Gs = rdmsr(MSR_GS_BASE);
    if (u64Gs == 0) {
        return &g_aCpus[0];
    }
    return (struct gj_cpu *)(gj_vaddr_t)u64Gs;
}

u32
cpu_id(void)
{
    return cpu_current()->u32CpuId;
}

void
cpu_set_current_thread(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu = cpu_current();

    pCpu->pCurrent = pThr;
    /*
     * Install SYSCALL stack top only when fully switched to this thr.
     * schedule() calls thread_install_kstack() after switch_context so we
     * never point KERNEL_RSP at pNext while still running on pCur's frame.
     * First entry (trampoline) still needs KERNEL_RSP before enter_user —
     * trampoline calls thread_install_kstack explicitly.
     */
    /* TSS.RSP0 stays the dedicated IRQ stack from gdt_init (not thr kstack). */
    (void)tss_get_rsp0;
    (void)pThr;
}

struct gj_thread *
cpu_current_thread(void)
{
    return cpu_current()->pCurrent;
}

u64
cpu_read_cr3(void)
{
    u64 u64Cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
    return u64Cr3;
}

void
cpu_load_cr3(u64 u64Cr3)
{
    struct gj_cpu *pCpu = cpu_current();

    if (u64Cr3 == 0) {
        return;
    }
    if (pCpu->u64Cr3 == u64Cr3) {
        return;
    }
    __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    pCpu->u64Cr3 = u64Cr3;
}
