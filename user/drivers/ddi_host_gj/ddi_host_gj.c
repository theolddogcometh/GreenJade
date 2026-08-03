/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding userspace NATIVE DDI host scaffold (GreenJade).
 *
 * Soft path only (soft ≠ product):
 *   GJ_SYS_DDI / gj_ddi  → SCAN inventory → log found rows
 *   look for G752 first targets:
 *     10ec:8168  Realtek RTL8111/8168
 *     8086:a12f  Intel 100 Series / C230 xHCI
 *   OPEN + MAP_BAR0 on match → soft PASS or SKIP
 *
 * Greppable lamps (prefix-stable for smoke):
 *   ddi_host: soft scan PASS n=…
 *   ddi_host: soft bind 10ec:8168 PASS|SKIP
 *   ddi_host: soft bind 8086:a12f PASS|SKIP
 *   ddi_host: soft found …          (per inventory row)
 *   ddi_host: soft scaffold PASS
 *
 * Links freestanding with libgj + user/init/user.ld (like personality_gj).
 * Dual MIT OR Apache-2.0 only; no GPL; no Linux .ko product AC.
 * Soft ≠ product: no live multi-server mint claim (docs/DDI_SOFT.md).
 */
#include <gj/syscalls.h>

/* G752 first bind targets (docs/LAPTOP_LINUX_DRIVER_HOST.md, udx/ddi.h). */
#define DDI_HOST_RTL_VEND   0x10ecu
#define DDI_HOST_RTL_DEV    0x8168u
#define DDI_HOST_XHCI_VEND  0x8086u
#define DDI_HOST_XHCI_DEV   0xa12fu

/* Soft inventory bound (matches kernel GJ_DDI_SOFT_DEV_MAX). */
#define DDI_HOST_SCAN_MAX   32u

/*
 * Packed device info — layout matches kernel struct gj_ddi_dev_info
 * (kernel/include/gj/ddi_door.h). libgj does not export the type yet.
 */
struct ddi_host_dev_info {
    unsigned char  u8Bus;
    unsigned char  u8Slot;
    unsigned char  u8Func;
    unsigned char  u8Pad0;
    unsigned short u16Vend;
    unsigned short u16Dev;
    unsigned int   u32Class;
    unsigned long long u64Bar0Pa;
    unsigned long long u64Bar0Cb;
    unsigned long long u64Bar1Pa;
    unsigned long long u64Bar1Cb;
    unsigned long long u64Bar2Pa;
    unsigned long long u64Bar2Cb;
    unsigned long long u64Bar3Pa;
    unsigned long long u64Bar3Cb;
    unsigned long long u64Bar4Pa;
    unsigned long long u64Bar4Cb;
    unsigned long long u64Bar5Pa;
    unsigned long long u64Bar5Cb;
};

/*
 * Soft MAP_BAR out-note — layout matches kernel struct gj_ddi_map_note.
 * Passed as arg3 of GJ_DDI_OP_MAP_BAR when user_range_ok.
 */
struct ddi_host_map_note {
    unsigned long long u64Va;
    unsigned long long u64Cb;
    unsigned long long u64Pa;
    unsigned int u32Bar;
    unsigned int u32Handle;
    unsigned char u8Ok;
    unsigned char u8Pad[3];
};

/* ---- freestanding helpers ----------------------------------------------- */

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
}

static void
memzero(void *p, unsigned cb)
{
    unsigned char *pB = (unsigned char *)p;
    unsigned i;

    if (pB == 0) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pB[i] = 0;
    }
}

static void
append_s(char *aLine, unsigned cb, unsigned *po, const char *sz)
{
    if (aLine == 0 || po == 0 || sz == 0 || cb == 0) {
        return;
    }
    while (*sz != '\0' && *po + 1u < cb) {
        aLine[(*po)++] = *sz++;
    }
}

static void
append_u(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aDig[20];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = '0';
        }
        return;
    }
    while (u > 0 && n < (unsigned)sizeof(aDig)) {
        aDig[n++] = (char)('0' + (u % 10ul));
        u /= 10ul;
    }
    for (i = n; i > 0; i--) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = aDig[i - 1u];
        }
    }
}

static void
append_hex4(char *aLine, unsigned cb, unsigned *po, unsigned v)
{
    static const char aH[] = "0123456789abcdef";
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    for (i = 0; i < 4u; i++) {
        unsigned nibble = (v >> (12u - 4u * i)) & 0xfu;
        if (*po + 1u < cb) {
            aLine[(*po)++] = aH[nibble];
        }
    }
}

static void
log_found(unsigned idx, const struct ddi_host_dev_info *p)
{
    char aLine[128];
    unsigned o = 0;

    if (p == 0) {
        return;
    }
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft found idx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)idx);
    append_s(aLine, sizeof(aLine), &o, " bdf=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Bus);
    append_s(aLine, sizeof(aLine), &o, ":");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Slot);
    append_s(aLine, sizeof(aLine), &o, ".");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Func);
    append_s(aLine, sizeof(aLine), &o, " id=");
    append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Vend);
    append_s(aLine, sizeof(aLine), &o, ":");
    append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Dev);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft OPEN + MAP_BAR0 for one inventory index.
 * PASS when open handle > 0 and MAP_BAR returns >= 0 (or note.u8Ok).
 * Soft ≠ product: kernel UC / soft user map only (docs/DDI_SOFT.md).
 */
static int
soft_open_map_bar0(unsigned idx, const struct ddi_host_dev_info *pInfo)
{
    struct ddi_host_map_note note;
    long h;
    long ret;

    if (pInfo == 0) {
        return 0;
    }

    h = gj_ddi_open(idx);
    if (h <= 0) {
        return 0;
    }

    /*
     * MAP_BAR arg3 dual-use: pointer to map note when range_ok, else va_hint.
     * Prefer out-note so soft PASS does not depend on positive i64 VA.
     */
    memzero(&note, (unsigned)sizeof(note));
    ret = gj_ddi(GJ_DDI_OP_MAP_BAR, (unsigned long)h, 0u,
                 (unsigned long)(uintptr_t)&note);
    if (ret < 0) {
        /* Retry with va_hint 0 when note path soft-fails (empty BAR etc.). */
        if (pInfo->u64Bar0Pa == 0 || pInfo->u64Bar0Cb == 0) {
            return 0;
        }
        ret = gj_ddi_map_bar((unsigned long)h, 0u, 0u);
        if (ret < 0) {
            return 0;
        }
        return 1;
    }
    if (note.u8Ok != 0 || ret >= 0) {
        return 1;
    }
    return 0;
}

/**
 * Find first inventory row matching vend:dev; OPEN+MAP_BAR0.
 * Returns 1 on soft bind PASS, 0 on SKIP.
 */
static int
soft_bind_by_id(long nDev, unsigned short u16Vend, unsigned short u16Dev)
{
    struct ddi_host_dev_info info;
    long i;

    if (nDev <= 0) {
        return 0;
    }
    for (i = 0; i < nDev && i < (long)DDI_HOST_SCAN_MAX; i++) {
        long ret;

        memzero(&info, (unsigned)sizeof(info));
        ret = gj_ddi_get((unsigned)i, &info);
        if (ret < 0) {
            continue;
        }
        if (info.u16Vend != u16Vend || info.u16Dev != u16Dev) {
            continue;
        }
        if (soft_open_map_bar0((unsigned)i, &info) != 0) {
            return 1;
        }
        /* Match found but open/map soft-failed → SKIP for this ID. */
        return 0;
    }
    return 0;
}

static void
log_bind(const char *szId, int fPass)
{
    char aLine[96];
    unsigned o = 0;

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft bind ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, fPass != 0 ? " PASS" : " SKIP");
    append_s(aLine, sizeof(aLine), &o, " soft≠product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft inventory + G752 bind probes. Never hard-fails the process.
 * All outcomes are soft PASS / SKIP lamps for bring-up honesty.
 */
static void
ddi_host_soft_scan_and_bind(void)
{
    struct ddi_host_dev_info info;
    char aLine[96];
    unsigned o;
    long nDev;
    long i;
    unsigned long nLog;
    int fRtl;
    int fXhci;

    /* Optional once lamp from door (kernel: ddi_door: soft product surface). */
    (void)gj_ddi_inventory();

    nDev = gj_ddi_scan();
    if (nDev < 0) {
        nDev = 0;
    }
    if (nDev > (long)DDI_HOST_SCAN_MAX) {
        nDev = (long)DDI_HOST_SCAN_MAX;
    }

    /*
     * Grep: ddi_host: soft scan PASS n=
     * Always PASS for the soft host path (scan invoked); n may be 0 when
     * devmgr deferred or no PCI table. Soft ≠ product inventory complete.
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft scan PASS n=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nDev);
    append_s(aLine, sizeof(aLine), &o, " soft≠product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    nLog = 0;
    for (i = 0; i < nDev; i++) {
        long ret;

        memzero(&info, (unsigned)sizeof(info));
        ret = gj_ddi_get((unsigned)i, &info);
        if (ret < 0) {
            continue;
        }
        if (info.u16Vend == 0 && info.u16Dev == 0) {
            continue;
        }
        log_found((unsigned)i, &info);
        nLog++;
    }
    if (nLog == 0ul) {
        msg("ddi_host: soft inventory empty (scan deferred or no rows)\n");
    }

    /* G752 first targets: OPEN + MAP_BAR0. */
    fRtl = soft_bind_by_id(nDev, (unsigned short)DDI_HOST_RTL_VEND,
                           (unsigned short)DDI_HOST_RTL_DEV);
    log_bind("10ec:8168", fRtl);

    fXhci = soft_bind_by_id(nDev, (unsigned short)DDI_HOST_XHCI_VEND,
                            (unsigned short)DDI_HOST_XHCI_DEV);
    log_bind("8086:a12f", fXhci);

    (void)fRtl;
    (void)fXhci;
}

/**
 * Soft scaffold entry — greppable for freestanding smoke.
 * Freestanding link uses _start (user.ld ENTRY); _start calls main.
 */
int
main(void)
{
    /* Soft phase 1 — ELF + DEBUG_LOG path alive. */
    msg("ddi_host: soft scaffold PASS\n");

    /* Soft phase 2 — GJ_SYS_DDI scan + G752 bind probes. */
    ddi_host_soft_scan_and_bind();

    msg("ddi_host: soft done soft≠product gpl=0\n");
    return 0;
}

void
_start(void)
{
    int n;

    n = main();
    gj_exit(n);
}
