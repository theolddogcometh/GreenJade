/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Walk/modify active 4-level page tables (boot identity 2MiB + 4K splits).
 * Per-process CR3 clone: share kernel half, private user half (G-AS-*).
 *
 * P-MEM-5 HHDM: vmm_hhdm_init maps phys [0, paMax) at GJ_HHDM_BASE with
 * 2 MiB pages so PMM freelist nodes for PA ≥ 4 GiB are addressable. Call
 * pmm_release_high after success (main.c) for hierarchical free on large
 * RAM (768G soak_tib). Never identity-map high PAs into freelist links.
 *
 * Soft VMM inventory (Wave 10 exclusive deepen):
 *   - Live AS / COW table / HHDM / device UC / ensure_id snaps
 *   - Cumulative create/destroy/break/share/table counters
 *   greppable: "vmm: soft …"
 *
 * Greppable: "vmm: HHDM base=" "vmm: as_create" "vmm: as_destroy leaf="
 *            "vmm: COW break" "vmm: as_clone_user" "vmm: map_device_uc"
 *            "vmm: ensure_identity_rw" (… soft PASS)
 *            "vmm: soft inventory" "vmm: soft as" "vmm: soft cow"
 *            "vmm: soft hhdm" "vmm: soft device_uc" "vmm: soft ensure_id"
 */
#include <gj/apic.h>
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

#define PTE_P   (1ull << 0)
#define PTE_W   (1ull << 1)
#define PTE_U   (1ull << 2)
#define PTE_PS  (1ull << 7) /* page size: 2MiB/1GiB */
/* Available software bit — marks fork COW leaf (cleared on break). */
#define PTE_COW (1ull << 9)
#define PTE_NX  (1ull << 63)
#define PTE_ADDR_MASK 0x000ffffffffff000ull

#define GJ_VMM_ANON_BASE 0x0000000040000000ull
#define GJ_VMM_ANON_END  0x0000000080000000ull

/* Shared COW data frames: refcount until last AS drops. */
#define GJ_COW_REF_MAX 256u

struct gj_cow_ref {
    gj_paddr_t pa;
    u32        cRef;
    u8         u8Used;
    u8         aPad[3];
};

static u64 *g_pKernelPml4;
static u64  g_u64KernelCr3;
static u64  g_u64AnonNext = GJ_VMM_ANON_BASE;
static u64 *g_pAnonCursor = &g_u64AnonNext;
static int  g_fHhdmReady;
static u64  g_u64HhdmMapped;
static struct gj_cow_ref g_aCowRef[GJ_COW_REF_MAX];
static u32               g_cCowLive;
static u32               g_cCowFrees;
/* Lifetime / soft-path observability (G-AS + soft CAP). */
static u32               g_cAsCreate;
static u32               g_cAsDestroy;
static u32               g_cAsLive;
static u32               g_cCowBreak;
static u32               g_cCowShareOk;
static u32               g_cCowShareFull;
static u32               g_cCowTable;
static u32               g_cMapDeviceUc;
static u32               g_cMapDeviceUcPages;
static u32               g_cEnsureIdCall;
static u32               g_cEnsureIdFix;

/*
 * Soft product inventory (Wave 10 exclusive). Cumulative unless noted live/peak.
 * Diagnostics only — never hard-gate product AS/COW/HHDM policy.
 * greppable: vmm: soft
 */
static u32 g_cAsLivePeak;   /* high-water private AS live count */
static u32 g_cSoftInvLogs;  /* soft_inventory_log emissions */
static u8  g_fSoftInvOnce;  /* one-shot deep dump after product activity */

static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

static u64
read_cr3(void)
{
    u64 u64Cr3;

    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
    return u64Cr3;
}

/*
 * COW data-frame refcount. Sole owner frames are not tracked (implicit ref 1).
 * First share registers ref=2; further forks/inc; drop free at 0.
 */
static int
cow_ref_find(gj_paddr_t pa)
{
    u32 i;

    pa &= PTE_ADDR_MASK;
    for (i = 0; i < GJ_COW_REF_MAX; i++) {
        if (g_aCowRef[i].u8Used && g_aCowRef[i].pa == pa) {
            return (int)i;
        }
    }
    return -1;
}

/* Establish shared ownership: parent was sole → ref 2; else +1. */
static int
cow_ref_share(gj_paddr_t pa)
{
    int iSlot;
    u32 i;

    pa &= PTE_ADDR_MASK;
    if (pa == 0) {
        return -1;
    }
    iSlot = cow_ref_find(pa);
    if (iSlot >= 0) {
        g_aCowRef[iSlot].cRef++;
        g_cCowShareOk++;
        return 0;
    }
    for (i = 0; i < GJ_COW_REF_MAX; i++) {
        if (!g_aCowRef[i].u8Used) {
            g_aCowRef[i].u8Used = 1;
            g_aCowRef[i].pa = pa;
            g_aCowRef[i].cRef = 2; /* parent + child */
            g_cCowLive++;
            g_cCowShareOk++;
            return 0;
        }
    }
    g_cCowShareFull++;
    return -1; /* table full — caller may still share (leak risk) */
}

/*
 * Drop one COW reference. Returns remaining count after drop:
 *   0 → caller may pmm_free (last ref or untracked private/orphan)
 *  >0 → still shared; do not free
 * fWasCow: if untracked, COW leaves still free (orphan); private always free.
 */
static u32
cow_ref_drop(gj_paddr_t pa, int fWasCow)
{
    int iSlot;

    (void)fWasCow;
    pa &= PTE_ADDR_MASK;
    if (pa == 0) {
        return 1; /* no free */
    }
    iSlot = cow_ref_find(pa);
    if (iSlot < 0) {
        /* Not in table: private data or orphan COW → free */
        return 0;
    }
    if (g_aCowRef[iSlot].cRef > 1u) {
        g_aCowRef[iSlot].cRef--;
        return g_aCowRef[iSlot].cRef;
    }
    g_aCowRef[iSlot].u8Used = 0;
    g_aCowRef[iSlot].cRef = 0;
    g_aCowRef[iSlot].pa = 0;
    if (g_cCowLive > 0) {
        g_cCowLive--;
    }
    g_cCowFrees++;
    return 0;
}

/* Release one leaf data frame under destroy/unmap/break. */
static void
release_leaf_frame(gj_paddr_t pa, int fCow)
{
    if (pa == 0) {
        return;
    }
    if (cow_ref_drop(pa, fCow) == 0) {
        pmm_free(pa);
    }
}

/**
 * Greppable soft VMM inventory (product / smoke).
 *   vmm: soft inventory …
 *   vmm: soft as …
 *   vmm: soft cow …
 *   vmm: soft hhdm …
 *   vmm: soft device_uc …
 *   vmm: soft ensure_id …
 * greppable: vmm: soft
 * Honesty: soft counters only — not product AS/COW/HHDM complete or bar3.
 */
static void
soft_inventory_log(void)
{
    u32 i;
    u32 cCowSlots = 0;
    u32 cCowRefs = 0;
    u64 u64Anon;
    int fHhdm;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    for (i = 0; i < GJ_COW_REF_MAX; i++) {
        if (g_aCowRef[i].u8Used) {
            cCowSlots++;
            cCowRefs += g_aCowRef[i].cRef;
        }
    }
    if (g_cAsLive > g_cAsLivePeak) {
        g_cAsLivePeak = g_cAsLive;
    }

    fHhdm = g_fHhdmReady ? 1 : 0;
    u64Anon = (g_pAnonCursor != NULL) ? *g_pAnonCursor : g_u64AnonNext;

    /* Grep: vmm: soft inventory */
    kprintf("vmm: soft inventory hhdm=%d as_live=%u as_peak=%u cow_live=%u "
            "cow_slots=%u cow_refs=%u logs=%u "
            "(soft; not product AS complete)\n",
            fHhdm, g_cAsLive, g_cAsLivePeak, g_cCowLive, cCowSlots, cCowRefs,
            g_cSoftInvLogs);

    /* Grep: vmm: soft as */
    kprintf("vmm: soft as create=%u destroy=%u live=%u peak=%u "
            "ker_cr3=0x%lx anon_next=0x%lx band=[0x%lx,0x%lx)\n",
            g_cAsCreate, g_cAsDestroy, g_cAsLive, g_cAsLivePeak,
            (unsigned long)g_u64KernelCr3, (unsigned long)u64Anon,
            (unsigned long)GJ_VMM_ANON_BASE, (unsigned long)GJ_VMM_ANON_END);

    /* Grep: vmm: soft cow */
    kprintf("vmm: soft cow break=%u share_ok=%u share_full=%u tbl_cow=%u "
            "live=%u frees=%u slots_used=%u slots_max=%u refsum=%u\n",
            g_cCowBreak, g_cCowShareOk, g_cCowShareFull, g_cCowTable,
            g_cCowLive, g_cCowFrees, cCowSlots, (unsigned)GJ_COW_REF_MAX,
            cCowRefs);

    /* Grep: vmm: soft hhdm */
    kprintf("vmm: soft hhdm ready=%d base=0x%lx mapped=0x%lx "
            "p_mem5=1 (soft inventory)\n",
            fHhdm, (unsigned long)GJ_HHDM_BASE,
            (unsigned long)g_u64HhdmMapped);

    /* Grep: vmm: soft device_uc */
    kprintf("vmm: soft device_uc maps=%u pages=%u base=0x%lx span=0x%lx\n",
            g_cMapDeviceUc, g_cMapDeviceUcPages,
            (unsigned long)GJ_DEVICE_MMIO_BASE,
            (unsigned long)GJ_DEVICE_MMIO_SPAN);

    /* Grep: vmm: soft ensure_id */
    kprintf("vmm: soft ensure_id calls=%u fixed=%u\n", g_cEnsureIdCall,
            g_cEnsureIdFix);
}

/**
 * After first product AS/COW/device/ensure activity, print soft inventory once
 * (mirrors memobj soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_cAsCreate == 0 && g_cMapDeviceUc == 0 && g_cCowBreak == 0 &&
        g_cEnsureIdCall == 0 && g_cAsDestroy == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

void
vmm_tlb_flush_page(gj_vaddr_t va)
{
    /* G-AS-4: multi-CPU shootdown when more than one CPU online */
    tlb_shootdown(va);
}

static u64 *
phys_to_virt(gj_paddr_t pa)
{
    if (g_fHhdmReady) {
        return (u64 *)(gj_vaddr_t)(GJ_HHDM_BASE + (u64)pa);
    }
    /*
     * Identity map for low memory (boot only). High PA before HHDM must not
     * be walked here — PMM holds high regions pending until vmm_hhdm_init.
     */
    return (u64 *)(gj_vaddr_t)pa;
}

gj_vaddr_t
hhdm_to_virt(gj_paddr_t pa)
{
    if (g_fHhdmReady) {
        return (gj_vaddr_t)(GJ_HHDM_BASE + (u64)pa);
    }
    return (gj_vaddr_t)pa;
}

gj_paddr_t
hhdm_to_phys(gj_vaddr_t va)
{
    if (g_fHhdmReady && va >= (gj_vaddr_t)GJ_HHDM_BASE) {
        return (gj_paddr_t)(va - (gj_vaddr_t)GJ_HHDM_BASE);
    }
    return (gj_paddr_t)va;
}

int
hhdm_ready(void)
{
    return g_fHhdmReady;
}

static gj_paddr_t
alloc_table(void)
{
    gj_paddr_t pa = pmm_alloc();

    if (pa == 0) {
        return 0;
    }
    memset(phys_to_virt(pa), 0, GJ_PAGE_SIZE);
    return pa;
}

/* Active PML4 from current CR3 (G-AS-5: maps under active space). */
static u64 *
active_pml4(void)
{
    return phys_to_virt(read_cr3() & PTE_ADDR_MASK);
}

void
vmm_init(void)
{
    g_u64KernelCr3 = read_cr3() & PTE_ADDR_MASK;
    g_pKernelPml4 = phys_to_virt(g_u64KernelCr3);
    g_u64AnonNext = GJ_VMM_ANON_BASE;
    g_pAnonCursor = &g_u64AnonNext;
    g_fHhdmReady = 0;
    g_u64HhdmMapped = 0;
    kprintf("vmm: kernel pml4@%p cr3=0x%lx anon[%lx-%lx)\n",
            (void *)g_pKernelPml4,
            (unsigned long)g_u64KernelCr3,
            (unsigned long)GJ_VMM_ANON_BASE,
            (unsigned long)GJ_VMM_ANON_END);
    /* Wave 10: greppable soft inventory baseline after template bind. */
    soft_inventory_log();
}

/*
 * Map phys [0, paMax) at GJ_HHDM_BASE using 2 MiB large pages (P-MEM-5).
 * Installs into kernel PML4 so all process clones that share kernel half
 * (COW path) keep HHDM once present in template.
 * Safety: cap at GJ_PMM_MAX_PHYS_TIB; round up to 2 MiB; skip existing 1 GiB
 * PDPT leaves; NX on data large pages. After success, pmm_release_high may
 * hierarchical-free high RAM into order freelists (768G soak path).
 */
gj_status_t
vmm_hhdm_init(u64 paMax)
{
    u64 u64Pa;
    u64 u64Limit;
    u64 *pPdpt = NULL;
    u64 *pPd = NULL;
    u64 u64CurPdptPa = 0;
    u64 u64CurPdPa = 0;
    gj_paddr_t paTbl;
    u64 cMapped = 0;

    if (g_pKernelPml4 == NULL) {
        return GJ_ERR_INVAL;
    }
    /* Cap at design headroom (TiB → bytes: << 40) */
    u64Limit = (u64)GJ_PMM_MAX_PHYS_TIB << 40;
    if (paMax == 0 || paMax > u64Limit) {
        paMax = u64Limit;
    }
    /* Round up to 2 MiB */
    paMax = (paMax + (1ull << 21) - 1) & ~((1ull << 21) - 1);

    for (u64Pa = 0; u64Pa < paMax; u64Pa += (1ull << 21)) {
        u64 u64Va = GJ_HHDM_BASE + u64Pa;
        u64 u64I4 = (u64Va >> 39) & 0x1ff;
        u64 u64I3 = (u64Va >> 30) & 0x1ff;
        u64 u64I2 = (u64Va >> 21) & 0x1ff;

        if ((g_pKernelPml4[u64I4] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                return GJ_ERR_NOMEM;
            }
            g_pKernelPml4[u64I4] = paTbl | PTE_P | PTE_W;
            u64CurPdptPa = paTbl;
            pPdpt = phys_to_virt(paTbl);
        } else {
            u64CurPdptPa = g_pKernelPml4[u64I4] & PTE_ADDR_MASK;
            pPdpt = phys_to_virt(u64CurPdptPa);
        }

        if ((pPdpt[u64I3] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                return GJ_ERR_NOMEM;
            }
            pPdpt[u64I3] = paTbl | PTE_P | PTE_W;
            u64CurPdPa = paTbl;
            pPd = phys_to_virt(paTbl);
        } else if ((pPdpt[u64I3] & PTE_PS) != 0) {
            /* Already a 1 GiB mapping — skip covering this 2 MiB */
            continue;
        } else {
            u64CurPdPa = pPdpt[u64I3] & PTE_ADDR_MASK;
            pPd = phys_to_virt(u64CurPdPa);
        }

        if ((pPd[u64I2] & PTE_P) == 0) {
            /* 2 MiB large page: phys | P | W | PS | NX */
            pPd[u64I2] = (u64Pa & PTE_ADDR_MASK) | PTE_P | PTE_W | PTE_PS | PTE_NX;
            cMapped++;
        }
    }

    /* Reload CR3 to flush TLB for new maps */
    {
        u64 u64Cr3 = read_cr3();

        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    }
    g_fHhdmReady = 1;
    g_u64HhdmMapped = paMax;
    /* phys_to_virt now uses HHDM — refresh kernel pml4 pointer */
    g_pKernelPml4 = phys_to_virt(g_u64KernelCr3);
    /* Greppable: vmm: HHDM base= (P-MEM-5; unlocks pmm high hierarchical free) */
    kprintf("vmm: HHDM base=0x%lx mapped=0x%lx (%lu x 2MiB)\n",
            (unsigned long)GJ_HHDM_BASE, (unsigned long)paMax,
            (unsigned long)cMapped);
    /* Wave 10: soft inventory after HHDM ready. */
    soft_inventory_log();
    return GJ_OK;
}

u64
vmm_kernel_cr3(void)
{
    return g_u64KernelCr3;
}

gj_status_t
vmm_map_device(gj_paddr_t pa, u64 cb)
{
    u64 u64Pa;
    u64 u64End;
    u64 *pPdpt = NULL;
    u64 *pPd = NULL;
    extern char __kernel_start[];
    extern char __kernel_end[];
    u64 u64Ker0;
    u64 u64Ker1;

    if (g_pKernelPml4 == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    /*
     * Never identity-map device MMIO over kernel image/BSS. Soft CAP probes
     * must use high BARs or a dedicated UC window (not low identity).
     */
    u64Ker0 = (u64)(gj_vaddr_t)__kernel_start & ~((1ull << 21) - 1);
    u64Ker1 = ((u64)(gj_vaddr_t)__kernel_end + (1ull << 21) - 1) &
              ~((1ull << 21) - 1);
    u64Pa = (u64)pa & ~((1ull << 21) - 1);
    u64End = ((u64)pa + cb + (1ull << 21) - 1) & ~((1ull << 21) - 1);
    if (u64Pa < u64Ker1 && u64End > u64Ker0) {
        kprintf("vmm: map_device reject pa=0x%lx (kernel image collision)\n",
                (unsigned long)pa);
        return GJ_ERR_PERM;
    }
    /* Align to 2 MiB covering the range */

    for (; u64Pa < u64End; u64Pa += (1ull << 21)) {
        u64 u64Va = u64Pa; /* identity */
        u64 u64I4 = (u64Va >> 39) & 0x1ff;
        u64 u64I3 = (u64Va >> 30) & 0x1ff;
        u64 u64I2 = (u64Va >> 21) & 0x1ff;
        gj_paddr_t paTbl;

        /* Canonical check: bits 48..63 must match bit 47 */
        if (((u64Va >> 47) & 1ull) != 0) {
            return GJ_ERR_INVAL;
        }

        if ((g_pKernelPml4[u64I4] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                return GJ_ERR_NOMEM;
            }
            g_pKernelPml4[u64I4] = paTbl | PTE_P | PTE_W;
            pPdpt = phys_to_virt(paTbl);
        } else {
            pPdpt = phys_to_virt(g_pKernelPml4[u64I4] & PTE_ADDR_MASK);
        }

        if ((pPdpt[u64I3] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                return GJ_ERR_NOMEM;
            }
            pPdpt[u64I3] = paTbl | PTE_P | PTE_W;
            pPd = phys_to_virt(paTbl);
        } else if ((pPdpt[u64I3] & PTE_PS) != 0) {
            continue; /* already 1 GiB */
        } else {
            pPd = phys_to_virt(pPdpt[u64I3] & PTE_ADDR_MASK);
        }

        if ((pPd[u64I2] & PTE_P) == 0) {
            /* PCD|PWT for uncacheable-ish device; PS 2 MiB */
            pPd[u64I2] = (u64Pa & PTE_ADDR_MASK) | PTE_P | PTE_W | PTE_PS |
                         PTE_NX | (1ull << 4) | (1ull << 3);
        }
    }
    {
        u64 u64Cr3 = read_cr3();

        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    }
    return GJ_OK;
}

gj_status_t
vmm_map_device_uc(gj_paddr_t pa, u64 cb, gj_vaddr_t *pVaOut)
{
    u64 u64Pa;
    u64 u64End;
    u64 *pPdpt = NULL;
    u64 *pPd = NULL;
    u32 cNew = 0;
    u32 cHad = 0;

    if (g_pKernelPml4 == NULL || cb == 0 || pVaOut == NULL) {
        kprintf("vmm: map_device_uc soft reject inval pa=0x%lx cb=0x%lx\n",
                (unsigned long)pa, (unsigned long)cb);
        return GJ_ERR_INVAL;
    }
    if ((u64)pa >= GJ_DEVICE_MMIO_SPAN ||
        (u64)pa + cb > GJ_DEVICE_MMIO_SPAN) {
        kprintf("vmm: map_device_uc soft reject span pa=0x%lx cb=0x%lx\n",
                (unsigned long)pa, (unsigned long)cb);
        return GJ_ERR_INVAL;
    }
    u64Pa = (u64)pa & ~((1ull << 21) - 1);
    u64End = ((u64)pa + cb + (1ull << 21) - 1) & ~((1ull << 21) - 1);
    *pVaOut = (gj_vaddr_t)(GJ_DEVICE_MMIO_BASE + (u64)pa);

    for (; u64Pa < u64End; u64Pa += (1ull << 21)) {
        u64 u64Va = GJ_DEVICE_MMIO_BASE + u64Pa;
        u64 u64I4 = (u64Va >> 39) & 0x1ff;
        u64 u64I3 = (u64Va >> 30) & 0x1ff;
        u64 u64I2 = (u64Va >> 21) & 0x1ff;
        gj_paddr_t paTbl;

        if ((g_pKernelPml4[u64I4] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                kprintf("vmm: map_device_uc soft nomem pa=0x%lx\n",
                        (unsigned long)pa);
                return GJ_ERR_NOMEM;
            }
            g_pKernelPml4[u64I4] = paTbl | PTE_P | PTE_W;
            pPdpt = phys_to_virt(paTbl);
        } else {
            pPdpt = phys_to_virt(g_pKernelPml4[u64I4] & PTE_ADDR_MASK);
        }

        if ((pPdpt[u64I3] & PTE_P) == 0) {
            paTbl = alloc_table();
            if (paTbl == 0) {
                kprintf("vmm: map_device_uc soft nomem pa=0x%lx\n",
                        (unsigned long)pa);
                return GJ_ERR_NOMEM;
            }
            pPdpt[u64I3] = paTbl | PTE_P | PTE_W;
            pPd = phys_to_virt(paTbl);
        } else if ((pPdpt[u64I3] & PTE_PS) != 0) {
            cHad++;
            continue;
        } else {
            pPd = phys_to_virt(pPdpt[u64I3] & PTE_ADDR_MASK);
        }

        if ((pPd[u64I2] & PTE_P) == 0) {
            /* PCD|PWT uncacheable-ish device; PS 2 MiB */
            pPd[u64I2] = (u64Pa & PTE_ADDR_MASK) | PTE_P | PTE_W | PTE_PS |
                         PTE_NX | (1ull << 4) | (1ull << 3);
            cNew++;
        } else {
            cHad++;
        }
    }
    {
        u64 u64Cr3 = read_cr3();

        __asm__ volatile("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    }
    g_cMapDeviceUc++;
    g_cMapDeviceUcPages += cNew;
    /* Soft CAP greppable (AHCI/NVMe BAR probes). */
    kprintf("vmm: map_device_uc pa=0x%lx va=0x%lx cb=0x%lx pages=%u had=%u "
            "total=%u soft PASS\n",
            (unsigned long)pa, (unsigned long)*pVaOut, (unsigned long)cb, cNew,
            cHad, g_cMapDeviceUc);
    soft_inventory_maybe_once();
    return GJ_OK;
}

void
vmm_set_anon_cursor(u64 *pCursor)
{
    g_pAnonCursor = (pCursor != NULL) ? pCursor : &g_u64AnonNext;
}

u64 *
vmm_get_anon_cursor(void)
{
    return g_pAnonCursor;
}

/*
 * G-AS-2 (identity-map bring-up):
 *   New PML4 shares all lower tables with the kernel template so identity
 *   access to PMM frames and kernel data still works under the child CR3.
 *   First user map COWs the page-table path (walk_pte) so PTE installs are
 *   private to this AS. True empty user half lands with HHDM later.
 */
u64
vmm_as_create(void)
{
    gj_paddr_t paPml4;
    u64 *pDst;
    u64 *pSrc;
    u32 u32I4;
    u32 cShared = 0;

    if (g_pKernelPml4 == NULL) {
        kprintf("vmm: as_create fail no_template\n");
        return 0;
    }
    paPml4 = alloc_table();
    if (paPml4 == 0) {
        kprintf("vmm: as_create fail nomem live=%u\n", g_cAsLive);
        return 0;
    }
    pDst = phys_to_virt(paPml4);
    pSrc = g_pKernelPml4;

    for (u32I4 = 0; u32I4 < 512; u32I4++) {
        /* Share kernel half (and identity) by pointer; COW on map. */
        pDst[u32I4] = pSrc[u32I4];
        if ((pSrc[u32I4] & PTE_P) != 0) {
            cShared++;
        }
    }

    g_cAsCreate++;
    g_cAsLive++;
    if (g_cAsLive > g_cAsLivePeak) {
        g_cAsLivePeak = g_cAsLive;
    }
    kprintf("vmm: as_create cr3=0x%lx shared_slots=%u live=%u total=%u "
            "(shared lower, COW on map) PASS\n",
            (unsigned long)paPml4, cShared, g_cAsLive, g_cAsCreate);
    soft_inventory_maybe_once();
    return (u64)paPml4;
}

/* True when active CR3 is not the kernel template (private AS). */
static int
as_is_private(void)
{
    u64 u64Cr3 = read_cr3() & PTE_ADDR_MASK;

    return g_u64KernelCr3 != 0 && u64Cr3 != g_u64KernelCr3;
}

/*
 * If *pEntry points at a table still shared with the kernel template at this
 * level, allocate a private copy and retarget *pEntry (COW).
 */
static gj_status_t
cow_table_entry(u64 *pEntry, u64 u64KerEntry)
{
    gj_paddr_t paNew;
    u64 *pSrc;
    u64 *pDst;

    if (pEntry == NULL || (*pEntry & PTE_P) == 0) {
        return GJ_OK;
    }
    if ((*pEntry & PTE_PS) != 0) {
        return GJ_OK; /* large page — split path handles */
    }
    if (*pEntry != u64KerEntry) {
        return GJ_OK; /* already private */
    }
    paNew = alloc_table();
    if (paNew == 0) {
        return GJ_ERR_NOMEM;
    }
    pSrc = phys_to_virt(*pEntry & PTE_ADDR_MASK);
    pDst = phys_to_virt(paNew);
    memcpy(pDst, pSrc, GJ_PAGE_SIZE);
    /* keep flags from original entry; new physical table */
    *pEntry = (paNew & PTE_ADDR_MASK) | (*pEntry & ~PTE_ADDR_MASK);
    g_cCowTable++;
    (void)u64KerEntry;
    return GJ_OK;
}

/*
 * Free private user-side tables + leaf data frames.
 * Shared kernel tables must not be freed.
 *   COW leaves  → refcount drop (free on last ref)
 *   Private user leaves in product VA band → pmm_free
 * Skips identity (PA==VA) and kernel-shared PT subtrees.
 */
gj_status_t
vmm_as_destroy(u64 u64Cr3)
{
    u64 *pPml4;
    u64 *pKer;
    u32 u32I4;
    u32 u32I3;
    u32 u32I2;
    u32 u32I1;
    u32 cLeaf = 0;
    u32 cCowDrop = 0;
    u32 cPrivFree = 0;
    u32 cTables = 0;
    u32 cSharedSkip = 0;
    u64 *pPdpt;
    u64 *pPd;
    u64 *pPt;

    if (u64Cr3 == 0 || u64Cr3 == g_u64KernelCr3) {
        kprintf("vmm: as_destroy reject cr3=0x%lx (kernel/zero)\n",
                (unsigned long)u64Cr3);
        return GJ_ERR_INVAL;
    }
    pPml4 = phys_to_virt(u64Cr3 & PTE_ADDR_MASK);
    pKer = g_pKernelPml4;

    for (u32I4 = 0; u32I4 < 512; u32I4++) {
        if ((pPml4[u32I4] & PTE_P) == 0) {
            continue;
        }
        if (pKer != NULL && pPml4[u32I4] == pKer[u32I4]) {
            cSharedSkip++;
            continue; /* shared */
        }
        pPdpt = phys_to_virt(pPml4[u32I4] & PTE_ADDR_MASK);
        for (u32I3 = 0; u32I3 < 512; u32I3++) {
            u64 *pKerPd = NULL;

            if ((pPdpt[u32I3] & PTE_P) == 0 || (pPdpt[u32I3] & PTE_PS)) {
                continue;
            }
            /* If this PD matches kernel, shared — skip free */
            if (pKer != NULL && (pKer[u32I4] & PTE_P) != 0) {
                u64 *pKerPdpt = phys_to_virt(pKer[u32I4] & PTE_ADDR_MASK);

                if (pPdpt[u32I3] == pKerPdpt[u32I3]) {
                    cSharedSkip++;
                    continue;
                }
                if ((pKerPdpt[u32I3] & PTE_P) != 0 &&
                    (pKerPdpt[u32I3] & PTE_PS) == 0) {
                    pKerPd = phys_to_virt(pKerPdpt[u32I3] & PTE_ADDR_MASK);
                }
            }
            pPd = phys_to_virt(pPdpt[u32I3] & PTE_ADDR_MASK);
            for (u32I2 = 0; u32I2 < 512; u32I2++) {
                u64 u64KerPdEnt = 0;
                u64 *pKerPt = NULL;

                if ((pPd[u32I2] & PTE_P) == 0 || (pPd[u32I2] & PTE_PS)) {
                    continue;
                }
                /*
                 * After COW of a PD, most PT pointers still match the kernel
                 * template (shared). Freeing those destroys identity maps —
                 * only free PTs that differ from the kernel entry.
                 */
                if (pKerPd != NULL) {
                    u64KerPdEnt = pKerPd[u32I2];
                }
                if (u64KerPdEnt != 0 && pPd[u32I2] == u64KerPdEnt) {
                    cSharedSkip++;
                    continue; /* shared PT with kernel */
                }
                if (u64KerPdEnt != 0 && (u64KerPdEnt & PTE_P) != 0 &&
                    (u64KerPdEnt & PTE_PS) == 0) {
                    pKerPt = phys_to_virt(u64KerPdEnt & PTE_ADDR_MASK);
                }
                pPt = phys_to_virt(pPd[u32I2] & PTE_ADDR_MASK);
                for (u32I1 = 0; u32I1 < 512u; u32I1++) {
                    u64 u64Pte = pPt[u32I1];
                    gj_paddr_t pa;
                    gj_vaddr_t va;
                    int fCow;

                    if ((u64Pte & PTE_P) == 0 || (u64Pte & PTE_U) == 0) {
                        continue;
                    }
                    /* Identical to kernel leaf → still shared content */
                    if (pKerPt != NULL && u64Pte == pKerPt[u32I1]) {
                        continue;
                    }
                    va = ((u64)u32I4 << 39) | ((u64)u32I3 << 30) |
                         ((u64)u32I2 << 21) | ((u64)u32I1 << 12);
                    pa = u64Pte & PTE_ADDR_MASK;
                    /* Identity leftovers (boot map) — never free */
                    if (pa == (gj_paddr_t)va) {
                        continue;
                    }
                    /*
                     * Product user band only (≥8 MiB). Low identity + kernel
                     * BSS (can extend past 0x400000 with large embeds) must
                     * never be freed as user leaves.
                     */
                    if (va < 0x0000000000800000ull ||
                        va >= 0x0000000080000000ull) {
                        continue;
                    }
                    fCow = ((u64Pte & PTE_COW) != 0) ? 1 : 0;
                    if (fCow) {
                        release_leaf_frame(pa, 1);
                        cCowDrop++;
                    } else {
                        /* Private sole-owner user page */
                        pmm_free(pa);
                        cPrivFree++;
                    }
                    pPt[u32I1] = 0;
                    cLeaf++;
                }
                pmm_free(pPd[u32I2] & PTE_ADDR_MASK);
                cTables++;
            }
            pmm_free(pPdpt[u32I3] & PTE_ADDR_MASK);
            cTables++;
        }
        pmm_free(pPml4[u32I4] & PTE_ADDR_MASK);
        cTables++;
    }
    pmm_free(u64Cr3 & PTE_ADDR_MASK);
    cTables++; /* PML4 */
    g_cAsDestroy++;
    if (g_cAsLive > 0) {
        g_cAsLive--;
    }
    /* Always greppable (leaf may be 0 for empty smoke AS). */
    kprintf("vmm: as_destroy leaf=%u priv=%u cow_drop=%u tables=%u "
            "shared_skip=%u live=%u frees=%u as_live=%u tbl_cow=%u "
            "cr3=0x%lx PASS\n",
            cLeaf, cPrivFree, cCowDrop, cTables, cSharedSkip, g_cCowLive,
            g_cCowFrees, g_cAsLive, g_cCowTable, (unsigned long)u64Cr3);
    soft_inventory_maybe_once();
    return GJ_OK;
}

static u64
prot_to_flags(u32 u32Prot)
{
    u64 u64F = PTE_P;

    if (u32Prot & GJ_VMM_PROT_WRITE) {
        u64F |= PTE_W;
    }
    if (u32Prot & GJ_VMM_PROT_USER) {
        u64F |= PTE_U;
    }
    if ((u32Prot & GJ_VMM_PROT_EXEC) == 0) {
        u64F |= PTE_NX;
    }
    return u64F;
}

/*
 * User mappings require U=1 on *every* level of the walk (Intel SDM).
 * Hardened kernel paths often have U=0; set U (and W) when installing user PTEs.
 */
static void
ensure_user_flag(u64 *pEntry)
{
    if (pEntry != NULL && (*pEntry & PTE_P) != 0) {
        *pEntry |= PTE_U | PTE_W;
    }
}

/* Ensure PML4/PDPT/PD path exists; return PD entry pointer for va's 2MiB slot. */
static u64 *
walk_pd_entry(gj_vaddr_t va, int fCreate)
{
    u64 u64I4 = (va >> 39) & 0x1ff;
    u64 u64I3 = (va >> 30) & 0x1ff;
    u64 u64I2 = (va >> 21) & 0x1ff;
    u64 *pPml4 = active_pml4();
    u64 *pPml4e;
    u64 *pPdpt;
    u64 *pPdpte;
    u64 *pPd;
    gj_paddr_t pa;
    int fCow = fCreate && as_is_private();

    pPml4e = &pPml4[u64I4];
    if ((*pPml4e & PTE_P) == 0) {
        if (!fCreate) {
            return NULL;
        }
        pa = alloc_table();
        if (pa == 0) {
            return NULL;
        }
        *pPml4e = pa | PTE_P | PTE_W | PTE_U;
    } else if (fCow && g_pKernelPml4 != NULL) {
        if (cow_table_entry(pPml4e, g_pKernelPml4[u64I4]) != GJ_OK) {
            return NULL;
        }
    }
    if (fCreate) {
        ensure_user_flag(pPml4e);
    }
    pPdpt = phys_to_virt(*pPml4e & PTE_ADDR_MASK);
    pPdpte = &pPdpt[u64I3];
    if ((*pPdpte & PTE_P) == 0) {
        if (!fCreate) {
            return NULL;
        }
        pa = alloc_table();
        if (pa == 0) {
            return NULL;
        }
        *pPdpte = pa | PTE_P | PTE_W | PTE_U;
    } else if (fCow && g_pKernelPml4 != NULL &&
               (g_pKernelPml4[u64I4] & PTE_P) != 0) {
        u64 *pKerPdpt = phys_to_virt(g_pKernelPml4[u64I4] & PTE_ADDR_MASK);

        if (cow_table_entry(pPdpte, pKerPdpt[u64I3]) != GJ_OK) {
            return NULL;
        }
        pPdpt = phys_to_virt(*pPml4e & PTE_ADDR_MASK);
        pPdpte = &pPdpt[u64I3];
    }
    if (fCreate) {
        ensure_user_flag(pPdpte);
    }
    if ((*pPdpte & PTE_PS) != 0) {
        /* 1GiB page — not used by boot; refuse */
        return NULL;
    }
    pPd = phys_to_virt(*pPdpte & PTE_ADDR_MASK);
    return &pPd[u64I2];
}

/* Split a 2MiB large page PDE into 512x 4K PTEs (identity by default). */
static gj_status_t
split_large_pde(u64 *pPde, gj_vaddr_t va)
{
    gj_paddr_t paLarge;
    gj_paddr_t paPt;
    u64 *pPt;
    u64 u64Flags;
    u32 i;

    if ((*pPde & PTE_P) == 0) {
        return GJ_ERR_NOENT;
    }
    if ((*pPde & PTE_PS) == 0) {
        return GJ_OK; /* already a table */
    }
    paLarge = *pPde & PTE_ADDR_MASK;
    u64Flags = *pPde & (PTE_W | PTE_U | PTE_NX);
    paPt = alloc_table();
    if (paPt == 0) {
        return GJ_ERR_NOMEM;
    }
    pPt = phys_to_virt(paPt);
    for (i = 0; i < 512; i++) {
        pPt[i] = (paLarge + (u64)i * GJ_PAGE_SIZE) | PTE_P | u64Flags;
    }
    *pPde = paPt | PTE_P | PTE_W | PTE_U;
    vmm_tlb_flush_page(va & ~((1ull << 21) - 1));
    return GJ_OK;
}

static u64 *
walk_pte(gj_vaddr_t va, int fCreate)
{
    u64 *pPde;
    u64 *pPt;
    u64 u64I1;
    gj_status_t st;
    gj_paddr_t pa;
    int fCow = fCreate && as_is_private();
    u64 u64I4 = (va >> 39) & 0x1ff;
    u64 u64I3 = (va >> 30) & 0x1ff;
    u64 u64I2 = (va >> 21) & 0x1ff;

    pPde = walk_pd_entry(va, fCreate);
    if (pPde == NULL) {
        return NULL;
    }
    /* COW PD entry if still shared with kernel before split/modify */
    if (fCow && g_pKernelPml4 != NULL && (g_pKernelPml4[u64I4] & PTE_P) != 0) {
        u64 *pKerPdpt = phys_to_virt(g_pKernelPml4[u64I4] & PTE_ADDR_MASK);

        if ((pKerPdpt[u64I3] & PTE_P) != 0 && (pKerPdpt[u64I3] & PTE_PS) == 0) {
            u64 *pKerPd = phys_to_virt(pKerPdpt[u64I3] & PTE_ADDR_MASK);
            u64 u64KerPde = pKerPd[u64I2];

            if ((*pPde & PTE_P) != 0 && *pPde == u64KerPde &&
                (u64KerPde & PTE_PS) == 0) {
                /* Shared PT pointer — clone PT before PTE write */
                if (cow_table_entry(pPde, u64KerPde) != GJ_OK) {
                    return NULL;
                }
            }
        }
    }
    if ((*pPde & PTE_P) != 0 && (*pPde & PTE_PS) != 0) {
        /* Split large page into private PT (always private after split) */
        st = split_large_pde(pPde, va);
        if (st != GJ_OK) {
            return NULL;
        }
    }
    if ((*pPde & PTE_P) == 0) {
        if (!fCreate) {
            return NULL;
        }
        pa = alloc_table();
        if (pa == 0) {
            return NULL;
        }
        *pPde = pa | PTE_P | PTE_W | PTE_U;
    } else if (fCow && g_pKernelPml4 != NULL && (g_pKernelPml4[u64I4] & PTE_P)) {
        u64 *pKerPdpt = phys_to_virt(g_pKernelPml4[u64I4] & PTE_ADDR_MASK);

        if ((pKerPdpt[u64I3] & PTE_P) != 0 && (pKerPdpt[u64I3] & PTE_PS) == 0) {
            u64 *pKerPd = phys_to_virt(pKerPdpt[u64I3] & PTE_ADDR_MASK);

            if (cow_table_entry(pPde, pKerPd[u64I2]) != GJ_OK) {
                return NULL;
            }
        }
    }
    if (fCreate) {
        ensure_user_flag(pPde);
    }
    pPt = phys_to_virt(*pPde & PTE_ADDR_MASK);
    u64I1 = (va >> 12) & 0x1ff;
    return &pPt[u64I1];
}

/*
 * Kernel image + BSS live in low identity VA (link at 1 MiB; embeds grow BSS).
 * Never install user/COW/RO leaves over that range — it bricks kmain stores.
 */
static int
va_in_kernel_identity(gj_vaddr_t va)
{
    extern char __kernel_start[];
    extern char __kernel_end[];
    gj_vaddr_t va0 =
        (gj_vaddr_t)__kernel_start & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
    gj_vaddr_t va1 = ((gj_vaddr_t)__kernel_end + GJ_PAGE_SIZE - 1) &
                     ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);

    return va >= va0 && va < va1;
}

gj_status_t
vmm_map_page(gj_vaddr_t va, gj_paddr_t pa, u32 u32Prot)
{
    u64 *pPte;
    u64 u64F;

    if ((va & (GJ_PAGE_SIZE - 1)) != 0 || (pa & (GJ_PAGE_SIZE - 1)) != 0) {
        return GJ_ERR_INVAL;
    }
    /*
     * Refuse user or non-identity maps over kernel image/BSS. Product user
     * band starts at 8 MiB; classic PE 0x400000 is no longer safe.
     */
    if (va_in_kernel_identity(va)) {
        if ((u32Prot & GJ_VMM_PROT_USER) != 0 || pa != (gj_paddr_t)va) {
            return GJ_ERR_PERM;
        }
        /* Kernel identity: always R/W, never user/COW. */
        u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_EXEC;
    }
    /* W^X filtered at Linux ABI unless CapJit; vmm maps as requested. */
    pPte = walk_pte(va, 1);
    if (pPte == NULL) {
        return GJ_ERR_NOMEM;
    }
    u64F = prot_to_flags(u32Prot);
    *pPte = (pa & PTE_ADDR_MASK) | u64F;
    /* Clear software COW bit on explicit map */
    *pPte &= ~PTE_COW;
    vmm_tlb_flush_page(va);
    return GJ_OK;
}

gj_status_t
vmm_unmap_page(gj_vaddr_t va)
{
    u64 *pPte;
    u64 u64Pte;
    gj_paddr_t pa;

    pPte = walk_pte(va, 0);
    if (pPte == NULL || (*pPte & PTE_P) == 0) {
        return GJ_ERR_NOENT;
    }
    u64Pte = *pPte;
    pa = u64Pte & PTE_ADDR_MASK;
    *pPte = 0;
    vmm_tlb_flush_page(va);
    /* COW shared frames only — private free stays with munmap callers */
    if ((u64Pte & PTE_U) != 0 && (u64Pte & PTE_COW) != 0 && pa != 0 &&
        pa != (gj_paddr_t)va) {
        release_leaf_frame(pa, 1);
    }
    return GJ_OK;
}

gj_status_t
vmm_protect_page(gj_vaddr_t va, u32 u32Prot)
{
    u64 *pPte;
    gj_paddr_t pa;

    /* JIT may pass W|X; non-JIT callers must filter first. */
    pPte = walk_pte(va, 0);
    if (pPte == NULL || (*pPte & PTE_P) == 0) {
        return GJ_ERR_NOENT;
    }
    /* Never demote kernel identity/BSS to RO or user. */
    if (va_in_kernel_identity(va)) {
        pa = *pPte & PTE_ADDR_MASK;
        if (pa == (gj_paddr_t)va || pa == 0) {
            *pPte = ((u64)va & PTE_ADDR_MASK) | PTE_P | PTE_W;
            vmm_tlb_flush_page(va);
            return GJ_OK;
        }
        return GJ_ERR_PERM;
    }
    pa = *pPte & PTE_ADDR_MASK;
    *pPte = pa | prot_to_flags(u32Prot);
    vmm_tlb_flush_page(va);
    return GJ_OK;
}

/*
 * Repair identity R/W leaves under the active CR3. Writes *pFixed when set.
 * Soft path: never panic; caller logs greppable ensure_identity_rw soft PASS.
 */
static gj_status_t
ensure_identity_rw_active(gj_vaddr_t va, size_t cb, u32 *pFixed)
{
    gj_vaddr_t vaEnd;
    gj_vaddr_t vaPage;
    u32 cFix = 0;

    if (pFixed != NULL) {
        *pFixed = 0;
    }
    if (cb == 0) {
        return GJ_OK;
    }
    vaPage = va & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
    vaEnd = (va + cb + GJ_PAGE_SIZE - 1) & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
    for (; vaPage < vaEnd; vaPage += GJ_PAGE_SIZE) {
        u64 *pPte = walk_pte(vaPage, 1);
        u64 u64Pte;
        gj_paddr_t pa;

        if (pPte == NULL) {
            return GJ_ERR_NOMEM;
        }
        u64Pte = *pPte;
        pa = u64Pte & PTE_ADDR_MASK;
        /*
         * Identity R/W kernel leaf: P|W, no U, no COW. Re-bind PA=VA when the
         * leaf was clobbered by a user/COW map or lost write permission.
         */
        if ((u64Pte & PTE_P) == 0 || (u64Pte & PTE_W) == 0 ||
            (u64Pte & PTE_U) != 0 || (u64Pte & PTE_COW) != 0 ||
            pa != (gj_paddr_t)vaPage) {
            *pPte = ((u64)vaPage & PTE_ADDR_MASK) | PTE_P | PTE_W;
            vmm_tlb_flush_page(vaPage);
            cFix++;
        }
    }
    if (pFixed != NULL) {
        *pFixed = cFix;
    }
    return GJ_OK;
}

gj_status_t
vmm_ensure_identity_rw(gj_vaddr_t va, size_t cb)
{
    u64 u64Saved;
    gj_status_t st;
    u32 cFixKer = 0;
    u32 cFixAct = 0;
    int fDual = 0;

    if (g_u64KernelCr3 == 0 || cb == 0) {
        kprintf("vmm: ensure_identity_rw soft reject va=0x%lx cb=%lu\n",
                (unsigned long)va, (unsigned long)cb);
        return GJ_ERR_INVAL;
    }
    g_cEnsureIdCall++;
    u64Saved = read_cr3();
    /* Repair kernel template first (shared lower tables when not COWed). */
    __asm__ volatile("mov %0, %%cr3" : : "r"(g_u64KernelCr3) : "memory");
    st = ensure_identity_rw_active(va, cb, &cFixKer);
    if (st != GJ_OK) {
        __asm__ volatile("mov %0, %%cr3" : : "r"(u64Saved) : "memory");
        kprintf("vmm: ensure_identity_rw soft nomem va=0x%lx cb=%lu\n",
                (unsigned long)va, (unsigned long)cb);
        return st;
    }
    /* Private AS may hold a COWed PT with RO leaves — repair active too. */
    if ((u64Saved & PTE_ADDR_MASK) != (g_u64KernelCr3 & PTE_ADDR_MASK)) {
        fDual = 1;
        __asm__ volatile("mov %0, %%cr3" : : "r"(u64Saved) : "memory");
        st = ensure_identity_rw_active(va, cb, &cFixAct);
        if (st != GJ_OK) {
            kprintf("vmm: ensure_identity_rw soft nomem dual va=0x%lx "
                    "cb=%lu\n",
                    (unsigned long)va, (unsigned long)cb);
            return st;
        }
    } else {
        __asm__ volatile("mov %0, %%cr3" : : "r"(u64Saved) : "memory");
    }
    g_cEnsureIdFix += cFixKer + cFixAct;
    /* Soft greppable always (fixed may be 0). */
    kprintf("vmm: ensure_identity_rw va=0x%lx cb=%lu fixed=%u ker=%u act=%u "
            "dual=%d total_fix=%u calls=%u soft PASS\n",
            (unsigned long)va, (unsigned long)cb, cFixKer + cFixAct, cFixKer,
            cFixAct, fDual, g_cEnsureIdFix, g_cEnsureIdCall);
    soft_inventory_maybe_once();
    return st;
}

gj_paddr_t
vmm_virt_to_phys(gj_vaddr_t va)
{
    u64 *pPte;

    pPte = walk_pte(va, 0);
    if (pPte == NULL || (*pPte & PTE_P) == 0) {
        return 0;
    }
    return (*pPte & PTE_ADDR_MASK) | (va & (GJ_PAGE_SIZE - 1));
}

u64
vmm_read_pte(gj_vaddr_t va)
{
    u64 *pPte;

    pPte = walk_pte(va, 0);
    if (pPte == NULL) {
        return 0;
    }
    return *pPte;
}

gj_vaddr_t
vmm_mmap_anon(u64 u64Hint, size_t cbLen, u32 u32Prot, int fFixed)
{
    size_t cbAligned;
    size_t cPages;
    size_t iPage;
    gj_vaddr_t vaBase;
    gj_paddr_t pa;
    gj_status_t st;
    u64 *pCursor = g_pAnonCursor;

    if (cbLen == 0 || pCursor == NULL) {
        return 0;
    }
    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = cbAligned / GJ_PAGE_SIZE;

    if (fFixed) {
        vaBase = (gj_vaddr_t)u64Hint;
        if ((vaBase & (GJ_PAGE_SIZE - 1)) != 0) {
            return 0;
        }
    } else {
        vaBase = (gj_vaddr_t)(*pCursor);
        if (u64Hint != 0 && u64Hint >= GJ_VMM_ANON_BASE &&
            u64Hint + cbAligned <= GJ_VMM_ANON_END) {
            vaBase = (gj_vaddr_t)u64Hint;
        }
        if (vaBase + cbAligned > GJ_VMM_ANON_END) {
            return 0;
        }
        if (vaBase == (gj_vaddr_t)(*pCursor)) {
            *pCursor += cbAligned;
        } else if (vaBase + cbAligned > *pCursor) {
            *pCursor = vaBase + cbAligned;
        }
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        pa = pmm_alloc();
        if (pa == 0) {
            while (iPage > 0) {
                iPage--;
                pa = vmm_virt_to_phys(vaBase + iPage * GJ_PAGE_SIZE);
                (void)vmm_unmap_page(vaBase + iPage * GJ_PAGE_SIZE);
                if (pa) {
                    pmm_free(pa & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1));
                }
            }
            return 0;
        }
        st = vmm_map_page(vaBase + iPage * GJ_PAGE_SIZE, pa,
                          u32Prot | GJ_VMM_PROT_USER);
        if (st != GJ_OK) {
            pmm_free(pa);
            while (iPage > 0) {
                iPage--;
                pa = vmm_virt_to_phys(vaBase + iPage * GJ_PAGE_SIZE);
                (void)vmm_unmap_page(vaBase + iPage * GJ_PAGE_SIZE);
                if (pa) {
                    pmm_free(pa & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1));
                }
            }
            return 0;
        }
        memset((void *)(vaBase + iPage * GJ_PAGE_SIZE), 0, GJ_PAGE_SIZE);
    }
    return vaBase;
}

gj_status_t
vmm_munmap(gj_vaddr_t va, size_t cbLen)
{
    size_t cbAligned;
    size_t cPages;
    size_t iPage;
    gj_paddr_t pa;
    u64 u64Pte;

    if ((va & (GJ_PAGE_SIZE - 1)) != 0 || cbLen == 0) {
        return GJ_ERR_INVAL;
    }
    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = cbAligned / GJ_PAGE_SIZE;
    for (iPage = 0; iPage < cPages; iPage++) {
        u64Pte = vmm_read_pte(va + iPage * GJ_PAGE_SIZE);
        pa = vmm_virt_to_phys(va + iPage * GJ_PAGE_SIZE);
        (void)vmm_unmap_page(va + iPage * GJ_PAGE_SIZE);
        /* Private non-COW: free here. COW already released in unmap_page. */
        if (pa && (u64Pte & (1ull << 9)) == 0) {
            pmm_free(pa & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1));
        }
    }
    return GJ_OK;
}

/*
 * Break a COW leaf under the active CR3: private copy + writable PTE.
 * Returns GJ_OK if break applied; GJ_ERR_NOENT if not a COW present page.
 */
gj_status_t
vmm_cow_break_page(gj_vaddr_t va)
{
    u64 *pPte;
    u64 u64Pte;
    gj_paddr_t paSrc;
    gj_paddr_t paDst;
    u64 *pSrc;
    u64 *pDst;
    u64 u64Flags;
    u64 u64Saved;

    if ((va & (GJ_PAGE_SIZE - 1)) != 0) {
        return GJ_ERR_INVAL;
    }
    pPte = walk_pte(va, 0);
    if (pPte == NULL) {
        return GJ_ERR_NOENT;
    }
    u64Pte = *pPte;
    if ((u64Pte & PTE_P) == 0 || (u64Pte & PTE_COW) == 0) {
        return GJ_ERR_NOENT;
    }
    paSrc = u64Pte & PTE_ADDR_MASK;
    paDst = pmm_alloc();
    if (paDst == 0) {
        kprintf("vmm: COW break va=0x%lx nomem live=%u\n", (unsigned long)va,
                g_cCowLive);
        return GJ_ERR_NOMEM;
    }
    u64Saved = read_cr3();
    __asm__ volatile("mov %0, %%cr3" : : "r"(g_u64KernelCr3) : "memory");
    pSrc = phys_to_virt(paSrc);
    pDst = phys_to_virt(paDst);
    memcpy(pDst, pSrc, GJ_PAGE_SIZE);
    __asm__ volatile("mov %0, %%cr3" : : "r"(u64Saved) : "memory");
    /* Keep U/NX; drop COW; grant W for the private frame */
    u64Flags = (u64Pte & ~(PTE_ADDR_MASK | PTE_COW)) | PTE_W | PTE_P;
    *pPte = (paDst & PTE_ADDR_MASK) | u64Flags;
    vmm_tlb_flush_page(va);
    g_cCowBreak++;
    /* Drop shared ref on old frame; free if last */
    {
        u32 u32Rem = cow_ref_drop(paSrc, 1);
        int fFreeOld = 0;

        if (u32Rem == 0) {
            pmm_free(paSrc);
            fFreeOld = 1;
            kprintf("vmm: COW break va=0x%lx pa=0x%lx old=0x%lx free_old "
                    "rem=%u live=%u frees=%u breaks=%u PASS\n",
                    (unsigned long)va, (unsigned long)paDst,
                    (unsigned long)paSrc, u32Rem, g_cCowLive, g_cCowFrees,
                    g_cCowBreak);
        } else {
            kprintf("vmm: COW break va=0x%lx pa=0x%lx old=0x%lx rem=%u "
                    "live=%u frees=%u breaks=%u PASS\n",
                    (unsigned long)va, (unsigned long)paDst,
                    (unsigned long)paSrc, u32Rem, g_cCowLive, g_cCowFrees,
                    g_cCowBreak);
        }
        (void)fFreeOld;
    }
    soft_inventory_maybe_once();
    return GJ_OK;
}

/*
 * Clone private user 4K pages from src AS into dst AS (fork).
 * Writable pages: share frame RO+COW in both AS (true COW); read-only pages
 * still get a private copy so exec/RO mappings stay isolated from demotion.
 * Skips fully-shared kernel subtrees and identity (PA == page VA) frames.
 */
gj_status_t
vmm_as_clone_user_pages(u64 u64SrcCr3, u64 u64DstCr3, u32 u32Max, u32 *pCopied)
{
    u64 *pSrcPml4;
    u64 *pKer;
    u64 u64Saved;
    u32 u32I4;
    u32 u32I3;
    u32 u32I2;
    u32 u32I1;
    u32 cCopy = 0;
    u32 cCow = 0;
    u32 cRoCopy = 0;
    u32 u32Limit = u32Max ? u32Max : 256u;

    if (pCopied != NULL) {
        *pCopied = 0;
    }
    if (u64SrcCr3 == 0 || u64DstCr3 == 0 ||
        (u64SrcCr3 & ~0xfffull) == (u64DstCr3 & ~0xfffull)) {
        kprintf("vmm: as_clone_user reject src=0x%lx dst=0x%lx\n",
                (unsigned long)u64SrcCr3, (unsigned long)u64DstCr3);
        return GJ_ERR_INVAL;
    }
    if (g_pKernelPml4 == NULL) {
        kprintf("vmm: as_clone_user reject no_template\n");
        return GJ_ERR_INVAL;
    }

    u64Saved = read_cr3();
    pSrcPml4 = phys_to_virt(u64SrcCr3 & PTE_ADDR_MASK);
    pKer = g_pKernelPml4;

    /* User canonical half: PML4[0..255] */
    for (u32I4 = 0; u32I4 < 256u && cCopy < u32Limit; u32I4++) {
        u64 *pPdpt;
        u64 *pKerPdpt = NULL;

        if ((pSrcPml4[u32I4] & PTE_P) == 0) {
            continue;
        }
        if (pSrcPml4[u32I4] == pKer[u32I4]) {
            continue; /* fully shared with kernel template */
        }
        if ((pSrcPml4[u32I4] & PTE_PS) != 0) {
            continue;
        }
        if ((pKer[u32I4] & PTE_P) != 0 && (pKer[u32I4] & PTE_PS) == 0) {
            pKerPdpt = phys_to_virt(pKer[u32I4] & PTE_ADDR_MASK);
        }
        pPdpt = phys_to_virt(pSrcPml4[u32I4] & PTE_ADDR_MASK);

        for (u32I3 = 0; u32I3 < 512u && cCopy < u32Limit; u32I3++) {
            u64 *pPd;
            u64 *pKerPd = NULL;

            if ((pPdpt[u32I3] & PTE_P) == 0 || (pPdpt[u32I3] & PTE_PS) != 0) {
                continue;
            }
            if (pKerPdpt != NULL && pPdpt[u32I3] == pKerPdpt[u32I3]) {
                continue;
            }
            if (pKerPdpt != NULL && (pKerPdpt[u32I3] & PTE_P) != 0 &&
                (pKerPdpt[u32I3] & PTE_PS) == 0) {
                pKerPd = phys_to_virt(pKerPdpt[u32I3] & PTE_ADDR_MASK);
            }
            pPd = phys_to_virt(pPdpt[u32I3] & PTE_ADDR_MASK);

            for (u32I2 = 0; u32I2 < 512u && cCopy < u32Limit; u32I2++) {
                u64 *pPt;
                u64 *pKerPt = NULL;

                if ((pPd[u32I2] & PTE_P) == 0 || (pPd[u32I2] & PTE_PS) != 0) {
                    continue;
                }
                if (pKerPd != NULL && pPd[u32I2] == pKerPd[u32I2]) {
                    continue;
                }
                if (pKerPd != NULL && (pKerPd[u32I2] & PTE_P) != 0 &&
                    (pKerPd[u32I2] & PTE_PS) == 0) {
                    pKerPt = phys_to_virt(pKerPd[u32I2] & PTE_ADDR_MASK);
                }
                pPt = phys_to_virt(pPd[u32I2] & PTE_ADDR_MASK);

                for (u32I1 = 0; u32I1 < 512u && cCopy < u32Limit; u32I1++) {
                    u64 u64Pte = pPt[u32I1];
                    gj_vaddr_t va;
                    gj_paddr_t paSrc;
                    gj_paddr_t paDst;
                    u32 u32Prot;
                    u64 *pDstPage;
                    u64 *pSrcPage;

                    if ((u64Pte & PTE_P) == 0) {
                        continue;
                    }
                    if ((u64Pte & PTE_U) == 0) {
                        continue;
                    }
                    if (pKerPt != NULL && u64Pte == pKerPt[u32I1]) {
                        continue;
                    }
                    va = ((u64)u32I4 << 39) | ((u64)u32I3 << 30) |
                         ((u64)u32I2 << 21) | ((u64)u32I1 << 12);
                    /*
                     * Clone product user maps only. Kernel identity+BSS lives
                     * in low VA (link at 1 MiB; BSS can extend past 0x400000
                     * once embeds grow) — never COW that range.
                     * Product band: ≥8 MiB … <2 GiB (INTERP/SO/PE high bases).
                     */
                    if (va < 0x0000000000800000ull ||
                        va >= 0x0000000080000000ull) {
                        continue;
                    }
                    paSrc = u64Pte & PTE_ADDR_MASK;
                    /* Identity leftovers after large-page split */
                    if (paSrc == (gj_paddr_t)va) {
                        continue;
                    }

                    /*
                     * Writable user page → true COW: demote parent to RO+COW,
                     * map same PA RO+COW into child (no eager copy).
                     * Read-only / non-W: private copy (keep parent RO alone).
                     * Never demote kernel identity/BSS (belt + product floor).
                     */
                    if (va_in_kernel_identity(va) ||
                        paSrc == (gj_paddr_t)va) {
                        continue;
                    }
                    if ((u64Pte & PTE_W) != 0 || (u64Pte & PTE_COW) != 0) {
                        u64 *pSrcPte;
                        u64 u64New;

                        /* Demote parent leaf under src CR3 */
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(u64SrcCr3)
                                         : "memory");
                        pSrcPte = walk_pte(va, 0);
                        if (pSrcPte != NULL && (*pSrcPte & PTE_P) != 0 &&
                            (*pSrcPte & PTE_U) != 0 &&
                            !va_in_kernel_identity(va)) {
                            u64New = (*pSrcPte | PTE_COW | PTE_P | PTE_U) &
                                     ~PTE_W;
                            *pSrcPte = u64New;
                            vmm_tlb_flush_page(va);
                        }
                        /* Track shared frame (parent + child) */
                        (void)cow_ref_share(paSrc);
                        /* Child: same frame, RO+COW */
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(u64DstCr3)
                                         : "memory");
                        u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_USER;
                        if ((u64Pte & PTE_NX) == 0) {
                            u32Prot |= GJ_VMM_PROT_EXEC;
                        }
                        if (vmm_map_page(va, paSrc, u32Prot) != GJ_OK) {
                            /* Undo child share only; never free (parent holds) */
                            (void)cow_ref_drop(paSrc, 1);
                            __asm__ volatile("mov %0, %%cr3"
                                             :
                                             : "r"(u64Saved)
                                             : "memory");
                            if (pCopied != NULL) {
                                *pCopied = cCopy;
                            }
                            return GJ_ERR_NOMEM;
                        }
                        {
                            u64 *pDstPte = walk_pte(va, 0);

                            if (pDstPte != NULL) {
                                *pDstPte =
                                    (*pDstPte | PTE_COW | PTE_P | PTE_U) &
                                    ~PTE_W;
                                vmm_tlb_flush_page(va);
                            }
                        }
                        cCopy++;
                        cCow++;
                        /* Back to src walk CR3 for next PTE via phys tables */
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(u64SrcCr3)
                                         : "memory");
                        continue;
                    }

                    paDst = pmm_alloc();
                    if (paDst == 0) {
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(u64Saved)
                                         : "memory");
                        if (pCopied != NULL) {
                            *pCopied = cCopy;
                        }
                        kprintf("vmm: as_clone_user nomem pages=%u cow=%u "
                                "rocopy=%u\n",
                                cCopy, cCow, cRoCopy);
                        return GJ_ERR_NOMEM;
                    }
                    /* Copy frame under kernel CR3 (identity/HHDM) */
                    __asm__ volatile("mov %0, %%cr3"
                                     :
                                     : "r"(g_u64KernelCr3)
                                     : "memory");
                    pSrcPage = phys_to_virt(paSrc);
                    pDstPage = phys_to_virt(paDst);
                    memcpy(pDstPage, pSrcPage, GJ_PAGE_SIZE);

                    u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_USER;
                    if ((u64Pte & PTE_NX) == 0) {
                        u32Prot |= GJ_VMM_PROT_EXEC;
                    }

                    __asm__ volatile("mov %0, %%cr3"
                                     :
                                     : "r"(u64DstCr3)
                                     : "memory");
                    if (vmm_map_page(va, paDst, u32Prot) != GJ_OK) {
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(g_u64KernelCr3)
                                         : "memory");
                        pmm_free(paDst);
                        __asm__ volatile("mov %0, %%cr3"
                                         :
                                         : "r"(u64Saved)
                                         : "memory");
                        if (pCopied != NULL) {
                            *pCopied = cCopy;
                        }
                        return GJ_ERR_NOMEM;
                    }
                    cCopy++;
                    cRoCopy++;
                    /* Restore src walk CR3 for next PTE read via phys_to_virt
                     * of tables (tables are accessed via phys, not walk_pte) */
                    __asm__ volatile("mov %0, %%cr3"
                                     :
                                     : "r"(u64SrcCr3)
                                     : "memory");
                }
            }
        }
    }

    __asm__ volatile("mov %0, %%cr3" : : "r"(u64Saved) : "memory");
    if (pCopied != NULL) {
        *pCopied = cCopy;
    }
    kprintf("vmm: as_clone_user src=0x%lx dst=0x%lx pages=%u cow=%u rocopy=%u "
            "cow_live=%u share_ok=%u share_full=%u tbl_cow=%u PASS\n",
            (unsigned long)u64SrcCr3, (unsigned long)u64DstCr3, cCopy, cCow,
            cRoCopy, g_cCowLive, g_cCowShareOk, g_cCowShareFull, g_cCowTable);
    soft_inventory_maybe_once();
    return GJ_OK;
}
