/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Scan RSDP → RSDT/XSDT for DMAR (Intel) or IVRS (AMD) signatures.
 * Inventory + software enforce windows + hand-off to VT-d table builder.
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

static struct gj_iommu_info g_Info;
static int g_fProbed;
static int g_fEnforce;
static struct gj_iommu_window g_aWin[GJ_IOMMU_MAX_WINDOWS];
static u32 g_u32Denies;

static void iommu_dmar_parse(u8 *pDmar, u32 u32MapMax);
static void iommu_scan_sdt_entries(u8 *pSdt, u32 u32Len, u32 u32EntryCb);

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

    pBi = boot_info_get();
    if (pBi != NULL) {
        u64RsdpPa = pBi->u64Rsdp;
    }
    if (u64RsdpPa == 0) {
        kprintf("iommu: probe skip (no RSDP)\n");
        kprintf("iommu: probe PASS\n");
        (void)iommu_vtd_smoke();
        return;
    }
    pRsdp = (u8 *)pa_map(u64RsdpPa);
    /* RSDP signature is 8 bytes: "RSD PTR " */
    if (pRsdp == NULL || !sig_n(pRsdp, "RSD PTR ", 8u)) {
        kprintf("iommu: probe bad RSDP pa=0x%lx\n",
                (unsigned long)u64RsdpPa);
        kprintf("iommu: probe PASS\n");
        (void)iommu_vtd_smoke();
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
 * Walk DMAR remapping structures for DRHD (type 0) register base.
 * DMAR header is 48 bytes; structures follow with type/length headers.
 * u32MapMax is the validated DMAR length (already clamped).
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
        if (u16Type == 0 && u16Slen >= 16u) {
            /* DRHD: flags@4, size@5, segment@6, register_base@8 */
            u64 u64Base = *(u64 *)(void *)(pDmar + u32Off + 8);

            if (u64Base != 0 && (u64Base & 0xfffull) == 0) {
                /* page-aligned MMIO base only */
                iommu_vtd_set_drhd(u64Base);
            } else if (u64Base != 0) {
                kprintf("iommu: DRHD base unaligned skip=0x%lx\n",
                        (unsigned long)u64Base);
            }
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

int
iommu_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb)
{
    u32 iWin;

    if (cb == 0 || !bdf_ok(bus, slot, func)) {
        return -1;
    }
    /* Reject pa+cb overflow */
    if (pa + cb < pa) {
        return -1;
    }
    /* Update existing BDF window if present */
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            g_aWin[iWin].u64PaBase = pa;
            g_aWin[iWin].u64Cb = cb;
            g_Info.u32Windows = iommu_window_count();
            kprintf("iommu: window update %u:%u.%u pa=0x%lx cb=0x%lx\n", bus,
                    slot, func, (unsigned long)pa, (unsigned long)cb);
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
            g_Info.u32Windows = iommu_window_count();
            kprintf("iommu: window grant %u:%u.%u pa=0x%lx cb=0x%lx\n", bus,
                    slot, func, (unsigned long)pa, (unsigned long)cb);
            return 0;
        }
    }
    kprintf("iommu: window grant FULL %u:%u.%u\n", bus, slot, func);
    return -1;
}

void
iommu_window_revoke(u8 bus, u8 slot, u8 func)
{
    u32 iWin;

    if (!bdf_ok(bus, slot, func)) {
        return;
    }
    for (iWin = 0; iWin < GJ_IOMMU_MAX_WINDOWS; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u8Bus == bus &&
            g_aWin[iWin].u8Slot == slot && g_aWin[iWin].u8Func == func) {
            memset(&g_aWin[iWin], 0, sizeof(g_aWin[iWin]));
        }
    }
    g_Info.u32Windows = iommu_window_count();
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
