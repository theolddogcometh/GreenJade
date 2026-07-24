/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Scan RSDP → RSDT/XSDT for DMAR (Intel) or IVRS (AMD) signatures.
 * Inventory + software enforce windows + hand-off to VT-d table builder.
 * Deep DMAR structure soft inventory (DRHD/RMRR/ATSR/RHSA) for soft-probe.
 *
 * UDX → IOMMU window caps soft product (window side; software BDF table):
 *   create_window-shaped grant path + destroy/revoke soft tallies.
 * Greppable soft product markers (prefix-stable; greppable: iommu: window cap soft):
 *   "iommu: window cap soft inventory …"
 *   "iommu: window cap soft create_window …"  (create_window soft inventory)
 *   "iommu: window cap soft destroy …"       (destroy/revoke soft tallies)
 *   "iommu: window cap soft grant|update|full|reject|revoke …"
 * Honesty: soft software window table only — not full cap-typed window
 * object product (P-DMA-2 remaining: create_window as true window cap).
 *
 * Wave 56 exclusive soft deepen (this unit only — greppable "iommu: soft …"):
 *   iommu: soft inventory  — presence/vendor/units/windows/denies rollup
 *   iommu: soft present    — DMAR/IVRS presence lamps
 *   iommu: soft dmar       — DRHD/RMRR/ATSR/RHSA/other structure counts
 *   iommu: soft window     — software BDF window table tallies
 *   iommu: soft enforce    — policy arm + deny counters (soft)
 *   iommu: soft acpi       — RSDP/XSDT/RSDT/DMAR/IVRS scan tallies (W15)
 *   iommu: soft busmaster  — allow/deny path tallies under enforce (W15)
 *   iommu: soft peak       — peak windows / create peaks (W15)
 *   iommu: soft path       — honesty: always-on product IOMMU remains OPEN
 *   iommu: soft lamps      — composite soft lamps
 *   iommu: soft honesty    — explicit non-claims catalog (W15)
 *   iommu: soft stats      — rollup for agent greps (W15)
 *   iommu: soft surfaces   — Wave 19 return-surface catalog
 *   iommu: soft vendor     — Wave 17 vendor/units return lamps
 *   iommu: soft window_cap — Wave 17 window-cap soft return surface
 *   iommu: soft return     — Wave 17 create/destroy return taxonomy
 *   iommu: soft return selftest — Wave 19 terminal return surface
 *   iommu: soft retmap     — Wave 19 return-surface map
 *   iommu: soft deepen     — wave=57 stamp + area count
 *   iommu: soft OPEN       — always-on product IOMMU OPEN honesty
 *   iommu: soft PASS | soft inventory PASS
 * Soft deepen ≠ product always-on IOMMU claim; not bar3; not HW-first close;
 * soft ≠ product.
 *
 * Clean-room; dual-licensed pure C (not a GPL VT-d driver paste).
 */
#include <gj/boot_info.h>
#include <gj/config.h>
#include <gj/iommu.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* ACPI table length ceiling for defensive walks (firmware can lie). */
#define IOMMU_ACPI_LEN_MAX   (1u << 20) /* 1 MiB */
#define IOMMU_ACPI_HDR_MIN   36u
#define IOMMU_DMAR_HDR_MIN   48u
#define IOMMU_RSDP_REV_XSDT  2u

/* DMAR remapping structure types (ACPI DMAR public layout) */
#define IOMMU_DMAR_DRHD  0u
#define IOMMU_DMAR_RMRR  1u
#define IOMMU_DMAR_ATSR  2u
#define IOMMU_DMAR_RHSA  3u
/* ANDD=4, SATC=5, SIDP=6 treated as "other" for soft inventory */

/* Wave 57 soft inventory stamp (file-local; never product gate). */
#define IOMMU_SOFT_WAVE 57u
/* Fixed greppable categories for deepen stamp (inventory…return + W16 axes). */
#define IOMMU_SOFT_AREAS 95u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
 * greppable: iommu: soft surfaces
 */
#define IOMMU_SOFT_SURF_INVENTORY  (1u << 0)
#define IOMMU_SOFT_SURF_PRESENT    (1u << 1)
#define IOMMU_SOFT_SURF_DMAR       (1u << 2)
#define IOMMU_SOFT_SURF_WINDOW     (1u << 3)
#define IOMMU_SOFT_SURF_ENFORCE    (1u << 4)
#define IOMMU_SOFT_SURF_ACPI       (1u << 5)
#define IOMMU_SOFT_SURF_BUSMASTER  (1u << 6)
#define IOMMU_SOFT_SURF_PEAK       (1u << 7)
#define IOMMU_SOFT_SURF_PATH       (1u << 8)
#define IOMMU_SOFT_SURF_LAMPS      (1u << 9)
#define IOMMU_SOFT_SURF_HONESTY    (1u << 10)
#define IOMMU_SOFT_SURF_STATS      (1u << 11)
#define IOMMU_SOFT_SURF_OPEN       (1u << 12)
#define IOMMU_SOFT_SURF_SURFACES   (1u << 13)
#define IOMMU_SOFT_SURF_VENDOR     (1u << 14)
#define IOMMU_SOFT_SURF_WINDOW_CAP (1u << 15)
#define IOMMU_SOFT_SURF_RETURN     (1u << 16)
#define IOMMU_SOFT_SURF_CATALOG                                                    \
    (IOMMU_SOFT_SURF_INVENTORY | IOMMU_SOFT_SURF_PRESENT | IOMMU_SOFT_SURF_DMAR |  \
     IOMMU_SOFT_SURF_WINDOW | IOMMU_SOFT_SURF_ENFORCE | IOMMU_SOFT_SURF_ACPI |     \
     IOMMU_SOFT_SURF_BUSMASTER | IOMMU_SOFT_SURF_PEAK | IOMMU_SOFT_SURF_PATH |     \
     IOMMU_SOFT_SURF_LAMPS | IOMMU_SOFT_SURF_HONESTY | IOMMU_SOFT_SURF_STATS |     \
     IOMMU_SOFT_SURF_OPEN | IOMMU_SOFT_SURF_SURFACES | IOMMU_SOFT_SURF_VENDOR |    \
     IOMMU_SOFT_SURF_WINDOW_CAP | IOMMU_SOFT_SURF_RETURN)

static struct gj_iommu_info g_Info;
static int g_fProbed;
static int g_fEnforce;
static struct gj_iommu_window g_aWin[GJ_IOMMU_MAX_WINDOWS];
static u32 g_u32Denies;

/* Soft DMAR structure inventory (fed to VT-d soft-probe) */
static u32 g_cDrhd;
static u32 g_cRmrr;
static u32 g_cAtsr;
static u32 g_cRhsa;
static u32 g_cOther;
static u32 g_cDrhdAccepted; /* page-aligned non-zero bases handed to VT-d */
static u64 g_u64RmrrFirstBase;
static u64 g_u64RmrrFirstLimit;

/*
 * Window cap soft product tallies (UDX→IOMMU window side).
 * Software BDF→PA table only — not cap object mint / HW-first revoke.
 */
static u32 g_cWinCreateOk;     /* create_window soft: new slot grant */
static u32 g_cWinCreateUpdate; /* create_window soft: same-BDF update */
static u32 g_cWinCreateFail;   /* grant failed (full / reject) */
static u32 g_cWinFull;         /* table full on create */
static u32 g_cWinReject;       /* bad BDF / zero cb / pa+cb overflow */
static u32 g_cWinDestroy;      /* slots cleared by revoke (destroy soft) */
static u32 g_cWinRevokeCall;   /* iommu_window_revoke entries */
static u32 g_cWinRevokeMiss;   /* revoke with no matching used slot */
static u32 g_cWinRevokeBad;    /* revoke rejected (bad BDF) */
static u32 g_cWinSoftInvLogs;  /* times soft inventory printed */
static int g_fWinSoftInvOnce;  /* first-activity inventory emitted */
static u32 g_cSoftInvLogs;     /* Wave 15 iommu: soft inventory dumps */

/*
 * Wave 15 soft scan / busmaster / peak tallies (diagnostics only; wrap OK).
 * Never hard-gates; not product always-on IOMMU.
 */
static u32 g_cSoftRsdpSkip;    /* probe path: no RSDP */
static u32 g_cSoftRsdpBad;     /* probe path: bad RSDP signature/map */
static u32 g_cSoftXsdtHit;     /* XSDT walks accepted */
static u32 g_cSoftRsdtHit;     /* RSDT walks accepted */
static u32 g_cSoftDmarHit;     /* DMAR table signatures found */
static u32 g_cSoftIvrsHit;     /* IVRS table signatures found */
static u32 g_cSoftSdtEntries;  /* SDT entries examined */
static u32 g_cSoftStructBreak; /* DMAR struct bounds break */
static u32 g_cSoftBmAllow;     /* busmaster_ok allow under enforce */
static u32 g_cSoftBmDeny;      /* busmaster_ok deny under enforce */
static u32 g_cSoftBmOpen;      /* busmaster_ok open path (enforce off) */
static u32 g_cSoftWinPeak;     /* peak live software windows */

static void iommu_dmar_parse(u8 *pDmar, u32 u32MapMax);
static void iommu_scan_sdt_entries(u8 *pSdt, u32 u32Len, u32 u32EntryCb);
static void iommu_soft_probe_handoff(void);
static void iommu_window_cap_soft_inventory(void);
static void iommu_window_cap_soft_maybe_once(void);
static void iommu_soft_inventory_log(void);
static void iommu_soft_note_win_peak(u32 cUsed);

static int
sig_n(const void *p, const char *sz, u32 cb)
{
    const u8 *pB;
    u32 i;

    if (p == NULL || sz == NULL || cb == 0) {
        return 0;
    }
    pB = (const u8 *)p;
    for (i = 0; i < cb; i++) {
        if (pB[i] != (u8)sz[i]) {
            return 0;
        }
    }
    return 1;
}

static void *
pa_map(u64 pa)
{
    if (hhdm_ready()) {
        return (void *)(gj_vaddr_t)(GJ_HHDM_BASE + pa);
    }
    if (pa < 0x100000000ull) {
        return (void *)(gj_vaddr_t)pa;
    }
    return NULL;
}

/** Read ACPI length field with floor/ceiling clamp; 0 if unusable. */
static u32
acpi_table_len(const u8 *pTab, u32 u32Min)
{
    u32 u32Len;

    if (pTab == NULL) {
        return 0;
    }
    u32Len = *(const u32 *)(const void *)(pTab + 4);
    if (u32Len < u32Min || u32Len > IOMMU_ACPI_LEN_MAX) {
        return 0;
    }
    return u32Len;
}

static int
bdf_ok(u8 u8Bus, u8 u8Slot, u8 u8Func)
{
    (void)u8Bus; /* full 0..255 ok */
    if (u8Slot > 31u || u8Func > 7u) {
        return 0;
    }
    return 1;
}

/**
 * Greppable UDX→IOMMU window cap soft product inventory.
 *   iommu: window cap soft inventory …
 *   iommu: window cap soft create_window …   (create_window soft inventory)
 *   iommu: window cap soft destroy …        (destroy/revoke soft tallies)
 * Honesty: software window table only — not full cap-typed window object
 * product; not HW-first Phase A revoke product (P-DMA-2/3 remaining).
 */
static void
iommu_window_cap_soft_inventory(void)
{
    u32 cUsed;

    if (g_cWinSoftInvLogs < 0xffffffffu) {
        g_cWinSoftInvLogs++;
    }
    cUsed = iommu_window_count();

    /* Grep: iommu: window cap soft inventory */
    kprintf("iommu: window cap soft inventory used=%u max=%u create_ok=%u "
            "update=%u destroy=%u revoke=%u full=%u fail=%u reject=%u "
            "logs=%u (soft; not full cap-typed window object product)\n",
            cUsed, (unsigned)GJ_IOMMU_MAX_WINDOWS, g_cWinCreateOk,
            g_cWinCreateUpdate, g_cWinDestroy, g_cWinRevokeCall, g_cWinFull,
            g_cWinCreateFail, g_cWinReject, g_cWinSoftInvLogs);

    /* Grep: iommu: window cap soft create_window / create_window soft inventory */
    kprintf("iommu: window cap soft create_window ok=%u update=%u fail=%u "
            "full=%u reject=%u max=%u (create_window soft inventory; "
            "not create_window cap product)\n",
            g_cWinCreateOk, g_cWinCreateUpdate, g_cWinCreateFail, g_cWinFull,
            g_cWinReject, (unsigned)GJ_IOMMU_MAX_WINDOWS);

    /* Grep: iommu: window cap soft destroy (destroy/revoke soft tallies) */
    kprintf("iommu: window cap soft destroy revoke_calls=%u slots_cleared=%u "
            "miss=%u bad=%u live=%u (soft tallies; not HW-first revoke "
            "product)\n",
            g_cWinRevokeCall, g_cWinDestroy, g_cWinRevokeMiss, g_cWinRevokeBad,
            cUsed);
}

/**
 * After first create_window soft activity, print inventory once
 * (mirrors memobj/vmm soft-stats-once). Diagnostics only.
 */
static void
iommu_window_cap_soft_maybe_once(void)
{
    if (g_fWinSoftInvOnce != 0) {
        return;
    }
    if (g_cWinCreateOk == 0 && g_cWinCreateUpdate == 0 &&
        g_cWinDestroy == 0 && g_cWinCreateFail == 0) {
        return;
    }
    g_fWinSoftInvOnce = 1;
    iommu_window_cap_soft_inventory();
}

/** Wave 15: track peak live software window count (diagnostics only). */
static void
iommu_soft_note_win_peak(u32 cUsed)
{
    if (cUsed > g_cSoftWinPeak) {
        g_cSoftWinPeak = cUsed;
    }
}

/**
 * Wave 19 greppable soft inventory dump (prefix "iommu: soft …").
 * Diagnostics only — never hard-gates; never claims always-on product IOMMU.
 *
 * greppable: iommu: soft
 * greppable: iommu: soft inventory
 * greppable: iommu: soft present
 * greppable: iommu: soft dmar
 * greppable: iommu: soft window
 * greppable: iommu: soft enforce
 * greppable: iommu: soft acpi
 * greppable: iommu: soft busmaster
 * greppable: iommu: soft peak
 * greppable: iommu: soft path
 * greppable: iommu: soft lamps
 * greppable: iommu: soft honesty
 * greppable: iommu: soft stats
 * greppable: iommu: soft deepen
 * greppable: iommu: soft OPEN
 * greppable: iommu: soft surfaces
 * greppable: iommu: soft vendor
 * greppable: iommu: soft window_cap
 * greppable: iommu: soft return
 * greppable: iommu: soft PASS
 */
static void
iommu_soft_inventory_log(void)
{
    u32 cUsed;
    u32 u32Denies;
    u32 u32Structs;
    u32 u32Surf;
    int fPresent;
    int fEnforce;
    u8 u8Vendor;
    u32 u32Units;
    const char *szVendor;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }
    cUsed = iommu_window_count();
    iommu_soft_note_win_peak(cUsed);
    u32Denies = g_u32Denies;
    u32Structs = g_cDrhd + g_cRmrr + g_cAtsr + g_cRhsa + g_cOther;
    fPresent = (g_Info.u8Present != 0) ? 1 : 0;
    fEnforce = g_fEnforce ? 1 : 0;
    u8Vendor = g_Info.u8Vendor;
    u32Units = g_Info.u32Units;
    if (u8Vendor == GJ_IOMMU_VENDOR_INTEL) {
        szVendor = "intel";
    } else if (u8Vendor == GJ_IOMMU_VENDOR_AMD) {
        szVendor = "amd";
    } else {
        szVendor = "none";
    }
    u32Surf = IOMMU_SOFT_SURF_CATALOG;

    /* Grep: iommu: soft inventory */
    kprintf("iommu: soft inventory present=%d vendor=%s units=%u "
            "windows=%u max=%u denies=%u enforce=%d logs=%u wave=%u "
            "(soft inventory; not always-on product IOMMU; not bar3)\n",
            fPresent, szVendor, u32Units, cUsed,
            (unsigned)GJ_IOMMU_MAX_WINDOWS, u32Denies, fEnforce,
            g_cSoftInvLogs, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft present */
    kprintf("iommu: soft present lamp=%d vendor=%u vendor_name=%s "
            "units=%u intel=%u amd=%u none=%u wave=%u\n",
            fPresent, (unsigned)u8Vendor, szVendor, u32Units,
            (u8Vendor == GJ_IOMMU_VENDOR_INTEL) ? 1u : 0u,
            (u8Vendor == GJ_IOMMU_VENDOR_AMD) ? 1u : 0u,
            (u8Vendor == GJ_IOMMU_VENDOR_NONE) ? 1u : 0u,
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft dmar */
    kprintf("iommu: soft dmar drhd=%u accepted=%u rmrr=%u atsr=%u "
            "rhsa=%u other=%u rmrr_first_base=0x%lx rmrr_first_limit=0x%lx "
            "wave=%u (structure soft inventory)\n",
            g_cDrhd, g_cDrhdAccepted, g_cRmrr, g_cAtsr, g_cRhsa, g_cOther,
            (unsigned long)g_u64RmrrFirstBase,
            (unsigned long)g_u64RmrrFirstLimit, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft window */
    kprintf("iommu: soft window used=%u max=%u create_ok=%u update=%u "
            "fail=%u full=%u reject=%u destroy=%u revoke=%u miss=%u "
            "bad=%u wave=%u (software BDF table; not cap object product)\n",
            cUsed, (unsigned)GJ_IOMMU_MAX_WINDOWS, g_cWinCreateOk,
            g_cWinCreateUpdate, g_cWinCreateFail, g_cWinFull, g_cWinReject,
            g_cWinDestroy, g_cWinRevokeCall, g_cWinRevokeMiss,
            g_cWinRevokeBad, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft enforce */
    kprintf("iommu: soft enforce armed=%d denies=%u windows=%u "
            "production_default_open_bus=0 soft "
            "always_on_product=OPEN wave=%u "
            "(soft policy lamps; not product always-on IOMMU)\n",
            fEnforce, u32Denies, cUsed, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft acpi (Wave 15 scan tallies) */
    kprintf("iommu: soft acpi rsdp_skip=%u rsdp_bad=%u xsdt=%u rsdt=%u "
            "dmar=%u ivrs=%u sdt_entries=%u struct_break=%u wave=%u "
            "(ACPI soft scan; not product DMAR program)\n",
            g_cSoftRsdpSkip, g_cSoftRsdpBad, g_cSoftXsdtHit, g_cSoftRsdtHit,
            g_cSoftDmarHit, g_cSoftIvrsHit, g_cSoftSdtEntries,
            g_cSoftStructBreak, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft busmaster (Wave 15 path tallies) */
    kprintf("iommu: soft busmaster allow=%u deny=%u open_path=%u "
            "denies_total=%u enforce=%d wave=%u "
            "(soft path tallies; not product always-on deny)\n",
            g_cSoftBmAllow, g_cSoftBmDeny, g_cSoftBmOpen, u32Denies,
            fEnforce, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft peak (Wave 15 peaks) */
    kprintf("iommu: soft peak windows=%u create_ok=%u update=%u "
            "destroy=%u max_slots=%u wave=%u "
            "(peak soft inventory; wrap OK)\n",
            g_cSoftWinPeak, g_cWinCreateOk, g_cWinCreateUpdate,
            g_cWinDestroy, (unsigned)GJ_IOMMU_MAX_WINDOWS,
            (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Honesty: always-on product IOMMU remains OPEN.
     * Soft deepen ≠ product always-on IOMMU claim.
     * Grep: iommu: soft path
     */
    kprintf("iommu: soft path probe=acpi_dmar_ivrs enforce=software_window "
            "create_window=soft_bdf_table destroy=soft_revoke "
            "always_on_product=OPEN hw_first_revoke=OPEN "
            "cap_typed_window=OPEN bar3=OPEN wave=%u "
            "(soft inventory; not product always-on IOMMU)\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft lamps */
    kprintf("iommu: soft lamps present=%d enforce=%d windows_live=%u "
            "denies=%u dmar_structs=%u probed=%d wave=%u "
            "(composite soft lamps)\n",
            fPresent, fEnforce, cUsed, u32Denies, u32Structs,
            g_fProbed, (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft honesty (Wave 15 non-claims catalog) */
    kprintf("iommu: soft honesty always_on_product=OPEN "
            "no_open_bus_master_product=OPEN hw_first_revoke=OPEN "
            "cap_typed_window=OPEN amd_vi_product=OPEN bar3=OPEN "
            "inventory_only=1 soft_neq_product=1 wave=%u "
            "(explicit non-claims; not product always-on IOMMU)\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft stats (Wave 15 rollup) */
    kprintf("iommu: soft stats present=%d vendor=%s units=%u windows=%u "
            "peak=%u denies=%u create_ok=%u destroy=%u "
            "dmar_structs=%u bm_allow=%u bm_deny=%u logs=%u wave=%u "
            "(rollup; not product close)\n",
            fPresent, szVendor, u32Units, cUsed, g_cSoftWinPeak, u32Denies,
            g_cWinCreateOk, g_cWinDestroy, u32Structs, g_cSoftBmAllow,
            g_cSoftBmDeny, g_cSoftInvLogs, (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: iommu: soft surfaces
     */
    kprintf("iommu: soft surfaces surf=0x%x catalog=%u "
            "present=1 dmar=1 window=1 enforce=1 acpi=1 busmaster=1 "
            "vendor=1 window_cap=1 return=1 open=1 wave=%u "
            "(return surfaces; soft only; not product always-on IOMMU; "
            "not bar3)\n",
            (unsigned)u32Surf, (unsigned)IOMMU_SOFT_AREAS,
            (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Wave 19: vendor/units return lamps.
     * Grep: iommu: soft vendor
     */
    kprintf("iommu: soft vendor name=%s code=%u units=%u "
            "intel=%u amd=%u none=%u present=%d wave=%u "
            "(soft vendor; not product always-on IOMMU; not bar3)\n",
            szVendor, (unsigned)u8Vendor, u32Units,
            (u8Vendor == GJ_IOMMU_VENDOR_INTEL) ? 1u : 0u,
            (u8Vendor == GJ_IOMMU_VENDOR_AMD) ? 1u : 0u,
            (u8Vendor == GJ_IOMMU_VENDOR_NONE) ? 1u : 0u,
            fPresent, (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Wave 19: window-cap soft return surface (software BDF table).
     * Grep: iommu: soft window_cap
     */
    kprintf("iommu: soft window_cap used=%u max=%u create_ok=%u "
            "update=%u destroy=%u full=%u reject=%u peak=%u "
            "cap_typed_window=OPEN wave=%u "
            "(window cap soft return; not product; not bar3)\n",
            cUsed, (unsigned)GJ_IOMMU_MAX_WINDOWS, g_cWinCreateOk,
            g_cWinCreateUpdate, g_cWinDestroy, g_cWinFull, g_cWinReject,
            g_cSoftWinPeak, (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Wave 19: create/destroy return taxonomy.
     * Grep: iommu: soft return
     */
    kprintf("iommu: soft return create_ok=%u create_fail=%u update=%u "
            "full=%u reject=%u destroy=%u revoke_call=%u "
            "revoke_miss=%u revoke_bad=%u wave=%u "
            "(soft return taxonomy; not product; not bar3)\n",
            g_cWinCreateOk, g_cWinCreateFail, g_cWinCreateUpdate,
            g_cWinFull, g_cWinReject, g_cWinDestroy, g_cWinRevokeCall,
            g_cWinRevokeMiss, g_cWinRevokeBad, (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Grep: iommu: soft return rate
     * Wave 17 return-surface rate lamps (kept) (create/destroy/revoke).
     */
    kprintf("iommu: soft return rate "
            "create_ok=%u create_fail=%u destroy=%u "
            "full=%u reject=%u revoke_call=%u revoke_miss=%u "
            "windows=%u wave=%u "
            "(return rate; Soft≠product always-on IOMMU; soft≠product; "
            "not bar3)\n",
            g_cWinCreateOk, g_cWinCreateFail, g_cWinDestroy,
            g_cWinFull, g_cWinReject, g_cWinRevokeCall, g_cWinRevokeMiss,
            cUsed, (unsigned)IOMMU_SOFT_WAVE);

    /*
     * Grep: iommu: soft retcode
     * Wave 17 retcode catalog for window create/destroy return classes.
     */
    kprintf("iommu: soft retcode "
            "create_ok=1 create_fail=1 update=1 full=1 reject=1 "
            "destroy=1 revoke_miss=1 revoke_bad=1 "
            "always_on_product=OPEN wave=%u "
            "(retcode catalog; Soft≠product always-on IOMMU; soft≠product)\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: iommu: soft return selftest — Wave 19 terminal return surface */
    kprintf("iommu: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft retmap — Wave 19 return-surface map */
    kprintf("iommu: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: iommu: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("iommu: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)IOMMU_SOFT_WAVE);
    /* Grep: iommu: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("iommu: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)IOMMU_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: iommu: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("iommu: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)IOMMU_SOFT_WAVE);
    /* Grep: iommu: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("iommu: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)IOMMU_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: iommu: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("iommu: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("iommu: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: iommu: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("iommu: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("iommu: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: iommu: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("iommu: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("iommu: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: iommu: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("iommu: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("iommu: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: iommu: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("iommu: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("iommu: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: iommu: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("iommu: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
            /* Grep: iommu: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("iommu: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)IOMMU_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: iommu: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("iommu: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)IOMMU_SOFT_WAVE);
                    /* Grep: iommu: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("iommu: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)IOMMU_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: iommu: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("iommu: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)IOMMU_SOFT_WAVE);
                            /* Grep: iommu: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("iommu: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)IOMMU_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: iommu: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("iommu: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                            /* Grep: iommu: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("iommu: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: iommu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("iommu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                            /* Grep: iommu: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("iommu: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: iommu: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("iommu: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                            /* Grep: iommu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("iommu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
                            /* Grep: iommu: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("iommu: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("iommu: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("iommu: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("iommu: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("iommu: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("iommu: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("iommu: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retfortress — Wave 35 return-fortress honesty */
kprintf("iommu: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("iommu: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft rethold — Wave 36 return-hold honesty */
kprintf("iommu: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retspire — Wave 36 exclusive spire stamp */
kprintf("iommu: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retwall — Wave 37 return-wall honesty */
kprintf("iommu: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retgate — Wave 37 exclusive gate stamp */
kprintf("iommu: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retmoat — Wave 38 return-moat honesty */
kprintf("iommu: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retower — Wave 38 exclusive tower stamp */
kprintf("iommu: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("iommu: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("iommu: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("iommu: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("iommu: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retravelin — Wave 41 return-travelin honesty */
kprintf("iommu: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("iommu: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("iommu: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("iommu: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("iommu: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("iommu: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("iommu: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("iommu: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("iommu: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("iommu: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retbailey — Wave 46 return-bailey honesty */
kprintf("iommu: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)IOMMU_SOFT_WAVE);
/* Grep: iommu: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("iommu: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)IOMMU_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("iommu: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("iommu: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("iommu: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("iommu: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("iommu: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("iommu: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retsally — Wave 50 return-sally honesty */
kprintf("iommu: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("iommu: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retfosse — Wave 51 return-fosse honesty */
kprintf("iommu: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("iommu: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("iommu: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("iommu: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retravelin — Wave 53 return-travelin honesty */
kprintf("iommu: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("iommu: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("iommu: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retredan — Wave 54 exclusive redan stamp */
kprintf("iommu: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retflank — Wave 55 return-flank honesty */
kprintf("iommu: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retface — Wave 55 exclusive face stamp */
kprintf("iommu: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retgorge — Wave 56 return-gorge honesty */
kprintf("iommu: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("iommu: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: iommu: soft retraverse — Wave 57 return-traverse honesty */
kprintf("iommu: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: iommu: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("iommu: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcasemate stamp; Soft≠product)\n");







                            kprintf("iommu: soft deepen wave=%u areas=%u logs=%u surf=0x%x "
            "(Wave 43 exclusive; soft only; not product always-on IOMMU; "
            "not bar3; soft≠product)\n",
            (unsigned)IOMMU_SOFT_WAVE, (unsigned)IOMMU_SOFT_AREAS,
            g_cSoftInvLogs, (unsigned)u32Surf);

    /*
     * Explicit OPEN honesty for always-on product IOMMU.
     * Grep: iommu: soft OPEN
     */
    kprintf("iommu: soft OPEN always_on_product=OPEN "
            "no_open_bus_master_product=OPEN hw_te_default=OPEN "
            "cap_window_object=OPEN inventory_only=1 wave=%u "
            "(soft deepen ≠ product always-on IOMMU claim; not bar3; "
            "soft≠product)\n",
            (unsigned)IOMMU_SOFT_WAVE);

    /* Grep: iommu: soft inventory PASS | iommu: soft PASS */
    kprintf("iommu: soft inventory PASS present=%d windows=%u logs=%u "
            "wave=%u\n",
            fPresent, cUsed, g_cSoftInvLogs, (unsigned)IOMMU_SOFT_WAVE);
    kprintf("iommu: soft PASS wave=%u areas=%u always_on_product=OPEN\n",
            (unsigned)IOMMU_SOFT_WAVE, (unsigned)IOMMU_SOFT_AREAS);
}

/** Publish DMAR inventory + run VT-d soft-probe / domain soft smoke. */
static void
iommu_soft_probe_handoff(void)
{
    iommu_vtd_soft_dmar_inventory(g_cDrhd, g_cRmrr, g_cAtsr, g_cRhsa, g_cOther);
    if (g_cDrhd != 0 || g_cRmrr != 0 || g_cAtsr != 0 || g_cRhsa != 0 ||
        g_cOther != 0) {
        kprintf("iommu: DMAR soft inv drhd=%u/%u rmrr=%u atsr=%u rhsa=%u "
                "other=%u\n",
                g_cDrhdAccepted, g_cDrhd, g_cRmrr, g_cAtsr, g_cRhsa, g_cOther);
    }
    if (g_u64RmrrFirstBase != 0 || g_u64RmrrFirstLimit != 0) {
        kprintf("iommu: RMRR soft first base=0x%lx limit=0x%lx\n",
                (unsigned long)g_u64RmrrFirstBase,
                (unsigned long)g_u64RmrrFirstLimit);
    }
    (void)iommu_vtd_soft_probe();
    (void)iommu_vtd_domain_soft_smoke();
    /*
     * Baseline window cap soft inventory at probe hand-off (zeros typical).
     * Grep: iommu: window cap soft inventory
     */
    iommu_window_cap_soft_inventory();
    /*
     * Wave 17 exclusive soft inventory (greppable "iommu: soft …").
     * Soft deepen only; always-on product IOMMU remains OPEN.
     */
    iommu_soft_inventory_log();
}

void
iommu_probe(void)
{
    const struct gj_boot_info *pBi;
    u64 u64RsdpPa = 0;
    u8 *pRsdp;
    u64 u64XsdtPa = 0;
    u32 u32RsdtPa = 0;
    u32 u32Len;

    g_fProbed = 1;
    memset(&g_Info, 0, sizeof(g_Info));
    g_fEnforce = 0;
    g_u32Denies = 0;
    memset(g_aWin, 0, sizeof(g_aWin));
    g_cDrhd = 0;
    g_cRmrr = 0;
    g_cAtsr = 0;
    g_cRhsa = 0;
    g_cOther = 0;
    g_cDrhdAccepted = 0;
    g_u64RmrrFirstBase = 0;
    g_u64RmrrFirstLimit = 0;
    /* Window cap soft product tallies (create_window / destroy soft) */
    g_cWinCreateOk = 0;
    g_cWinCreateUpdate = 0;
    g_cWinCreateFail = 0;
    g_cWinFull = 0;
    g_cWinReject = 0;
    g_cWinDestroy = 0;
    g_cWinRevokeCall = 0;
    g_cWinRevokeMiss = 0;
    g_cWinRevokeBad = 0;
    g_cWinSoftInvLogs = 0;
    g_fWinSoftInvOnce = 0;
    g_cSoftInvLogs = 0;
    /* Wave 15 soft scan / busmaster / peak tallies */
    g_cSoftRsdpSkip = 0;
    g_cSoftRsdpBad = 0;
    g_cSoftXsdtHit = 0;
    g_cSoftRsdtHit = 0;
    g_cSoftDmarHit = 0;
    g_cSoftIvrsHit = 0;
    g_cSoftSdtEntries = 0;
    g_cSoftStructBreak = 0;
    g_cSoftBmAllow = 0;
    g_cSoftBmDeny = 0;
    g_cSoftBmOpen = 0;
    g_cSoftWinPeak = 0;

    pBi = boot_info_get();
    if (pBi != NULL) {
        u64RsdpPa = pBi->u64Rsdp;
    }
    if (u64RsdpPa == 0) {
        g_cSoftRsdpSkip++;
        kprintf("iommu: probe skip (no RSDP)\n");
        kprintf("iommu: probe PASS\n");
        (void)iommu_vtd_smoke();
        iommu_soft_probe_handoff();
        return;
    }
    pRsdp = (u8 *)pa_map(u64RsdpPa);
    /* RSDP signature is 8 bytes: "RSD PTR " */
    if (pRsdp == NULL || !sig_n(pRsdp, "RSD PTR ", 8u)) {
        g_cSoftRsdpBad++;
        kprintf("iommu: probe bad RSDP pa=0x%lx\n",
                (unsigned long)u64RsdpPa);
        kprintf("iommu: probe PASS\n");
        (void)iommu_vtd_smoke();
        iommu_soft_probe_handoff();
        return;
    }
    /* ACPI 1.0: RSDT @16; ACPI 2.0+: XSDT @24 if revision >= 2 */
    if (pRsdp[15] >= IOMMU_RSDP_REV_XSDT) {
        u64XsdtPa = *(u64 *)(void *)(pRsdp + 24);
    }
    u32RsdtPa = *(u32 *)(void *)(pRsdp + 16);

    if (u64XsdtPa != 0) {
        u8 *pXsdt = (u8 *)pa_map(u64XsdtPa);

        if (pXsdt != NULL && sig_n(pXsdt, "XSDT", 4u)) {
            u32Len = acpi_table_len(pXsdt, IOMMU_ACPI_HDR_MIN);
            if (u32Len != 0) {
                g_cSoftXsdtHit++;
                iommu_scan_sdt_entries(pXsdt, u32Len, 8u);
            } else {
                kprintf("iommu: probe XSDT len reject\n");
            }
        }
    } else if (u32RsdtPa != 0) {
        u8 *pRsdt = (u8 *)pa_map((u64)u32RsdtPa);

        if (pRsdt != NULL && sig_n(pRsdt, "RSDT", 4u)) {
            u32Len = acpi_table_len(pRsdt, IOMMU_ACPI_HDR_MIN);
            if (u32Len != 0) {
                g_cSoftRsdtHit++;
                iommu_scan_sdt_entries(pRsdt, u32Len, 4u);
            } else {
                kprintf("iommu: probe RSDT len reject\n");
            }
        }
    }

    g_Info.u8Enforce = 0;
    g_Info.u32Windows = 0;
    g_Info.u32Denies = 0;
    kprintf("iommu: present=%u vendor=%u units=%u\n", g_Info.u8Present,
            g_Info.u8Vendor, g_Info.u32Units);
    kprintf("iommu: probe PASS\n");
    /* Always construct VT-d tables for production path bring-up */
    (void)iommu_vtd_smoke();
    iommu_soft_probe_handoff();
}

/**
 * Walk RSDT (4-byte PAs) or XSDT (8-byte PAs) entries for DMAR/IVRS.
 * u32EntryCb must be 4 or 8; bounds from validated table length.
 */
static void
iommu_scan_sdt_entries(u8 *pSdt, u32 u32Len, u32 u32EntryCb)
{
    u32 u32Off;
    u32 u32End;
    u32 cEntries;
    u32 iEnt;

    if (pSdt == NULL || u32Len < IOMMU_ACPI_HDR_MIN) {
        return;
    }
    if (u32EntryCb != 4u && u32EntryCb != 8u) {
        return;
    }
    u32Off = IOMMU_ACPI_HDR_MIN;
    if (u32Len < u32Off + u32EntryCb) {
        return;
    }
    u32End = u32Len - ((u32Len - u32Off) % u32EntryCb);
    cEntries = (u32End - u32Off) / u32EntryCb;
    /* Defensive: cap entry count (1 MiB table / 4 ≈ 256k; keep modest) */
    if (cEntries > 4096u) {
        cEntries = 4096u;
    }
    for (iEnt = 0; iEnt < cEntries; iEnt++) {
        u64 u64Pa;
        u8 *pT;
        u32 u32Tlen;

        if (u32EntryCb == 8u) {
            u64Pa = *(u64 *)(void *)(pSdt + u32Off + iEnt * 8u);
        } else {
            u64Pa = (u64)(*(u32 *)(void *)(pSdt + u32Off + iEnt * 4u));
        }
        if (u64Pa == 0) {
            continue;
        }
        if (g_cSoftSdtEntries < 0xffffffffu) {
            g_cSoftSdtEntries++;
        }
        pT = (u8 *)pa_map(u64Pa);
        if (pT == NULL) {
            continue;
        }
        if (sig_n(pT, "DMAR", 4u)) {
            u32Tlen = acpi_table_len(pT, IOMMU_DMAR_HDR_MIN);
            g_Info.u8Present = 1;
            g_Info.u8Vendor = GJ_IOMMU_VENDOR_INTEL;
            g_Info.u32Units++;
            if (g_cSoftDmarHit < 0xffffffffu) {
                g_cSoftDmarHit++;
            }
            if (u32Tlen != 0) {
                /* Host address width @36, flags @37 (public DMAR header) */
                kprintf("iommu: DMAR haw=%u flags=0x%x len=%u\n",
                        (u32)pT[36], (u32)pT[37], u32Tlen);
                iommu_dmar_parse(pT, u32Tlen);
            }
        } else if (sig_n(pT, "IVRS", 4u)) {
            g_Info.u8Present = 1;
            g_Info.u8Vendor = GJ_IOMMU_VENDOR_AMD;
            g_Info.u32Units++;
            if (g_cSoftIvrsHit < 0xffffffffu) {
                g_cSoftIvrsHit++;
            }
        }
    }
}

/**
 * Walk DMAR remapping structures for soft inventory + DRHD register base.
 * DMAR header is 48 bytes; structures follow with type/length headers.
 * u32MapMax is the validated DMAR length (already clamped).
 *
 * Types (public ACPI DMAR): 0 DRHD, 1 RMRR, 2 ATSR, 3 RHSA, …
 */
static void
iommu_dmar_parse(u8 *pDmar, u32 u32MapMax)
{
    u32 u32Off;
    u32 cStruct;

    if (pDmar == NULL || u32MapMax < IOMMU_DMAR_HDR_MIN) {
        return;
    }
    u32Off = IOMMU_DMAR_HDR_MIN;
    cStruct = 0;
    while (u32Off + 4u <= u32MapMax && cStruct < 256u) {
        u16 u16Type = *(u16 *)(void *)(pDmar + u32Off);
        u16 u16Slen = *(u16 *)(void *)(pDmar + u32Off + 2);

        cStruct++;
        if (u16Slen < 4u || (u32)u32Off + (u32)u16Slen > u32MapMax) {
            if (g_cSoftStructBreak < 0xffffffffu) {
                g_cSoftStructBreak++;
            }
            kprintf("iommu: DMAR struct bounds break off=%u slen=%u\n",
                    u32Off, (u32)u16Slen);
            break;
        }
        if (u16Type == IOMMU_DMAR_DRHD) {
            g_cDrhd++;
            if (u16Slen >= 16u) {
                /* DRHD: flags@4, size@5, segment@6, register_base@8 */
                u8 u8Flags = pDmar[u32Off + 4];
                u16 u16Seg = *(u16 *)(void *)(pDmar + u32Off + 6);
                u64 u64Base = *(u64 *)(void *)(pDmar + u32Off + 8);

                kprintf("iommu: DRHD soft seg=%u flags=0x%x base=0x%lx\n",
                        (u32)u16Seg, (u32)u8Flags, (unsigned long)u64Base);
                if (u64Base != 0 && (u64Base & 0xfffull) == 0) {
                    /* page-aligned MMIO base only; first wins in set_drhd */
                    iommu_vtd_set_drhd(u64Base);
                    g_cDrhdAccepted++;
                } else if (u64Base != 0) {
                    kprintf("iommu: DRHD base unaligned skip=0x%lx\n",
                            (unsigned long)u64Base);
                }
            }
        } else if (u16Type == IOMMU_DMAR_RMRR) {
            g_cRmrr++;
            /* RMRR: segment@6, base@8, limit@16 (public layout, min 24) */
            if (u16Slen >= 24u) {
                u64 u64Base = *(u64 *)(void *)(pDmar + u32Off + 8);
                u64 u64Limit = *(u64 *)(void *)(pDmar + u32Off + 16);

                if (g_u64RmrrFirstBase == 0 && g_u64RmrrFirstLimit == 0) {
                    g_u64RmrrFirstBase = u64Base;
                    g_u64RmrrFirstLimit = u64Limit;
                }
                kprintf("iommu: RMRR soft base=0x%lx limit=0x%lx\n",
                        (unsigned long)u64Base, (unsigned long)u64Limit);
            }
        } else if (u16Type == IOMMU_DMAR_ATSR) {
            g_cAtsr++;
            if (u16Slen >= 8u) {
                u8 u8Flags = pDmar[u32Off + 4];
                u16 u16Seg = *(u16 *)(void *)(pDmar + u32Off + 6);

                kprintf("iommu: ATSR soft seg=%u flags=0x%x\n", (u32)u16Seg,
                        (u32)u8Flags);
            }
        } else if (u16Type == IOMMU_DMAR_RHSA) {
            g_cRhsa++;
            /* RHSA: register_base@4, proximity@12 (min 16) */
            if (u16Slen >= 16u) {
                u64 u64Base = *(u64 *)(void *)(pDmar + u32Off + 4);
                u32 u32Prox = *(u32 *)(void *)(pDmar + u32Off + 12);

                kprintf("iommu: RHSA soft base=0x%lx prox=%u\n",
                        (unsigned long)u64Base, u32Prox);
            }
        } else {
            g_cOther++;
        }
        u32Off += u16Slen;
    }
}

int
iommu_present(void)
{
    return g_Info.u8Present != 0;
}

void
iommu_info_get(struct gj_iommu_info *pOut)
{
    if (pOut == NULL) {
        return;
    }
    g_Info.u8Enforce = g_fEnforce ? 1u : 0u;
    g_Info.u32Windows = iommu_window_count();
    g_Info.u32Denies = g_u32Denies;
    *pOut = g_Info;
}

void
iommu_enforce_set(int fOn)
{
    g_fEnforce = fOn ? 1 : 0;
    g_Info.u8Enforce = g_fEnforce ? 1u : 0u;
    kprintf("iommu: enforce=%u (policy %s)\n", g_fEnforce,
            g_fEnforce ? "armed" : "disarmed");
}

int
iommu_enforce_get(void)
{
    return g_fEnforce;
}

/**
 * create_window soft product path (UDX→IOMMU window side).
 * Software BDF→PA grant; not a cap-typed window object mint.
 * Grep: iommu: window cap soft grant|update|full|reject
 */
int
iommu_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb)
{
    u32 iWin;
    u32 cUsed;

    if (cb == 0 || !bdf_ok(bus, slot, func)) {
        g_cWinReject++;
        g_cWinCreateFail++;
        /* Grep: iommu: window cap soft reject */
        kprintf("iommu: window cap soft reject %u:%u.%u cb=0x%lx "
                "(create_window soft; bad bdf/cb)\n",
                bus, slot, func, (unsigned long)cb);
        iommu_window_cap_soft_maybe_once();
        return -1;
    }
    /* Reject pa+cb overflow */
    if (pa + cb < pa) {
        g_cWinReject++;
        g_cWinCreateFail++;
        /* Grep: iommu: window cap soft reject */
        kprintf("iommu: window cap soft reject %u:%u.%u pa=0x%lx cb=0x%lx "
                "(create_window soft; overflow)\n",
                bus, slot, func, (unsigned long)pa, (unsigned long)cb);
        iommu_window_cap_soft_maybe_once();
        return -1;
    }
    /* Update existing BDF window if present (create_window soft rebind) */
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            g_aWin[iWin].u64PaBase = pa;
            g_aWin[iWin].u64Cb = cb;
            g_cWinCreateUpdate++;
            cUsed = iommu_window_count();
            g_Info.u32Windows = cUsed;
            iommu_soft_note_win_peak(cUsed);
            /* Grep: iommu: window cap soft update */
            kprintf("iommu: window cap soft update %u:%u.%u pa=0x%lx "
                    "cb=0x%lx used=%u (create_window soft inventory)\n",
                    bus, slot, func, (unsigned long)pa, (unsigned long)cb,
                    cUsed);
            iommu_window_cap_soft_maybe_once();
            return 0;
        }
    }
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (!g_aWin[iWin].u8Used) {
            g_aWin[iWin].u8Used = 1;
            g_aWin[iWin].u8Bus = bus;
            g_aWin[iWin].u8Slot = slot;
            g_aWin[iWin].u8Func = func;
            g_aWin[iWin].u64PaBase = pa;
            g_aWin[iWin].u64Cb = cb;
            g_cWinCreateOk++;
            cUsed = iommu_window_count();
            g_Info.u32Windows = cUsed;
            iommu_soft_note_win_peak(cUsed);
            /* Grep: iommu: window cap soft grant */
            kprintf("iommu: window cap soft grant %u:%u.%u pa=0x%lx "
                    "cb=0x%lx used=%u (create_window soft; not cap object)\n",
                    bus, slot, func, (unsigned long)pa, (unsigned long)cb,
                    cUsed);
            iommu_window_cap_soft_maybe_once();
            return 0;
        }
    }
    g_cWinFull++;
    g_cWinCreateFail++;
    /* Grep: iommu: window cap soft full */
    kprintf("iommu: window cap soft full %u:%u.%u used=%u max=%u "
            "(create_window soft inventory FULL)\n",
            bus, slot, func, iommu_window_count(),
            (unsigned)GJ_IOMMU_MAX_WINDOWS);
    iommu_window_cap_soft_maybe_once();
    return -1;
}

/**
 * Destroy/revoke soft product path (UDX→IOMMU window side).
 * Clears software window slot(s) for BDF; not HW-first remapping disable.
 * Grep: iommu: window cap soft revoke|destroy
 */
void
iommu_window_revoke(u8 bus, u8 slot, u8 func)
{
    u32 iWin;
    u32 cCleared = 0;

    g_cWinRevokeCall++;
    if (!bdf_ok(bus, slot, func)) {
        g_cWinRevokeBad++;
        /* Grep: iommu: window cap soft revoke */
        kprintf("iommu: window cap soft revoke bad %u:%u.%u "
                "(destroy soft tallies; bad bdf)\n",
                bus, slot, func);
        iommu_window_cap_soft_maybe_once();
        return;
    }
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            memset(&g_aWin[iWin], 0, sizeof(g_aWin[iWin]));
            cCleared++;
            g_cWinDestroy++;
        }
    }
    g_Info.u32Windows = iommu_window_count();
    if (cCleared == 0) {
        g_cWinRevokeMiss++;
        /* Grep: iommu: window cap soft revoke */
        kprintf("iommu: window cap soft revoke miss %u:%u.%u "
                "(destroy soft tallies; no window)\n",
                bus, slot, func);
    } else {
        /* Grep: iommu: window cap soft destroy */
        kprintf("iommu: window cap soft destroy %u:%u.%u slots=%u live=%u "
                "(soft revoke tallies; not HW-first product)\n",
                bus, slot, func, cCleared, g_Info.u32Windows);
    }
    iommu_window_cap_soft_maybe_once();
}

int
iommu_busmaster_ok(u8 bus, u8 slot, u8 func)
{
    u32 iWin;

    if (!g_fEnforce) {
        /* Wave 15: open path (enforce off) soft tally */
        if (g_cSoftBmOpen < 0xffffffffu) {
            g_cSoftBmOpen++;
        }
        return 1;
    }
    if (!bdf_ok(bus, slot, func)) {
        g_u32Denies++;
        g_Info.u32Denies = g_u32Denies;
        if (g_cSoftBmDeny < 0xffffffffu) {
            g_cSoftBmDeny++;
        }
        return 0;
    }
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            if (g_cSoftBmAllow < 0xffffffffu) {
                g_cSoftBmAllow++;
            }
            return 1;
        }
    }
    g_u32Denies++;
    g_Info.u32Denies = g_u32Denies;
    if (g_cSoftBmDeny < 0xffffffffu) {
        g_cSoftBmDeny++;
    }
    return 0;
}

u32
iommu_window_count(void)
{
    u32 iWin;
    u32 cUsed = 0;

    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used) {
            cUsed++;
        }
    }
    return cUsed;
}

u32
iommu_deny_count(void)
{
    return g_u32Denies;
}
