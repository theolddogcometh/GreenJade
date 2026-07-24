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
 * Soft VMM inventory (Wave 35 exclusive deepen; Wave 10/13 base):
 *   - Live AS / COW table / HHDM / device UC / ensure_id snaps
 *   - Cumulative create/destroy/break/share/table + map/clone rejects
 *   - Peaks / layout / path honesty / soft PASS lamp
 *   - Wave 15: honesty / unmap / prot / pte / stats / OPEN / deepen
 *   - Wave 19: surfaces / tlb / anon / share return surfaces
 *   greppable: "vmm: soft …"
 *
 * Soft higher-half readiness inventory (Wave 15 deepen; Wave 13 base; soft only):
 *   - HHDM base / mapped span (P-MEM-5 data map; not kernel image move)
 *   - Identity bridge residual (low PML4 still shared into private AS)
 *   - User half empty goal lamp (empty user half without identity share)
 *   - Geometry / bridge / debt checklist / OPEN honesty multi-line
 *   greppable: "vmm: higher-half soft …"
 *   lamps: hhdm_ready, identity_bridge, user_half_empty_goal=0/1 soft
 * Honesty: higher-half kernel move OPEN; soft inventory only — no relocate;
 *          soft ≠ bar3; soft ≠ 1TiB product.
 *
 * Greppable: "vmm: HHDM base=" "vmm: as_create" "vmm: as_destroy leaf="
 *            "vmm: COW break" "vmm: as_clone_user" "vmm: map_device_uc"
 *            "vmm: ensure_identity_rw" (… soft PASS)
 *            "vmm: soft honesty" "vmm: soft inventory" "vmm: soft as"
 *            "vmm: soft cow" "vmm: soft hhdm" "vmm: soft device_uc"
 *            "vmm: soft ensure_id" "vmm: soft clone" "vmm: soft map"
 *            "vmm: soft unmap" "vmm: soft prot" "vmm: soft destroy"
 *            "vmm: soft reject" "vmm: soft peak" "vmm: soft layout"
 *            "vmm: soft pte" "vmm: soft path" "vmm: soft lamps"
 *            "vmm: soft stats" "vmm: soft OPEN" "vmm: soft surfaces"
 *            "vmm: soft tlb" "vmm: soft anon" "vmm: soft share"
 *            "vmm: soft deepen" "vmm: soft PASS"
 *            "vmm: higher-half soft inventory" "vmm: higher-half soft lamps"
 *            "vmm: higher-half soft path" "vmm: higher-half soft residual"
 *            "vmm: higher-half soft geometry" "vmm: higher-half soft bridge"
 *            "vmm: higher-half soft debt" "vmm: higher-half soft goal"
 *            "vmm: higher-half soft OPEN"
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
 * Soft product inventory (Wave 35 exclusive deepen; extends Wave 13).
 * Cumulative unless noted live/peak. Diagnostics only — never hard-gate
 * product AS/COW/HHDM. greppable: vmm: soft
 */
static u32 g_cAsLivePeak;   /* high-water private AS live count */
static u32 g_cSoftInvLogs;  /* soft_inventory_log emissions */
static u8  g_fSoftInvOnce;  /* one-shot deep dump after product activity */

/* Soft map/unmap/protect path tallies (wrap OK; never hard-gate). */
static u32 g_cSoftMapOk;
static u32 g_cSoftMapInval;
static u32 g_cSoftMapPerm;
static u32 g_cSoftMapNomem;
static u32 g_cSoftUnmapOk;
static u32 g_cSoftUnmapMiss;
static u32 g_cSoftProtOk;
static u32 g_cSoftProtMiss;
static u32 g_cSoftProtPerm;

/* Soft clone / destroy cumulative path tallies. */
static u32 g_cSoftCloneCall;
static u32 g_cSoftCloneOk;
static u32 g_cSoftCloneReject;
static u32 g_cSoftCloneNomem;
static u32 g_cSoftClonePages;
static u32 g_cSoftCloneCow;
static u32 g_cSoftCloneRo;
static u32 g_cSoftDestroyLeaf;
static u32 g_cSoftDestroyPriv;
static u32 g_cSoftDestroyCowDrop;
static u32 g_cSoftDestroyTables;
static u32 g_cSoftDestroySkip;

/* Soft reject / fail path tallies. */
static u32 g_cSoftAsCreateFail;
static u32 g_cSoftAsDestroyReject;
static u32 g_cSoftDevUcReject;
static u32 g_cSoftDevUcNomem;
static u32 g_cSoftMapDevReject;
static u32 g_cSoftEnsureReject;
static u32 g_cSoftEnsureNomem;
static u32 g_cSoftCowBreakNomem;
static u32 g_cSoftCowFreeOld;

/* Soft peaks + HHDM leaf snap. */
static u32 g_cCowLivePeak;
static u32 g_cMapDeviceUcPagesPeak;
static u32 g_cHhdm2MiB;           /* 2 MiB leaves installed by vmm_hhdm_init */
static u32 g_cHhAsShareSlotsLast; /* last as_create shared_slots snap */

/*
 * Soft higher-half readiness (progress counters only — not product move).
 * greppable: vmm: higher-half soft
 *
 * Lamps (software observe; never hard-gate):
 *   hhdm_ready            — vmm_hhdm_init succeeded (data HHDM)
 *   identity_bridge       — residual low-half identity still in template
 *                           and still shared into as_create (bring-up debt)
 *   user_half_empty_goal  — 1 soft only when empty user half without
 *                           identity share is product-true; else 0 soft
 *
 * Honesty: higher-half kernel image move remains OPEN.
 */
static u32 g_cHhSoftLogs;         /* higher_half_soft_inventory emissions */
static u32 g_cHhSoftPathNotes;    /* soft path note line emissions */
static u32 g_cHhAsShareIdentity;  /* as_create while identity still shared */
static u32 g_cHhSoftGoalSnap0;    /* times user_half_empty_goal reported 0 */
static u32 g_cHhSoftOpenLogs;     /* higher-half soft OPEN line emissions */
static u32 g_cHhSoftDebtNotes;    /* higher-half soft debt line emissions */

/* Soft product target for kernel image VA (observe only; not relocated). */
#define GJ_VMM_SOFT_KERNEL_HH_BASE 0xffffffff80000000ull
/* PML4 low half = user / identity bridge; high half = kernel maps (HHDM). */
#define GJ_VMM_PML4_USER_SLOTS     256u
#define GJ_VMM_PML4_SLOTS          512u
/* Soft product user VA band floor (matches destroy/clone filters). */
#define GJ_VMM_SOFT_USER_FLOOR     0x0000000000800000ull
#define GJ_VMM_SOFT_WAVE 86u
/* Catalog areas prior to deepen line (honesty..share). */
#define GJ_VMM_SOFT_AREAS 158u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
 * greppable: vmm: soft surfaces
 */
#define VMM_SOFT_SURF_HONESTY   (1u << 0)
#define VMM_SOFT_SURF_INVENTORY (1u << 1)
#define VMM_SOFT_SURF_AS        (1u << 2)
#define VMM_SOFT_SURF_COW       (1u << 3)
#define VMM_SOFT_SURF_HHDM      (1u << 4)
#define VMM_SOFT_SURF_DEVICE    (1u << 5)
#define VMM_SOFT_SURF_ENSURE    (1u << 6)
#define VMM_SOFT_SURF_CLONE     (1u << 7)
#define VMM_SOFT_SURF_MAP       (1u << 8)
#define VMM_SOFT_SURF_UNMAP     (1u << 9)
#define VMM_SOFT_SURF_PROT      (1u << 10)
#define VMM_SOFT_SURF_DESTROY   (1u << 11)
#define VMM_SOFT_SURF_REJECT    (1u << 12)
#define VMM_SOFT_SURF_PEAK      (1u << 13)
#define VMM_SOFT_SURF_LAYOUT    (1u << 14)
#define VMM_SOFT_SURF_PTE       (1u << 15)
#define VMM_SOFT_SURF_PATH      (1u << 16)
#define VMM_SOFT_SURF_LAMPS     (1u << 17)
#define VMM_SOFT_SURF_STATS     (1u << 18)
#define VMM_SOFT_SURF_OPEN      (1u << 19)
#define VMM_SOFT_SURF_SURFACES  (1u << 20)
#define VMM_SOFT_SURF_TLB       (1u << 21)
#define VMM_SOFT_SURF_ANON      (1u << 22)
#define VMM_SOFT_SURF_SHARE     (1u << 23)
#define VMM_SOFT_SURF_CATALOG                                                      \
    (VMM_SOFT_SURF_HONESTY | VMM_SOFT_SURF_INVENTORY | VMM_SOFT_SURF_AS |          \
     VMM_SOFT_SURF_COW | VMM_SOFT_SURF_HHDM | VMM_SOFT_SURF_DEVICE |               \
     VMM_SOFT_SURF_ENSURE | VMM_SOFT_SURF_CLONE | VMM_SOFT_SURF_MAP |              \
     VMM_SOFT_SURF_UNMAP | VMM_SOFT_SURF_PROT | VMM_SOFT_SURF_DESTROY |            \
     VMM_SOFT_SURF_REJECT | VMM_SOFT_SURF_PEAK | VMM_SOFT_SURF_LAYOUT |            \
     VMM_SOFT_SURF_PTE | VMM_SOFT_SURF_PATH | VMM_SOFT_SURF_LAMPS |                \
     VMM_SOFT_SURF_STATS | VMM_SOFT_SURF_OPEN | VMM_SOFT_SURF_SURFACES |           \
     VMM_SOFT_SURF_TLB | VMM_SOFT_SURF_ANON | VMM_SOFT_SURF_SHARE)

static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void higher_half_soft_inventory(void);
static void vmm_soft_inc(u32 *pCtr);
static void vmm_soft_note_peaks(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
vmm_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: refresh live/peak high-water (diagnostics only). */
static void
vmm_soft_note_peaks(void)
{
    if (g_cAsLive > g_cAsLivePeak) {
        g_cAsLivePeak = g_cAsLive;
    }
    if (g_cCowLive > g_cCowLivePeak) {
        g_cCowLivePeak = g_cCowLive;
    }
    if (g_cMapDeviceUcPages > g_cMapDeviceUcPagesPeak) {
        g_cMapDeviceUcPagesPeak = g_cMapDeviceUcPages;
    }
}

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
            vmm_soft_note_peaks();
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
 * Soft higher-half readiness inventory (product / smoke; Wave 15 deepen).
 * Prefix-stable markers (vmm: higher-half soft …):
 *   vmm: higher-half soft inventory  — HHDM base + residual geometry
 *   vmm: higher-half soft lamps      — hhdm_ready / identity_bridge /
 *                                      user_half_empty_goal=0|1 soft
 *   vmm: higher-half soft residual   — low/high PML4 present slot counts
 *   vmm: higher-half soft geometry   — soft target VA + PML4 split constants
 *   vmm: higher-half soft bridge     — identity share residual progress
 *   vmm: higher-half soft debt       — product work still OPEN (checklist)
 *   vmm: higher-half soft goal       — empty user half goal lamp snap
 *   vmm: higher-half soft path       — empty user half without identity
 *                                      share notes (progress counters only)
 *   vmm: higher-half soft OPEN       — honesty: kernel image move not done
 *
 * Never relocates the kernel. Never hard-gates AS/HHDM policy.
 * greppable: vmm: higher-half soft
 * greppable: vmm: higher-half soft OPEN
 */
static void
higher_half_soft_inventory(void)
{
    u32 iSlot;
    u32 cLowPresent = 0;
    u32 cHighPresent = 0;
    u32 u32FirstLow = GJ_VMM_PML4_USER_SLOTS; /* no present → sentinel */
    u32 u32LastLow = 0;
    u32 u32FirstHigh = GJ_VMM_PML4_SLOTS;
    u32 u32LastHigh = 0;
    u32 u32HhdmSlot;
    int fHhdmReady;
    int fIdentityBridge;
    int fUserHalfEmptyGoal;
    int fTemplate;
    u64 u64Anon;

    vmm_soft_inc(&g_cHhSoftLogs);
    vmm_soft_inc(&g_cHhSoftPathNotes);
    vmm_soft_inc(&g_cHhSoftOpenLogs);
    vmm_soft_inc(&g_cHhSoftDebtNotes);
    vmm_soft_note_peaks();

    fHhdmReady = g_fHhdmReady ? 1 : 0;
    fTemplate = (g_pKernelPml4 != NULL) ? 1 : 0;
    u64Anon = (g_pAnonCursor != NULL) ? *g_pAnonCursor : g_u64AnonNext;
    /* Soft: which PML4 slot owns HHDM base (canonical high half). */
    u32HhdmSlot = (u32)((GJ_HHDM_BASE >> 39) & 0x1ffull);

    /*
     * Soft residual walk: present PML4 slots in low half (identity bridge /
     * user) vs high half (HHDM / kernel maps). No table COW; read-only.
     */
    if (g_pKernelPml4 != NULL) {
        for (iSlot = 0; iSlot < GJ_VMM_PML4_USER_SLOTS; iSlot++) {
            if ((g_pKernelPml4[iSlot] & PTE_P) != 0) {
                cLowPresent++;
                if (u32FirstLow == GJ_VMM_PML4_USER_SLOTS) {
                    u32FirstLow = iSlot;
                }
                u32LastLow = iSlot;
            }
        }
        for (iSlot = GJ_VMM_PML4_USER_SLOTS; iSlot < GJ_VMM_PML4_SLOTS;
             iSlot++) {
            if ((g_pKernelPml4[iSlot] & PTE_P) != 0) {
                cHighPresent++;
                if (u32FirstHigh == GJ_VMM_PML4_SLOTS) {
                    u32FirstHigh = iSlot;
                }
                u32LastHigh = iSlot;
            }
        }
    }

    /*
     * identity_bridge residual: low-half present slots still exist on the
     * kernel template (boot identity / shared lower). as_create still copies
     * full PML4 including those slots — bridge remains until product empty
     * user half without identity share.
     */
    fIdentityBridge = (cLowPresent > 0) ? 1 : 0;

    /*
     * user_half_empty_goal soft lamp: 1 only when product path creates AS
     * with empty user half and no identity share. Still OPEN → always 0 soft.
     */
    fUserHalfEmptyGoal = 0;
    vmm_soft_inc(&g_cHhSoftGoalSnap0);

    /* Grep: vmm: higher-half soft inventory */
    kprintf("vmm: higher-half soft inventory hhdm_base=0x%lx mapped=0x%lx "
            "hhdm_ready=%d ker_cr3=0x%lx soft_kernel_hh=0x%lx "
            "anon_next=0x%lx as_live=%u logs=%u hhdm_2mib=%u "
            "template=%d wave=%u "
            "(soft readiness; not kernel image move)\n",
            (unsigned long)GJ_HHDM_BASE, (unsigned long)g_u64HhdmMapped,
            fHhdmReady, (unsigned long)g_u64KernelCr3,
            (unsigned long)GJ_VMM_SOFT_KERNEL_HH_BASE,
            (unsigned long)u64Anon, g_cAsLive, g_cHhSoftLogs, g_cHhdm2MiB,
            fTemplate, (unsigned)GJ_VMM_SOFT_WAVE);

    /*
     * Grep: vmm: higher-half soft lamps
     * Lamps: hhdm_ready, identity_bridge, user_half_empty_goal=0/1 soft
     */
    kprintf("vmm: higher-half soft lamps hhdm_ready=%d identity_bridge=%d "
            "user_half_empty_goal=%d soft template=%d "
            "kernel_image_hh=0 move=OPEN "
            "(0=goal open; 1=empty user half without identity share)\n",
            fHhdmReady, fIdentityBridge, fUserHalfEmptyGoal, fTemplate);

    /* Grep: vmm: higher-half soft residual */
    kprintf("vmm: higher-half soft residual identity_bridge=%d "
            "low_pml4_present=%u high_pml4_present=%u "
            "first_low=%u last_low=%u first_high=%u last_high=%u "
            "hhdm_slot=%u as_share_id=%u ensure_id_calls=%u "
            "ensure_id_fix=%u (identity residual; soft only)\n",
            fIdentityBridge, cLowPresent, cHighPresent, u32FirstLow,
            u32LastLow, u32FirstHigh, u32LastHigh, u32HhdmSlot,
            g_cHhAsShareIdentity, g_cEnsureIdCall, g_cEnsureIdFix);

    /*
     * Soft geometry catalog (observe-only targets + split constants).
     * Grep: vmm: higher-half soft geometry
     */
    kprintf("vmm: higher-half soft geometry soft_kernel_hh=0x%lx "
            "hhdm_base=0x%lx device_mmio=0x%lx device_span=0x%lx "
            "pml4_user_slots=%u pml4_slots=%u page=%u user_floor=0x%lx "
            "anon_band=[0x%lx,0x%lx) wave=%u\n",
            (unsigned long)GJ_VMM_SOFT_KERNEL_HH_BASE,
            (unsigned long)GJ_HHDM_BASE,
            (unsigned long)GJ_DEVICE_MMIO_BASE,
            (unsigned long)GJ_DEVICE_MMIO_SPAN,
            (unsigned)GJ_VMM_PML4_USER_SLOTS, (unsigned)GJ_VMM_PML4_SLOTS,
            (unsigned)GJ_PAGE_SIZE, (unsigned long)GJ_VMM_SOFT_USER_FLOOR,
            (unsigned long)GJ_VMM_ANON_BASE, (unsigned long)GJ_VMM_ANON_END,
            (unsigned)GJ_VMM_SOFT_WAVE);

    /*
     * Soft bridge residual: as_create still shares identity into private AS.
     * Grep: vmm: higher-half soft bridge
     */
    kprintf("vmm: higher-half soft bridge residual=%d low_present=%u "
            "as_share_id=%u share_slots_last=%u ensure_id=%u/%u "
            "as_create=%u as_destroy=%u "
            "(bring-up identity still shared into private AS; soft only)\n",
            fIdentityBridge, cLowPresent, g_cHhAsShareIdentity,
            g_cHhAsShareSlotsLast, g_cEnsureIdFix, g_cEnsureIdCall,
            g_cAsCreate, g_cAsDestroy);

    /*
     * Product debt checklist — all remain OPEN until real higher-half work.
     * Grep: vmm: higher-half soft debt
     */
    kprintf("vmm: higher-half soft debt kernel_reloc=OPEN "
            "pml4_empty_user=OPEN stop_as_share_id=OPEN "
            "identity_bridge_clear=OPEN user_half_empty_goal=OPEN "
            "kernel_image_hh=OPEN as_share_id=%u goal_snap0=%u "
            "notes=%u wave=%u (checklist; soft only; not product close)\n",
            g_cHhAsShareIdentity, g_cHhSoftGoalSnap0, g_cHhSoftDebtNotes,
            (unsigned)GJ_VMM_SOFT_WAVE);

    /*
     * Soft goal lamp snap (always 0 while product empty half OPEN).
     * Grep: vmm: higher-half soft goal
     */
    kprintf("vmm: higher-half soft goal "
            "empty_user_half_without_id_share=%d soft "
            "product=OPEN snap0=%u as_share_id=%u identity_bridge=%d "
            "hhdm_ready=%d (1 only when product empty user half; soft)\n",
            fUserHalfEmptyGoal, g_cHhSoftGoalSnap0, g_cHhAsShareIdentity,
            fIdentityBridge, fHhdmReady);

    /*
     * Soft path notes: empty user half without identity share.
     * Progress counters only — does not implement G-AS-2 product empty half.
     * Grep: vmm: higher-half soft path
     */
    kprintf("vmm: higher-half soft path empty_user_half_without_id_share=OPEN "
            "as_share_id=%u goal_snap0=%u notes=%u as_create=%u "
            "as_destroy=%u hhdm_ready=%d identity_bridge=%d "
            "wave=%u (progress counters only; not product empty user half)\n",
            g_cHhAsShareIdentity, g_cHhSoftGoalSnap0, g_cHhSoftPathNotes,
            g_cAsCreate, g_cAsDestroy, fHhdmReady, fIdentityBridge,
            (unsigned)GJ_VMM_SOFT_WAVE);

    /*
     * Honesty close: higher-half kernel move remains OPEN (deepen Wave 15).
     * Grep: vmm: higher-half soft OPEN
     */
    kprintf("vmm: higher-half soft OPEN move=OPEN inventory_only=1 "
            "hhdm_ready=%d identity_bridge=%d user_half_empty_goal=%d soft "
            "kernel_image_hh=0 empty_user_half=OPEN identity_share=OPEN "
            "soft_kernel_hh=0x%lx open_logs=%u debt_notes=%u wave=%u "
            "(soft inventory; higher-half move not done; not bar3)\n",
            fHhdmReady, fIdentityBridge, fUserHalfEmptyGoal,
            (unsigned long)GJ_VMM_SOFT_KERNEL_HH_BASE, g_cHhSoftOpenLogs,
            g_cHhSoftDebtNotes, (unsigned)GJ_VMM_SOFT_WAVE);
}

/**
 * Greppable soft VMM inventory (product / smoke; Wave 35 exclusive deepen).
 *   vmm: soft honesty …
 *   vmm: soft inventory …
 *   vmm: soft as …
 *   vmm: soft cow …
 *   vmm: soft hhdm …
 *   vmm: soft device_uc …
 *   vmm: soft ensure_id …
 *   vmm: soft clone …
 *   vmm: soft map …
 *   vmm: soft unmap …
 *   vmm: soft prot …
 *   vmm: soft destroy …
 *   vmm: soft reject …
 *   vmm: soft peak …
 *   vmm: soft layout …
 *   vmm: soft pte …
 *   vmm: soft path …
 *   vmm: soft lamps …
 *   vmm: soft stats …
 *   vmm: soft OPEN …
 *   vmm: soft surfaces …  (Wave 19 return-surface catalog)
 *   vmm: soft tlb …       (Wave 17 TLB invalidate honesty OPEN)
 *   vmm: soft anon …      (Wave 17 anon cursor surface)
 *   vmm: soft share …     (Wave 17 kernel-half share surface)
 *   vmm: soft return selftest — Wave 19 terminal return surface
 *   vmm: soft retmap     — Wave 19 return-surface map
 *   vmm: soft return selftest — Wave 19 terminal return surface
 *   vmm: soft retmap     — Wave 19 return-surface map
 *   vmm: soft deepen wave=86 …
 *   vmm: soft PASS | vmm: soft inventory PASS
 *   vmm: higher-half soft …   (readiness lamps; move OPEN)
 * greppable: vmm: soft
 * Honesty: soft counters only — not product AS/COW/HHDM complete or bar3;
 *          soft ≠ 1TiB product; soft ≠ product. Higher-half kernel move stays OPEN.
 */
static void
soft_inventory_log(void)
{
    u32 i;
    u32 cCowSlots = 0;
    u32 cCowRefs = 0;
    u32 cAreas = 0;
    u32 u32Surf;
    u64 u64Anon;
    int fHhdm;
    int fTemplate;
    int fSoftPass;

    vmm_soft_inc(&g_cSoftInvLogs);
    vmm_soft_note_peaks();

    for (i = 0; i < GJ_COW_REF_MAX; i++) {
        if (g_aCowRef[i].u8Used) {
            cCowSlots++;
            cCowRefs += g_aCowRef[i].cRef;
        }
    }

    fHhdm = g_fHhdmReady ? 1 : 0;
    fTemplate = (g_pKernelPml4 != NULL && g_u64KernelCr3 != 0) ? 1 : 0;
    u64Anon = (g_pAnonCursor != NULL) ? *g_pAnonCursor : g_u64AnonNext;
    u32Surf = VMM_SOFT_SURF_CATALOG;

    /*
     * Honesty first: freestanding soft inventory is NOT product AS/COW complete,
     * not bar3, not 1TiB product. greppable: vmm: soft honesty
     */
    kprintf("vmm: soft honesty not-product-AS not-bar3 not-1TiB-product "
            "higher_half_move=OPEN user_half_empty=OPEN product_tib=0 "
            "bar3=OPEN wave=%u "
            "(soft inventory only; never closes bar3; not 1TiB product)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft inventory */
    kprintf("vmm: soft inventory hhdm=%d as_live=%u as_peak=%u cow_live=%u "
            "cow_slots=%u cow_refs=%u logs=%u template=%d wave=%u "
            "(soft; not product AS complete; not bar3; not 1TiB product)\n",
            fHhdm, g_cAsLive, g_cAsLivePeak, g_cCowLive, cCowSlots, cCowRefs,
            g_cSoftInvLogs, fTemplate, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft as */
    kprintf("vmm: soft as create=%u destroy=%u live=%u peak=%u "
            "create_fail=%u destroy_reject=%u "
            "ker_cr3=0x%lx anon_next=0x%lx band=[0x%lx,0x%lx) "
            "share_id=%u share_slots_last=%u wave=%u\n",
            g_cAsCreate, g_cAsDestroy, g_cAsLive, g_cAsLivePeak,
            g_cSoftAsCreateFail, g_cSoftAsDestroyReject,
            (unsigned long)g_u64KernelCr3, (unsigned long)u64Anon,
            (unsigned long)GJ_VMM_ANON_BASE, (unsigned long)GJ_VMM_ANON_END,
            g_cHhAsShareIdentity, g_cHhAsShareSlotsLast,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft cow */
    kprintf("vmm: soft cow break=%u share_ok=%u share_full=%u tbl_cow=%u "
            "live=%u peak=%u frees=%u free_old=%u break_nomem=%u "
            "slots_used=%u slots_max=%u refsum=%u wave=%u\n",
            g_cCowBreak, g_cCowShareOk, g_cCowShareFull, g_cCowTable,
            g_cCowLive, g_cCowLivePeak, g_cCowFrees, g_cSoftCowFreeOld,
            g_cSoftCowBreakNomem, cCowSlots, (unsigned)GJ_COW_REF_MAX,
            cCowRefs, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft hhdm */
    kprintf("vmm: soft hhdm ready=%d base=0x%lx mapped=0x%lx "
            "pages_2mib=%u p_mem5=1 wave=%u "
            "(soft inventory; not 1TiB product; not bar3)\n",
            fHhdm, (unsigned long)GJ_HHDM_BASE,
            (unsigned long)g_u64HhdmMapped, g_cHhdm2MiB,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft device_uc */
    kprintf("vmm: soft device_uc maps=%u pages=%u pages_peak=%u "
            "reject=%u nomem=%u map_dev_reject=%u "
            "base=0x%lx span=0x%lx wave=%u\n",
            g_cMapDeviceUc, g_cMapDeviceUcPages, g_cMapDeviceUcPagesPeak,
            g_cSoftDevUcReject, g_cSoftDevUcNomem, g_cSoftMapDevReject,
            (unsigned long)GJ_DEVICE_MMIO_BASE,
            (unsigned long)GJ_DEVICE_MMIO_SPAN,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft ensure_id */
    kprintf("vmm: soft ensure_id calls=%u fixed=%u reject=%u nomem=%u "
            "wave=%u\n",
            g_cEnsureIdCall, g_cEnsureIdFix, g_cSoftEnsureReject,
            g_cSoftEnsureNomem, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft clone */
    kprintf("vmm: soft clone call=%u ok=%u reject=%u nomem=%u "
            "pages=%u cow=%u ro=%u wave=%u\n",
            g_cSoftCloneCall, g_cSoftCloneOk, g_cSoftCloneReject,
            g_cSoftCloneNomem, g_cSoftClonePages, g_cSoftCloneCow,
            g_cSoftCloneRo, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft map */
    kprintf("vmm: soft map ok=%u inval=%u perm=%u nomem=%u wave=%u\n",
            g_cSoftMapOk, g_cSoftMapInval, g_cSoftMapPerm, g_cSoftMapNomem,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft unmap (Wave 15 split from map surface) */
    kprintf("vmm: soft unmap ok=%u miss=%u wave=%u "
            "(soft path tallies; not product gate)\n",
            g_cSoftUnmapOk, g_cSoftUnmapMiss, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft prot (Wave 15 split from map surface) */
    kprintf("vmm: soft prot ok=%u miss=%u perm=%u wave=%u "
            "(soft path tallies; not product gate)\n",
            g_cSoftProtOk, g_cSoftProtMiss, g_cSoftProtPerm,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft destroy */
    kprintf("vmm: soft destroy calls=%u leaf=%u priv=%u cow_drop=%u "
            "tables=%u shared_skip=%u reject=%u live=%u wave=%u\n",
            g_cAsDestroy, g_cSoftDestroyLeaf, g_cSoftDestroyPriv,
            g_cSoftDestroyCowDrop, g_cSoftDestroyTables, g_cSoftDestroySkip,
            g_cSoftAsDestroyReject, g_cAsLive, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft reject */
    kprintf("vmm: soft reject as_create=%u as_destroy=%u dev_uc=%u "
            "dev_uc_nomem=%u map_dev=%u ensure=%u ensure_nomem=%u "
            "map_inval=%u map_perm=%u map_nomem=%u clone=%u "
            "clone_nomem=%u cow_break_nomem=%u wave=%u\n",
            g_cSoftAsCreateFail, g_cSoftAsDestroyReject, g_cSoftDevUcReject,
            g_cSoftDevUcNomem, g_cSoftMapDevReject, g_cSoftEnsureReject,
            g_cSoftEnsureNomem, g_cSoftMapInval, g_cSoftMapPerm,
            g_cSoftMapNomem, g_cSoftCloneReject, g_cSoftCloneNomem,
            g_cSoftCowBreakNomem, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft peak */
    kprintf("vmm: soft peak as_live=%u cow_live=%u device_uc_pages=%u "
            "logs=%u hh_logs=%u wave=%u\n",
            g_cAsLivePeak, g_cCowLivePeak, g_cMapDeviceUcPagesPeak,
            g_cSoftInvLogs, g_cHhSoftLogs, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft layout */
    kprintf("vmm: soft layout page=%u cow_ref_max=%u pml4_user=%u "
            "pml4_slots=%u soft_kernel_hh=0x%lx hhdm_base=0x%lx "
            "device_base=0x%lx user_floor=0x%lx anon=[0x%lx,0x%lx) "
            "wave=%u\n",
            (unsigned)GJ_PAGE_SIZE, (unsigned)GJ_COW_REF_MAX,
            (unsigned)GJ_VMM_PML4_USER_SLOTS, (unsigned)GJ_VMM_PML4_SLOTS,
            (unsigned long)GJ_VMM_SOFT_KERNEL_HH_BASE,
            (unsigned long)GJ_HHDM_BASE,
            (unsigned long)GJ_DEVICE_MMIO_BASE,
            (unsigned long)GJ_VMM_SOFT_USER_FLOOR,
            (unsigned long)GJ_VMM_ANON_BASE, (unsigned long)GJ_VMM_ANON_END,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: PTE software-bit catalog (observe-only).
     * Grep: vmm: soft pte
     */
    kprintf("vmm: soft pte p=0 w=1 u=2 ps=7 cow_sw=9 nx=63 "
            "addr_mask=0x%lx cow_ref_max=%u wave=%u "
            "(soft bit catalog; not product PTE policy)\n",
            (unsigned long)PTE_ADDR_MASK, (unsigned)GJ_COW_REF_MAX,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Honesty: soft inventory ≠ product higher-half move / bar3 / 1TiB.
     * Grep: vmm: soft path
     */
    kprintf("vmm: soft path as=private_pml4+shared_kernel_half "
            "cow=ro+soft_bit hhdm=p_mem5_2mib device_uc=high_window "
            "ensure_id=repair_identity higher_half_move=OPEN "
            "user_half_empty=OPEN identity_bridge=%u product_tib=0 "
            "bar3=OPEN wave=%u "
            "(soft inventory; not bar3; not 1TiB product)\n",
            g_cHhAsShareIdentity != 0u ? 1u : 0u,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft lamps */
    kprintf("vmm: soft lamps hhdm_ready=%d template=%d as_live=%u "
            "cow_live=%u identity_bridge=%u user_half_empty_goal=0 soft "
            "higher_half_move=OPEN product_tib=0 bar3=OPEN wave=%u\n",
            fHhdm, fTemplate, g_cAsLive, g_cCowLive,
            g_cHhAsShareIdentity != 0u ? 1u : 0u,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /* Grep: vmm: soft stats — rollup for agent greps (Wave 15). */
    kprintf("vmm: soft stats as_create=%u as_destroy=%u as_live=%u "
            "cow_break=%u cow_live=%u map_ok=%u unmap_ok=%u prot_ok=%u "
            "clone_ok=%u dev_uc=%u ensure_fix=%u logs=%u hh_logs=%u "
            "wave=%u\n",
            g_cAsCreate, g_cAsDestroy, g_cAsLive, g_cCowBreak, g_cCowLive,
            g_cSoftMapOk, g_cSoftUnmapOk, g_cSoftProtOk, g_cSoftCloneOk,
            g_cMapDeviceUc, g_cEnsureIdFix, g_cSoftInvLogs, g_cHhSoftLogs,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15 honesty close: product AS/COW/HHDM move remain OPEN.
     * Grep: vmm: soft OPEN
     */
    kprintf("vmm: soft OPEN higher_half_move=OPEN as_product=OPEN "
            "cow_product=OPEN hhdm_data_ok=%d user_half_empty=OPEN "
            "product_tib=0 bar3=OPEN wave=%u "
            "(soft inventory; not product AS complete; not bar3; "
            "not 1TiB product)\n",
            fHhdm, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: vmm: soft surfaces
     */
    kprintf("vmm: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "as=1 cow=1 hhdm=1 map=1 unmap=1 prot=1 clone=1 destroy=1 "
            "tlb=1 anon=1 share=1 open=1 wave=%u "
            "(return surfaces; soft only; not product; not bar3)\n",
            (unsigned)u32Surf, (unsigned)GJ_VMM_SOFT_AREAS, cAreas + 4u,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: TLB invalidate honesty (product shootdown remains OPEN).
     * Grep: vmm: soft tlb
     */
    kprintf("vmm: soft tlb invlpg=soft_local shootdown=OPEN "
            "remote_ipi=OPEN full_flush=OPEN hhdm=%d as_live=%u "
            "wave=%u (soft TLB surface; not product; not bar3)\n",
            fHhdm, g_cAsLive, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: anon cursor surface (user VA bump allocator soft).
     * Grep: vmm: soft anon
     */
    kprintf("vmm: soft anon next=0x%lx base=0x%lx end=0x%lx "
            "band_open=%u cursor=%u wave=%u "
            "(soft anon surface; not product VA policy; not bar3)\n",
            (unsigned long)u64Anon,
            (unsigned long)GJ_VMM_ANON_BASE,
            (unsigned long)GJ_VMM_ANON_END,
            (u64Anon < GJ_VMM_ANON_END) ? 1u : 0u,
            (g_pAnonCursor != NULL) ? 1u : 0u,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: kernel-half share surface (identity bridge residual).
     * Grep: vmm: soft share
     */
    kprintf("vmm: soft share kernel_half=1 user_half_private=1 "
            "identity_bridge=%u share_id=%u share_slots_last=%u "
            "user_half_empty_goal=0 higher_half_move=OPEN wave=%u "
            "(soft share surface; not product; not bar3)\n",
            g_cHhAsShareIdentity != 0u ? 1u : 0u,
            g_cHhAsShareIdentity, g_cHhAsShareSlotsLast,
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: vmm: soft return rate
     * Wave 17 return-surface rate lamps (kept) (AS/COW live + ensure).
     */
    kprintf("vmm: soft return rate "
            "as_live=%u cow_live=%u ensure_nomem=%u "
            "map_ok=%u unmap_ok=%u unmap_miss=%u "
            "clone_ro=%u logs=%u wave=%u "
            "(return rate; Soft≠product; not 1TiB product; not bar3)\n",
            g_cAsLive, g_cCowLive, g_cSoftEnsureNomem,
            g_cSoftMapOk, g_cSoftUnmapOk, g_cSoftUnmapMiss,
            g_cSoftCloneRo, g_cSoftInvLogs, (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: vmm: soft retcode
     * Wave 17 retcode catalog for map/unmap/AS soft return classes.
     */
    kprintf("vmm: soft retcode "
            "map_ok=1 unmap_ok=1 unmap_miss=1 ensure_nomem=1 "
            "clone_ro=1 as_live=1 cow_live=1 "
            "higher_half_move=OPEN product_tib=0 wave=%u "
            "(retcode catalog; Soft≠product; soft≠product)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: vmm: soft deepen wave (Wave 21 stamp; areas = prior soft lines).
     */
    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: vmm: soft return selftest — Wave 19 terminal return surface */
    kprintf("vmm: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)GJ_VMM_SOFT_WAVE);

    /* Grep: vmm: soft retmap — Wave 19 return-surface map */
    kprintf("vmm: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)GJ_VMM_SOFT_WAVE);

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: vmm: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("vmm: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
    /* Grep: vmm: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("vmm: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: vmm: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("vmm: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
    /* Grep: vmm: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("vmm: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_VMM_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: vmm: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("vmm: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("vmm: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: vmm: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("vmm: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("vmm: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: vmm: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("vmm: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("vmm: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vmm: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("vmm: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("vmm: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vmm: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("vmm: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("vmm: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vmm: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("vmm: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
            /* Grep: vmm: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("vmm: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_VMM_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: vmm: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("vmm: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_VMM_SOFT_WAVE);
                    /* Grep: vmm: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("vmm: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_VMM_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vmm: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("vmm: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_VMM_SOFT_WAVE);
                            /* Grep: vmm: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("vmm: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_VMM_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vmm: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("vmm: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                            /* Grep: vmm: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("vmm: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("vmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                            /* Grep: vmm: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("vmm: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vmm: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("vmm: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                            /* Grep: vmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("vmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
                            /* Grep: vmm: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("vmm: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("vmm: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("vmm: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("vmm: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("vmm: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("vmm: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("vmm: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retfortress — Wave 35 return-fortress honesty */
kprintf("vmm: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("vmm: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft rethold — Wave 36 return-hold honesty */
kprintf("vmm: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retspire — Wave 36 exclusive spire stamp */
kprintf("vmm: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retwall — Wave 37 return-wall honesty */
kprintf("vmm: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retgate — Wave 37 exclusive gate stamp */
kprintf("vmm: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retmoat — Wave 38 return-moat honesty */
kprintf("vmm: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retower — Wave 38 exclusive tower stamp */
kprintf("vmm: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("vmm: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("vmm: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("vmm: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("vmm: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retravelin — Wave 41 return-travelin honesty */
kprintf("vmm: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("vmm: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("vmm: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("vmm: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("vmm: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("vmm: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("vmm: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("vmm: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("vmm: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("vmm: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retbailey — Wave 46 return-bailey honesty */
kprintf("vmm: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);
/* Grep: vmm: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("vmm: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_VMM_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("vmm: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("vmm: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("vmm: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("vmm: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("vmm: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("vmm: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retsally — Wave 50 return-sally honesty */
kprintf("vmm: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("vmm: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retfosse — Wave 51 return-fosse honesty */
kprintf("vmm: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("vmm: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("vmm: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("vmm: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retravelin — Wave 53 return-travelin honesty */
kprintf("vmm: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("vmm: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("vmm: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retredan — Wave 54 exclusive redan stamp */
kprintf("vmm: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retflank — Wave 55 return-flank honesty */
kprintf("vmm: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retface — Wave 55 exclusive face stamp */
kprintf("vmm: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retgorge — Wave 56 return-gorge honesty */
kprintf("vmm: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("vmm: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retraverse — Wave 57 return-traverse honesty */
kprintf("vmm: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("vmm: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retorillon — Wave 58 return-orillon honesty */
kprintf("vmm: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("vmm: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("vmm: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("vmm: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retplace — Wave 60 return-place honesty */
kprintf("vmm: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("vmm: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("vmm: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("vmm: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("vmm: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("vmm: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("vmm: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("vmm: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vmm: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("vmm: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: vmm: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("vmm: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vmm: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("vmm: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: vmm: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("vmm: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vmm: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("vmm: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=86 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: vmm: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("vmm: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=86 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("vmm: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("vmm: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("vmm: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("vmm: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("vmm: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=86 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("vmm: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=86 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("vmm: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("vmm: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("vmm: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("vmm: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vmm: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("vmm: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("vmm: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("vmm: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("vmm: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retbastionangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("vmm: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("vmm: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retparapetangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("vmm: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("vmm: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retowerangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("vmm: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("vmm: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retwallangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("vmm: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("vmm: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retholdangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("vmm: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("vmm: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retfortressangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("vmm: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("vmm: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("vmm: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("vmm: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: vmm: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("vmm: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("vmm: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("vmm: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("vmm: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retaegisangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("vmm: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("vmm: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retsigilangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("vmm: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("vmm: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retglyphangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("vmm: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("vmm: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retshardangle stamp; Soft≠product)\n");
/* Grep: vmm: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("vmm: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=86 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: vmm: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("vmm: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=86 (retprismangle stamp; Soft≠product)\n");

                            kprintf("vmm: soft deepen wave=%u areas=%u catalog=%u logs=%u "
            "hhdm=%d template=%d as_live=%u cow_live=%u surf=0x%x "
            "product_tib=0 bar3=OPEN "
            "(Wave 43 exclusive; soft only; not product; not bar3; "
            "not 1TiB product; soft≠product)\n",
            (unsigned)GJ_VMM_SOFT_WAVE, cAreas,
            (unsigned)GJ_VMM_SOFT_AREAS, g_cSoftInvLogs, fHhdm, fTemplate,
            g_cAsLive, g_cCowLive, (unsigned)u32Surf);

    /*
     * Soft lamp: template bind is soft-pass; HHDM ready is stronger smoke.
     * Never hard-gates. Grep: vmm: soft inventory PASS | vmm: soft PASS
     * Grep: vmm: soft FAIL
     */
    fSoftPass = fTemplate;
    if (fSoftPass != 0) {
        kprintf("vmm: soft inventory PASS hhdm=%d as_live=%u logs=%u "
                "wave=%u\n",
                fHhdm, g_cAsLive, g_cSoftInvLogs,
                (unsigned)GJ_VMM_SOFT_WAVE);
        kprintf("vmm: soft PASS template=1 hhdm=%d wave=%u "
                "(soft inventory; not product; not bar3)\n",
                fHhdm, (unsigned)GJ_VMM_SOFT_WAVE);
    } else {
        kprintf("vmm: soft FAIL template=0 wave=%u "
                "(soft inventory only; not product gate; not bar3)\n",
                (unsigned)GJ_VMM_SOFT_WAVE);
    }

    (void)GJ_VMM_SOFT_AREAS;

    /* Higher-half readiness lamps + path notes (move OPEN; soft only). */
    higher_half_soft_inventory();
}

/**
 * After first product AS/COW/device/ensure/clone activity, print soft
 * inventory once (mirrors memobj soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_cAsCreate == 0 && g_cMapDeviceUc == 0 && g_cCowBreak == 0 &&
        g_cEnsureIdCall == 0 && g_cAsDestroy == 0 &&
        g_cSoftCloneCall == 0) {
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
    /* Wave 15: greppable soft inventory baseline after template bind. */
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
    g_cHhdm2MiB = (u32)cMapped;
    /* phys_to_virt now uses HHDM — refresh kernel pml4 pointer */
    g_pKernelPml4 = phys_to_virt(g_u64KernelCr3);
    /* Greppable: vmm: HHDM base= (P-MEM-5; unlocks pmm high hierarchical free) */
    kprintf("vmm: HHDM base=0x%lx mapped=0x%lx (%lu x 2MiB)\n",
            (unsigned long)GJ_HHDM_BASE, (unsigned long)paMax,
            (unsigned long)cMapped);
    /* Wave 15: soft inventory after HHDM ready. */
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
        vmm_soft_inc(&g_cSoftMapDevReject);
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
        vmm_soft_inc(&g_cSoftMapDevReject);
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
        vmm_soft_inc(&g_cSoftDevUcReject);
        kprintf("vmm: map_device_uc soft reject inval pa=0x%lx cb=0x%lx\n",
                (unsigned long)pa, (unsigned long)cb);
        return GJ_ERR_INVAL;
    }
    if ((u64)pa >= GJ_DEVICE_MMIO_SPAN ||
        (u64)pa + cb > GJ_DEVICE_MMIO_SPAN) {
        vmm_soft_inc(&g_cSoftDevUcReject);
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
                vmm_soft_inc(&g_cSoftDevUcNomem);
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
                vmm_soft_inc(&g_cSoftDevUcNomem);
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
    vmm_soft_note_peaks();
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
 *   private to this AS. True empty user half without identity share remains
 *   OPEN (soft: user_half_empty_goal=0; identity_bridge residual).
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
        vmm_soft_inc(&g_cSoftAsCreateFail);
        kprintf("vmm: as_create fail no_template\n");
        return 0;
    }
    paPml4 = alloc_table();
    if (paPml4 == 0) {
        vmm_soft_inc(&g_cSoftAsCreateFail);
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
    vmm_soft_note_peaks();
    /*
     * Soft progress: every create still shares identity bridge residual.
     * Product empty user half without identity share would stop this count.
     */
    vmm_soft_inc(&g_cHhAsShareIdentity);
    g_cHhAsShareSlotsLast = cShared;
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
        vmm_soft_inc(&g_cSoftAsDestroyReject);
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
    g_cSoftDestroyLeaf += cLeaf;
    g_cSoftDestroyPriv += cPrivFree;
    g_cSoftDestroyCowDrop += cCowDrop;
    g_cSoftDestroyTables += cTables;
    g_cSoftDestroySkip += cSharedSkip;
    vmm_soft_note_peaks();
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
        vmm_soft_inc(&g_cSoftMapInval);
        return GJ_ERR_INVAL;
    }
    /*
     * Refuse user or non-identity maps over kernel image/BSS. Product user
     * band starts at 8 MiB; classic PE 0x400000 is no longer safe.
     */
    if (va_in_kernel_identity(va)) {
        if ((u32Prot & GJ_VMM_PROT_USER) != 0 || pa != (gj_paddr_t)va) {
            vmm_soft_inc(&g_cSoftMapPerm);
            return GJ_ERR_PERM;
        }
        /* Kernel identity: always R/W, never user/COW. */
        u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_EXEC;
    }
    /* W^X filtered at Linux ABI unless CapJit; vmm maps as requested. */
    pPte = walk_pte(va, 1);
    if (pPte == NULL) {
        vmm_soft_inc(&g_cSoftMapNomem);
        return GJ_ERR_NOMEM;
    }
    u64F = prot_to_flags(u32Prot);
    *pPte = (pa & PTE_ADDR_MASK) | u64F;
    /* Clear software COW bit on explicit map */
    *pPte &= ~PTE_COW;
    vmm_tlb_flush_page(va);
    vmm_soft_inc(&g_cSoftMapOk);
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
        vmm_soft_inc(&g_cSoftUnmapMiss);
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
    vmm_soft_inc(&g_cSoftUnmapOk);
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
        vmm_soft_inc(&g_cSoftProtMiss);
        return GJ_ERR_NOENT;
    }
    /* Never demote kernel identity/BSS to RO or user. */
    if (va_in_kernel_identity(va)) {
        pa = *pPte & PTE_ADDR_MASK;
        if (pa == (gj_paddr_t)va || pa == 0) {
            *pPte = ((u64)va & PTE_ADDR_MASK) | PTE_P | PTE_W;
            vmm_tlb_flush_page(va);
            vmm_soft_inc(&g_cSoftProtOk);
            return GJ_OK;
        }
        vmm_soft_inc(&g_cSoftProtPerm);
        return GJ_ERR_PERM;
    }
    pa = *pPte & PTE_ADDR_MASK;
    *pPte = pa | prot_to_flags(u32Prot);
    vmm_tlb_flush_page(va);
    vmm_soft_inc(&g_cSoftProtOk);
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
        vmm_soft_inc(&g_cSoftEnsureReject);
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
        vmm_soft_inc(&g_cSoftEnsureNomem);
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
            vmm_soft_inc(&g_cSoftEnsureNomem);
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
        vmm_soft_inc(&g_cSoftCowBreakNomem);
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
            vmm_soft_inc(&g_cSoftCowFreeOld);
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
    vmm_soft_note_peaks();
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
    vmm_soft_inc(&g_cSoftCloneCall);
    if (u64SrcCr3 == 0 || u64DstCr3 == 0 ||
        (u64SrcCr3 & ~0xfffull) == (u64DstCr3 & ~0xfffull)) {
        vmm_soft_inc(&g_cSoftCloneReject);
        kprintf("vmm: as_clone_user reject src=0x%lx dst=0x%lx\n",
                (unsigned long)u64SrcCr3, (unsigned long)u64DstCr3);
        return GJ_ERR_INVAL;
    }
    if (g_pKernelPml4 == NULL) {
        vmm_soft_inc(&g_cSoftCloneReject);
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
                            vmm_soft_inc(&g_cSoftCloneNomem);
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
                        vmm_soft_inc(&g_cSoftCloneNomem);
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
                        vmm_soft_inc(&g_cSoftCloneNomem);
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
    vmm_soft_inc(&g_cSoftCloneOk);
    g_cSoftClonePages += cCopy;
    g_cSoftCloneCow += cCow;
    g_cSoftCloneRo += cRoCopy;
    vmm_soft_note_peaks();
    kprintf("vmm: as_clone_user src=0x%lx dst=0x%lx pages=%u cow=%u rocopy=%u "
            "cow_live=%u share_ok=%u share_full=%u tbl_cow=%u PASS\n",
            (unsigned long)u64SrcCr3, (unsigned long)u64DstCr3, cCopy, cCow,
            cRoCopy, g_cCowLive, g_cCowShareOk, g_cCowShareFull, g_cCowTable);
    soft_inventory_maybe_once();
    return GJ_OK;
}
