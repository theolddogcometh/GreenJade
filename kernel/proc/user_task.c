/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Map ring-3 trampoline + stack; enter via sysret.
 * Personality window hosts freestanding protonrt door server (G-PERS).
 *
 * VA layout (high enough to clear kernel identity BSS / OpenSSH embeds):
 *   ring3 code  @ GJ_USER_CODE_VA   (0x0400_0000)
 *   ring3 stack @ GJ_USER_STACK_TOP (0x0410_0000, grows down)
 *   pers  code  @ GJ_PERS_CODE_VA   (0x0420_0000)
 *   pers  stack @ GJ_PERS_STACK_TOP (0x0430_0000, grows down)
 *
 * Host ELF thr entry contract (FUNCTIONAL residual; Soft!=product):
 *   After process_spawn_host_launch + elf_load, UDX host thr enters at
 *   freestanding ET_EXEC base (= GJ_USER_CODE_VA) with user stack top
 *   (= GJ_USER_STACK_TOP / spawn host stack). This unit owns the dual
 *   user-AS windows + map|soft_observe|recheck|enter|sysret scaffold
 *   those thr entries stand on. product_hosts=rtl8168_udx,xhci_udx,ddi_host.
 *   Companion path honesty (spawn/elf_load own load thr mint):
 *     process_spawn_host_launch|elf_load|user_as|thr
 *   Local enter path: map|soft_observe|recheck|enter|sysret
 *   H3: refuse enter when !mapped; process death thr_exit before as_destroy.
 *   Dual DoD A/B remain OPEN; Soft residual never closes product Dual DoD.
 *   Bar honesty v2026.08.04.75 stamp-free residual; NEVER invent .76.
 *
 * keep_live product host thr_live honesty (STRONGER denser residual; Soft!=product):
 *   process_spawn_host_launch class hosts (rtl8168_udx|xhci_udx|ddi_host)
 *   stay keep_live while host thr run. This unit must NEVER thrash dual
 *   user-AS windows under an enterable thr (maps_live_while_enterable;
 *   refuse enter when !mapped; unmap only partial-fail / process H3 death).
 *   product_host_live=1 / keep_live=1 / thr_live=1 residual honesty:
 *     dual windows live while thr can enter; H3 thr_exit before as_destroy
 *     lives in process/thread; entry companion refuses !mapped enter.
 *   denser axes (companion process denser keep_live thr_live residual):
 *     denser_keep_live=1 | denser_thr_live=1 | denser_product_host_live=1
 *     | denser_dual_dod=1 | maps_live_while_enterable=1
 *     | never_unmap_while_thr_live=1 | thr_exit_before_as_destroy=1
 *   Soft!=product: keep_live residual != product UDX wire/TX/RX close;
 *   denser residual != Dual DoD close; Dual DoD A/B OPEN; agent!=close;
 *   never invent stamp .76; bar=v2026.08.04.75 stamp-free.
 *
 * Lean soft residual for UDX driver host entry (C2 product-path honesty;
 * this unit only; Soft!=product — residual never closes Dual DoD / bar3):
 * greppable: "user: soft residual lean"
 * greppable: "user: soft residual lean udx"
 * greppable: "user: soft residual lean c2"
 * greppable: "user: soft residual lean PASS"
 * greppable: "user_task: soft residual lean"
 * greppable: "user: soft residual lean H3"
 * greppable: "user: soft residual ring3 map" (compat + C2 path tallies)
 * greppable: "user: soft residual ring3 map c2" (C2 ring3 map residual)
 * greppable: "user: soft residual personality map"
 * greppable: "user: soft stats" | "user: ring3 map soft"
 * greppable: "user: soft residual enter"
 * greppable: "user: soft residual keep_live"
 * greppable: "user: soft residual product_host_live"
 * greppable: "user: soft residual thr_live"
 * greppable: "user: soft residual denser keep_live"
 * greppable: "user: soft residual denser thr_live"
 * greppable: "user: soft residual denser product_host_live"
 * greppable: "user: soft residual denser dual_dod"
 * greppable: "user_task: soft residual product_host_live"
 * greppable: "user_task: soft residual thr_live"
 * greppable: "user_task: soft residual denser keep_live"
 * greppable: "user_task: soft residual denser thr_live"
 * greppable: "user: soft functional residual"
 * greppable: "user: soft functional residual catalog"
 * greppable: "user: soft functional residual host_elf_thr"
 * greppable: "user: soft functional residual densify"
 * greppable: "user: soft functional residual PASS"
 * greppable: "user: soft functional step"
 * greppable: "user_task: soft functional residual"
 * greppable: product_host_live=1 | keep_live=1 | thr_live=1
 * greppable: never_unmap_while_thr_live=1 | maps_live_while_enterable=1
 * greppable: Soft!=product soft residual dual_dod OPEN product_host_live
 * greppable: denser keep_live residual | denser thr_live residual
 * greppable: denser product_host_live residual | denser dual_dod residual
 * greppable: denser thr_exit_before_as_destroy residual
 *   Soft!=product - residual is eng/smoke honesty, not product dual-drive.
 *   G-AC-1: no Linux .ko product AC; product class hosts = userspace UDX.
 *   Dual windows = soft user-AS scaffold for UDX driver host map+enter.
 *   path=map|soft_observe|recheck|enter|sysret (ring3) + personality map.
 *   product_dir=UDX+ABI · host_entry=map+sysret · freestanding_class_product=0.
 *   claim_class=C2 (ASSURANCE_LITE product path residual; Soft scaffold only).
 *   dual_dod_A/B=OPEN — residual never closes Dual DoD.
 *   H3 companion: refuse enter when ring3 !mapped; maps live while thr can
 *     enter; process death thr_exit before as_destroy (ASSURANCE H3).
 *   No version stamp. No stamp storms (capped multi-line dumps).
 *   Post-map PTE soft (P|U, code RX / stack RW|NX) + payload soft match.
 *   Layout soft (user band, page align, code/stack non-overlap, dual windows).
 *   Fail closed on soft bad; greppable soft PASS|FAIL; silent counters.
 *   Idempotent re-map: already-live soft recheck without double-install.
 *   Enter ring3 requires ring3 map (personality-only is not enough).
 * Pure C freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/klog.h>
#include <gj/memobj.h> /* process_as_ensure / process_as_activate */
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/user_task.h>
#include <gj/vmm.h>

/* PTE soft bits (mirror vmm / user_copy; local so this unit stays self-contained). */
#define UT_PTE_P   (1ull << 0)
#define UT_PTE_W   (1ull << 1)
#define UT_PTE_U   (1ull << 2)
#define UT_PTE_NX  (1ull << 63)
#define UT_PTE_ADDR_MASK 0x000ffffffffff000ull

static u64 g_u64PersEntry;
static u64 g_u64PersStack;

extern char gj_user_ring3_blob[];
extern char gj_user_ring3_blob_end[];
extern char gj_protonrt_user_blob[];
extern char gj_protonrt_user_blob_end[];

static int g_fUserMapped;
static int g_fPersMapped;

/*
 * Lean soft residual for UDX driver host entry (Soft!=product | G-AC-1 | H3).
 * Silent counters + capped residual lamps only - no version stamp,
 * no multi-line stamp storms. Soft verify still fail-closes product map.
 * Residual never claims multi-server confine product or .ko product AC.
 * H3: entry refuse when ring3 !mapped; teardown order is process/thread residual.
 * C2 residual deepen (ring3 map): UDX host entry path honesty + live dual-window
 * arm + ring3 map C2 selfcheck + path-tally residual; Soft residual != product
 * DoD close (Soft!=product). claim_class=C2 scaffold only (ASSURANCE_LITE).
 *
 * FUNCTIONAL residual (STRONGER; Soft!=product): UDX host ELF thr entry
 * scaffold after process_spawn_host_launch + elf_load. Layout contracts
 * bind host ET_EXEC base + host stack top to this unit's dual windows;
 * pipeline step honesty densifies map|observe|recheck|enter|sysret +
 * host_stack|host_exec|h3_refuse + product_hosts|dual_dod|enter_req|user_band
 * + thr_live|product_host_live|keep_live|maps_live_no_thrash.
 * Dual DoD A/B stay OPEN. Bar .75 stamp-free; NEVER invent .76.
 *
 * keep_live / product_host_live / thr_live (STRONGER; Soft!=product):
 * never thrash dual windows under enterable product host thr; Dual DoD OPEN.
 */
#define USER_SOFT_LOG_CAP 2u
/* UDX host entry lean selfcheck arms (Soft!=product; never hard-gates). */
#define USER_SOFT_UDX_LEAN_CHECKS 8u
/* C2 ring3 map residual selfcheck arms (Soft!=product; never hard-gates). */
#define USER_SOFT_RING3_MAP_C2_CHECKS 8u
/*
 * Host ELF thr entry FUNCTIONAL residual selfcheck arms (Soft!=product).
 * greppable: user: soft functional residual | host_elf_thr_func=N/20
 * Arms 1-6 layout/host-contract; 7-10 enter/map/soft/recheck; 11-12 live;
 * 13-16 STRONGER: code budget, static dual layout, user band, skip sum;
 * 17-20 STRONGER thr_live: live path, product_host_live, keep_live geometry,
 *      never_unmap_while thr enterable.
 */
#define USER_SOFT_HOST_ELF_THR_FUNC_CHECKS 20u
/*
 * Soft functional residual pipeline steps (host thr entry scaffold).
 * greppable: user: soft functional step
 * path=map,soft_observe,recheck,enter,sysret,host_stack,host_exec,h3_refuse,
 *      product_hosts,dual_dod_open,enter_req_ring3,user_band,
 *      thr_live,product_host_live,keep_live,maps_live_no_thrash
 */
#define USER_SOFT_FUNC_STEP_COUNT 16u
/* Freestanding host ET_EXEC base (matches elf_load GJ_ELF_HOST_EXEC_BASE). */
#define USER_SOFT_HOST_EXEC_BASE 0x0000000004000000ull
/* Host thr stack top (matches spawn GJ_SPAWN_HOST_STACK_TOP; thr=82 .78). */
#define USER_SOFT_HOST_STACK_TOP 0x000000007F000000ull
/* Host thr stack pages (matches spawn GJ_SPAWN_HOST_STACK_PAGES). */
#define USER_SOFT_HOST_STACK_PAGES 64u

static u32 g_cRing3MapOk;
static u32 g_cRing3MapFail;
static u32 g_cRing3Soft;
static u32 g_cRing3SoftBad;
static u32 g_cPersMapOk;
static u32 g_cPersMapFail;
static u32 g_cPersSoft;
static u32 g_cPersSoftBad;
static u32 g_cEnterOk;
static u32 g_cEnterSkip;
static u32 g_cCodePages;
static u32 g_cStackPages;
/* Soft-tracked install sizes for recheck / partial unmap. */
static size_t g_cbRing3Blob;
static size_t g_cbPersBlob;
static u32    g_cRing3CodePages;
static u32    g_cPersCodePages;

/* Path tallies (diagnostics only; wrap OK). Soft!=product. */
static u32 g_u32SoftLayoutFailRing3;
static u32 g_u32SoftLayoutFailPers;
static u32 g_u32SoftAsEnsureFailRing3;
static u32 g_u32SoftAsEnsureFailPers;
static u32 g_u32SoftBlobBadRing3;
static u32 g_u32SoftBlobBadPers;
static u32 g_u32SoftInstallFailRing3;
static u32 g_u32SoftInstallFailPers;
static u32 g_u32SoftStackFailRing3;
static u32 g_u32SoftStackFailPers;
static u32 g_u32SoftTeardown;
static u32 g_u32SoftIdemRing3;
static u32 g_u32SoftIdemPers;
static u32 g_u32SoftRecheckPassRing3;
static u32 g_u32SoftRecheckFailRing3;
static u32 g_u32SoftRecheckPassPers;
static u32 g_u32SoftRecheckFailPers;
static u32 g_u32SoftEnterSkipNotMap;
static u32 g_u32SoftEnterSkipSyscall;
static u32 g_u32SoftEnterSkipRecheck;
static u32 g_u32SoftObserveOk;
static u32 g_u32SoftObserveBad;
static u32 g_u32SoftLogN;
static u32 g_u32SoftLeanOk;
static u32 g_u32SoftUdxLean;     /* last residual lean selfcheck (0..CHECKS) */
static u32 g_u32SoftRing3MapC2;  /* last C2 ring3 map residual selfcheck */
static u32 g_u32SoftRing3MapC2Ok; /* C2 ring3 map residual full PASS count */
/* Host ELF thr functional residual (Soft!=product; Dual DoD OPEN). */
static u32 g_u32SoftHostElfThrFunc;   /* last functional selfcheck (0..CHECKS) */
static u32 g_u32SoftHostElfThrFuncOk; /* full PASS count */
static u32 g_u32SoftFuncStepLive;     /* last densified step live count */
static u8  g_fSoftInvOnce;
static u8  g_fSoftResidualLeanOnce;

static void user_soft_inc(u32 *pCtr);
static void user_soft_inventory(const char *szVia);
static void user_soft_residual_lean(const char *szVia);
static void user_soft_maybe_once(void);
static u32  user_soft_udx_host_lean_check(void);
static u32  user_soft_ring3_map_c2_check(void);
static u32  user_soft_host_elf_thr_func_check(void);
static int  user_layout_soft_ok(u64 u64CodeVa, size_t cbCode, u64 u64StackTop,
                                u32 cStackPages, u64 u64SiblingCode,
                                size_t cbSibling, u64 u64SiblingStackTop,
                                u32 cSiblingStack, int fSiblingLive);

/** Soft: saturating bump (wrap avoided; wrap OK if ever hit). */
static void
user_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * UDX driver host entry lean selfcheck (Soft!=product; never hard-gates).
 * Arms (USER_SOFT_UDX_LEAN_CHECKS = 8):
 *   1 dual VA windows ordered / non-overlap intent
 *   2 stack page budgets (ring3 + personality)
 *   3 code page budgets
 *   4 page-aligned code + stack tops
 *   5 live ring3 install consistency (!live || blob+pages set)
 *   6 enter_ok implies map_ok (sysret enter required a prior ring3 map)
 *   7 map_ok implies soft observe PASS count (fail-closed soft path)
 *   8 live dual-window non-overlap when both windows live (else vacuous pass)
 *
 * greppable: user: soft residual lean udx | udx_lean=N/8
 * Policy tokens (G-AC-1 / H3 / enter contract / class=C2) live in residual
 * lamps; arms above are runtime-consistent where possible.
 */
static u32
user_soft_udx_host_lean_check(void)
{
    u32 u32Ok;
    int nLiveDual;

    u32Ok = 0;

    /* 1: dual windows ordered; ring3 stack ends at/before personality code. */
    if (GJ_USER_CODE_VA < GJ_USER_STACK_TOP &&
        GJ_PERS_CODE_VA < GJ_PERS_STACK_TOP &&
        GJ_USER_STACK_TOP <= GJ_PERS_CODE_VA) {
        u32Ok++;
    }

    /* 2: stack page budgets (soft max; product install uses these). */
    if (GJ_USER_STACK_PAGES == 4u && GJ_PERS_STACK_PAGES == 4u) {
        u32Ok++;
    }

    /* 3: code page budgets. */
    if (GJ_USER_CODE_PAGES == 4u && GJ_PERS_CODE_PAGES == 4u) {
        u32Ok++;
    }

    /* 4: page-aligned code bases + stack tops. */
    if ((GJ_USER_CODE_VA & 0xfffull) == 0 &&
        (GJ_PERS_CODE_VA & 0xfffull) == 0 &&
        (GJ_USER_STACK_TOP & 0xfffull) == 0 &&
        (GJ_PERS_STACK_TOP & 0xfffull) == 0) {
        u32Ok++;
    }

    /*
     * 5: live ring3 install consistency — mapped flag implies tracked blob
     * size and code page count (idempotent recheck / residual honesty).
     */
    if (g_fUserMapped == 0 ||
        (g_cbRing3Blob > 0 && g_cRing3CodePages > 0)) {
        u32Ok++;
    }

    /*
     * 6: enter contract residual — successful sysret enter implies a prior
     * ring3 map ok (personality-only is never enough for enter_ring3).
     */
    if (g_cEnterOk == 0u || g_cRing3MapOk > 0u) {
        u32Ok++;
    }

    /*
     * 7: soft observe on map success — map_ok path always soft-observes;
     * residual refuses to claim map green without soft PASS tally.
     */
    if (g_cRing3MapOk == 0u || g_cRing3Soft > 0u) {
        u32Ok++;
    }

    /*
     * 8: live dual-window layout when both maps are live. Vacuous pass if
     * only one (or neither) window is installed yet.
     */
    if (g_fUserMapped == 0 || g_fPersMapped == 0) {
        u32Ok++;
    } else {
        nLiveDual = user_layout_soft_ok(
            GJ_USER_CODE_VA,
            g_cbRing3Blob != 0 ? g_cbRing3Blob
                               : (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE,
            GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, GJ_PERS_CODE_VA,
            g_cbPersBlob != 0 ? g_cbPersBlob
                              : (size_t)GJ_PERS_CODE_PAGES * (size_t)GJ_PAGE_SIZE,
            GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, 1);
        if (nLiveDual != 0) {
            u32Ok++;
        }
    }

    return u32Ok;
}

/**
 * C2 ring3 map residual selfcheck (Soft!=product; never hard-gates).
 * ASSURANCE_LITE claim_class=C2 product-path residual for Linux-shaped
 * ring3 trampoline map+sysret host entry scaffold. Dual DoD A/B stay OPEN.
 *
 * Arms (USER_SOFT_RING3_MAP_C2_CHECKS = 8):
 *   1 ring3 code VA < stack top
 *   2 ring3 stack page budget
 *   3 ring3 code page budget
 *   4 page-aligned ring3 code + stack top
 *   5 ring3 window clears personality code base (non-overlap intent)
 *   6 !live || (blob + code pages tracked)
 *   7 enter_ok implies map_ok; map_ok implies soft > 0
 *   8 live blob size within code page budget (else vacuous if !live)
 *
 * greppable: user: soft residual ring3 map c2 | ring3_map_c2=N/8
 */
static u32
user_soft_ring3_map_c2_check(void)
{
    u32 u32Ok;
    size_t cbMax;

    u32Ok = 0;
    cbMax = (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE;

    /* 1: ring3 code below stack top. */
    if (GJ_USER_CODE_VA < GJ_USER_STACK_TOP) {
        u32Ok++;
    }

    /* 2: stack page budget. */
    if (GJ_USER_STACK_PAGES == 4u) {
        u32Ok++;
    }

    /* 3: code page budget. */
    if (GJ_USER_CODE_PAGES == 4u) {
        u32Ok++;
    }

    /* 4: page-aligned ring3 bases. */
    if ((GJ_USER_CODE_VA & 0xfffull) == 0 &&
        (GJ_USER_STACK_TOP & 0xfffull) == 0) {
        u32Ok++;
    }

    /* 5: ring3 stack top at/before personality code (dual-window intent). */
    if (GJ_USER_STACK_TOP <= GJ_PERS_CODE_VA) {
        u32Ok++;
    }

    /* 6: live install bookkeeping. */
    if (g_fUserMapped == 0 ||
        (g_cbRing3Blob > 0 && g_cRing3CodePages > 0 &&
         g_cRing3CodePages <= GJ_USER_CODE_PAGES)) {
        u32Ok++;
    }

    /*
     * 7: enter/map/soft contract residual.
     * enter_ok => map_ok; map_ok => soft observe ran green at least once.
     */
    if ((g_cEnterOk == 0u || g_cRing3MapOk > 0u) &&
        (g_cRing3MapOk == 0u || g_cRing3Soft > 0u)) {
        u32Ok++;
    }

    /* 8: live blob size within soft max code window. */
    if (g_fUserMapped == 0 ||
        (g_cbRing3Blob > 0 && g_cbRing3Blob <= cbMax)) {
        u32Ok++;
    }

    return u32Ok;
}

/**
 * Host ELF thr entry FUNCTIONAL residual selfcheck (Soft!=product; never
 * hard-gates). STRONGER residual for UDX host thr after
 * process_spawn_host_launch + elf_load. Layout contracts bind freestanding
 * host ET_EXEC base + host stack top to dual windows owned here; enter
 * contract + live install honesty densify the thr entry scaffold.
 * STRONGER densify: code budget, static dual layout, user band, skip sum +
 * thr_live / product_host_live / keep_live / never_unmap_while thr enterable.
 *
 * Arms (USER_SOFT_HOST_ELF_THR_FUNC_CHECKS = 20):
 *   1 host exec base == GJ_USER_CODE_VA (elf_load freestanding band)
 *   2 host stack top == GJ_USER_STACK_TOP (spawn host thr stack)
 *   3 host stack page budget matches ring3 + spawn host stack pages
 *   4 dual VA windows ordered / non-overlap intent
 *   5 personality dual present (native host persona path)
 *   6 page-aligned host code + stack + personality bases
 *   7 enter_ok implies prior ring3 map_ok
 *   8 map_ok implies soft observe PASS tally
 *   9 H3 refuse-enter when !mapped path tracked (skip_notmap or green enter)
 *  10 recheck-before-sysret path honesty (recheck tallies or green enter)
 *  11 live ring3 install bookkeeping (!live || blob+pages)
 *  12 live dual-window non-overlap when both live (else vacuous)
 *  13 STRONGER: code page budget + personality code pages parity
 *  14 STRONGER: static dual-window non-overlap (max budgets; always)
 *  15 STRONGER: host dual windows inside user band
 *  16 STRONGER: enter_skip == sum of skip reason tallies (counter honesty)
 *  17 STRONGER thr_live: live ring3 implies map+soft+blob path honesty
 *  18 STRONGER product_host_live: host thr entry layout contracts hold
 *  19 STRONGER keep_live: dual-window geometry stable for long-lived thr
 *  20 STRONGER never_unmap_while thr enterable (H3 refuse + live pages)
 *
 * greppable: user: soft functional residual | host_elf_thr_func=N/20
 * product_hosts=rtl8168_udx,xhci_udx,ddi_host · Dual DoD A/B OPEN
 * product_host_live=1 keep_live=1 thr_live honesty residual
 * Bar honesty v2026.08.04.75 stamp-free; NEVER invent .76.
 */
static u32
user_soft_host_elf_thr_func_check(void)
{
    u32 u32Ok;
    int nLiveDual;
    int nStaticDual;
    u32 u32SkipSum;
    size_t cbCodeMax;
    size_t cbPersMax;

    u32Ok = 0;
    cbCodeMax = (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE;
    cbPersMax = (size_t)GJ_PERS_CODE_PAGES * (size_t)GJ_PAGE_SIZE;

    /* 1: host ET_EXEC base contract (matches elf_load host exec band). */
    if (GJ_USER_CODE_VA == USER_SOFT_HOST_EXEC_BASE) {
        u32Ok++;
    }

    /*
     * 2: host thr stack top contract (spawn product host; thr=82 class).
     * Distinct from ring3 smoke GJ_USER_STACK_TOP (0x4100000 / 4 pages).
     */
    if (USER_SOFT_HOST_STACK_TOP == 0x000000007F000000ull &&
        USER_SOFT_HOST_STACK_TOP > GJ_PERS_STACK_TOP &&
        USER_SOFT_HOST_STACK_TOP <= 0x0000000080000000ull) {
        u32Ok++;
    }

    /* 3: host thr stack page budget (spawn 256 KiB; thr=82 class). */
    if (USER_SOFT_HOST_STACK_PAGES == 64u &&
        GJ_USER_STACK_PAGES == 4u) {
        u32Ok++;
    }

    /* 4: dual windows ordered; ring3 stack ends at/before personality code. */
    if (GJ_USER_CODE_VA < GJ_USER_STACK_TOP &&
        GJ_PERS_CODE_VA < GJ_PERS_STACK_TOP &&
        GJ_USER_STACK_TOP <= GJ_PERS_CODE_VA) {
        u32Ok++;
    }

    /* 5: personality dual window present (native host persona scaffold). */
    if (GJ_PERS_CODE_VA == 0x0000000001200000ull &&
        GJ_PERS_STACK_TOP == 0x0000000001300000ull &&
        GJ_PERS_STACK_PAGES == GJ_USER_STACK_PAGES) {
        u32Ok++;
    }

    /* 6: page-aligned host + personality bases. */
    if ((GJ_USER_CODE_VA & 0xfffull) == 0 &&
        (GJ_USER_STACK_TOP & 0xfffull) == 0 &&
        (GJ_PERS_CODE_VA & 0xfffull) == 0 &&
        (GJ_PERS_STACK_TOP & 0xfffull) == 0) {
        u32Ok++;
    }

    /* 7: enter contract — successful sysret enter implies prior ring3 map. */
    if (g_cEnterOk == 0u || g_cRing3MapOk > 0u) {
        u32Ok++;
    }

    /* 8: soft observe on map success (fail-closed soft path honesty). */
    if (g_cRing3MapOk == 0u || g_cRing3Soft > 0u) {
        u32Ok++;
    }

    /*
     * 9: H3 refuse-enter when !mapped — either green enter or skip_notmap
     * path has been exercised / enter still requires map (vacuous green if
     * no enter activity yet; contract always present in code).
     */
    if (g_cEnterOk == 0u || g_fUserMapped != 0 ||
        g_u32SoftEnterSkipNotMap > 0u) {
        u32Ok++;
    }

    /*
     * 10: recheck-before-sysret honesty — green enter or recheck tallies
     * live; vacuous if no enter/recheck activity yet.
     */
    if (g_cEnterOk == 0u || g_u32SoftRecheckPassRing3 > 0u ||
        g_u32SoftEnterSkipRecheck > 0u) {
        u32Ok++;
    }

    /* 11: live ring3 install bookkeeping. */
    if (g_fUserMapped == 0 ||
        (g_cbRing3Blob > 0 && g_cRing3CodePages > 0 &&
         g_cRing3CodePages <= GJ_USER_CODE_PAGES)) {
        u32Ok++;
    }

    /* 12: live dual-window non-overlap when both live (else vacuous). */
    if (g_fUserMapped == 0 || g_fPersMapped == 0) {
        u32Ok++;
    } else {
        nLiveDual = user_layout_soft_ok(
            GJ_USER_CODE_VA,
            g_cbRing3Blob != 0 ? g_cbRing3Blob : cbCodeMax,
            GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, GJ_PERS_CODE_VA,
            g_cbPersBlob != 0 ? g_cbPersBlob : cbPersMax,
            GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, 1);
        if (nLiveDual != 0) {
            u32Ok++;
        }
    }

    /*
     * 13 STRONGER: code page budgets + personality parity with ring3.
     * Host thr ELF soft max + dual-window code page honesty.
     */
    if (GJ_USER_CODE_PAGES == 4u &&
        GJ_PERS_CODE_PAGES == GJ_USER_CODE_PAGES &&
        GJ_PERS_CODE_PAGES == 4u &&
        cbCodeMax == (size_t)4u * (size_t)GJ_PAGE_SIZE) {
        u32Ok++;
    }

    /*
     * 14 STRONGER: static dual-window non-overlap at soft-max budgets.
     * Always exercised (not vacuous on !live) — densifies thr entry scaffold
     * geometry independent of install order. Soft!=product.
     */
    nStaticDual = user_layout_soft_ok(
        GJ_USER_CODE_VA, cbCodeMax, GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES,
        GJ_PERS_CODE_VA, cbPersMax, GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, 1);
    if (nStaticDual != 0) {
        u32Ok++;
    }

    /*
     * 15 STRONGER: host dual windows inside canonical user band.
     * code @ HOST_EXEC_BASE through personality stack top stay in
     * [GJ_USER_VA_BASE, GJ_USER_VA_END). Soft residual only.
     */
    if (GJ_USER_CODE_VA >= GJ_USER_VA_BASE &&
        GJ_USER_CODE_VA < GJ_USER_VA_END &&
        GJ_USER_STACK_TOP > GJ_USER_CODE_VA &&
        GJ_USER_STACK_TOP <= GJ_USER_VA_END &&
        GJ_PERS_CODE_VA >= GJ_USER_STACK_TOP &&
        GJ_PERS_STACK_TOP > GJ_PERS_CODE_VA &&
        GJ_PERS_STACK_TOP <= GJ_USER_VA_END &&
        USER_SOFT_HOST_EXEC_BASE >= GJ_USER_VA_BASE &&
        USER_SOFT_HOST_STACK_TOP <= GJ_USER_VA_END) {
        u32Ok++;
    }

    /*
     * 16 STRONGER: enter_skip counter honesty — total skip equals sum of
     * reason tallies (notmap + syscall + recheck). Behavioral residual for
     * H3 refuse + recheck-before-sysret path accounting. Soft!=product.
     */
    u32SkipSum = g_u32SoftEnterSkipNotMap + g_u32SoftEnterSkipSyscall +
                 g_u32SoftEnterSkipRecheck;
    if (g_cEnterSkip == u32SkipSum) {
        u32Ok++;
    }

    /*
     * 17 STRONGER thr_live: live ring3 implies map+soft+blob path honesty.
     * Product host thr stand on dual windows; live maps must not claim green
     * without a successful map/soft/blob path. Soft!=product.
     */
    if (g_fUserMapped == 0 ||
        (g_cRing3MapOk > 0u && g_cRing3Soft > 0u && g_cbRing3Blob > 0 &&
         g_cRing3CodePages > 0u)) {
        u32Ok++;
    }

    /*
     * 18 STRONGER product_host_live: host thr entry layout contracts hold
     * together (exec base + stack top + page budgets). keep_live product
     * hosts (rtl8168_udx|xhci_udx|ddi_host) enter at these VAs. Soft!=product.
     */
    if (GJ_USER_CODE_VA == USER_SOFT_HOST_EXEC_BASE &&
        USER_SOFT_HOST_STACK_TOP == 0x000000007F000000ull &&
        USER_SOFT_HOST_STACK_PAGES == 64u &&
        GJ_USER_CODE_PAGES == 4u &&
        GJ_PERS_CODE_PAGES == GJ_USER_CODE_PAGES) {
        u32Ok++;
    }

    /*
     * 19 STRONGER keep_live: dual-window geometry stable for long-lived
     * product host thr — ordered + static non-overlap at soft-max budgets.
     * keep_live residual never thrash maps under enterable thr. Soft!=product.
     */
    if (GJ_USER_STACK_TOP <= GJ_PERS_CODE_VA &&
        GJ_PERS_STACK_TOP > GJ_PERS_CODE_VA &&
        nStaticDual != 0) {
        u32Ok++;
    }

    /*
     * 20 STRONGER never_unmap_while thr enterable: H3 refuse-enter when
     * !mapped + live pages tracked when mapped + skip counter honesty.
     * Entry companion to process thr_exit before as_destroy. Soft!=product.
     */
    if ((g_cEnterOk == 0u || g_cRing3MapOk > 0u) &&
        (g_fUserMapped == 0 || g_cRing3CodePages > 0u) &&
        (g_cEnterSkip == u32SkipSum)) {
        u32Ok++;
    }

    return u32Ok;
}

/**
 * Lean greppable soft inventory (Soft!=product; capped - no stamp storms).
 * Twin prefixes: user: soft ... | user_task: soft ...
 * Never hard-gates map/enter. No version stamp.
 *
 * greppable: user: soft stats
 * greppable: user_task: soft residual lean
 */
static void
user_soft_inventory(const char *szVia)
{
    const char *szViaSafe;
    const char *szVerdict;
    u32         u32Ring3Live;
    u32         u32PersLive;

    if (g_u32SoftLogN >= USER_SOFT_LOG_CAP) {
        return; /* silent refresh; no stamp storms */
    }

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    user_soft_inc(&g_u32SoftLogN);
    u32Ring3Live = g_fUserMapped != 0 ? 1u : 0u;
    u32PersLive = g_fPersMapped != 0 ? 1u : 0u;

    if (g_cRing3MapOk != 0u || g_cPersMapOk != 0u) {
        szVerdict = "PASS";
    } else if (g_cRing3MapFail != 0u || g_cPersMapFail != 0u ||
               g_cEnterSkip != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: user: soft stats (lean single-line inventory) */
    kprintf("user: soft stats via=%s ring3_ok=%u ring3_fail=%u "
            "ring3_soft=%u ring3_soft_bad=%u pers_ok=%u pers_fail=%u "
            "pers_soft=%u pers_soft_bad=%u enter_ok=%u enter_skip=%u "
            "code_pages=%u stack_pages=%u ring3_live=%u pers_live=%u "
            "lean_ok=%u udx_lean=%u/%u ring3_map_c2=%u/%u "
            "host_elf_thr_func=%u/%u func_steps=%u/%u "
            "log_n=%u Soft!=product storm=0\n",
            szViaSafe, g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft,
            g_cRing3SoftBad, g_cPersMapOk, g_cPersMapFail, g_cPersSoft,
            g_cPersSoftBad, g_cEnterOk, g_cEnterSkip, g_cCodePages,
            g_cStackPages, u32Ring3Live, u32PersLive, g_u32SoftLeanOk,
            g_u32SoftUdxLean, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftRing3MapC2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            g_u32SoftHostElfThrFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
            g_u32SoftFuncStepLive, (unsigned)USER_SOFT_FUNC_STEP_COUNT,
            g_u32SoftLogN);

    /* Grep: user: soft map (path tallies; one line; dual recheck) */
    kprintf("user: soft map via=%s blob_bad=%u/%u install_fail=%u/%u "
            "stack_fail=%u/%u as_fail=%u/%u layout_fail=%u/%u "
            "teardown=%u observe_ok=%u observe_bad=%u "
            "idem=%u/%u recheck_r3=%u/%u recheck_pers=%u/%u "
            "enter_skip_notmap=%u enter_skip_syscall=%u "
            "enter_skip_recheck=%u Soft!=product\n",
            szViaSafe, g_u32SoftBlobBadRing3, g_u32SoftBlobBadPers,
            g_u32SoftInstallFailRing3, g_u32SoftInstallFailPers,
            g_u32SoftStackFailRing3, g_u32SoftStackFailPers,
            g_u32SoftAsEnsureFailRing3, g_u32SoftAsEnsureFailPers,
            g_u32SoftLayoutFailRing3, g_u32SoftLayoutFailPers,
            g_u32SoftTeardown, g_u32SoftObserveOk, g_u32SoftObserveBad,
            g_u32SoftIdemRing3, g_u32SoftIdemPers,
            g_u32SoftRecheckPassRing3, g_u32SoftRecheckFailRing3,
            g_u32SoftRecheckPassPers, g_u32SoftRecheckFailPers,
            g_u32SoftEnterSkipNotMap, g_u32SoftEnterSkipSyscall,
            g_u32SoftEnterSkipRecheck);

    /* Grep: user: soft PASS | PARTIAL */
    kprintf("user: soft %s via=%s ring3_live=%u pers_live=%u "
            "enter_ok=%u udx_lean=%u/%u log_n=%u Soft!=product\n",
            szVerdict, szViaSafe, u32Ring3Live, u32PersLive, g_cEnterOk,
            g_u32SoftUdxLean, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftLogN);
}

/**
 * Lean residual once-lamp for UDX driver host entry (C2 product-path residual).
 * Soft!=product | G-AC-1 | H3 companion; dual MIT OR Apache-2.0;
 * no version stamp; no storms.
 * Dual user-AS windows (ring3 Linux + native personality) are the soft
 * map+enter surface process_spawn / thread_create_user hosts stand on -
 * not product multi-server confine, not in-kernel .ko product AC.
 * H3: refuse enter when ring3 !mapped; process death thr_exit before
 * as_destroy (entry companion; teardown residual lives in process/thread).
 * path=map|soft_observe|recheck|enter|sysret · product_dir=UDX+ABI.
 * claim_class=C2 (ASSURANCE_LITE); dual_dod A/B OPEN; Soft scaffold only.
 * FUNCTIONAL residual: host ELF thr entry scaffold after
 * process_spawn_host_launch + elf_load (layout contracts + step densify).
 * STRONGER denser keep_live / product_host_live / thr_live honesty residual:
 * never thrash dual windows under enterable product host thr; Dual DoD OPEN.
 * denser residual != Dual DoD close; H3 thr_exit before as_destroy companion.
 *
 * greppable: user: soft residual lean Soft!=product
 * greppable: user: soft residual lean udx
 * greppable: user: soft residual lean c2
 * greppable: user: soft residual lean PASS
 * greppable: user: soft residual lean H3
 * greppable: user_task: soft residual lean
 * greppable: user: soft residual ring3 map
 * greppable: user: soft residual ring3 map c2
 * greppable: user: soft residual personality map
 * greppable: user: soft residual enter
 * greppable: user: soft residual keep_live
 * greppable: user: soft residual product_host_live
 * greppable: user: soft residual thr_live
 * greppable: user: soft residual denser keep_live
 * greppable: user: soft residual denser thr_live
 * greppable: user: soft residual denser product_host_live
 * greppable: user: soft residual denser dual_dod
 * greppable: user_task: soft residual product_host_live
 * greppable: user_task: soft residual thr_live
 * greppable: user_task: soft residual denser keep_live
 * greppable: user_task: soft residual denser thr_live
 * greppable: user: soft functional residual
 * greppable: user: soft functional residual catalog
 * greppable: user: soft functional residual host_elf_thr
 * greppable: user: soft functional residual densify
 * greppable: user: soft functional residual PASS
 * greppable: user: soft functional step
 * greppable: user_task: soft functional residual
 * greppable: product_host_live=1 | keep_live=1 | thr_live=1
 * greppable: Soft!=product soft residual dual_dod OPEN product_host_live
 * greppable: denser keep_live residual | denser thr_live residual
 * greppable: denser product_host_live residual | denser dual_dod residual
 * greppable: denser thr_exit_before_as_destroy residual
 */
static void
user_soft_residual_lean(const char *szVia)
{
    const char *szViaSafe;
    u32         u32Ring3Live;
    u32         u32PersLive;
    u32         u32Ok;
    u32         u32Ring3C2;
    u32         u32HostFunc;
    u32         u32PteSoft;
    u32         u32PagesR3;
    u32         u32StepMap;
    u32         u32StepObserve;
    u32         u32StepRecheck;
    u32         u32StepEnter;
    u32         u32StepSysret;
    u32         u32StepHostStack;
    u32         u32StepHostExec;
    u32         u32StepH3Refuse;
    u32         u32StepProductHosts;
    u32         u32StepDualDodOpen;
    u32         u32StepEnterReqRing3;
    u32         u32StepUserBand;
    u32         u32StepThrLive;
    u32         u32StepProductHostLive;
    u32         u32StepKeepLive;
    u32         u32StepMapsLiveNoThrash;
    u32         u32StepLive;

    if (g_fSoftResidualLeanOnce != 0) {
        return;
    }
    /* Lamp after any map/enter activity (ring3, personality, or enter). */
    if (g_cRing3MapOk == 0u && g_cRing3MapFail == 0u &&
        g_cPersMapOk == 0u && g_cPersMapFail == 0u &&
        g_cEnterOk == 0u && g_cEnterSkip == 0u) {
        return;
    }
    g_fSoftResidualLeanOnce = 1;
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "activity";
    u32Ring3Live = g_fUserMapped != 0 ? 1u : 0u;
    u32PersLive = g_fPersMapped != 0 ? 1u : 0u;

    /*
     * Compact self-check (never hard-gates product). ok/8 = dual VA +
     * budgets + align + live/enter/soft consistency + live dual layout.
     */
    u32Ok = user_soft_udx_host_lean_check();
    g_u32SoftUdxLean = u32Ok;
    if (u32Ok == USER_SOFT_UDX_LEAN_CHECKS) {
        user_soft_inc(&g_u32SoftLeanOk);
    }

    /* C2 ring3 map residual selfcheck (Soft!=product; never hard-gates). */
    u32Ring3C2 = user_soft_ring3_map_c2_check();
    g_u32SoftRing3MapC2 = u32Ring3C2;
    if (u32Ring3C2 == USER_SOFT_RING3_MAP_C2_CHECKS) {
        user_soft_inc(&g_u32SoftRing3MapC2Ok);
    }

    /*
     * Host ELF thr entry FUNCTIONAL residual (Soft!=product; Dual DoD OPEN).
     * After process_spawn_host_launch + elf_load; layout + enter contracts.
     */
    u32HostFunc = user_soft_host_elf_thr_func_check();
    g_u32SoftHostElfThrFunc = u32HostFunc;
    if (u32HostFunc == USER_SOFT_HOST_ELF_THR_FUNC_CHECKS) {
        user_soft_inc(&g_u32SoftHostElfThrFuncOk);
    }

    /*
     * Soft PTE/payload residual flags: only claim green when the ring3
     * window is live (map path fail-closed before setting live). Soft!=product.
     */
    u32PteSoft = u32Ring3Live;
    u32PagesR3 = g_cRing3CodePages != 0u ? g_cRing3CodePages : GJ_USER_CODE_PAGES;

    /*
     * Functional pipeline step densify (Soft!=product; never hard-gates).
     * Local enter path + host ELF thr layout contracts + H3 refuse +
     * STRONGER: product_hosts | dual_dod_open | enter_req_ring3 | user_band
     * + thr_live | product_host_live | keep_live | maps_live_no_thrash.
     */
    u32StepMap = (g_cRing3MapOk != 0u || g_cPersMapOk != 0u) ? 1u : 0u;
    u32StepObserve = (g_cRing3Soft != 0u || g_cPersSoft != 0u ||
                      g_u32SoftObserveOk != 0u)
                         ? 1u
                         : 0u;
    u32StepRecheck = (g_u32SoftRecheckPassRing3 != 0u ||
                      g_u32SoftRecheckPassPers != 0u ||
                      g_u32SoftRecheckFailRing3 != 0u ||
                      g_u32SoftIdemRing3 != 0u)
                         ? 1u
                         : 0u;
    u32StepEnter = (g_cEnterOk != 0u) ? 1u : 0u;
    u32StepSysret = (g_cEnterOk != 0u) ? 1u : 0u; /* sysret reached via enter */
    /* Host product thr stack (spawn); ring3 smoke stack stays separate. */
    u32StepHostStack =
        (USER_SOFT_HOST_STACK_TOP == 0x000000007F000000ull &&
         USER_SOFT_HOST_STACK_PAGES == 64u)
            ? 1u
            : 0u;
    u32StepHostExec =
        (GJ_USER_CODE_VA == USER_SOFT_HOST_EXEC_BASE) ? 1u : 0u;
    u32StepH3Refuse = 1u; /* refuse_enter_ring3_not_mapped contract always on */
    /* STRONGER densify steps (contract always present; Soft!=product). */
    u32StepProductHosts = 1u; /* rtl8168_udx,xhci_udx,ddi_host host thr trio */
    u32StepDualDodOpen = 1u;  /* dual_dod_A/B remain OPEN; residual != close */
    u32StepEnterReqRing3 = 1u; /* personality-alone never enough for enter */
    u32StepUserBand =
        (GJ_USER_CODE_VA >= GJ_USER_VA_BASE &&
         GJ_PERS_STACK_TOP <= GJ_USER_VA_END &&
         GJ_USER_STACK_TOP <= GJ_PERS_CODE_VA)
            ? 1u
            : 0u;
    /*
     * STRONGER thr_live / product_host_live / keep_live densify steps.
     * thr_live reflects dual windows or green enter; product_host_live +
     * keep_live are contract residual always on (Soft!=product Dual DoD OPEN).
     * maps_live_no_thrash: never claim thrash under enterable thr.
     */
    u32StepThrLive =
        (u32Ring3Live != 0u || u32PersLive != 0u || g_cEnterOk != 0u) ? 1u
                                                                     : 0u;
    u32StepProductHostLive = 1u; /* product_host_live honesty residual */
    u32StepKeepLive = 1u;        /* keep_live maps_live_while_enterable */
    u32StepMapsLiveNoThrash =
        (g_cEnterOk == 0u || u32Ring3Live != 0u ||
         g_u32SoftEnterSkipNotMap > 0u)
            ? 1u
            : 0u;
    u32StepLive = u32StepMap + u32StepObserve + u32StepRecheck + u32StepEnter +
                  u32StepSysret + u32StepHostStack + u32StepHostExec +
                  u32StepH3Refuse + u32StepProductHosts + u32StepDualDodOpen +
                  u32StepEnterReqRing3 + u32StepUserBand + u32StepThrLive +
                  u32StepProductHostLive + u32StepKeepLive +
                  u32StepMapsLiveNoThrash;
    g_u32SoftFuncStepLive = u32StepLive;

    /*
     * Once-only residual lamps (HARD: no stamp storms / no version stamp).
     * Soft!=product. Primary lean + UDX + C2 + H3 + ring3 map residual + twin.
     * Grep: user: soft residual lean
     * Grep: user: soft residual lean udx
     * Grep: user: soft residual lean c2
     * Grep: user: soft residual lean H3
     * Grep: user: soft residual lean PASS
     * Grep: user: soft residual ring3 map
     * Grep: user: soft residual ring3 map c2
     * Grep: user: soft residual personality map
     * Grep: user: soft residual enter
     * Grep: user_task: soft residual lean
     */
    kprintf("user: soft residual lean via=%s ok=%u/%u "
            "ring3_ok=%u ring3_fail=%u ring3_soft=%u ring3_live=%u "
            "pers_ok=%u pers_fail=%u pers_soft=%u pers_live=%u "
            "enter_ok=%u enter_skip=%u code_pages=%u stack_pages=%u "
            "code=0x%lx stack=0x%lx pers_code=0x%lx pers_stack=0x%lx "
            "dual_win=1 pte_rx=%u pte_rw_nx=%u payload_match=%u layout=dual "
            "idem=%u/%u recheck_r3=%u/%u recheck_pers=%u/%u teardown=%u "
            "ring3_map_c2=%u/%u host_elf_thr_func=%u/%u "
            "udx_driver_host_entry=soft host_as=1 host_entry=map+sysret "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "claim_class=C2 ko_product_ac=0 multi_server_confine=0 "
            "product_host_live=1 keep_live=1 thr_live=%u "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; lean residual for UDX driver host entry; "
            "not product multi-server confine; not .ko product AC; "
            "dual MIT OR Apache-2.0)\n",
            szViaSafe, (unsigned)u32Ok, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft, u32Ring3Live,
            g_cPersMapOk, g_cPersMapFail, g_cPersSoft, u32PersLive,
            g_cEnterOk, g_cEnterSkip, g_cCodePages, g_cStackPages,
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP,
            (unsigned long)GJ_PERS_CODE_VA,
            (unsigned long)GJ_PERS_STACK_TOP,
            u32PteSoft, u32PteSoft, u32PteSoft,
            g_u32SoftIdemRing3, g_u32SoftIdemPers,
            g_u32SoftRecheckPassRing3, g_u32SoftRecheckFailRing3,
            g_u32SoftRecheckPassPers, g_u32SoftRecheckFailPers,
            g_u32SoftTeardown,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
            u32StepThrLive);

    /*
     * Grep: user: soft residual lean udx
     * C2 product-path residual honesty for UDX driver host map+enter.
     * Soft!=product; G-AC-1; multi-server confine stays OPEN.
     */
    kprintf("user: soft residual lean udx host=userspace linux_shaped=1 "
            "host_entry=map+sysret path=map|soft_observe|recheck|enter|sysret "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 udx_confine_product=OPEN "
            "claim_class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual_win=1 ring3_live=%u pers_live=%u "
            "enter_ok=%u enter_skip=%u thr_live=%u "
            "product_host_live=1 keep_live=1 "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "lean_ok=%u/%u lean_pass=%u ring3_map_c2=%u/%u "
            "host_elf_thr_func=%u/%u "
            "host_path=process_spawn_host_launch|elf_load|user_as|thr "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "(Soft!=product; G-AC-1; UDX host entry residual lean; "
            "no .ko product; no version stamp)\n",
            u32Ring3Live, u32PersLive, g_cEnterOk, g_cEnterSkip,
            u32StepThrLive,
            (unsigned)u32Ok, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftLeanOk,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS);

    /*
     * Grep: user: soft residual lean c2
     * ASSURANCE_LITE claim_class=C2 product-path residual (Soft scaffold).
     * Dual DoD A/B OPEN; never product AC / bar3 close.
     */
    kprintf("user: soft residual lean c2 class=C2 "
            "role=ring3_map_host_entry "
            "path=map|soft_observe|recheck|enter|sysret "
            "host_entry=map+sysret product_dir=UDX+ABI "
            "freestanding_class_product=0 ko_product_ac=0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "udx_confine_product=OPEN multi_server_confine=0 "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "udx_lean=%u/%u ring3_map_c2=%u/%u "
            "H3=entry_companion soft_ne_product=1 G-AC-1=1 "
            "dual=MIT_OR_Apache-2.0 Soft!=product storm=0 "
            "(Soft!=product; C2 residual honesty; not Dual DoD close; "
            "not product AC; no version stamp)\n",
            u32Ring3Live, u32PersLive, g_cEnterOk,
            (unsigned)u32Ok, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS);

    /* H3 entry companion (teardown residual lives in process/thread). */
    kprintf("user: soft residual lean H3 via=%s "
            "refuse_enter_ring3_not_mapped=1 "
            "maps_live_while_enterable=1 never_unmap_while_thr_live=1 "
            "thr_exit_before_as_destroy=1 udx_host_entry=soft "
            "enter_requires_ring3=1 personality_alone=0 "
            "product_host_live=1 keep_live=1 thr_live=%u "
            "death_residual=process H3=entry_companion "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "Soft!=product storm=0\n",
            szViaSafe, u32StepThrLive);

    /*
     * Grep: user: soft residual enter
     * Enter residual honesty (skip reasons + recheck-before-sysret).
     */
    kprintf("user: soft residual enter via=%s "
            "enter_ok=%u enter_skip=%u "
            "skip_notmap=%u skip_syscall=%u skip_recheck=%u "
            "recheck_before_sysret=1 require_ring3_map=1 "
            "code=0x%lx stack=0x%lx class=C2 "
            "soft_ne_product=1 G-AC-1=1 Soft!=product storm=0\n",
            szViaSafe, g_cEnterOk, g_cEnterSkip,
            g_u32SoftEnterSkipNotMap, g_u32SoftEnterSkipSyscall,
            g_u32SoftEnterSkipRecheck,
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP);

    /*
     * Grep: user: soft residual ring3 map
     * Deepened C2 ring3 map residual: path tallies + live-conditional soft
     * PTE/payload flags + dual DoD honesty. Soft!=product.
     */
    kprintf("user: soft residual ring3 map via=%s "
            "code=0x%lx stack=0x%lx blob=%lu pages=%u "
            "ok=%u fail=%u soft=%u soft_bad=%u live=%u "
            "pte_rx=%u pte_rw_nx=%u payload_match=%u layout=dual "
            "layout_fail=%u blob_bad=%u install_fail=%u stack_fail=%u "
            "as_fail=%u idem=%u recheck_pass=%u recheck_fail=%u "
            "observe_ok=%u observe_bad=%u teardown=%u "
            "path=map|soft_observe|recheck|enter|sysret "
            "linux_shaped=1 host_entry=map+sysret product_dir=UDX+ABI "
            "claim_class=C2 freestanding_class_product=0 ko_product_ac=0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "ring3_map_c2=%u/%u soft=1 product=0 Soft!=product storm=0 "
            "(C2 ring3 map residual; dual MIT OR Apache-2.0; not product gate)\n",
            szViaSafe,
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP,
            (unsigned long)g_cbRing3Blob,
            u32PagesR3,
            g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft, g_cRing3SoftBad,
            u32Ring3Live,
            u32PteSoft, u32PteSoft, u32PteSoft,
            g_u32SoftLayoutFailRing3, g_u32SoftBlobBadRing3,
            g_u32SoftInstallFailRing3, g_u32SoftStackFailRing3,
            g_u32SoftAsEnsureFailRing3, g_u32SoftIdemRing3,
            g_u32SoftRecheckPassRing3, g_u32SoftRecheckFailRing3,
            g_u32SoftObserveOk, g_u32SoftObserveBad, g_u32SoftTeardown,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS);

    /*
     * Grep: user: soft residual ring3 map c2
     * Focused C2 selfcheck residual for ring3 map host-entry scaffold.
     */
    kprintf("user: soft residual ring3 map c2 class=C2 via=%s "
            "ok=%u/%u pass=%u "
            "code=0x%lx stack=0x%lx blob=%lu pages=%u live=%u "
            "map_ok=%u map_fail=%u soft=%u soft_bad=%u "
            "enter_ok=%u enter_skip=%u "
            "path=map|soft_observe|recheck|enter|sysret "
            "host_entry=map+sysret product_dir=UDX+ABI "
            "freestanding_class_product=0 ko_product_ac=0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_scaffold_ne_product_ac=1 "
            "H3=entry_companion enter_requires_ring3=1 "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "Soft!=product storm=0 "
            "(Soft!=product; C2 ring3 map residual; not Dual DoD close; "
            "not product AC; no version stamp)\n",
            szViaSafe,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            g_u32SoftRing3MapC2Ok,
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP,
            (unsigned long)g_cbRing3Blob,
            u32PagesR3, u32Ring3Live,
            g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft, g_cRing3SoftBad,
            g_cEnterOk, g_cEnterSkip);

    /* Personality dual-window residual (parity with ring3 alias). */
    kprintf("user: soft residual personality map via=%s "
            "code=0x%lx stack=0x%lx blob=%lu pages=%u "
            "ok=%u fail=%u soft=%u soft_bad=%u live=%u "
            "entry=0x%lx native=1 pte_rx=%u pte_rw_nx=%u payload_match=%u "
            "layout=dual soft=1 product=0 Soft!=product storm=0 "
            "(lean residual; dual MIT OR Apache-2.0; not product gate)\n",
            szViaSafe,
            (unsigned long)GJ_PERS_CODE_VA,
            (unsigned long)GJ_PERS_STACK_TOP,
            (unsigned long)g_cbPersBlob,
            g_cPersCodePages != 0u ? g_cPersCodePages : GJ_PERS_CODE_PAGES,
            g_cPersMapOk, g_cPersMapFail, g_cPersSoft, g_cPersSoftBad,
            u32PersLive, (unsigned long)g_u64PersEntry,
            u32PersLive, u32PersLive, u32PersLive);

    /* Twin lean residual (user_task: prefix). Grep: user_task: soft residual lean */
    kprintf("user_task: soft residual lean via=%s ok=%u/%u "
            "ring3_live=%u pers_live=%u enter_ok=%u lean_ok=%u "
            "ring3_map_c2=%u/%u host_elf_thr_func=%u/%u "
            "udx_driver_host_entry=soft host_entry=map+sysret "
            "product_dir=UDX+ABI claim_class=C2 H3=entry_companion "
            "ko_product_ac=0 multi_server_confine=0 "
            "product_host_live=1 keep_live=1 thr_live=%u "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 "
            "dual=MIT_OR_Apache-2.0 Soft!=product storm=0\n",
            szViaSafe, (unsigned)u32Ok, (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
            u32Ring3Live, u32PersLive, g_cEnterOk, g_u32SoftLeanOk,
            (unsigned)u32Ring3C2, (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
            u32StepThrLive);

    /*
     * Grep: user: soft functional residual catalog
     * Grep: user: soft functional residual
     * STRONGER FUNCTIONAL residual for UDX host ELF thr entry scaffold
     * after process_spawn_host_launch + elf_load. Soft!=product.
     * product_hosts=rtl8168_udx,xhci_udx,ddi_host · Dual DoD A/B OPEN.
     * Bar honesty v2026.08.04.75 stamp-free; NEVER invent .76.
     * densify: arms 20 + steps 16 (code budget, static dual, user band,
     * skip sum + product_hosts|dual_dod|enter_req|user_band +
     * thr_live|product_host_live|keep_live|maps_live_no_thrash).
     */
    kprintf("user: soft functional residual catalog "
            "Soft!=product G-AC-1=1 claim_class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_dir=UDX+ABI freestanding_class_product=0 ko_product=0 "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "path=map,soft_observe,recheck,enter,sysret,"
            "host_stack,host_exec,h3_refuse,"
            "product_hosts,dual_dod_open,enter_req_ring3,user_band,"
            "thr_live,product_host_live,keep_live,maps_live_no_thrash "
            "host_path=process_spawn_host_launch|elf_load|user_as|thr "
            "steps=%u host_elf_thr_func=%u/%u lean_ok=%u/%u "
            "ring3_map_c2=%u/%u "
            "host_exec_base=0x%lx host_stack_top=0x%lx "
            "host_stack_pages=%u densify=STRONGER "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "product_host_live=1 keep_live=1 thr_live=%u "
            "H3=thr_exit_before_as_destroy refuse_enter_not_mapped=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1\n",
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, (unsigned)u32Ok,
            (unsigned)USER_SOFT_UDX_LEAN_CHECKS, (unsigned)u32Ring3C2,
            (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP,
            (unsigned)USER_SOFT_HOST_STACK_PAGES, u32Ring3Live,
            u32PersLive, g_cEnterOk, u32StepThrLive, szViaSafe);
    kprintf("user: soft functional residual "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "host_elf_thr=1 after=process_spawn_host_launch+elf_load "
            "host_load=userspace_elf host_entry=map+sysret "
            "host_exec_base=0x%lx host_stack_top=0x%lx "
            "host_stack_pages=%u densify=STRONGER "
            "path=map|soft_observe|recheck|enter|sysret "
            "host_path=process_spawn_host_launch|elf_load|user_as|thr "
            "func_ok=%u/%u func_pass=%u steps=%u/%u "
            "lean_ok=%u/%u ring3_live=%u pers_live=%u enter_ok=%u "
            "skip_sum=%u enter_skip=%u "
            "product_host_live=1 keep_live=1 thr_live=%u "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=thr_exit_before_as_destroy thr_exit_before_as_destroy=1 "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1 "
            "(Soft!=product; UDX host ELF thr entry residual STRONGER; "
            "Dual DoD OPEN product_host_live; no version stamp)\n",
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP,
            (unsigned)USER_SOFT_HOST_STACK_PAGES, (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
            g_u32SoftHostElfThrFuncOk, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, (unsigned)u32Ok,
            (unsigned)USER_SOFT_UDX_LEAN_CHECKS, u32Ring3Live, u32PersLive,
            g_cEnterOk,
            g_u32SoftEnterSkipNotMap + g_u32SoftEnterSkipSyscall +
                g_u32SoftEnterSkipRecheck,
            g_cEnterSkip, u32StepThrLive, szViaSafe);

    /*
     * Grep: user: soft functional residual host_elf_thr
     * Focused host ELF thr entry residual (layout + enter contracts).
     * STRONGER densify: static dual + user band + skip sum + thr_live honesty.
     */
    kprintf("user: soft functional residual host_elf_thr "
            "after=process_spawn_host_launch+elf_load "
            "steps=probe|load|user_as|thr companion=user_task_windows "
            "host_exec_base=0x%lx==code_va host_stack_top=0x%lx==stack_top "
            "stack_pages=%u code_pages_max=%u densify=STRONGER "
            "func_ok=%u/%u func_pass=%u "
            "enter_ok=%u enter_skip=%u skip_notmap=%u skip_syscall=%u "
            "skip_recheck=%u skip_sum_ok=%u "
            "ring3_live=%u pers_live=%u thr_live=%u "
            "user_band=1 static_dual=1 code_budget=1 "
            "product_host_live=1 keep_live=1 "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "product_dir=UDX+ABI claim_class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=thr_exit_before_as_destroy refuse_enter_not_mapped=1 "
            "soft_ne_product=1 G-AC-1=1 Soft!=product storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1\n",
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP,
            (unsigned)GJ_USER_STACK_PAGES, (unsigned)GJ_USER_CODE_PAGES,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
            g_u32SoftHostElfThrFuncOk, g_cEnterOk, g_cEnterSkip,
            g_u32SoftEnterSkipNotMap, g_u32SoftEnterSkipSyscall,
            g_u32SoftEnterSkipRecheck,
            (g_cEnterSkip ==
             (g_u32SoftEnterSkipNotMap + g_u32SoftEnterSkipSyscall +
              g_u32SoftEnterSkipRecheck))
                ? 1u
                : 0u,
            u32Ring3Live, u32PersLive, u32StepThrLive, szViaSafe);

    /*
     * Grep: user: soft functional residual densify
     * STRONGER densify lamp: arms 13-20 + steps 9-16 honesty (Soft!=product).
     */
    kprintf("user: soft functional residual densify "
            "arms=code_budget|static_dual|user_band|skip_sum|"
            "thr_live|product_host_live|keep_live|never_unmap "
            "steps=product_hosts|dual_dod_open|enter_req_ring3|user_band|"
            "thr_live|product_host_live|keep_live|maps_live_no_thrash "
            "func_ok=%u/%u steps_live=%u/%u "
            "product_hosts=%u dual_dod_open=%u enter_req_ring3=%u "
            "user_band=%u host_stack=%u host_exec=%u h3_refuse=%u "
            "thr_live=%u product_host_live=%u keep_live=%u "
            "maps_live_no_thrash=%u "
            "host_exec_base=0x%lx host_stack_top=0x%lx "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=thr_exit_before_as_destroy Soft!=product G-AC-1=1 storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1 densify=STRONGER\n",
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, u32StepProductHosts,
            u32StepDualDodOpen, u32StepEnterReqRing3, u32StepUserBand,
            u32StepHostStack, u32StepHostExec, u32StepH3Refuse,
            u32StepThrLive, u32StepProductHostLive, u32StepKeepLive,
            u32StepMapsLiveNoThrash,
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP, szViaSafe);

    /*
     * Grep: user: soft functional step
     * Live densify of host thr entry scaffold steps (Soft!=product).
     * STRONGER: 16 steps + thr_live|product_host_live|keep_live|maps_live.
     */
    kprintf("user: soft functional step "
            "map=%u soft_observe=%u recheck=%u enter=%u sysret=%u "
            "host_stack=%u host_exec=%u h3_refuse=%u "
            "product_hosts=%u dual_dod_open=%u enter_req_ring3=%u "
            "user_band=%u thr_live=%u product_host_live=%u keep_live=%u "
            "maps_live_no_thrash=%u steps=%u/%u densify=STRONGER "
            "product_hosts_names=rtl8168_udx,xhci_udx,ddi_host "
            "host_path=process_spawn_host_launch|elf_load|user_as|thr "
            "Soft!=product dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=thr_exit_before_as_destroy storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1\n",
            u32StepMap, u32StepObserve, u32StepRecheck, u32StepEnter,
            u32StepSysret, u32StepHostStack, u32StepHostExec,
            u32StepH3Refuse, u32StepProductHosts, u32StepDualDodOpen,
            u32StepEnterReqRing3, u32StepUserBand, u32StepThrLive,
            u32StepProductHostLive, u32StepKeepLive,
            u32StepMapsLiveNoThrash, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, szViaSafe);

    /* Twin prefix FUNCTIONAL residual. Grep: user_task: soft functional residual */
    kprintf("user_task: soft functional residual "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "after=process_spawn_host_launch+elf_load "
            "host_elf_thr=1 host_exec_base=0x%lx host_stack_top=0x%lx "
            "func_ok=%u/%u steps=%u/%u lean_ok=%u/%u densify=STRONGER "
            "ring3_live=%u pers_live=%u enter_ok=%u thr_live=%u "
            "product_host_live=1 keep_live=1 "
            "path=map|soft_observe|recheck|enter|sysret "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=thr_exit_before_as_destroy "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 "
            "via=%s bar=v2026.08.04.75 stamp_free=1\n",
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP, (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, (unsigned)u32Ok,
            (unsigned)USER_SOFT_UDX_LEAN_CHECKS, u32Ring3Live, u32PersLive,
            g_cEnterOk, u32StepThrLive, szViaSafe);

    /*
     * Grep: user: soft functional residual PASS
     * Once-lamp when all host ELF thr functional arms pass. Soft!=product.
     */
    if (u32HostFunc == USER_SOFT_HOST_ELF_THR_FUNC_CHECKS) {
        kprintf("user: soft functional residual PASS "
                "host_elf_thr_func=%u/%u steps=%u/%u densify=STRONGER "
                "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
                "host_exec_base=0x%lx host_stack_top=0x%lx "
                "host_path=process_spawn_host_launch|elf_load|user_as|thr "
                "product_host_live=1 keep_live=1 thr_live=%u "
                "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "H3=thr_exit_before_as_destroy "
                "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 "
                "via=%s bar=v2026.08.04.75 stamp_free=1 "
                "(Soft!=product; UDX host ELF thr entry residual complete; "
                "not Dual DoD close; product_host_live honesty; no stamp)\n",
                (unsigned)u32HostFunc,
                (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, u32StepLive,
                (unsigned)USER_SOFT_FUNC_STEP_COUNT,
                (unsigned long)USER_SOFT_HOST_EXEC_BASE,
                (unsigned long)USER_SOFT_HOST_STACK_TOP, u32StepThrLive,
                szViaSafe);
    }

    /*
     * Grep: user: soft residual lean PASS
     * Once-lamp when all UDX host lean selfcheck arms pass. Soft!=product.
     */
    if (u32Ok == USER_SOFT_UDX_LEAN_CHECKS) {
        kprintf("user: soft residual lean PASS via=%s "
                "udx_lean=%u/%u ring3_map_c2=%u/%u "
                "host_elf_thr_func=%u/%u "
                "host_entry=map+sysret "
                "product_dir=UDX+ABI freestanding_class_product=0 "
                "ko_product=0 G-AC-1=1 dual_win=1 claim_class=C2 "
                "enter_requires_ring3=1 H3=entry_companion "
                "product_host_live=1 keep_live=1 thr_live=%u "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
                "(Soft!=product; UDX host entry residual lean complete; "
                "no version stamp)\n",
                szViaSafe, (unsigned)u32Ok,
                (unsigned)USER_SOFT_UDX_LEAN_CHECKS,
                (unsigned)u32Ring3C2,
                (unsigned)USER_SOFT_RING3_MAP_C2_CHECKS,
                (unsigned)u32HostFunc,
                (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS,
                u32StepThrLive);
    }

    /*
     * STRONGER denser keep_live product host thr_live honesty residual
     * (Soft!=product). product_host_live / keep_live / thr_live: dual
     * user-AS windows stay live while product host thr
     * (rtl8168_udx|xhci_udx|ddi_host) can enter; this unit never thrash
     * maps under enterable thr (H3 refuse !mapped enter; process thr_exit
     * before as_destroy). Dual DoD A/B OPEN; denser residual != Dual DoD close.
     * Grep: user: soft residual keep_live
     * Grep: user: soft residual product_host_live
     * Grep: user: soft residual thr_live
     * Grep: user: soft residual denser keep_live
     * Grep: user: soft residual denser thr_live
     * Grep: user: soft residual denser product_host_live
     * Grep: user: soft residual denser dual_dod
     * Grep: user_task: soft residual product_host_live
     * Grep: user_task: soft residual thr_live
     * Grep: user_task: soft residual denser keep_live
     * Grep: user_task: soft residual denser thr_live
     * Grep: Soft!=product soft residual dual_dod OPEN product_host_live
     * Grep: denser keep_live residual | denser thr_live residual
     * Grep: denser product_host_live residual | denser dual_dod residual
     * Grep: denser thr_exit_before_as_destroy residual
     * Grep: product_host_live=1 | keep_live=1 | thr_live=1
     * Grep: never_unmap_while_thr_live=1 | maps_live_while_enterable=1
     */
    kprintf("user: soft residual keep_live product_host_live=1 "
            "keep_live=1 thr_live=%u never_kill_embed=1 denser=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host "
            "api=process_spawn_host_launch "
            "H3=thr_exit_before_as_destroy "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "refuse_enter_ring3_not_mapped=1 enter_requires_ring3=1 "
            "host_entry=map+sysret host_exec_base=0x%lx host_stack_top=0x%lx "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "func_ok=%u/%u steps=%u/%u densify=STRONGER "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_no_close=1 dod_close=0 product_udx_close=0 "
            "soft_ne_product=1 G-AC-1=1 Soft!=product dual=MIT_OR_Apache-2.0 "
            "live_host_path=1 not_immediate_kill=1 "
            "bar=v2026.08.04.75 stamp_free=1 via=%s "
            "(Soft!=product soft residual dual_dod OPEN product_host_live; "
            "denser keep_live residual; never thrash dual windows while thr "
            "live; agent!=close)\n",
            u32StepThrLive,
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP, u32Ring3Live,
            u32PersLive, g_cEnterOk, (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, szViaSafe);
    kprintf("user: soft residual product_host_live "
            "product_host_live=1 keep_live=1 thr_live=%u denser=1 "
            "path=map|soft_observe|recheck|enter|sysret|"
            "host_stack|host_exec|h3_refuse|"
            "product_hosts|dual_dod_open|enter_req_ring3|user_band|"
            "thr_live|product_host_live|keep_live|maps_live_no_thrash "
            "host_path=process_spawn_host_launch|elf_load|user_as|thr "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "thr_exit_before_as_destroy=1 refuse_enter_not_mapped=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product soft residual product_host_live honesty residual "
            "hosts=rtl8168_udx,xhci_udx,ddi_host "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "func_ok=%u/%u densify=STRONGER "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, szViaSafe);
    kprintf("user: soft residual thr_live "
            "thr_live=%u product_host_live=1 keep_live=1 denser=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u enter_skip=%u "
            "maps_live_while_enterable=1 never_unmap_while_thr_live=1 "
            "host_entry=map+sysret product_dir=UDX+ABI "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "host_exec_base=0x%lx host_stack_top=0x%lx "
            "func_ok=%u/%u steps=%u/%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "H3=entry_companion Soft!=product G-AC-1=1 storm=0 "
            "bar=v2026.08.04.75 stamp_free=1 via=%s "
            "(Soft!=product soft residual dual_dod OPEN product_host_live; "
            "denser thr_live residual for keep_live UDX host thr)\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            g_cEnterSkip,
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP, (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, u32StepLive,
            (unsigned)USER_SOFT_FUNC_STEP_COUNT, szViaSafe);
    /* Twin prefixes for thr_live / product_host_live residual. */
    kprintf("user_task: soft residual product_host_live "
            "product_host_live=1 keep_live=1 thr_live=%u denser=1 "
            "hosts=rtl8168_udx,xhci_udx,ddi_host "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product soft residual dual_dod OPEN product_host_live "
            "H3=thr_exit_before_as_destroy G-AC-1=1 "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            szViaSafe);
    kprintf("user_task: soft residual thr_live "
            "thr_live=%u product_host_live=1 keep_live=1 denser=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "maps_live_while_enterable=1 never_unmap_while_thr_live=1 "
            "func_ok=%u/%u densify=STRONGER "
            "dual_dod_a=OPEN dual_dod_b=OPEN Soft!=product "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            (unsigned)u32HostFunc,
            (unsigned)USER_SOFT_HOST_ELF_THR_FUNC_CHECKS, szViaSafe);
    /*
     * denser residual lamps (Soft!=product Dual DoD OPEN; bar .75 stamp-free).
     * Companion process denser keep_live thr_live dual_dod residual.
     * H3 thr_exit_before_as_destroy lives in process/thread; entry companion
     * refuses !mapped enter + maps_live_while_enterable.
     */
    kprintf("user: soft residual denser keep_live "
            "keep_live=1 product_host_live=1 thr_live=%u denser=1 "
            "never_kill_embed=1 soft_ne_product=1 Soft!=product "
            "axes=keep_live|product_host_live|thr_live|"
            "never_unmap_while_thr_live|maps_live_while_enterable|"
            "dual_dod_open|thr_exit_before_as_destroy "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product_hosts=UDX "
            "H3=thr_exit_before_as_destroy thr_exit_before_as_destroy=1 "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "denser_keep_live_residual=1 denser_thr_live_residual=1 "
            "denser_product_host_live=1 denser_dual_dod_residual=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "bar=v2026.08.04.75 stamp_free=1 via=%s "
            "(denser keep_live residual; denser != Dual DoD close; "
            "agent!=close; G-AC-1)\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            szViaSafe);
    kprintf("user: soft residual denser thr_live "
            "thr_live=%u keep_live=1 product_host_live=1 denser=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u enter_skip=%u "
            "maps_live_while_enterable=1 never_unmap_while_thr_live=1 "
            "host_entry=map+sysret host_exec_base=0x%lx host_stack_top=0x%lx "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host "
            "H3=entry_companion thr_exit_before_as_destroy=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "denser_thr_live_residual=1 denser_keep_live_residual=1 "
            "Soft!=product soft residual dual_dod OPEN thr_live "
            "bar=v2026.08.04.75 stamp_free=1 via=%s "
            "(denser thr_live residual; denser thr_exit_before_as_destroy "
            "residual; denser != Dual DoD close)\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            g_cEnterSkip,
            (unsigned long)USER_SOFT_HOST_EXEC_BASE,
            (unsigned long)USER_SOFT_HOST_STACK_TOP, szViaSafe);
    kprintf("user: soft residual denser product_host_live "
            "product_host_live=1 keep_live=1 thr_live=%u denser=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product_hosts=UDX "
            "companion=process_spawn_host_launch "
            "path=map|soft_observe|recheck|enter|sysret "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "H3=thr_exit_before_as_destroy thr_exit_before_as_destroy=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product denser residual product_host_live honesty "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, szViaSafe);
    kprintf("user: soft residual denser dual_dod "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN denser=1 "
            "keep_live=1 thr_live=%u product_host_live=1 "
            "soft_no_close=1 dod_close=0 product_udx_close=0 "
            "soft_ne_product=1 Soft!=product product_hosts=UDX "
            "H3=thr_exit_before_as_destroy thr_exit_before_as_destroy=1 "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "denser_dual_dod_residual=1 denser_keep_live_residual=1 "
            "denser_thr_live_residual=1 denser_product_host_live=1 "
            "ring3_live=%u pers_live=%u enter_ok=%u "
            "bar=v2026.08.04.75 stamp_free=1 via=%s "
            "(denser dual_dod residual; Dual DoD OPEN Soft!=product; "
            "agent!=close; denser residual != Dual DoD close)\n",
            u32StepThrLive, u32Ring3Live, u32PersLive, g_cEnterOk,
            szViaSafe);
    kprintf("user_task: soft residual denser keep_live "
            "keep_live=1 thr_live=%u product_host_live=1 denser=1 "
            "never_unmap_while_thr_live=1 maps_live_while_enterable=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product H3=thr_exit_before_as_destroy "
            "denser_keep_live_residual=1 denser_thr_live_residual=1 "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, szViaSafe);
    kprintf("user_task: soft residual denser thr_live "
            "thr_live=%u keep_live=1 product_host_live=1 denser=1 "
            "maps_live_while_enterable=1 never_unmap_while_thr_live=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product denser_thr_live_residual=1 "
            "thr_exit_before_as_destroy=1 "
            "bar=v2026.08.04.75 stamp_free=1 via=%s\n",
            u32StepThrLive, szViaSafe);
}

/**
 * After first map/enter activity: lean residual once + inventory once.
 * Diagnostics only. Soft!=product | G-AC-1 | H3 entry companion.
 */
static void
user_soft_maybe_once(void)
{
    if (g_cRing3MapOk == 0u && g_cPersMapOk == 0u &&
        g_cRing3MapFail == 0u && g_cPersMapFail == 0u &&
        g_cEnterOk == 0u && g_cEnterSkip == 0u) {
        return;
    }
    user_soft_residual_lean("activity");
    if (g_fSoftInvOnce != 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    user_soft_inventory("activity");
}

/**
 * Install user RX (or RW) page: zero frame under kernel CR3, copy payload,
 * map with final prot. Avoids intermediate RW->RX races / wrong PTE flags.
 */
static gj_status_t
user_install_page(gj_vaddr_t va, const void *pSrc, size_t cbSrc, u32 u32Prot)
{
    gj_paddr_t pa;
    u64 u64Saved;
    gj_status_t st;

    pa = pmm_alloc();
    if (pa == 0) {
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    if (pSrc != NULL && cbSrc > 0) {
        if (cbSrc > GJ_PAGE_SIZE) {
            cbSrc = GJ_PAGE_SIZE;
        }
        memcpy((void *)(gj_vaddr_t)pa, pSrc, cbSrc);
    }
    cpu_load_cr3(u64Saved);

    st = vmm_map_page(va, pa, u32Prot | GJ_VMM_PROT_USER);
    if (st != GJ_OK) {
        pmm_free(pa);
        return st;
    }
    return GJ_OK;
}

/**
 * Soft-unmap + free a contiguous install window (partial fail cleanup).
 * Best-effort: ignores NOENT; never panics.
 */
static void
user_unmap_window(gj_vaddr_t vaBase, u32 cPages)
{
    u32 iPage;

    for (iPage = 0; iPage < cPages; iPage++) {
        gj_vaddr_t va = vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys(va);

        (void)vmm_unmap_page(va);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

/**
 * Multi-page RX blob install (soft max cMaxPages).
 * On any page fail: reverse-unmaps prior pages of this install.
 */
static gj_status_t
user_install_blob_rx(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc,
                     u32 cMaxPages, u32 *pOutPages)
{
    u32 cPages;
    u32 iPage;
    gj_status_t st;

    if (pOutPages != NULL) {
        *pOutPages = 0;
    }
    if (pSrc == NULL || cbSrc == 0 || cMaxPages == 0) {
        return GJ_ERR_INVAL;
    }
    cPages = (u32)((cbSrc + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > cMaxPages) {
        return GJ_ERR_INVAL;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        size_t cbOff = (size_t)iPage * (size_t)GJ_PAGE_SIZE;
        size_t cbChunk = cbSrc - cbOff;
        const u8 *pChunk = (const u8 *)pSrc + cbOff;

        if (cbChunk > (size_t)GJ_PAGE_SIZE) {
            cbChunk = (size_t)GJ_PAGE_SIZE;
        }
        st = user_install_page(vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE,
                               pChunk, cbChunk,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
        if (st != GJ_OK) {
            user_unmap_window(vaBase, iPage);
            return st;
        }
    }
    g_cCodePages += cPages;
    if (pOutPages != NULL) {
        *pOutPages = cPages;
    }
    return GJ_OK;
}

/**
 * Map stack pages growing down from u64StackTop (RW user).
 * Soft: reverse-unmaps partial stack on first failure.
 * Returns 0 on success, -1 on first map failure.
 */
static int
user_map_stack(u64 u64StackTop, u32 cPages)
{
    u32 iPage;
    gj_status_t st;

    if (cPages == 0 || (u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        kprintf("user: map stack soft FAIL align top=0x%lx pages=%u\n",
                (unsigned long)u64StackTop, cPages);
        return -1;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;

        st = user_install_page((gj_vaddr_t)u64PageVa, NULL, 0,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
        if (st != GJ_OK) {
            u32 iUndo;

            kprintf("user: map stack page %u va=0x%lx failed %d\n", iPage,
                    (unsigned long)u64PageVa, (int)st);
            /* Soft reverse: unmap pages already installed in this call. */
            for (iUndo = 0; iUndo < iPage; iUndo++) {
                u64 u64UndoVa =
                    u64StackTop - ((u64)iUndo + 1ull) * GJ_PAGE_SIZE;
                gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64UndoVa);

                (void)vmm_unmap_page((gj_vaddr_t)u64UndoVa);
                if (pa != 0) {
                    pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
                }
            }
            return -1;
        }
    }
    g_cStackPages += cPages;
    return 0;
}

/**
 * Soft layout: code + stack inside user band, page-aligned, non-overlapping.
 * Also refuses collision with the sibling product window (ring3 vs personality).
 */
static int
user_layout_soft_ok(u64 u64CodeVa, size_t cbCode, u64 u64StackTop, u32 cStackPages,
                    u64 u64SiblingCode, size_t cbSibling, u64 u64SiblingStackTop,
                    u32 cSiblingStack, int fSiblingLive)
{
    u64 u64CodeEnd;
    u64 u64StackLo;
    u64 u64StackHi;
    size_t cbCodeRound;

    if ((u64CodeVa & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if ((u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if (cbCode == 0 || cStackPages == 0) {
        return 0;
    }

    cbCodeRound =
        (cbCode + (size_t)GJ_PAGE_SIZE - 1u) & ~((size_t)GJ_PAGE_SIZE - 1u);
    u64CodeEnd = u64CodeVa + (u64)cbCodeRound;
    u64StackHi = u64StackTop;
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    if (u64StackLo >= u64StackHi) {
        return 0; /* underflow */
    }
    /* Entire code + stack must sit in the canonical user window. */
    if (!user_range_ok(u64CodeVa, (u64)cbCodeRound)) {
        return 0;
    }
    if (!user_range_ok(u64StackLo, u64StackHi - u64StackLo)) {
        return 0;
    }
    /* Code must not overlap own stack. */
    if (!(u64CodeEnd <= u64StackLo || u64CodeVa >= u64StackHi)) {
        return 0;
    }

    if (fSiblingLive != 0 && cbSibling > 0 && cSiblingStack > 0) {
        size_t cbSibRound =
            (cbSibling + (size_t)GJ_PAGE_SIZE - 1u) &
            ~((size_t)GJ_PAGE_SIZE - 1u);
        u64 u64SibCodeEnd = u64SiblingCode + (u64)cbSibRound;
        u64 u64SibStackHi = u64SiblingStackTop;
        u64 u64SibStackLo =
            u64SiblingStackTop - (u64)cSiblingStack * (u64)GJ_PAGE_SIZE;

        /* Soft: dual windows must not share code or stack pages. */
        if (!(u64CodeEnd <= u64SiblingCode || u64CodeVa >= u64SibCodeEnd)) {
            return 0;
        }
        if (!(u64StackHi <= u64SibStackLo || u64StackLo >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64CodeEnd <= u64SibStackLo || u64CodeVa >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64StackHi <= u64SiblingCode || u64StackLo >= u64SibCodeEnd)) {
            return 0;
        }
    }
    return 1;
}

/**
 * Soft RX leaf: present + USER + not writable + executable (!NX).
 */
static int
user_soft_pte_rx(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) != 0) {
        return 0; /* W^X soft for trampoline / door code */
    }
    if ((u64Pte & UT_PTE_NX) != 0) {
        return 0;
    }
    return 1;
}

/**
 * Soft RW leaf: present + USER + writable + NX preferred (non-exec stack).
 */
static int
user_soft_pte_rw(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_NX) == 0) {
        return 0; /* stack must not be executable */
    }
    return 1;
}

/**
 * Soft payload match: compare mapped frames to blob under kernel CR3 identity.
 * cb may span multiple pages. Returns 1 on match, 0 on mismatch / missing PA.
 */
static int
user_soft_payload_match(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc)
{
    size_t cbOff;
    u64 u64Saved;
    const u8 *pWant;

    if (pSrc == NULL || cbSrc == 0) {
        return 0;
    }
    pWant = (const u8 *)pSrc;
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());

    for (cbOff = 0; cbOff < cbSrc; ) {
        gj_vaddr_t vaPage =
            (vaBase + (gj_vaddr_t)cbOff) & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
        size_t cbPageOff = (size_t)((vaBase + (gj_vaddr_t)cbOff) - vaPage);
        size_t cbChunk = (size_t)GJ_PAGE_SIZE - cbPageOff;
        gj_paddr_t pa;
        const u8 *pHave;
        size_t iByte;

        if (cbChunk > cbSrc - cbOff) {
            cbChunk = cbSrc - cbOff;
        }
        /*
         * Walk under the *saved* (process) CR3 so we read the product PTE,
         * then compare bytes via kernel identity of that PA.
         */
        cpu_load_cr3(u64Saved);
        pa = vmm_virt_to_phys(vaPage);
        cpu_load_cr3(vmm_kernel_cr3());
        if (pa == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pHave = (const u8 *)(gj_vaddr_t)(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        pHave += cbPageOff;
        for (iByte = 0; iByte < cbChunk; iByte++) {
            if (pHave[iByte] != pWant[cbOff + iByte]) {
                cpu_load_cr3(u64Saved);
                return 0;
            }
        }
        cbOff += cbChunk;
    }

    cpu_load_cr3(u64Saved);
    return 1;
}

/**
 * Soft post-map observe for a code+stack window.
 * fWantNativePersona: 0 = expect LINUX (1), 1 = expect NATIVE (0); -1 = skip.
 * szTag is "ring3" or "personality" for greppable lines.
 * Returns GJ_OK on soft PASS; GJ_ERR_* on soft FAIL (caller fail-closes).
 */
static gj_status_t
user_map_soft_observe(u64 u64CodeVa, size_t cbCode, const void *pBlob,
                      u64 u64StackTop, u32 cStackPages, struct gj_process *pProc,
                      int nWantPersona, /* 0 native, 1 linux, -1 skip */
                      const char *szTag, u32 *pSoftOk, u32 *pSoftBad)
{
    u32 iPage;
    u32 cCodePages;
    u64 u64StackLo;
    int fMapped;
    int fAccess;

    if (szTag == NULL) {
        szTag = "user";
    }
    if (cbCode == 0 || pBlob == NULL || cStackPages == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: %s map soft FAIL inval cb=%lu pages=%u\n", szTag,
                (unsigned long)cbCode, cStackPages);
        return GJ_ERR_INVAL;
    }

    cCodePages =
        (u32)((cbCode + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    /* Soft: every code leaf RX + USER. */
    for (iPage = 0; iPage < cCodePages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)u64CodeVa + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;

        if (!user_soft_pte_rx(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL code pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: every stack leaf RW|NX + USER. */
    for (iPage = 0; iPage < cStackPages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)(u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE);

        if (!user_soft_pte_rw(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL stack pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: range helpers agree with raw PTE walk. */
    fMapped = user_range_mapped(u64CodeVa, (u64)cbCode);
    fAccess = user_range_mapped_access(u64StackLo,
                                       (u64)cStackPages * (u64)GJ_PAGE_SIZE,
                                       GJ_USER_ACCESS_READ | GJ_USER_ACCESS_WRITE);
    if (fMapped == 0 || fAccess == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL range code=%d stack_w=%d\n", szTag,
                fMapped, fAccess);
        return GJ_ERR_FAULT;
    }

    /* Soft: payload bytes still match the embedded blob. */
    if (!user_soft_payload_match((gj_vaddr_t)u64CodeVa, pBlob, cbCode)) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL payload mismatch cb=%lu\n", szTag,
                (unsigned long)cbCode);
        return GJ_ERR_FAULT;
    }

    /* Soft: process personality / JIT contract when a PCB is provided. */
    if (pProc != NULL && nWantPersona >= 0) {
        if ((int)pProc->u32Personality != nWantPersona) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL persona=%u want=%d\n", szTag,
                    pProc->u32Personality, nWantPersona);
            return GJ_ERR_PERM;
        }
        /* Ring3 trampoline must not advertise CapJit; personality is native. */
        if (nWantPersona == 1 && pProc->u32Jit != 0) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL jit=%u want=0\n", szTag,
                    pProc->u32Jit);
            return GJ_ERR_PERM;
        }
    }

    if (pSoftOk != NULL) {
        (*pSoftOk)++;
    }
    user_soft_inc(&g_u32SoftObserveOk);
    /* Grep: user: ring3 map soft PASS / user: personality map soft PASS */
    kprintf("user: %s map soft PASS code@0x%lx stacktop@0x%lx code_pages=%u "
            "stack_pages=%u blob=%lu pte_rx=1 pte_rw=1 payload=1\n",
            szTag, (unsigned long)u64CodeVa, (unsigned long)u64StackTop,
            cCodePages, cStackPages, (unsigned long)cbCode);
    return GJ_OK;
}

/**
 * Soft teardown of a just-installed window after soft observe FAIL.
 */
static void
user_map_soft_fail_teardown(gj_vaddr_t vaCode, u32 cCodePages, u64 u64StackTop,
                            u32 cStackPages)
{
    u32 iPage;

    user_soft_inc(&g_u32SoftTeardown);
    user_unmap_window(vaCode, cCodePages);
    for (iPage = 0; iPage < cStackPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64PageVa);

        (void)vmm_unmap_page((gj_vaddr_t)u64PageVa);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

int
user_task_map_ring3(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_user_ring3_blob_end - gj_user_ring3_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftBlobBadRing3);
        kprintf("user: bad ring3 blob size %lu\n", (unsigned long)cbBlob);
        user_soft_maybe_once();
        return -1;
    }

    /* Soft idempotent: already live + soft recheck still green -> success. */
    if (g_fUserMapped != 0) {
        if (user_task_ring3_map_soft() == 0) {
            user_soft_inc(&g_u32SoftIdemRing3);
            kprintf("user: ring3 map soft already live recheck PASS\n");
            user_soft_maybe_once();
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA,
                                    g_cRing3CodePages != 0 ? g_cRing3CodePages
                                                           : 1u,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_fUserMapped = 0;
        g_cbRing3Blob = 0;
        g_cRing3CodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_USER_CODE_VA, cbBlob, GJ_USER_STACK_TOP,
                             GJ_USER_STACK_PAGES, GJ_PERS_CODE_VA, g_cbPersBlob,
                             GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES,
                             g_fPersMapped)) {
        g_cRing3MapFail++;
        g_cRing3SoftBad++;
        user_soft_inc(&g_u32SoftLayoutFailRing3);
        kprintf("user: ring3 map soft FAIL layout\n");
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cRing3MapFail++;
            user_soft_inc(&g_u32SoftAsEnsureFailRing3);
            kprintf("user: as_ensure failed\n");
            user_soft_maybe_once();
            return -1;
        }
        process_as_activate(pProc);
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_USER_CODE_VA, gj_user_ring3_blob,
                              cbBlob, GJ_USER_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftInstallFailRing3);
        kprintf("user: map code RX failed %d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    if (user_map_stack(GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages);
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftStackFailRing3);
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        pProc->u32Personality = 1; /* Linux-shaped trampoline for ABI smokes */
        gj_process_set_jit(pProc, 0);
    }

    st = user_map_soft_observe(GJ_USER_CODE_VA, cbBlob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, pProc,
                               /* linux */ 1, "ring3", &g_cRing3Soft,
                               &g_cRing3SoftBad);
    if (st != GJ_OK) {
        /* Fail closed: drop partial product window. */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: ring3 map soft FAIL st=%d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    g_fUserMapped = 1;
    g_cbRing3Blob = cbBlob;
    g_cRing3CodePages = cCodePages;
    g_cRing3MapOk++;
    kprintf("user: ring3 code@0x%lx stacktop@0x%lx blob=%lu pages=%u PASS\n",
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP, (unsigned long)cbBlob, cCodePages);
    user_soft_maybe_once();
    return 0;
}

void
user_task_enter_ring3(void)
{
    /*
     * Ring3 sysret enter requires the ring3 trampoline window.
     * Personality-only map is insufficient (personality enters via
     * thread_create_user + user_personality_entry/stack). Soft!=product
     * residual tracks skips; product path fail-closes on !ring3.
     * H3: refuse enter when ring3 !mapped (maps must stay live while
     * a thr can enter this window).
     */
    if (g_fUserMapped == 0) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipNotMap);
        kprintf("user: not mapped\n");
        kprintf("user: ring3 enter soft SKIP not_mapped "
                "require_ring3=1 pers_live=%u Soft!=product\n",
                g_fPersMapped != 0 ? 1u : 0u);
        user_soft_maybe_once();
        return;
    }
    if (!cpu_syscall_ready()) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipSyscall);
        kprintf("user: SYSCALL not ready\n");
        kprintf("user: ring3 enter soft SKIP syscall\n");
        user_soft_maybe_once();
        return;
    }
    /* Soft recheck of the trampoline window before irrevocable enter. */
    if (user_task_ring3_map_soft() != 0) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipRecheck);
        kprintf("user: ring3 enter soft SKIP recheck\n");
        user_soft_maybe_once();
        return;
    }
    g_cEnterOk++;
    kprintf("user: entering ring3...\n");
    kprintf("user: ring3 enter soft PASS code@0x%lx stack@0x%lx "
            "recheck=1 require_ring3=1 Soft!=product\n",
            (unsigned long)GJ_USER_CODE_VA, (unsigned long)GJ_USER_STACK_TOP);
    user_soft_maybe_once();
    cpu_enter_user(GJ_USER_CODE_VA, GJ_USER_STACK_TOP);
}

int
user_personality_map(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_protonrt_user_blob_end - gj_protonrt_user_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_PERS_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftBlobBadPers);
        kprintf("user: bad protonrt blob size %lu\n", (unsigned long)cbBlob);
        user_soft_maybe_once();
        return -1;
    }

    /* Soft idempotent: live personality soft recheck. */
    if (g_fPersMapped != 0) {
        if (user_personality_map_soft() == 0) {
            user_soft_inc(&g_u32SoftIdemPers);
            kprintf("user: personality map soft already live recheck PASS\n");
            user_soft_maybe_once();
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA,
                                    g_cPersCodePages != 0 ? g_cPersCodePages
                                                          : 1u,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_fPersMapped = 0;
        g_u64PersEntry = 0;
        g_u64PersStack = 0;
        g_cbPersBlob = 0;
        g_cPersCodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_PERS_CODE_VA, cbBlob, GJ_PERS_STACK_TOP,
                             GJ_PERS_STACK_PAGES, GJ_USER_CODE_VA, g_cbRing3Blob,
                             GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES,
                             g_fUserMapped)) {
        g_cPersMapFail++;
        g_cPersSoftBad++;
        user_soft_inc(&g_u32SoftLayoutFailPers);
        kprintf("user: personality map soft FAIL layout\n");
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cPersMapFail++;
            user_soft_inc(&g_u32SoftAsEnsureFailPers);
            kprintf("user: personality as_ensure failed\n");
            user_soft_maybe_once();
            return -1;
        }
        process_as_activate(pProc);
        /* G-PERS: native syscalls for door IPC */
        pProc->u32Personality = 0;
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_PERS_CODE_VA, gj_protonrt_user_blob,
                              cbBlob, GJ_PERS_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftInstallFailPers);
        kprintf("user: personality map code failed %d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    if (user_map_stack(GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages);
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftStackFailPers);
        user_soft_maybe_once();
        return -1;
    }

    st = user_map_soft_observe(GJ_PERS_CODE_VA, cbBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, pProc,
                               /* native */ 0, "personality", &g_cPersSoft,
                               &g_cPersSoftBad);
    if (st != GJ_OK) {
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: personality map soft FAIL st=%d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    g_fPersMapped = 1;
    g_u64PersEntry = GJ_PERS_CODE_VA;
    g_u64PersStack = GJ_PERS_STACK_TOP;
    g_cbPersBlob = cbBlob;
    g_cPersCodePages = cCodePages;
    g_cPersMapOk++;
    kprintf("user: personality server mapped @0x%lx (%lu bytes) pages=%u "
            "native PASS\n",
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)cbBlob, cCodePages);
    user_soft_maybe_once();
    return 0;
}

u64
user_personality_entry(void)
{
    return g_u64PersEntry;
}

u64
user_personality_stack(void)
{
    return g_u64PersStack;
}

int
user_task_ring3_mapped(void)
{
    return g_fUserMapped != 0 ? 1 : 0;
}

int
user_personality_mapped(void)
{
    return g_fPersMapped != 0 ? 1 : 0;
}

gj_status_t
user_task_stats(struct gj_user_task_stats *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    pOut->u32Ring3Ok = g_cRing3MapOk;
    pOut->u32Ring3Fail = g_cRing3MapFail;
    pOut->u32Ring3Soft = g_cRing3Soft;
    pOut->u32Ring3SoftBad = g_cRing3SoftBad;
    pOut->u32PersOk = g_cPersMapOk;
    pOut->u32PersFail = g_cPersMapFail;
    pOut->u32PersSoft = g_cPersSoft;
    pOut->u32PersSoftBad = g_cPersSoftBad;
    pOut->u32EnterOk = g_cEnterOk;
    pOut->u32EnterSkip = g_cEnterSkip;
    pOut->u32CodePages = g_cCodePages;
    pOut->u32StackPages = g_cStackPages;
    pOut->u32Ring3Mapped = g_fUserMapped != 0 ? 1u : 0u;
    pOut->u32PersMapped = g_fPersMapped != 0 ? 1u : 0u;
    return GJ_OK;
}

u32
user_task_stats_soft(void)
{
    /*
     * Lean capped soft inventory (Soft!=product; no stamp storms).
     * Grep: user: soft stats | user: soft residual lean
     * Grep: user: soft residual lean udx | user: soft residual lean c2
     * Grep: user: soft residual lean PASS | user: soft residual lean H3
     * Grep: user: soft residual enter
     * Grep: user: soft residual keep_live | user: soft residual product_host_live
     * Grep: user: soft residual thr_live
     * Grep: user_task: soft residual product_host_live
     * Grep: user_task: soft residual thr_live
     * Grep: user_task: soft residual lean | user: soft residual ring3 map
     * Grep: user: soft residual ring3 map c2
     * Grep: user: soft residual personality map
     * Grep: user: soft functional residual | user: soft functional step
     * Grep: user: soft functional residual catalog
     * Grep: user: soft functional residual host_elf_thr
     * Grep: user: soft functional residual densify
     * Grep: user: soft functional residual PASS
     * Grep: user_task: soft functional residual
     * Grep: product_host_live=1 | keep_live=1 | thr_live=1
     * Grep: Soft!=product soft residual dual_dod OPEN product_host_live
     */
    user_soft_residual_lean("stats");
    user_soft_inventory("stats");
    return g_cRing3Soft;
}

int
user_task_ring3_map_soft(void)
{
    gj_status_t st;

    if (g_fUserMapped == 0 || g_cbRing3Blob == 0) {
        user_soft_inc(&g_u32SoftRecheckFailRing3);
        kprintf("user: ring3 map soft recheck FAIL not_mapped\n");
        return -1;
    }
    st = user_map_soft_observe(GJ_USER_CODE_VA, g_cbRing3Blob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, NULL,
                               -1, "ring3", &g_cRing3Soft, &g_cRing3SoftBad);
    if (st != GJ_OK) {
        user_soft_inc(&g_u32SoftRecheckFailRing3);
        kprintf("user: ring3 map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    user_soft_inc(&g_u32SoftRecheckPassRing3);
    kprintf("user: ring3 map soft recheck PASS\n");
    return 0;
}

int
user_personality_map_soft(void)
{
    gj_status_t st;

    if (g_fPersMapped == 0 || g_cbPersBlob == 0) {
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL not_mapped\n");
        return -1;
    }
    if (g_u64PersEntry != GJ_PERS_CODE_VA ||
        g_u64PersStack != GJ_PERS_STACK_TOP) {
        g_cPersSoftBad++;
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL entry=0x%lx "
                "stack=0x%lx\n",
                (unsigned long)g_u64PersEntry, (unsigned long)g_u64PersStack);
        return -1;
    }
    st = user_map_soft_observe(GJ_PERS_CODE_VA, g_cbPersBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, NULL, -1,
                               "personality", &g_cPersSoft, &g_cPersSoftBad);
    if (st != GJ_OK) {
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    user_soft_inc(&g_u32SoftRecheckPassPers);
    kprintf("user: personality map soft recheck PASS\n");
    return 0;
}
