/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI / devmgr inventory for the ABI-first pivot.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko.
 *
 * Soft only:
 *   - init lamp (devmgr: soft init PASS) — idempotent; may defer bus walk
 *   - Real type-0 PCI config walk via CF8/CFC (inline helpers in this TU)
 *   - Soft PCI inventory: BDF, vendor:device, class/subclass/prog-if, BAR0..5
 *   - BAR size via classic write-all-1s probe; BAR restored after probe
 *   - DMA window grant note (soft table; no live cap mint / VT-d program)
 *   - Soft helpers: bdf_register, find_by_id, count, get, note_window_grant
 *
 * Product later: match → grant MMIO/IRQ/DMA caps to userspace hosts
 * (docs/DDI_SOFT.md). See also SECURITY_CORE_DESIGN.md, ABI_FIRST_PIVOT.md.
 *
 * G752VT laptop targets (lamps when present):
 *   Realtek NIC  10ec:8168
 *   Intel xHCI   8086:a12f
 *
 * Greppable markers (keep stable):
 *   devmgr: soft init PASS
 *   devmgr: soft pci scan PASS n=N
 *   devmgr: soft found 10ec:8168
 *   devmgr: soft found 8086:a12f
 *   devmgr: soft dma window note
 */
#include <gj/devmgr.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

/* ---- PCI config (type-0 mechanism; inline in this TU only) ------------ */
#define DEVMGR_PCI_CFG_ADDR 0xCF8u
#define DEVMGR_PCI_CFG_DATA 0xCFCu
#define DEVMGR_PCI_BUS_MAX  8u  /* buses 0..7 */
#define DEVMGR_PCI_SLOT_MAX 32u /* slots 0..31 */
#define DEVMGR_PCI_FUNC_MAX 8u  /* funcs 0..7 */

/* G752VT greppable target IDs */
#define DEVMGR_VID_REALTEK 0x10ecu
#define DEVMGR_DID_RTL8168 0x8168u
#define DEVMGR_VID_INTEL   0x8086u
#define DEVMGR_DID_XHCI    0xa12fu

static int  g_fReady;
static int  g_fScanned;
static u32  g_cInitCalls;
static u32  g_cPciScanCalls;
static u32  g_cWinNotes;
static u32  g_cWinRejects;
static u32  g_cWinUsed;
static u32  g_cBdfUsed;
static u32  g_cBdfRegs;
static u32  g_cBdfRejects;
static u32  g_cPciUsed;

/* Soft window note table (inventory only). */
static struct gj_devmgr_window_grant g_aWin[GJ_DEVMGR_SOFT_WIN_MAX];

/* Soft BDF registration table (manual inventory; no live PCI). */
static struct gj_devmgr_bdf g_aBdf[GJ_DEVMGR_SOFT_BDF_MAX];
static u8                   g_aBdfOk[GJ_DEVMGR_SOFT_BDF_MAX];

/* Soft PCI function inventory from CF8/CFC walk. */
static struct gj_devmgr_pci_fn g_aPci[GJ_DEVMGR_SOFT_BDF_MAX];

/* ---- Inline I/O + config helpers (this file only) --------------------- */

static inline void
devmgr_outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
devmgr_inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
devmgr_pci_cfg_addr(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    return 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
           ((u32)u8Func << 8) | ((u32)u8Off & 0xfcu);
}

static u32
devmgr_pci_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    devmgr_outl(DEVMGR_PCI_CFG_ADDR,
                devmgr_pci_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    return devmgr_inl(DEVMGR_PCI_CFG_DATA);
}

static void
devmgr_pci_cfg_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    devmgr_outl(DEVMGR_PCI_CFG_ADDR,
                devmgr_pci_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    devmgr_outl(DEVMGR_PCI_CFG_DATA, u32Val);
}

/**
 * Classic BAR size probe: write all-1s, read mask, restore original.
 * Handles I/O, 32-bit MEM, and 64-bit MEM (consumes next BAR index).
 * On 64-bit, *pSkipNext is set so the caller skips the high dword slot.
 */
static void
devmgr_bar_probe(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8BarIdx,
                 struct gj_devmgr_bar *pBar, int *pSkipNext)
{
    u8  u8Off;
    u32 u32Lo;
    u32 u32SaveLo;
    u32 u32RawLo;
    u64 u64Pa;
    u64 u64Sz;
    int fMem;
    int fPref;
    int f64;

    if (pSkipNext != NULL) {
        *pSkipNext = 0;
    }
    if (pBar == NULL || u8BarIdx >= GJ_DEVMGR_SOFT_BAR_MAX) {
        return;
    }

    memset(pBar, 0, sizeof(*pBar));
    pBar->u8Index = u8BarIdx;

    u8Off = (u8)(0x10u + (u8)(u8BarIdx * 4u));
    u32Lo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
    u32SaveLo = u32Lo;

    /* Empty / unimplemented BAR: leave zeros. */
    if (u32Lo == 0u || u32Lo == 0xffffffffu) {
        return;
    }

    fMem = ((u32Lo & 1u) == 0u) ? 1 : 0;
    fPref = 0;
    f64 = 0;
    u64Pa = 0;
    u64Sz = 0;

    if (!fMem) {
        /* I/O BAR: low 2 bits are flags; base in bits 31:2. */
        u64Pa = (u64)(u32Lo & ~0x3u);

        devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
        u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
        devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);

        u32RawLo &= ~0x3u;
        if (u32RawLo != 0u) {
            u64Sz = (u64)((~u32RawLo) + 1u);
            /* I/O space is 32-bit; clamp absurd masks. */
            if (u64Sz > 0x10000ull) {
                u64Sz = 0x10000ull;
            }
        }
    } else {
        u32 u32Type = (u32Lo >> 1) & 3u;

        fPref = ((u32Lo & 0x8u) != 0u) ? 1 : 0;
        u64Pa = (u64)(u32Lo & ~0xfu);

        if (u32Type == 2u && u8BarIdx < 5u) {
            /* 64-bit memory BAR: high dword is next BAR register. */
            u8  u8OffHi = (u8)(u8Off + 4u);
            u32 u32Hi = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8OffHi);
            u32 u32SaveHi = u32Hi;
            u32 u32RawHi;
            u64 u64Mask;

            f64 = 1;
            u64Pa |= ((u64)u32Hi << 32);

            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8OffHi,
                                   0xffffffffu);
            u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
            u32RawHi = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8OffHi);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8OffHi, u32SaveHi);

            u64Mask = ((u64)u32RawHi << 32) | (u64)(u32RawLo & ~0xfu);
            if (u64Mask != 0ull) {
                u64Sz = (~u64Mask) + 1ull;
            }
            if (pSkipNext != NULL) {
                *pSkipNext = 1;
            }
        } else {
            /* 32-bit memory BAR (type 0) or reserved type 1. */
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
            u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);

            u32RawLo &= ~0xfu;
            if (u32RawLo != 0u) {
                u64Sz = (u64)((~u32RawLo) + 1u);
            }
        }
    }

    pBar->u8Mem = (u8)fMem;
    pBar->u8Prefetch = (u8)fPref;
    pBar->u8Is64 = (u8)f64;
    pBar->u64Pa = u64Pa;
    pBar->u64Cb = u64Sz;
}

static void
devmgr_pci_record_fn(u8 u8Bus, u8 u8Slot, u8 u8Func, u16 u16Vend, u16 u16Dev)
{
    struct gj_devmgr_pci_fn *pFn;
    u32 u32ClassDw;
    u32 u32HdrDw;
    u8  u8Hdr;
    u8  iBar;
    int fSkipNext;

    if (g_cPciUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
        return;
    }

    pFn = &g_aPci[g_cPciUsed];
    memset(pFn, 0, sizeof(*pFn));
    pFn->bdf.u8Bus = u8Bus;
    pFn->bdf.u8Slot = u8Slot;
    pFn->bdf.u8Func = u8Func;
    pFn->u16Vendor = u16Vend;
    pFn->u16Device = u16Dev;

    /* Class code dword @ 0x08: rev | prog-if | subclass | class */
    u32ClassDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x08u);
    pFn->u8ProgIf = (u8)((u32ClassDw >> 8) & 0xffu);
    pFn->u8Subclass = (u8)((u32ClassDw >> 16) & 0xffu);
    pFn->u8Class = (u8)((u32ClassDw >> 24) & 0xffu);

    /* Header Type @ 0x0e (in dword @ 0x0c). */
    u32HdrDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x0cu);
    u8Hdr = (u8)((u32HdrDw >> 16) & 0xffu);
    pFn->u8HdrType = (u8)(u8Hdr & 0x7fu);

    /*
     * BAR0..5 size probe only for type-0 headers. Type-1 bridge layout
     * reuses 0x18+ for bus numbers — never write all-1s there.
     */
    if (pFn->u8HdrType == 0u) {
        fSkipNext = 0;
        for (iBar = 0u; iBar < GJ_DEVMGR_SOFT_BAR_MAX; iBar++) {
            if (fSkipNext) {
                /* High dword of prior 64-bit BAR — leave zero stub. */
                pFn->aBar[iBar].u8Index = iBar;
                fSkipNext = 0;
                continue;
            }
            devmgr_bar_probe(u8Bus, u8Slot, u8Func, iBar, &pFn->aBar[iBar],
                             &fSkipNext);
        }
    }

    g_cPciUsed++;
}

/**
 * Full type-0 mechanism walk: bus 0..7 × slot 0..31 × func 0..7.
 * Skip empty vendor 0xffff / 0x0000. Always visit funcs 0..7 (skip empty).
 */
static void
devmgr_pci_walk(void)
{
    u32 u32Bus;
    u8  u8Slot;
    u8  u8Func;

    g_cPciUsed = 0u;
    memset(g_aPci, 0, sizeof(g_aPci));

    for (u32Bus = 0u; u32Bus < DEVMGR_PCI_BUS_MAX; u32Bus++) {
        for (u8Slot = 0u; u8Slot < DEVMGR_PCI_SLOT_MAX; u8Slot++) {
            for (u8Func = 0u; u8Func < DEVMGR_PCI_FUNC_MAX; u8Func++) {
                u8  u8Bus = (u8)u32Bus;
                u32 u32Id;
                u16 u16Vend;
                u16 u16Dev;

                u32Id = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x00u);
                u16Vend = (u16)(u32Id & 0xffffu);
                u16Dev = (u16)((u32Id >> 16) & 0xffffu);

                if (u16Vend == 0xffffu || u16Vend == 0x0000u) {
                    continue;
                }

                devmgr_pci_record_fn(u8Bus, u8Slot, u8Func, u16Vend, u16Dev);

                if (g_cPciUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
                    return;
                }
            }
        }
    }
}

static void
devmgr_pci_emit_found_lamps(void)
{
    u32 i;
    int fRtl = 0;
    int fXhci = 0;

    for (i = 0u; i < g_cPciUsed; i++) {
        if (g_aPci[i].u16Vendor == DEVMGR_VID_REALTEK &&
            g_aPci[i].u16Device == DEVMGR_DID_RTL8168) {
            fRtl = 1;
        }
        if (g_aPci[i].u16Vendor == DEVMGR_VID_INTEL &&
            g_aPci[i].u16Device == DEVMGR_DID_XHCI) {
            fXhci = 1;
        }
    }

    /* Grep: devmgr: soft found 10ec:8168 */
    if (fRtl) {
        kprintf("devmgr: soft found 10ec:8168\n");
    }
    /* Grep: devmgr: soft found 8086:a12f */
    if (fXhci) {
        kprintf("devmgr: soft found 8086:a12f\n");
    }
}

void
devmgr_soft_init(void)
{
    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: devmgr: soft init PASS */
        kprintf("devmgr: soft init PASS (idempotent call=%u)\n",
                (unsigned)g_cInitCalls);
        return;
    }

    g_cWinUsed = 0u;
    g_cWinNotes = 0u;
    g_cWinRejects = 0u;
    g_cPciScanCalls = 0u;
    g_cBdfUsed = 0u;
    g_cBdfRegs = 0u;
    g_cBdfRejects = 0u;
    g_cPciUsed = 0u;
    g_fScanned = 0;
    memset(g_aWin, 0, sizeof(g_aWin));
    memset(g_aBdf, 0, sizeof(g_aBdf));
    memset(g_aBdfOk, 0, sizeof(g_aBdfOk));
    memset(g_aPci, 0, sizeof(g_aPci));
    g_fReady = 1;

    /*
     * Soft lamps for ABI-first DDI (docs/DDI_SOFT.md).
     * First init defers CF8/CFC walk — call devmgr_soft_pci_scan() to fill.
     * Soft≠product — no cap mint, no VT-d program.
     */
    /* Grep: devmgr: soft init PASS */
    kprintf("devmgr: soft init PASS win_max=%u bar_max=%u bdf_max=%u\n",
            (unsigned)GJ_DEVMGR_SOFT_WIN_MAX,
            (unsigned)GJ_DEVMGR_SOFT_BAR_MAX,
            (unsigned)GJ_DEVMGR_SOFT_BDF_MAX);
    /* Scan deferred until first pci_scan (greppable honesty). */
    kprintf("devmgr: soft pci scan deferred (call pci_scan to fill)\n");
    /* Grep: devmgr: soft dma window note */
    kprintf("devmgr: soft dma window note (soft table ready; no VT-d)\n");
}

int
devmgr_soft_ready(void)
{
    return g_fReady;
}

u32
devmgr_soft_pci_scan(struct gj_devmgr_bdf *pOut, u32 cMax)
{
    g_cPciScanCalls++;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    /* First call walks config space; later calls reuse soft inventory. */
    if (!g_fScanned) {
        devmgr_pci_walk();
        g_fScanned = 1;
    }

    if (pOut != NULL && cMax > 0u) {
        u32 i;
        u32 cCopy = g_cPciUsed;

        if (cCopy > cMax) {
            cCopy = cMax;
        }
        for (i = 0u; i < cCopy; i++) {
            pOut[i] = g_aPci[i].bdf;
        }
    }

    /* Grep: devmgr: soft pci scan PASS n=N */
    kprintf("devmgr: soft pci scan PASS n=%u call=%u\n",
            (unsigned)g_cPciUsed, (unsigned)g_cPciScanCalls);
    devmgr_pci_emit_found_lamps();

    return g_cPciUsed;
}

int
devmgr_soft_bdf_register(const struct gj_devmgr_bdf *pBdf)
{
    u32 i;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    if (pBdf == NULL) {
        g_cBdfRejects++;
        return -1;
    }

    /* Soft slot/func range (PCI type-0). */
    if (pBdf->u8Slot > 31u || pBdf->u8Func > 7u) {
        g_cBdfRejects++;
        return -1;
    }

    /* Dedup: return existing soft index if already registered. */
    for (i = 0u; i < g_cBdfUsed; i++) {
        if (g_aBdfOk[i] != 0u &&
            g_aBdf[i].u8Bus == pBdf->u8Bus &&
            g_aBdf[i].u8Slot == pBdf->u8Slot &&
            g_aBdf[i].u8Func == pBdf->u8Func) {
            return (int)i;
        }
    }

    if (g_cBdfUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
        g_cBdfRejects++;
        return -1;
    }

    i = g_cBdfUsed;
    g_aBdf[i] = *pBdf;
    g_aBdf[i].u8Pad = 0u;
    g_aBdfOk[i] = 1u;
    g_cBdfUsed++;
    g_cBdfRegs++;

    return (int)i;
}

int
devmgr_soft_find_by_id(u16 u16Vend, u16 u16Dev, struct gj_devmgr_pci_fn *pOut)
{
    u32 i;

    if (pOut == NULL) {
        return -1;
    }

    if (!g_fReady) {
        devmgr_soft_init();
    }

    /* Ensure inventory exists so find works after init alone. */
    if (!g_fScanned) {
        (void)devmgr_soft_pci_scan(NULL, 0u);
    }

    for (i = 0u; i < g_cPciUsed; i++) {
        if (g_aPci[i].u16Vendor == u16Vend &&
            g_aPci[i].u16Device == u16Dev) {
            *pOut = g_aPci[i];
            return 0;
        }
    }

    memset(pOut, 0, sizeof(*pOut));
    return -1;
}

u32
devmgr_soft_count(void)
{
    return g_cPciUsed;
}

int
devmgr_soft_get(u32 iIdx, struct gj_devmgr_pci_fn *pOut)
{
    if (pOut == NULL || iIdx >= g_cPciUsed) {
        return -1;
    }
    *pOut = g_aPci[iIdx];
    return 0;
}

int
devmgr_soft_dma_window_note(const struct gj_devmgr_bdf *pBdf,
                            u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                            struct gj_devmgr_window_grant *pGrantOut)
{
    struct gj_devmgr_window_grant *pSlot;
    u32 iWin;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    if (pBdf == NULL || u64Cb == 0u) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    /* Soft slot/func range (PCI type-0). */
    if (pBdf->u8Slot > 31u || pBdf->u8Func > 7u) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    /* Overflow check on PA range. */
    if (u64PaBase + u64Cb < u64PaBase) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    if (g_cWinUsed >= GJ_DEVMGR_SOFT_WIN_MAX) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        /* Grep: devmgr: soft dma window note */
        kprintf("devmgr: soft dma window note REJECT full bdf=%u:%u.%u "
                "pa=0x%lx cb=0x%lx\n",
                (unsigned)pBdf->u8Bus, (unsigned)pBdf->u8Slot,
                (unsigned)pBdf->u8Func, (unsigned long)u64PaBase,
                (unsigned long)u64Cb);
        return -1;
    }

    iWin = g_cWinUsed;
    pSlot = &g_aWin[iWin];
    pSlot->bdf = *pBdf;
    pSlot->u64PaBase = u64PaBase;
    pSlot->u64Cb = u64Cb;
    pSlot->u8Rw = u8Rw;
    pSlot->u8Ok = 1u;
    pSlot->u8Pad[0] = 0u;
    pSlot->u8Pad[1] = 0u;
    pSlot->u32WinId = iWin;
    g_cWinUsed++;
    g_cWinNotes++;

    if (pGrantOut != NULL) {
        *pGrantOut = *pSlot;
    }

    /* Grep: devmgr: soft dma window note */
    kprintf("devmgr: soft dma window note id=%u bdf=%u:%u.%u pa=0x%lx "
            "cb=0x%lx rw=0x%x used=%u\n",
            (unsigned)iWin, (unsigned)pBdf->u8Bus, (unsigned)pBdf->u8Slot,
            (unsigned)pBdf->u8Func, (unsigned long)u64PaBase,
            (unsigned long)u64Cb, (unsigned)u8Rw, (unsigned)g_cWinUsed);

    return 0;
}

int
devmgr_soft_note_window_grant(const struct gj_devmgr_bdf *pBdf,
                              u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                              struct gj_devmgr_window_grant *pGrantOut)
{
    /* Soft table only — no live VT-d program (docs/DDI_SOFT.md). */
    return devmgr_soft_dma_window_note(pBdf, u64PaBase, u64Cb, u8Rw,
                                       pGrantOut);
}

u32
devmgr_soft_pci_scan_calls(void)
{
    return g_cPciScanCalls;
}

u32
devmgr_soft_bdf_count(void)
{
    return g_cBdfUsed;
}

u32
devmgr_soft_bdf_rejects(void)
{
    return g_cBdfRejects;
}

u32
devmgr_soft_window_notes(void)
{
    return g_cWinNotes;
}

u32
devmgr_soft_window_rejects(void)
{
    return g_cWinRejects;
}
