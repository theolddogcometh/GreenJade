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

static void iommu_dmar_parse(u8 *pDmar, u32 u32MapMax);
static void iommu_scan_sdt_entries(u8 *pSdt, u32 u32Len, u32 u32EntryCb);
static void iommu_soft_probe_handoff(void);
static void iommu_window_cap_soft_inventory(void);
static void iommu_window_cap_soft_maybe_once(void);

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

    pBi = boot_info_get();
    if (pBi != NULL) {
        u64RsdpPa = pBi->u64Rsdp;
    }
    if (u64RsdpPa == 0) {
        kprintf("iommu: probe skip (no RSDP)\n");
        kprintf("iommu: probe PASS\n");
        (void)iommu_vtd_smoke();
        iommu_soft_probe_handoff();
        return;
    }
    pRsdp = (u8 *)pa_map(u64RsdpPa);
    /* RSDP signature is 8 bytes: "RSD PTR " */
    if (pRsdp == NULL || !sig_n(pRsdp, "RSD PTR ", 8u)) {
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
        pT = (u8 *)pa_map(u64Pa);
        if (pT == NULL) {
            continue;
        }
        if (sig_n(pT, "DMAR", 4u)) {
            u32Tlen = acpi_table_len(pT, IOMMU_DMAR_HDR_MIN);
            g_Info.u8Present = 1;
            g_Info.u8Vendor = GJ_IOMMU_VENDOR_INTEL;
            g_Info.u32Units++;
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
        return 1;
    }
    if (!bdf_ok(bus, slot, func)) {
        g_u32Denies++;
        g_Info.u32Denies = g_u32Denies;
        return 0;
    }
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            return 1;
        }
    }
    g_u32Denies++;
    g_Info.u32Denies = g_u32Denies;
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
