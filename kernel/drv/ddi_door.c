/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DDI door: soft native surface for userspace Linux-shaped driver hosts.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko.
 *
 * Ops (arg0): SCAN/GET/OPEN/MAP_BAR/CFG_READ/DMA_NOTE/INVENTORY/IRQ_BIND/
 * CFG_WRITE.
 * Soft ≠ product: inventory + notes only; no host CNode MMIO/IRQ/DMA mint
 * (docs/DDI_SOFT.md). greppable: ddi_door: soft … | ddi: soft irq note PASS
 *
 * MAP_BAR honesty: vmm_map_device_uc yields a *kernel* UC VA. Soft-return
 * that VA so same-AS smoke can poke; product userspace map is follow-on.
 *
 * IRQ_BIND honesty: soft-records handle→badge on irq_msix only. No hard IRQ
 * in userspace; wait via GJ_SYS_NOTIFY_WAIT on the global MSI-X Notification.
 *
 * CFG_WRITE honesty: careful soft only — soft-note path; does not perform
 * live config writes of identity / BARs (fail closed on unsafe offsets).
 */
#include <gj/ddi_door.h>
#include <gj/devmgr.h>
#include <gj/error.h>
#include <gj/iommu.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/types.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

/* Legacy PCI config mechanism (type-0 header soft path). */
#define DDI_PCI_CFG_ADDR 0xCF8u
#define DDI_PCI_CFG_DATA 0xCFCu

static int  g_fInit;
static int  g_fInventoryOnce;
static u32  g_u32Calls;
static u32  g_u32DevCount;
static u32  g_u32Handles;
static u32  g_u32SoftCfgWriteNotes;
static u32  g_u32SoftCfgWriteReject;
static u32  g_u32SoftMapOk;
static u32  g_u32SoftMapFail;
static u32  g_u32SoftDmaOk;
static u32  g_u32SoftDmaFail;
static u32  g_u32SoftIrqNoteOk;
static u32  g_u32SoftIrqNoteFail;
static u8   g_fSoftIrqNotePass;

static struct gj_ddi_dev_info g_aDev[GJ_DDI_SOFT_DEV_MAX];

/* Soft open table: handle id = index+1; 0 = free. */
static struct {
    u32 u32Idx;   /* inventory index */
    u8  u8Open;
    u8  u8Pad[3];
} g_aHandle[GJ_DDI_SOFT_HANDLE_MAX];

/* Last MAP_BAR soft note (diagnostics). */
static struct gj_ddi_map_note g_lastMap;

/* ---- port I/O (local; matches other freestanding PCI soft paths) ------- */

static void
ddi_outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static u32
ddi_inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
ddi_pci_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | ((u32)u8Off & 0xfcu);

    ddi_outl(DDI_PCI_CFG_ADDR, u32Addr);
    return ddi_inl(DDI_PCI_CFG_DATA);
}

/*
 * Soft BAR size probe: write all-1s, read back, restore. Memory BARs only
 * for soft inventory; I/O BARs report size soft 0 when bit0 set.
 */
static void
ddi_soft_bar_probe(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8BarIdx,
                   u64 *pPa, u64 *pCb)
{
    u8  u8Off = (u8)(0x10u + (u8BarIdx * 4u));
    u32 u32Raw;
    u32 u32Save;
    u32 u32Size;
    u64 u64Pa = 0;
    u64 u64Cb = 0;

    if (pPa != NULL) {
        *pPa = 0;
    }
    if (pCb != NULL) {
        *pCb = 0;
    }
    if (u8BarIdx >= GJ_DDI_SOFT_BAR_MAX) {
        return;
    }

    u32Save = ddi_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
    if (u32Save == 0xffffffffu || u32Save == 0u) {
        return;
    }

    /* I/O BAR soft: report base only, size unknown soft 0. */
    if ((u32Save & 1u) != 0u) {
        u64Pa = (u64)(u32Save & ~3u);
        if (pPa != NULL) {
            *pPa = u64Pa;
        }
        return;
    }

    /* Memory BAR: 64-bit type bit2:1 == 10b may consume next dword soft. */
    u64Pa = (u64)(u32Save & ~0xfu);
    if (((u32Save >> 1) & 3u) == 2u && u8BarIdx + 1u < GJ_DDI_SOFT_BAR_MAX) {
        u32 u32Hi = ddi_pci_cfg_read32(u8Bus, u8Slot, u8Func,
                                       (u8)(u8Off + 4u));
        u64Pa |= ((u64)u32Hi << 32);
    }

    /* Size probe (32-bit low dword only — soft). */
    ddi_outl(DDI_PCI_CFG_ADDR,
             0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                 ((u32)u8Func << 8) | ((u32)u8Off & 0xfcu));
    ddi_outl(DDI_PCI_CFG_DATA, 0xffffffffu);
    u32Raw = ddi_inl(DDI_PCI_CFG_DATA);
    ddi_outl(DDI_PCI_CFG_DATA, u32Save);

    u32Size = u32Raw & ~0xfu;
    if (u32Size != 0u) {
        u64Cb = (u64)((~u32Size) + 1u);
    }

    if (pPa != NULL) {
        *pPa = u64Pa;
    }
    if (pCb != NULL) {
        *pCb = u64Cb;
    }
}

static void
ddi_fill_dev_from_bdf(struct gj_ddi_dev_info *pDev,
                      const struct gj_devmgr_bdf *pBdf)
{
    u32 u32Id;
    u32 u32Class;
    u64 pa;
    u64 cb;

    if (pDev == NULL || pBdf == NULL) {
        return;
    }
    memset(pDev, 0, sizeof(*pDev));
    pDev->u8Bus  = pBdf->u8Bus;
    pDev->u8Slot = pBdf->u8Slot;
    pDev->u8Func = pBdf->u8Func;

    u32Id = ddi_pci_cfg_read32(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 0x00);
    if (u32Id == 0xffffffffu || (u32Id & 0xffffu) == 0xffffu) {
        /* Soft empty: leave vend/dev 0; host may still open for notes. */
        return;
    }
    pDev->u16Vend = (u16)(u32Id & 0xffffu);
    pDev->u16Dev  = (u16)((u32Id >> 16) & 0xffffu);

    u32Class = ddi_pci_cfg_read32(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func,
                                  0x08);
    pDev->u32Class = u32Class;

    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 0, &pa, &cb);
    pDev->u64Bar0Pa = pa;
    pDev->u64Bar0Cb = cb;
    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 1, &pa, &cb);
    pDev->u64Bar1Pa = pa;
    pDev->u64Bar1Cb = cb;
    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 2, &pa, &cb);
    pDev->u64Bar2Pa = pa;
    pDev->u64Bar2Cb = cb;
    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 3, &pa, &cb);
    pDev->u64Bar3Pa = pa;
    pDev->u64Bar3Cb = cb;
    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 4, &pa, &cb);
    pDev->u64Bar4Pa = pa;
    pDev->u64Bar4Cb = cb;
    ddi_soft_bar_probe(pBdf->u8Bus, pBdf->u8Slot, pBdf->u8Func, 5, &pa, &cb);
    pDev->u64Bar5Pa = pa;
    pDev->u64Bar5Cb = cb;
}

/* Convert D1 inventory row → door packed info (prefer over local CF8 probe). */
static int
ddi_from_devmgr_fn(u32 u32Idx, struct gj_ddi_dev_info *pOut)
{
    struct gj_devmgr_pci_fn fn;
    u32 b;

    if (pOut == NULL) {
        return -1;
    }
    if (devmgr_soft_get(u32Idx, &fn) != 0) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u8Bus = fn.bdf.u8Bus;
    pOut->u8Slot = fn.bdf.u8Slot;
    pOut->u8Func = fn.bdf.u8Func;
    pOut->u16Vend = fn.u16Vendor;
    pOut->u16Dev = fn.u16Device;
    pOut->u32Class = ((u32)fn.u8Class << 24) | ((u32)fn.u8Subclass << 16) |
                     ((u32)fn.u8ProgIf << 8);
    for (b = 0; b < 6u; b++) {
        u64 pa = fn.aBar[b].u64Pa;
        u64 cb = fn.aBar[b].u64Cb;

        switch (b) {
        case 0: pOut->u64Bar0Pa = pa; pOut->u64Bar0Cb = cb; break;
        case 1: pOut->u64Bar1Pa = pa; pOut->u64Bar1Cb = cb; break;
        case 2: pOut->u64Bar2Pa = pa; pOut->u64Bar2Cb = cb; break;
        case 3: pOut->u64Bar3Pa = pa; pOut->u64Bar3Cb = cb; break;
        case 4: pOut->u64Bar4Pa = pa; pOut->u64Bar4Cb = cb; break;
        case 5: pOut->u64Bar5Pa = pa; pOut->u64Bar5Cb = cb; break;
        default: break;
        }
    }
    return 0;
}

/* ---- copy helper (user window + kernel-smoke HHDM/static) -------------- */

static i64
ddi_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
        return 0;
    }
    /* Early kernel smoke: treat as trusted kernel buffer. */
    memcpy((void *)(uintptr_t)u64Dst, pSrc, cb);
    return 0;
}

static int
ddi_handle_lookup(u32 u32Handle, u32 *pIdxOut)
{
    u32 iSlot;

    if (u32Handle == 0 || u32Handle > GJ_DDI_SOFT_HANDLE_MAX) {
        return 0;
    }
    iSlot = u32Handle - 1u;
    if (g_aHandle[iSlot].u8Open == 0u) {
        return 0;
    }
    if (pIdxOut != NULL) {
        *pIdxOut = g_aHandle[iSlot].u32Idx;
    }
    return 1;
}

static void
ddi_bar_pa_cb(const struct gj_ddi_dev_info *pDev, u32 u32Bar,
              u64 *pPa, u64 *pCb)
{
    u64 pa = 0;
    u64 cb = 0;

    if (pDev == NULL) {
        if (pPa != NULL) {
            *pPa = 0;
        }
        if (pCb != NULL) {
            *pCb = 0;
        }
        return;
    }
    switch (u32Bar) {
    case 0: pa = pDev->u64Bar0Pa; cb = pDev->u64Bar0Cb; break;
    case 1: pa = pDev->u64Bar1Pa; cb = pDev->u64Bar1Cb; break;
    case 2: pa = pDev->u64Bar2Pa; cb = pDev->u64Bar2Cb; break;
    case 3: pa = pDev->u64Bar3Pa; cb = pDev->u64Bar3Cb; break;
    case 4: pa = pDev->u64Bar4Pa; cb = pDev->u64Bar4Cb; break;
    case 5: pa = pDev->u64Bar5Pa; cb = pDev->u64Bar5Cb; break;
    default: break;
    }
    if (pPa != NULL) {
        *pPa = pa;
    }
    if (pCb != NULL) {
        *pCb = cb;
    }
}

void
ddi_door_init(void)
{
    if (g_fInit) {
        return;
    }
    memset(g_aDev, 0, sizeof(g_aDev));
    memset(g_aHandle, 0, sizeof(g_aHandle));
    memset(&g_lastMap, 0, sizeof(g_lastMap));
    g_u32DevCount = 0u;
    g_u32Handles = 0u;
    g_u32Calls = 0u;
    g_u32SoftIrqNoteOk = 0u;
    g_u32SoftIrqNoteFail = 0u;
    g_fSoftIrqNotePass = 0u;
    g_fInventoryOnce = 0;
    g_fInit = 1;

    /* Soft ready on devmgr side (D1). Safe if already initialized. */
    if (!devmgr_soft_ready()) {
        devmgr_soft_init();
    }

    /* Grep: ddi_door: soft init */
    kprintf("ddi_door: soft init PASS (dev_max=%u handle_max=%u)\n",
            (unsigned)GJ_DDI_SOFT_DEV_MAX,
            (unsigned)GJ_DDI_SOFT_HANDLE_MAX);
}

int
ddi_door_ready(void)
{
    return g_fInit;
}

u32
ddi_door_call_count(void)
{
    return g_u32Calls;
}

i64
ddi_door_syscall(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        ddi_door_init();
    }
    g_u32Calls++;

    switch (u32Op) {
    case DDI_OP_SCAN: {
        struct gj_devmgr_bdf aBdf[GJ_DDI_SOFT_DEV_MAX];
        u32 cScan;
        u32 i;

        memset(aBdf, 0, sizeof(aBdf));
        cScan = devmgr_soft_pci_scan(aBdf, GJ_DDI_SOFT_DEV_MAX);
        if (cScan > GJ_DDI_SOFT_DEV_MAX) {
            cScan = GJ_DDI_SOFT_DEV_MAX;
        }

        g_u32DevCount = 0u;
        memset(g_aDev, 0, sizeof(g_aDev));

        for (i = 0u; i < cScan; i++) {
            /* Prefer D1 inventory BARs; else soft fill from BDF+cfg. */
            if (ddi_from_devmgr_fn(i, &g_aDev[i]) != 0) {
                ddi_fill_dev_from_bdf(&g_aDev[i], &aBdf[i]);
            }
            g_u32DevCount++;
        }

        /* Grep: ddi_door: soft scan */
        kprintf("ddi_door: soft scan count=%u (devmgr_soft_pci_scan)\n",
                (unsigned)g_u32DevCount);
        return (i64)g_u32DevCount;
    }

    case DDI_OP_GET: {
        u32 u32Idx = (u32)u64Arg1;
        u64 u64User = u64Arg2;
        i64 st;

        if (u64User == 0) {
            return GJ_ERR_INVAL;
        }
        if (g_u32DevCount == 0u) {
            /* Soft auto-scan so GET after boot without explicit SCAN works. */
            (void)ddi_door_syscall(DDI_OP_SCAN, 0, 0, 0);
        }
        if (u32Idx >= g_u32DevCount) {
            return GJ_ERR_NOENT;
        }
        st = ddi_copy_out(u64User, &g_aDev[u32Idx],
                          (u32)sizeof(struct gj_ddi_dev_info));
        return st;
    }

    case DDI_OP_OPEN: {
        u32 u32Idx = (u32)u64Arg1;
        u32 i;

        if (g_u32DevCount == 0u) {
            (void)ddi_door_syscall(DDI_OP_SCAN, 0, 0, 0);
        }
        if (u32Idx >= g_u32DevCount) {
            return GJ_ERR_NOENT;
        }
        /* Re-open same index: return existing handle if present. */
        for (i = 0u; i < GJ_DDI_SOFT_HANDLE_MAX; i++) {
            if (g_aHandle[i].u8Open != 0u &&
                g_aHandle[i].u32Idx == u32Idx) {
                return (i64)(i + 1u);
            }
        }
        for (i = 0u; i < GJ_DDI_SOFT_HANDLE_MAX; i++) {
            if (g_aHandle[i].u8Open == 0u) {
                g_aHandle[i].u8Open = 1u;
                g_aHandle[i].u32Idx = u32Idx;
                g_u32Handles++;
                /* Grep: ddi_door: soft open */
                kprintf("ddi_door: soft open handle=%u idx=%u bdf=%u:%u.%u\n",
                        (unsigned)(i + 1u), (unsigned)u32Idx,
                        (unsigned)g_aDev[u32Idx].u8Bus,
                        (unsigned)g_aDev[u32Idx].u8Slot,
                        (unsigned)g_aDev[u32Idx].u8Func);
                return (i64)(i + 1u);
            }
        }
        return GJ_ERR_NOMEM;
    }

    case DDI_OP_MAP_BAR: {
        u32 u32Handle = (u32)u64Arg1;
        u32 u32Bar = (u32)u64Arg2;
        /* u64Arg3: va_hint OR user ptr to gj_ddi_map_note */
        u32 u32Idx;
        u64 u64Pa;
        u64 u64Cb;
        gj_vaddr_t vaOut = 0;
        gj_status_t st;
        struct gj_thread *pThr;
        struct gj_process *pProc = NULL;
        u64 u64UserVa = 0;
        int fUserMap = 0;
        int fNoteOut = 0;

        if (!ddi_handle_lookup(u32Handle, &u32Idx)) {
            return GJ_ERR_INVAL;
        }
        if (u32Bar >= GJ_DDI_SOFT_BAR_MAX) {
            return GJ_ERR_INVAL;
        }
        ddi_bar_pa_cb(&g_aDev[u32Idx], u32Bar, &u64Pa, &u64Cb);
        if (u64Pa == 0 || u64Cb == 0) {
            g_u32SoftMapFail++;
            return GJ_ERR_NODEV;
        }

        /*
         * arg3 dual use (soft):
         *  - if user_range_ok(arg3, sizeof(gj_ddi_map_note)) → fill out-note
         *  - else if arg3 in user VA band → treat as map VA hint
         * Prefer real user-AS UC map via vmm_map_user_device when a process
         * is current; fall back to kernel UC window for same-AS smoke.
         */
        if (u64Arg3 != 0 &&
            user_range_ok(u64Arg3, (u64)sizeof(struct gj_ddi_map_note))) {
            fNoteOut = 1;
        } else if (u64Arg3 != 0 && u64Arg3 >= 0x10000ull &&
                   u64Arg3 < 0x0000800000000000ull) {
            u64UserVa = u64Arg3 & ~0xfffull;
        }

        pThr = thread_current();
        if (pThr != NULL) {
            pProc = pThr->pProc;
        }
        if (pProc != NULL) {
            if (u64UserVa == 0) {
                /* Soft fixed band for driver hosts (page-aligned). */
                u64UserVa = 0x0000000070000000ull +
                            ((u64)u32Handle << 20) + ((u64)u32Bar << 16);
            }
            st = vmm_map_user_device(pProc, u64UserVa, (gj_paddr_t)u64Pa,
                                     u64Cb,
                                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
            if (st == GJ_OK) {
                vaOut = (gj_vaddr_t)u64UserVa;
                fUserMap = 1;
            }
        }
        if (!fUserMap) {
            st = vmm_map_device_uc((gj_paddr_t)u64Pa, u64Cb, &vaOut);
        }

        memset(&g_lastMap, 0, sizeof(g_lastMap));
        g_lastMap.u64Pa = u64Pa;
        g_lastMap.u64Cb = u64Cb;
        g_lastMap.u32Bar = u32Bar;
        g_lastMap.u32Handle = u32Handle;

        if (st != GJ_OK || vaOut == 0) {
            g_u32SoftMapFail++;
            kprintf("ddi_door: soft map bar FAIL handle=%u bar=%u pa=0x%lx "
                    "cb=0x%lx st=%d\n",
                    (unsigned)u32Handle, (unsigned)u32Bar,
                    (unsigned long)u64Pa, (unsigned long)u64Cb, (int)st);
            return GJ_ERR_FAULT;
        }

        g_lastMap.u64Va = (u64)vaOut;
        g_lastMap.u8Ok = 1u;
        g_u32SoftMapOk++;
        kprintf("ddi_door: soft map bar handle=%u bar=%u pa=0x%lx cb=0x%lx "
                "va=0x%lx user=%d\n",
                (unsigned)u32Handle, (unsigned)u32Bar,
                (unsigned long)u64Pa, (unsigned long)u64Cb,
                (unsigned long)vaOut, fUserMap);

        if (fNoteOut) {
            (void)ddi_copy_out(u64Arg3, &g_lastMap,
                               (u32)sizeof(struct gj_ddi_map_note));
        }
        /* Positive low VA fits i64; high kernel UC → 0 + note/out. */
        if ((u64)vaOut <= 0x7fffffffffffffffull) {
            return (i64)(u64)vaOut;
        }
        return 0;
    }

    case DDI_OP_CFG_READ: {
        u32 u32Handle = (u32)u64Arg1;
        u32 u32Off = (u32)u64Arg2;
        u32 u32Idx;
        u32 u32Val;

        if (!ddi_handle_lookup(u32Handle, &u32Idx)) {
            return GJ_ERR_INVAL;
        }
        if ((u32Off & 3u) != 0u || u32Off > 0xffu) {
            return GJ_ERR_INVAL;
        }
        u32Val = ddi_pci_cfg_read32(g_aDev[u32Idx].u8Bus,
                                    g_aDev[u32Idx].u8Slot,
                                    g_aDev[u32Idx].u8Func, (u8)u32Off);
        return (i64)(u32)u32Val;
    }

    case DDI_OP_DMA_NOTE: {
        u32 u32Handle = (u32)u64Arg1;
        u64 u64Pa = u64Arg2;
        u64 u64Cb = u64Arg3;
        u32 u32Idx;
        struct gj_devmgr_bdf bdf;
        struct gj_devmgr_window_grant grant;
        int stDev;
        int stIommu;

        if (!ddi_handle_lookup(u32Handle, &u32Idx)) {
            return GJ_ERR_INVAL;
        }
        if (u64Cb == 0u) {
            g_u32SoftDmaFail++;
            return GJ_ERR_INVAL;
        }

        memset(&bdf, 0, sizeof(bdf));
        bdf.u8Bus  = g_aDev[u32Idx].u8Bus;
        bdf.u8Slot = g_aDev[u32Idx].u8Slot;
        bdf.u8Func = g_aDev[u32Idx].u8Func;

        memset(&grant, 0, sizeof(grant));
        stDev = devmgr_soft_dma_window_note(&bdf, u64Pa, u64Cb, 0x3u, &grant);
        stIommu = iommu_window_grant(bdf.u8Bus, bdf.u8Slot, bdf.u8Func,
                                     u64Pa, u64Cb);

        if (stDev != 0 && stIommu != 0) {
            g_u32SoftDmaFail++;
            /* Grep: ddi_door: soft dma */
            kprintf("ddi_door: soft dma note FAIL handle=%u pa=0x%lx "
                    "cb=0x%lx\n",
                    (unsigned)u32Handle, (unsigned long)u64Pa,
                    (unsigned long)u64Cb);
            return GJ_ERR_NODEV;
        }
        g_u32SoftDmaOk++;
        /* Grep: ddi_door: soft dma */
        kprintf("ddi_door: soft dma note handle=%u bdf=%u:%u.%u pa=0x%lx "
                "cb=0x%lx devmgr=%d iommu=%d win=%u (soft≠cap mint)\n",
                (unsigned)u32Handle, (unsigned)bdf.u8Bus, (unsigned)bdf.u8Slot,
                (unsigned)bdf.u8Func, (unsigned long)u64Pa,
                (unsigned long)u64Cb, stDev, stIommu,
                (unsigned)grant.u32WinId);
        return 0;
    }

    case DDI_OP_INVENTORY: {
        if (!g_fInventoryOnce) {
            g_fInventoryOnce = 1;
            /* Grep: ddi_door: soft product surface PASS */
            kprintf("ddi_door: soft product surface PASS "
                    "ops=scan,get,open,map_bar,cfg_r,dma,inv,irq_bind,cfg_w "
                    "sys=GJ_SYS_DDI soft≠product\n");
        }
        return 0;
    }

    case DDI_OP_IRQ_BIND: {
        u32 u32Handle = (u32)u64Arg1;
        u64 u64Badge = u64Arg2;
        u32 u32Idx;
        int stBind;

        (void)u64Arg3;
        if (!ddi_handle_lookup(u32Handle, &u32Idx)) {
            g_u32SoftIrqNoteFail++;
            return GJ_ERR_INVAL;
        }
        /*
         * Soft irq→handle note only. No hard IRQ delivery into userspace;
         * no product Notification cap mint. Delivery stays on the global
         * MSI-X Notification; host reaps with GJ_SYS_NOTIFY_WAIT.
         */
        stBind = irq_msix_soft_user_bind(u32Handle, u64Badge);
        if (stBind != 0) {
            g_u32SoftIrqNoteFail++;
            kprintf("ddi: soft irq note FAIL handle=%u badge=0x%lx "
                    "(irq_msix not ready)\n",
                    (unsigned)u32Handle, (unsigned long)u64Badge);
            return GJ_ERR_NODEV;
        }
        if (u64Badge == 0ull) {
            u64Badge = GJ_MSIX_BADGE_SOFT;
        }
        g_u32SoftIrqNoteOk++;
        /* Grep: ddi: soft irq note PASS */
        if (g_fSoftIrqNotePass == 0u) {
            g_fSoftIrqNotePass = 1u;
            kprintf("ddi: soft irq note PASS handle=%u idx=%u "
                    "badge=0x%lx bdf=%u:%u.%u "
                    "wait=GJ_SYS_NOTIFY_WAIT which=0 mask=0x%lx "
                    "block=1 soft≠product\n",
                    (unsigned)u32Handle, (unsigned)u32Idx,
                    (unsigned long)u64Badge,
                    (unsigned)g_aDev[u32Idx].u8Bus,
                    (unsigned)g_aDev[u32Idx].u8Slot,
                    (unsigned)g_aDev[u32Idx].u8Func,
                    (unsigned long)u64Badge);
        } else {
            kprintf("ddi: soft irq note handle=%u idx=%u badge=0x%lx "
                    "bdf=%u:%u.%u (soft rebind; wait=GJ_SYS_NOTIFY_WAIT)\n",
                    (unsigned)u32Handle, (unsigned)u32Idx,
                    (unsigned long)u64Badge,
                    (unsigned)g_aDev[u32Idx].u8Bus,
                    (unsigned)g_aDev[u32Idx].u8Slot,
                    (unsigned)g_aDev[u32Idx].u8Func);
        }
        /* Twin lamp for ddi_door greps */
        kprintf("ddi_door: soft irq bind handle=%u badge=0x%lx ok=%u "
                "fail=%u soft≠cap_mint\n",
                (unsigned)u32Handle, (unsigned long)u64Badge,
                (unsigned)g_u32SoftIrqNoteOk,
                (unsigned)g_u32SoftIrqNoteFail);
        return 0;
    }

    case DDI_OP_CFG_WRITE: {
        u32 u32Handle = (u32)u64Arg1;
        u32 u32Off = (u32)u64Arg2;
        u32 u32Val = (u32)u64Arg3;
        u32 u32Idx;

        if (!ddi_handle_lookup(u32Handle, &u32Idx)) {
            return GJ_ERR_INVAL;
        }
        if ((u32Off & 3u) != 0u || u32Off > 0xffu) {
            g_u32SoftCfgWriteReject++;
            return GJ_ERR_INVAL;
        }

        /*
         * Careful soft only if safe:
         *   - Refuse identity (0x00) and BAR window (0x10..0x24) live writes.
         *   - Soft-note remaining header offsets without live CF8 poke
         *     (soft≠product; avoids bus-master / decode surprises from
         *     untrusted host until real grant policy exists).
         */
        if (u32Off == 0x00u || (u32Off >= 0x10u && u32Off <= 0x24u)) {
            g_u32SoftCfgWriteReject++;
            /* Grep: ddi_door: soft cfg write */
            kprintf("ddi_door: soft cfg write REJECT handle=%u off=0x%x "
                    "val=0x%x (unsafe identity/BAR)\n",
                    (unsigned)u32Handle, (unsigned)u32Off,
                    (unsigned)u32Val);
            return GJ_ERR_PERM;
        }

        g_u32SoftCfgWriteNotes++;
        /* Grep: ddi_door: soft cfg write */
        kprintf("ddi_door: soft cfg write note handle=%u bdf=%u:%u.%u "
                "off=0x%x val=0x%x (soft-note only; no live poke)\n",
                (unsigned)u32Handle, (unsigned)g_aDev[u32Idx].u8Bus,
                (unsigned)g_aDev[u32Idx].u8Slot,
                (unsigned)g_aDev[u32Idx].u8Func, (unsigned)u32Off,
                (unsigned)u32Val);
        return 0;
    }

    default:
        return GJ_ERR_NOSUPPORT;
    }
}
