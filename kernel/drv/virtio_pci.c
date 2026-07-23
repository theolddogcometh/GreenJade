/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-pci modern transport + virtqueues (OASIS virtio 1.1).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Soft product depth (common-cfg, features, queue setup):
 *   - modern common-cfg cap walk + soft reset
 *   - feature read/write helpers + soft negotiate ladder
 *   - queue soft size clamp, disable-before-setup, enable verify
 *
 * Greppable product markers (prefix-stable):
 *   virtio: scan PASS
 *   virtio: modern common@
 *   virtio: features
 *   virtio: features soft
 *   virtio: q soft
 *   virtio: q%u size=
 *   virtio: driver_ok
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/vmm.h>

#define PCI_CFG_ADDR 0xCF8u
#define PCI_CFG_DATA 0xCFCu

#define PCI_CMD      0x04u
#define PCI_CMD_IO   0x1u
#define PCI_CMD_MEM  0x2u
#define PCI_CMD_BUS  0x4u
#define PCI_CAP_PTR  0x34u
#define PCI_CAP_ID_VNDR 0x09u

/* Local aliases to public GJ_VIRTIO_PCI_COMMON_* (keep body greppable). */
#define VIRTIO_PCI_COMMON_DFSELECT   GJ_VIRTIO_PCI_COMMON_DFSELECT
#define VIRTIO_PCI_COMMON_DF         GJ_VIRTIO_PCI_COMMON_DF
#define VIRTIO_PCI_COMMON_GFSELECT   GJ_VIRTIO_PCI_COMMON_GFSELECT
#define VIRTIO_PCI_COMMON_GF         GJ_VIRTIO_PCI_COMMON_GF
#define VIRTIO_PCI_COMMON_MSIX       GJ_VIRTIO_PCI_COMMON_MSIX
#define VIRTIO_PCI_COMMON_NUMQ       GJ_VIRTIO_PCI_COMMON_NUMQ
#define VIRTIO_PCI_COMMON_STATUS     GJ_VIRTIO_PCI_COMMON_STATUS
#define VIRTIO_PCI_COMMON_CFGGEN     GJ_VIRTIO_PCI_COMMON_CFGGEN
#define VIRTIO_PCI_COMMON_Q_SELECT   GJ_VIRTIO_PCI_COMMON_Q_SELECT
#define VIRTIO_PCI_COMMON_Q_SIZE     GJ_VIRTIO_PCI_COMMON_Q_SIZE
#define VIRTIO_PCI_COMMON_Q_MSIX     GJ_VIRTIO_PCI_COMMON_Q_MSIX
#define VIRTIO_PCI_COMMON_Q_ENABLE   GJ_VIRTIO_PCI_COMMON_Q_ENABLE
#define VIRTIO_PCI_COMMON_Q_NOFF     GJ_VIRTIO_PCI_COMMON_Q_NOFF
#define VIRTIO_PCI_COMMON_Q_DESC     GJ_VIRTIO_PCI_COMMON_Q_DESC
#define VIRTIO_PCI_COMMON_Q_DRIVER   GJ_VIRTIO_PCI_COMMON_Q_DRIVER
#define VIRTIO_PCI_COMMON_Q_DEVICE   GJ_VIRTIO_PCI_COMMON_Q_DEVICE

static struct gj_virtio_dev g_aDevs[GJ_VIRTIO_MAX_DEVS];
static u32                  g_cDevs;

static void
outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile ("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static u32
inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile ("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
pci_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = (1u << 31) | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xFCu);

    outl(PCI_CFG_ADDR, u32Addr);
    return inl(PCI_CFG_DATA);
}

static u16
pci_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32 = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    return (u16)((u32 >> ((u8Off & 2u) * 8u)) & 0xffffu);
}

static u8
pci_read8(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32 = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    return (u8)((u32 >> ((u8Off & 3u) * 8u)) & 0xffu);
}

static void
pci_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    u32 u32Addr = (1u << 31) | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xFCu);

    outl(PCI_CFG_ADDR, u32Addr);
    outl(PCI_CFG_DATA, u32Val);
}

static void
pci_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Word = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    u32 u32Shift = (u8Off & 2u) * 8u;

    u32Word &= ~(0xffffu << u32Shift);
    u32Word |= ((u32)u16Val << u32Shift);
    pci_write32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu, u32Word);
}

static u32
kind_from_device(u16 u16Device)
{
    switch (u16Device) {
    case GJ_VIRTIO_PCI_DEV_NET_MODERN:
    case 0x1000u:
        return 1;
    case GJ_VIRTIO_PCI_DEV_BLK_MODERN:
    case 0x1001u:
        return 2;
    case GJ_VIRTIO_PCI_DEV_SCSI_MODERN:
    case 0x1004u:
        return 6;
    case GJ_VIRTIO_PCI_DEV_CONSOLE:
    case 0x1003u:
        return 5;
    case GJ_VIRTIO_PCI_DEV_GPU_MODERN:
        return 3;
    case GJ_VIRTIO_PCI_DEV_INPUT:
        return 4;
    default:
        return 0;
    }
}

static const char *
kind_name(u32 u32Kind)
{
    switch (u32Kind) {
    case 1:
        return "net";
    case 2:
        return "blk";
    case 3:
        return "gpu";
    case 4:
        return "input";
    case 5:
        return "console";
    case 6:
        return "scsi";
    default:
        return "virtio";
    }
}

/* Resolve capability BAR+offset to an MMIO pointer; NULL if unusable. */
static volatile u8 *
bar_ptr(struct gj_virtio_dev *pDev, u8 u8Bar, u32 u32Off)
{
    if (pDev == NULL || u8Bar >= 6 || pDev->u64Bar[u8Bar] == 0) {
        return NULL;
    }
    if ((pDev->u32BarIsIo & (1u << u8Bar)) != 0) {
        return NULL; /* MMIO only for modern */
    }
    return (volatile u8 *)(gj_vaddr_t)(pDev->u64Bar[u8Bar] + u32Off);
}

static u8
mmio_r8(volatile u8 *p)
{
    return p ? *p : 0;
}

static void
mmio_w8(volatile u8 *p, u8 u8V)
{
    if (p) {
        *p = u8V;
    }
}

static u16
mmio_r16(volatile u8 *p)
{
    return p ? *(volatile u16 *)p : 0;
}

static void
mmio_w16(volatile u8 *p, u16 u16V)
{
    if (p) {
        *(volatile u16 *)p = u16V;
    }
}

static u32
mmio_r32(volatile u8 *p)
{
    return p ? *(volatile u32 *)p : 0;
}

static void
mmio_w32(volatile u8 *p, u32 u32V)
{
    if (p) {
        *(volatile u32 *)p = u32V;
    }
}

static void
mmio_w64(volatile u8 *p, u64 u64V)
{
    if (p) {
        *(volatile u64 *)p = u64V;
    }
}

/* ---- common-cfg feature helpers (soft; greppable) -------------------- */

static u64
common_features_read(struct gj_virtio_dev *pDev, int fGuest)
{
    volatile u8 *pCommon;
    u32 u32Sel;
    u32 u32Val;
    u32 u32Lo;
    u32 u32Hi;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    pCommon = pDev->pCommon;
    if (fGuest) {
        u32Sel = VIRTIO_PCI_COMMON_GFSELECT;
        u32Val = VIRTIO_PCI_COMMON_GF;
    } else {
        u32Sel = VIRTIO_PCI_COMMON_DFSELECT;
        u32Val = VIRTIO_PCI_COMMON_DF;
    }
    mmio_w32(pCommon + u32Sel, 0);
    u32Lo = mmio_r32(pCommon + u32Val);
    mmio_w32(pCommon + u32Sel, 1);
    u32Hi = mmio_r32(pCommon + u32Val);
    return ((u64)u32Hi << 32) | (u64)u32Lo;
}

static void
common_features_write_guest(struct gj_virtio_dev *pDev, u64 u64Drv)
{
    volatile u8 *pCommon;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    pCommon = pDev->pCommon;
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GFSELECT, 0);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GF, (u32)u64Drv);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GFSELECT, 1);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GF, (u32)(u64Drv >> 32));
}

/* Soft power-of-two size clamp into [1, u16Max] (0 if max is 0). */
static u16
q_soft_size(u16 u16Want, u16 u16Max)
{
    u16 u16P2;

    if (u16Max == 0) {
        return 0;
    }
    if (u16Want == 0) {
        u16Want = 1;
    }
    if (u16Want > u16Max) {
        u16Want = u16Max;
    }
    /* Round down to power of two within max (virtio queue_size requirement). */
    u16P2 = 1;
    while ((u16)(u16P2 << 1) <= u16Want && (u16)(u16P2 << 1) <= u16Max &&
           (u16P2 << 1) != 0) {
        u16P2 = (u16)(u16P2 << 1);
    }
    if (u16P2 > u16Max) {
        /* max itself may not be power-of-two on broken hosts — soft clamp */
        u16P2 = 1;
        while ((u16)(u16P2 << 1) <= u16Max && (u16P2 << 1) != 0) {
            u16P2 = (u16)(u16P2 << 1);
        }
    }
    return u16P2;
}

static void
q_ring_free(struct gj_virtq *pQ)
{
    if (pQ == NULL) {
        return;
    }
    if (pQ->paDesc != 0) {
        pmm_free(pQ->paDesc);
        pQ->paDesc = 0;
        pQ->pDesc = NULL;
    }
    if (pQ->paAvail != 0) {
        pmm_free(pQ->paAvail);
        pQ->paAvail = 0;
        pQ->pAvail = NULL;
    }
    if (pQ->paUsed != 0) {
        pmm_free(pQ->paUsed);
        pQ->paUsed = 0;
        pQ->pUsed = NULL;
    }
}

static gj_paddr_t
alloc_zero_page(void)
{
    gj_paddr_t pa = pmm_alloc();

    if (pa == 0) {
        return 0;
    }
    memset((void *)hhdm_to_virt(pa), 0, GJ_PAGE_SIZE);
    return pa;
}

void
virtio_init(void)
{
    memset(g_aDevs, 0, sizeof(g_aDevs));
    g_cDevs = 0;
}

u32
virtio_pci_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Id;
    u16 u16Vendor;
    u16 u16Device;
    u32 iBar;

    g_cDevs = 0;
    for (u8Bus = 0; u8Bus < 8; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32; u8Slot++) {
            for (u8Func = 0; u8Func < 8; u8Func++) {
                struct gj_virtio_dev *pDev;

                u32Id = pci_read32(u8Bus, u8Slot, u8Func, 0);
                u16Vendor = (u16)(u32Id & 0xFFFFu);
                u16Device = (u16)(u32Id >> 16);
                if (u16Vendor == 0xFFFFu || u16Vendor == 0) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                if (u16Vendor != GJ_VIRTIO_PCI_VENDOR) {
                    continue;
                }
                if (g_cDevs >= GJ_VIRTIO_MAX_DEVS) {
                    goto done;
                }
                pDev = &g_aDevs[g_cDevs];
                memset(pDev, 0, sizeof(*pDev));
                pDev->u16Vendor = u16Vendor;
                pDev->u16Device = u16Device;
                pDev->u8Bus = u8Bus;
                pDev->u8Slot = u8Slot;
                pDev->u8Func = u8Func;
                pDev->u32Kind = kind_from_device(u16Device);
                for (iBar = 0; iBar < 6; iBar++) {
                    u32 u32Bar = pci_read32(u8Bus, u8Slot, u8Func,
                                            (u8)(0x10u + iBar * 4u));

                    if ((u32Bar & 1u) != 0) {
                        pDev->u32BarIsIo |= (1u << iBar);
                        pDev->u64Bar[iBar] = (u64)(u32Bar & ~0x3u);
                    } else {
                        pDev->u64Bar[iBar] = (u64)(u32Bar & ~0xFull);
                        if ((u32Bar & 0x6u) == 0x4u && iBar + 1 < 6) {
                            /* 64-bit BAR */
                            u32 u32Hi = pci_read32(u8Bus, u8Slot, u8Func,
                                                   (u8)(0x10u + (iBar + 1) * 4u));
                            pDev->u64Bar[iBar] |= ((u64)u32Hi << 32);
                            iBar++;
                        }
                    }
                }
                /* High MMIO BARs (common with ≥4 GiB RAM) need explicit maps */
                for (iBar = 0; iBar < 6; iBar++) {
                    if (pDev->u64Bar[iBar] == 0 ||
                        (pDev->u32BarIsIo & (1u << iBar)) != 0) {
                        continue;
                    }
                    if (pDev->u64Bar[iBar] >= 0x100000000ull ||
                        pDev->u64Bar[iBar] >= 0xf0000000ull) {
                        if (vmm_map_device((gj_paddr_t)pDev->u64Bar[iBar],
                                           2ull * 1024ull * 1024ull) != GJ_OK) {
                            kprintf("virtio: map bar%u 0x%lx failed\n", iBar,
                                    (unsigned long)pDev->u64Bar[iBar]);
                        }
                    }
                }
                kprintf("virtio: pci %x:%x.%u id=%x:%x bars=%lx/%lx/%lx %s\n",
                        (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
                        (unsigned)u16Vendor, (unsigned)u16Device,
                        (unsigned long)pDev->u64Bar[0],
                        (unsigned long)pDev->u64Bar[1],
                        (unsigned long)pDev->u64Bar[4],
                        kind_name(pDev->u32Kind));
                g_cDevs++;
            }
        }
    }
done:
    if (g_cDevs > 0) {
        kprintf("virtio: scan PASS found %u device(s)\n", g_cDevs);
    } else {
        kprintf("virtio: scan found 0 device(s)\n");
    }
    return g_cDevs;
}

u32
virtio_dev_count(void)
{
    return g_cDevs;
}

struct gj_virtio_dev *
virtio_dev_get(u32 u32Index)
{
    if (u32Index >= g_cDevs) {
        return NULL;
    }
    return &g_aDevs[u32Index];
}

/*
 * Walk PCI vendor caps and wire modern virtio MMIO windows.
 * Requires a non-NULL pDev from virtio_pci_scan(); leaves pCommon set on success.
 */
gj_status_t
virtio_pci_setup(struct gj_virtio_dev *pDev)
{
    u8 u8Cap;
    u16 u16Cmd;

    if (pDev == NULL) {
        return GJ_ERR_INVAL;
    }

    pDev->fModern = 0;
    pDev->u64FeaturesDev = 0;
    pDev->u64FeaturesDrv = 0;
    pDev->pCommon = NULL;
    pDev->pNotify = NULL;
    pDev->pIsr = NULL;
    pDev->pDevice = NULL;
    pDev->u32NotifyMult = 0;
    pDev->u32NumQueues = 0;

    /* Enable memory + bus master before touching BARs */
    u16Cmd = pci_read16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CMD);
    u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
    pci_write16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CMD, u16Cmd);

    {
        u16 u16Status = pci_read16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, 0x06);

        if ((u16Status & 0x10u) == 0) {
            kprintf("virtio: %x:%x no cap list (status=0x%x)\n",
                    (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
                    (unsigned)u16Status);
            return GJ_ERR_NOSUPPORT;
        }
    }
    u8Cap = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CAP_PTR) &
            0xFCu;
    while (u8Cap != 0) {
        u8 u8Id = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, u8Cap);
        u8 u8Next = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                              (u8)(u8Cap + 1));
        u8 u8Len = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                             (u8)(u8Cap + 2));

        if (u8Id == PCI_CAP_ID_VNDR && u8Len >= 16) {
            u8 u8Type = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                  (u8)(u8Cap + 3));
            u8 u8Bar = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                 (u8)(u8Cap + 4));
            u32 u32Off = pci_read32(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                    (u8)(u8Cap + 8));
            volatile u8 *pBase = bar_ptr(pDev, u8Bar, u32Off);

            /* Skip caps whose BAR is missing/IO — keep prior fields intact */
            if (pBase == NULL) {
                /* fall through to next cap */
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_COMMON_CFG) {
                pDev->pCommon = pBase;
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_NOTIFY_CFG) {
                pDev->pNotify = pBase;
                if (u8Len >= 20) {
                    pDev->u32NotifyMult =
                        pci_read32(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                   (u8)(u8Cap + 16));
                } else {
                    pDev->u32NotifyMult = 0;
                }
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_ISR_CFG) {
                pDev->pIsr = pBase;
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_DEVICE_CFG) {
                pDev->pDevice = pBase;
            }
        }
        if (u8Next == 0 || u8Next == u8Cap) {
            break;
        }
        u8Cap = u8Next & 0xFCu;
    }

    if (pDev->pCommon == NULL) {
        kprintf("virtio: %x:%x no common cfg (legacy?)\n",
                (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot);
        return GJ_ERR_NOSUPPORT;
    }

    /* Soft device reset; wait until status reads zero */
    virtio_reset(pDev);
    pDev->u32NumQueues = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_NUMQ);
    pDev->fModern = 1;
    kprintf("virtio: %x:%x modern common@%p queues=%u notify_mult=%u"
            " isr=%u devcfg=%u\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (void *)pDev->pCommon, pDev->u32NumQueues,
            (unsigned)pDev->u32NotifyMult,
            pDev->pIsr != NULL ? 1u : 0u,
            pDev->pDevice != NULL ? 1u : 0u);
    return GJ_OK;
}

void
virtio_reset(struct gj_virtio_dev *pDev)
{
    u32 iSpin;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    mmio_w8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS, 0);
    for (iSpin = 0; iSpin < GJ_VIRTIO_RESET_SPINS; iSpin++) {
        if (mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS) == 0) {
            break;
        }
        __asm__ volatile ("pause");
    }
    pDev->u64FeaturesDev = 0;
    pDev->u64FeaturesDrv = 0;
    if (mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS) != 0) {
        kprintf("virtio: reset soft timeout status=0x%x\n",
                (unsigned)mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS));
    }
}

void
virtio_set_status(struct gj_virtio_dev *pDev, u8 u8Status)
{
    if (pDev && pDev->pCommon) {
        mmio_w8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS, u8Status);
    }
}

u8
virtio_get_status(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    return mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS);
}

void
virtio_driver_ok(struct gj_virtio_dev *pDev)
{
    u8 u8St;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    u8St = (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK);
    virtio_set_status(pDev, u8St);
    kprintf("virtio: driver_ok %x:%x status=0x%x features=0x%lx\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (unsigned)virtio_get_status(pDev),
            (unsigned long)pDev->u64FeaturesDrv);
}

u8
virtio_isr_read(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pIsr == NULL) {
        return 0;
    }
    return mmio_r8(pDev->pIsr);
}

u8
virtio_config_generation(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    return mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_CFGGEN);
}

u64
virtio_features_device(struct gj_virtio_dev *pDev)
{
    return common_features_read(pDev, 0);
}

u64
virtio_features_driver(struct gj_virtio_dev *pDev)
{
    return common_features_read(pDev, 1);
}

u64
virtio_features_negotiated(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL) {
        return 0;
    }
    return pDev->u64FeaturesDrv;
}

int
virtio_features_has(struct gj_virtio_dev *pDev, u64 u64Bit)
{
    if (pDev == NULL || u64Bit == 0) {
        return 0;
    }
    return (pDev->u64FeaturesDrv & u64Bit) != 0 ? 1 : 0;
}

gj_status_t
virtio_negotiate(struct gj_virtio_dev *pDev, u64 u64WantFeatures)
{
    u64 u64Dev;
    u64 u64Drv;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return GJ_ERR_INVAL;
    }

    /*
     * Soft reset first — OASIS requires reset before re-init; also makes
     * feature ladders (retry with smaller want masks) correct.
     */
    virtio_reset(pDev);

    virtio_set_status(pDev, GJ_VIRTIO_S_ACKNOWLEDGE);
    virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER));

    u64Dev = common_features_read(pDev, 0);
    u64Drv = u64Dev & u64WantFeatures;

    common_features_write_guest(pDev, u64Drv);

    virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                 GJ_VIRTIO_S_FEATURES_OK));
    if ((virtio_get_status(pDev) & GJ_VIRTIO_S_FEATURES_OK) == 0) {
        kprintf("virtio: FEATURES_OK rejected want=0x%lx dev=0x%lx\n",
                (unsigned long)u64WantFeatures, (unsigned long)u64Dev);
        virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                     GJ_VIRTIO_S_FAILED));
        pDev->u64FeaturesDev = u64Dev;
        pDev->u64FeaturesDrv = 0;
        return GJ_ERR_NOSUPPORT;
    }

    /* Soft snapshot from programmed guest features (read-back). */
    pDev->u64FeaturesDev = u64Dev;
    pDev->u64FeaturesDrv = common_features_read(pDev, 1);
    if (pDev->u64FeaturesDrv == 0) {
        pDev->u64FeaturesDrv = u64Drv; /* soft: some hosts omit GF read-back */
    }
    kprintf("virtio: features dev=0x%lx drv=0x%lx want=0x%lx v1=%u\n",
            (unsigned long)pDev->u64FeaturesDev,
            (unsigned long)pDev->u64FeaturesDrv,
            (unsigned long)u64WantFeatures,
            (unsigned)((pDev->u64FeaturesDrv & GJ_VIRTIO_F_VERSION_1) != 0));
    return GJ_OK;
}

gj_status_t
virtio_negotiate_soft(struct gj_virtio_dev *pDev, const u64 *pWants, u32 cWants,
                      u64 *pOutDrv)
{
    u32 i;
    gj_status_t st;
    u64 u64Last = 0;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return GJ_ERR_INVAL;
    }
    /* Soft default ladder when caller omits masks: V1 then transitional empty. */
    {
        static const u64 aDefault[] = {
            GJ_VIRTIO_F_VERSION_1,
            0
        };
        const u64 *pTry = pWants;
        u32 cTry = cWants;

        if (pTry == NULL || cTry == 0) {
            pTry = aDefault;
            cTry = 2;
        }

        for (i = 0; i < cTry; i++) {
            u64Last = pTry[i];
            st = virtio_negotiate(pDev, u64Last);
            if (st == GJ_OK) {
                kprintf("virtio: features soft step=%u/%u want=0x%lx drv=0x%lx\n",
                        (unsigned)(i + 1), (unsigned)cTry,
                        (unsigned long)u64Last,
                        (unsigned long)pDev->u64FeaturesDrv);
                if (pOutDrv != NULL) {
                    *pOutDrv = pDev->u64FeaturesDrv;
                }
                return GJ_OK;
            }
        }
        cWants = cTry;
    }
    kprintf("virtio: features soft FAIL steps=%u last_want=0x%lx\n",
            (unsigned)cWants, (unsigned long)u64Last);
    if (pOutDrv != NULL) {
        *pOutDrv = 0;
    }
    return GJ_ERR_NOSUPPORT;
}

u16
virtio_q_max_size(struct gj_virtio_dev *pDev, u16 u16QIdx)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    return mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE);
}

void
virtio_q_disable(struct gj_virtio_dev *pDev, u16 u16QIdx)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 0);
}

gj_status_t
virtio_q_setup(struct gj_virtio_dev *pDev, struct gj_virtq *pQ, u16 u16QIdx,
               u16 u16Size)
{
    u16 u16Max;
    u16 u16Want;
    u16 u16Enabled;
    gj_paddr_t paDesc;
    gj_paddr_t paAvail;
    gj_paddr_t paUsed;

    if (pDev == NULL || pQ == NULL || pDev->pCommon == NULL) {
        return GJ_ERR_INVAL;
    }
    if (u16Size == 0 || u16Size > GJ_VIRTQ_MAX_SIZE) {
        return GJ_ERR_INVAL;
    }
    if (pDev->u32NumQueues != 0 && (u32)u16QIdx >= pDev->u32NumQueues) {
        kprintf("virtio: q soft idx=%u beyond num_queues=%u\n",
                (unsigned)u16QIdx, pDev->u32NumQueues);
        return GJ_ERR_NOSUPPORT;
    }

    memset(pQ, 0, sizeof(*pQ));
    pQ->pDev = pDev;
    pQ->u16QueueIdx = u16QIdx;
    u16Want = u16Size;

    /* Soft: disable before reprogramming (safe re-setup / probe retry). */
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 0);

    u16Max = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE);
    if (u16Max == 0) {
        kprintf("virtio: q soft idx=%u max_size=0 (absent)\n",
                (unsigned)u16QIdx);
        return GJ_ERR_NOSUPPORT;
    }

    u16Size = q_soft_size(u16Want, u16Max);
    if (u16Size == 0 || u16Size > GJ_VIRTQ_MAX_SIZE) {
        /* Soft: also clamp to our ring struct limit */
        if (u16Max >= GJ_VIRTQ_MAX_SIZE) {
            u16Size = q_soft_size(GJ_VIRTQ_MAX_SIZE, GJ_VIRTQ_MAX_SIZE);
        } else {
            u16Size = q_soft_size(u16Max, u16Max);
        }
    }
    if (u16Size > GJ_VIRTQ_MAX_SIZE) {
        u16Size = q_soft_size(GJ_VIRTQ_MAX_SIZE, GJ_VIRTQ_MAX_SIZE);
    }
    if (u16Size == 0) {
        return GJ_ERR_NOSUPPORT;
    }
    if (u16Size != u16Want) {
        kprintf("virtio: q soft idx=%u size clamp want=%u -> %u (max=%u)\n",
                (unsigned)u16QIdx, (unsigned)u16Want, (unsigned)u16Size,
                (unsigned)u16Max);
    }

    pQ->u16Size = u16Size;
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE, u16Size);

    /*
     * Three dedicated pages: desc | avail | used (soft non-contiguous OK).
     * Free any partial alloc on soft failure.
     */
    paDesc = alloc_zero_page();
    paAvail = alloc_zero_page();
    paUsed = alloc_zero_page();
    if (paDesc == 0 || paAvail == 0 || paUsed == 0) {
        if (paDesc != 0) {
            pmm_free(paDesc);
        }
        if (paAvail != 0) {
            pmm_free(paAvail);
        }
        if (paUsed != 0) {
            pmm_free(paUsed);
        }
        kprintf("virtio: q soft idx=%u nomem\n", (unsigned)u16QIdx);
        return GJ_ERR_NOMEM;
    }
    pQ->paDesc = paDesc;
    pQ->pDesc = (struct gj_virtq_desc *)hhdm_to_virt(paDesc);
    pQ->paAvail = paAvail;
    pQ->pAvail = (struct gj_virtq_avail *)hhdm_to_virt(paAvail);
    pQ->paUsed = paUsed;
    pQ->pUsed = (struct gj_virtq_used *)hhdm_to_virt(paUsed);

    /* Free list of descriptors */
    pQ->u16NumFree = u16Size;
    pQ->u16FreeHead = 0;
    pQ->u16LastUsed = 0;
    {
        u16 i;

        for (i = 0; i < u16Size - 1; i++) {
            pQ->pDesc[i].u16Next = (u16)(i + 1);
        }
        pQ->pDesc[u16Size - 1].u16Next = 0xffff;
    }

    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DESC, pQ->paDesc);
    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DRIVER, pQ->paAvail);
    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DEVICE, pQ->paUsed);
    pQ->u16NotifyOff = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_NOFF);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 1);

    /* Soft enable verify */
    u16Enabled = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE);
    if (u16Enabled == 0) {
        kprintf("virtio: q soft idx=%u enable rejected\n", (unsigned)u16QIdx);
        q_ring_free(pQ);
        memset(pQ, 0, sizeof(*pQ));
        return GJ_ERR_IO;
    }

    kprintf("virtio: q%u size=%u max=%u desc=0x%lx notify_off=%u soft ok\n",
            (unsigned)u16QIdx, (unsigned)u16Size, (unsigned)u16Max,
            (unsigned long)pQ->paDesc, (unsigned)pQ->u16NotifyOff);
    return GJ_OK;
}

int
virtio_q_push_head(struct gj_virtq *pQ, u16 u16Head)
{
    u16 u16Aidx;

    if (pQ == NULL || pQ->pAvail == NULL) {
        return -1;
    }
    __asm__ volatile ("mfence" ::: "memory");
    u16Aidx = (u16)(pQ->pAvail->u16Idx % pQ->u16Size);
    pQ->pAvail->aRing[u16Aidx] = u16Head;
    __asm__ volatile ("mfence" ::: "memory");
    pQ->pAvail->u16Idx = (u16)(pQ->pAvail->u16Idx + 1);
    __asm__ volatile ("mfence" ::: "memory");
    return (int)u16Head;
}

int
virtio_q_alloc_desc(struct gj_virtq *pQ)
{
    u16 u16Head;

    if (pQ == NULL || pQ->u16NumFree == 0) {
        return -1;
    }
    u16Head = pQ->u16FreeHead;
    pQ->u16FreeHead = pQ->pDesc[u16Head].u16Next;
    pQ->u16NumFree--;
    return (int)u16Head;
}

int
virtio_q_add(struct gj_virtq *pQ, gj_paddr_t pa, u32 u32Len, int fWrite)
{
    int iHead;

    if (pQ == NULL || pa == 0 || u32Len == 0) {
        return -1;
    }
    iHead = virtio_q_alloc_desc(pQ);
    if (iHead < 0) {
        return -1;
    }
    pQ->pDesc[iHead].u64Addr = (u64)pa;
    pQ->pDesc[iHead].u32Len = u32Len;
    pQ->pDesc[iHead].u16Flags = fWrite ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[iHead].u16Next = 0;
    return virtio_q_push_head(pQ, (u16)iHead);
}

int
virtio_q_add2(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
              gj_paddr_t pa1, u32 u32Len1, int fWrite1)
{
    u16 u16A;
    u16 u16B;

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || u32Len0 == 0 || u32Len1 == 0) {
        return -1;
    }
    if (pQ->u16NumFree < 2) {
        return -1;
    }
    {
        int ia = virtio_q_alloc_desc(pQ);
        int ib = virtio_q_alloc_desc(pQ);

        if (ia < 0 || ib < 0) {
            return -1;
        }
        u16A = (u16)ia;
        u16B = (u16)ib;
    }
    pQ->pDesc[u16A].u64Addr = (u64)pa0;
    pQ->pDesc[u16A].u32Len = u32Len0;
    pQ->pDesc[u16A].u16Flags =
        (u16)((fWrite0 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16A].u16Next = u16B;

    pQ->pDesc[u16B].u64Addr = (u64)pa1;
    pQ->pDesc[u16B].u32Len = u32Len1;
    pQ->pDesc[u16B].u16Flags = fWrite1 ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[u16B].u16Next = 0;
    return virtio_q_push_head(pQ, u16A);
}

int
virtio_q_add3(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
              gj_paddr_t pa1, u32 u32Len1, int fWrite1,
              gj_paddr_t pa2, u32 u32Len2, int fWrite2)
{
    u16 u16A;
    u16 u16B;
    u16 u16C;

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || pa2 == 0 ||
        u32Len0 == 0 || u32Len1 == 0 || u32Len2 == 0) {
        return -1;
    }
    if (pQ->u16NumFree < 3) {
        return -1;
    }
    {
        int ia = virtio_q_alloc_desc(pQ);
        int ib = virtio_q_alloc_desc(pQ);
        int ic = virtio_q_alloc_desc(pQ);

        if (ia < 0 || ib < 0 || ic < 0) {
            return -1;
        }
        u16A = (u16)ia;
        u16B = (u16)ib;
        u16C = (u16)ic;
    }
    pQ->pDesc[u16A].u64Addr = (u64)pa0;
    pQ->pDesc[u16A].u32Len = u32Len0;
    pQ->pDesc[u16A].u16Flags =
        (u16)((fWrite0 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16A].u16Next = u16B;

    pQ->pDesc[u16B].u64Addr = (u64)pa1;
    pQ->pDesc[u16B].u32Len = u32Len1;
    pQ->pDesc[u16B].u16Flags =
        (u16)((fWrite1 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16B].u16Next = u16C;

    pQ->pDesc[u16C].u64Addr = (u64)pa2;
    pQ->pDesc[u16C].u32Len = u32Len2;
    pQ->pDesc[u16C].u16Flags = fWrite2 ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[u16C].u16Next = 0;
    return virtio_q_push_head(pQ, u16A);
}

void
virtio_q_kick(struct gj_virtq *pQ)
{
    volatile u8 *pNotify;
    u32 u32Off;

    if (pQ == NULL || pQ->pDev == NULL || pQ->pDev->pNotify == NULL) {
        return;
    }
    u32Off = (u32)pQ->u16NotifyOff * pQ->pDev->u32NotifyMult;
    pNotify = pQ->pDev->pNotify + u32Off;
    mmio_w16(pNotify, pQ->u16QueueIdx);
}

i32
virtio_q_poll_id(struct gj_virtq *pQ, u32 u32Spins, u32 *pOutId)
{
    u32 i;

    if (pQ == NULL) {
        return -1;
    }
    for (i = 0; i < u32Spins; i++) {
        __asm__ volatile ("mfence" ::: "memory");
        if (pQ->pUsed->u16Idx != pQ->u16LastUsed) {
            u16 u16Slot = (u16)(pQ->u16LastUsed % pQ->u16Size);
            u32 u32Id = pQ->pUsed->aRing[u16Slot].u32Id;
            u32 u32Len = pQ->pUsed->aRing[u16Slot].u32Len;

            pQ->u16LastUsed = (u16)(pQ->u16LastUsed + 1);
            /* free descriptor chain */
            {
                u16 u16Cur = (u16)u32Id;

                for (;;) {
                    u16 u16Next = pQ->pDesc[u16Cur].u16Next;
                    u16 u16Flags = pQ->pDesc[u16Cur].u16Flags;

                    pQ->pDesc[u16Cur].u16Next = pQ->u16FreeHead;
                    pQ->u16FreeHead = u16Cur;
                    pQ->u16NumFree++;
                    if ((u16Flags & GJ_VIRTQ_DESC_F_NEXT) == 0) {
                        break;
                    }
                    u16Cur = u16Next;
                }
            }
            if (pOutId != NULL) {
                *pOutId = u32Id;
            }
            return (i32)u32Len;
        }
        __asm__ volatile ("pause");
    }
    return -1;
}

i32
virtio_q_poll(struct gj_virtq *pQ, u32 u32Spins)
{
    return virtio_q_poll_id(pQ, u32Spins, NULL);
}

u16
virtio_q_num_free(struct gj_virtq *pQ)
{
    if (pQ == NULL) {
        return 0;
    }
    return pQ->u16NumFree;
}

u32
virtio_q_reap(struct gj_virtq *pQ, u32 u32Max)
{
    u32 n = 0;

    if (pQ == NULL || u32Max == 0) {
        return 0;
    }
    while (n < u32Max) {
        if (virtio_q_poll_id(pQ, 1, NULL) < 0) {
            break;
        }
        n++;
    }
    return n;
}
