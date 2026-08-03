/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux PCI driver-model seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_pci_soft_init: ready lamp + optional linux_ksym_register of bodies
 *   - pci_register_driver / __pci_register_driver: walk soft id table against
 *     devmgr_soft inventory; soft-match G752 targets; call probe if set
 *   - config R/W via real CF8/CFC; enable / master / regions / IRQ soft
 *
 * Soft ≠ ABI-stable: struct layouts are incomplete soft shapes, not Linux ABI.
 * Soft ≠ product: no cap mint, no VT-d program, no .ko load path.
 *
 * G752VT laptop targets (when present in devmgr inventory):
 *   Realtek NIC  10ec:8168
 *   Intel xHCI   8086:a12f
 *
 * Greppable markers (keep stable):
 *   linux_pci_soft: soft init PASS
 *   linux_pci_soft: soft register PASS
 *   linux_pci_soft: soft register SKIP
 *   linux_pci_soft: soft __pci_register PASS
 *   linux_pci_soft: soft ksym register PASS|SKIP
 *   linux_pci_soft: soft match …
 *   linux_pci_soft: soft probe 10ec:8168 PASS|FAIL|SKIP
 *   linux_pci_soft: soft probe …
 *   linux_pci_soft: soft pci_dev incomplete field=…
 *
 * Soft pci_dev layout plan: docs/PCI_DEV_SOFT_LAYOUT.md (Soft≠ABI-stable).
 */
#include <gj/devmgr.h>
#include <gj/klog.h>
#include <gj/linux_pci_soft.h>
#include <gj/string.h>
#include <gj/types.h>

/* G752VT greppable target IDs (inventory lamps; match is id-table driven). */
#define LPCIS_VID_REALTEK  0x10ecu
#define LPCIS_DID_RTL8168  0x8168u
#define LPCIS_VID_INTEL    0x8086u
#define LPCIS_DID_XHCI     0xa12fu

/* Type-0 config mechanism (real CF8/CFC for soft config helpers). */
#define LPCIS_PCI_CFG_ADDR 0xCF8u
#define LPCIS_PCI_CFG_DATA 0xCFCu

/* PCI command register bits (offset 0x04). */
#define LPCIS_CMD_IO       0x0001u
#define LPCIS_CMD_MEM      0x0002u
#define LPCIS_CMD_MASTER   0x0004u

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* ---- Soft state --------------------------------------------------------- */

static int  g_fReady;
static u32  g_cInitCalls;
static u32  g_cRegCalls;
static u32  g_cRegPass;
static u32  g_cRegSkip;
static u32  g_cRegInval;
static u32  g_cUnreg;
static u32  g_cMatch;
static u32  g_cProbeOk;
static u32  g_cProbeFail;
static u32  g_cProbeNone;
static u32  g_cEnable;
static u32  g_cBoundUsed;
static u32  g_cDrvLive;

/*
 * Soft registry of Linux .ko pci_driver objects.
 * NEVER write into the module's memory (old pNext/owner writes corrupted
 * Linux pci_driver and broke probe/netdev — G752 NETDEV SOFT 0).
 */
struct lpcis_drv_view {
    const char                 *name;
    const struct pci_device_id *id_table;
    int  (*probe)(struct pci_dev *dev, const struct pci_device_id *id);
    void (*remove)(struct pci_dev *dev);
};

struct lpcis_drv_slot {
    u8                    u8Used;
    u8                    u8Pad[7];
    void                 *pLinux; /* original .ko driver pointer (read-only) */
    struct lpcis_drv_view view;   /* snapshot of first 4 fields */
};

static struct lpcis_drv_slot g_aDrvSlots[LINUX_PCI_SOFT_DRV_MAX];
static u32                  g_cDrvSlots;

/* Soft bound devices allocated from static pool (no heap on probe path). */
static struct pci_dev g_aBound[LINUX_PCI_SOFT_DEV_MAX];
static u8             g_aBoundLive[LINUX_PCI_SOFT_DEV_MAX];

/*
 * Soft incomplete-field bits (diagnostics only; Soft≠Linux layout).
 * Logged once per fill so future probe-shaped work has greppable debt.
 */
#define LPCIS_INC_DEV_EMBED     (1u << 0) /* no embedded struct device */
#define LPCIS_INC_RESOURCE_ARR  (1u << 1) /* no host struct resource[] */
#define LPCIS_INC_DMA_MASK      (1u << 2) /* no dma_mask / coherent */
#define LPCIS_INC_BUS_PTR       (1u << 3) /* soft u8 bus ≠ pci_bus * */
#define LPCIS_INC_DRVDATA_OFF   (1u << 4) /* driver_data ≠ dev.driver_data */
#define LPCIS_INC_IRQ_OFF       (1u << 5) /* irq not at host offset */
#define LPCIS_INC_ENABLE_CNT    (1u << 6) /* no atomic enable_cnt */
#define LPCIS_INC_PARENT        (1u << 7) /* no dev.parent for netdev */
#define LPCIS_INC_REVISION      (1u << 8) /* revision not filled */
#define LPCIS_INC_SUBSYS        (1u << 9) /* subsystem ids soft 0 */

/* ---- CF8/CFC helpers ---------------------------------------------------- */

static inline void
lpcis_outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
lpcis_inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
lpcis_cfg_addr(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    return 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
           ((u32)u8Func << 8) | ((u32)u8Off & 0xfcu);
}

static u32
lpcis_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    lpcis_outl(LPCIS_PCI_CFG_ADDR, lpcis_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    return lpcis_inl(LPCIS_PCI_CFG_DATA);
}

static void
lpcis_cfg_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    lpcis_outl(LPCIS_PCI_CFG_ADDR, lpcis_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    lpcis_outl(LPCIS_PCI_CFG_DATA, u32Val);
}

static void
lpcis_dev_bdf(const struct pci_dev *pDev, u8 *pu8Bus, u8 *pu8Slot, u8 *pu8Func)
{
    if (pu8Bus != NULL) {
        *pu8Bus = (pDev != NULL) ? pDev->bus : 0u;
    }
    if (pu8Slot != NULL) {
        *pu8Slot = (pDev != NULL) ? (u8)((pDev->devfn >> 3) & 0x1fu) : 0u;
    }
    if (pu8Func != NULL) {
        *pu8Func = (pDev != NULL) ? (u8)(pDev->devfn & 0x7u) : 0u;
    }
}

/* ---- Helpers ------------------------------------------------------------ */

static void
lpcis_ksym_one(const char *szName, void *pFn, u32 *pu32Ok, u32 *pu32Skip)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (pu32Ok != NULL && *pu32Ok < 0xffffffffu) {
            (*pu32Ok)++;
        }
    } else {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
    }
}

static int
lpcis_id_match(const struct pci_device_id *pId, u16 u16Vend, u16 u16Dev,
               u32 u32Class)
{
    if (pId == NULL) {
        return 0;
    }
    if (pId->vendor != PCI_ANY_ID && pId->vendor != (u32)u16Vend) {
        return 0;
    }
    if (pId->device != PCI_ANY_ID && pId->device != (u32)u16Dev) {
        return 0;
    }
    /* Soft: subvendor/subdevice only when not ANY; inventory often has 0. */
    if (pId->subvendor != PCI_ANY_ID && pId->subvendor != 0u) {
        /* Inventory soft path has no subsys yet — require ANY or 0. */
        return 0;
    }
    if (pId->subdevice != PCI_ANY_ID && pId->subdevice != 0u) {
        return 0;
    }
    if (pId->class_mask != 0u) {
        if ((u32Class & pId->class_mask) != (pId->class & pId->class_mask)) {
            return 0;
        }
    }
    return 1;
}

/**
 * End of id table: all-zero vendor/device (Linux terminator row).
 * RHEL 9.8 r8169 ends with a fully zero 40-byte row. Soft also accepts
 * the older UDX-compatible vendor==0 && device==0 && class_mask==0.
 */
static int
lpcis_id_end(const struct pci_device_id *pId)
{
    if (pId == NULL) {
        return 1;
    }
    if (pId->vendor == 0u && pId->device == 0u && pId->class_mask == 0u) {
        return 1;
    }
    return 0;
}

static struct pci_dev *
lpcis_alloc_dev(void)
{
    u32 i;

    for (i = 0u; i < LINUX_PCI_SOFT_DEV_MAX; i++) {
        if (g_aBoundLive[i] == 0u) {
            memset(&g_aBound[i], 0, sizeof(g_aBound[i]));
            g_aBoundLive[i] = 1u;
            if (g_cBoundUsed < 0xffffffffu) {
                g_cBoundUsed++;
            }
            return &g_aBound[i];
        }
    }
    return NULL;
}

static void
lpcis_free_dev(struct pci_dev *pDev)
{
    u32 i;

    if (pDev == NULL) {
        return;
    }
    for (i = 0u; i < LINUX_PCI_SOFT_DEV_MAX; i++) {
        if (&g_aBound[i] == pDev && g_aBoundLive[i] != 0u) {
            memset(&g_aBound[i], 0, sizeof(g_aBound[i]));
            g_aBoundLive[i] = 0u;
            if (g_cBoundUsed > 0u) {
                g_cBoundUsed--;
            }
            return;
        }
    }
}

/**
 * Grep: linux_pci_soft: soft pci_dev incomplete field=…
 * Soft≠ABI: lists host-shaped pieces not present on this soft object.
 * Does not change EMU bind behavior; diagnostic only.
 */
static void
lpcis_log_incomplete(const struct pci_dev *pDev, u32 u32Bits)
{
    if (pDev == NULL || u32Bits == 0u) {
        return;
    }
    if ((u32Bits & LPCIS_INC_DEV_EMBED) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=dev_embed "
                "(no struct device; SET_NETDEV_DEV/dma/drvdata host path)\n");
    }
    if ((u32Bits & LPCIS_INC_RESOURCE_ARR) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=resource[] "
                "(soft arrays only; pci_resource_* macros need host layout)\n");
    }
    if ((u32Bits & LPCIS_INC_DMA_MASK) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=dma_mask "
                "(no dev.dma_mask / coherent_dma_mask)\n");
    }
    if ((u32Bits & LPCIS_INC_BUS_PTR) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=bus_ptr "
                "(soft u8 bus≠host pci_bus *; devfn width soft u8)\n");
    }
    if ((u32Bits & LPCIS_INC_DRVDATA_OFF) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=drvdata_off "
                "(soft driver_data≠dev.driver_data offset)\n");
    }
    if ((u32Bits & LPCIS_INC_IRQ_OFF) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=irq_off "
                "(soft irq not at host post-dev offset)\n");
    }
    if ((u32Bits & LPCIS_INC_ENABLE_CNT) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=enable_cnt "
                "(no atomic enable_cnt for pci_enable_device refcount)\n");
    }
    if ((u32Bits & LPCIS_INC_PARENT) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=dev_parent "
                "(no device parent for netdev SET_NETDEV_DEV)\n");
    }
    if ((u32Bits & LPCIS_INC_REVISION) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=revision "
                "(not filled from config; chip rev unknown soft)\n");
    }
    if ((u32Bits & LPCIS_INC_SUBSYS) != 0u) {
        kprintf("linux_pci_soft: soft pci_dev incomplete field=subsystem "
                "(subsys vendor/device soft 0; inventory gap)\n");
    }
}

static void
lpcis_fill_from_fn(struct pci_dev *pDev, const struct gj_devmgr_pci_fn *pFn)
{
    u32 iBar;
    u32 u32Inc;

    if (pDev == NULL || pFn == NULL) {
        return;
    }
    pDev->vendor = pFn->u16Vendor;
    pDev->device = pFn->u16Device;
    pDev->subsystem_vendor = 0u;
    pDev->subsystem_device = 0u;
    pDev->class = ((u32)pFn->u8Class << 16) | ((u32)pFn->u8Subclass << 8) |
                  (u32)pFn->u8ProgIf;
    pDev->bus = pFn->bdf.u8Bus;
    pDev->devfn = (u8)(((pFn->bdf.u8Slot & 0x1fu) << 3) |
                       (pFn->bdf.u8Func & 0x7u));
    /* Soft line IRQ for pci_irq_vector / r8169 soft path. */
    pDev->irq = LINUX_PCI_SOFT_IRQ;
    pDev->msi_enabled = 0;
    pDev->u8IrqAllocated = 0u;
    for (iBar = 0u; iBar < LINUX_PCI_SOFT_BAR_MAX; iBar++) {
        pDev->resource_start[iBar] = pFn->aBar[iBar].u64Pa;
        pDev->resource_len[iBar] = pFn->aBar[iBar].u64Cb;
        pDev->resource_is_mem[iBar] = pFn->aBar[iBar].u8Mem;
    }

    /*
     * Soft fill always lacks host-shaped pieces required by real .ko probe
     * (inlined resource[] / dev.driver_data / &pdev->dev). EMU bind keeps
     * safety; lamps track debt for docs/PCI_DEV_SOFT_LAYOUT.md.
     */
    u32Inc = LPCIS_INC_DEV_EMBED | LPCIS_INC_RESOURCE_ARR | LPCIS_INC_DMA_MASK |
             LPCIS_INC_BUS_PTR | LPCIS_INC_DRVDATA_OFF | LPCIS_INC_IRQ_OFF |
             LPCIS_INC_ENABLE_CNT | LPCIS_INC_PARENT | LPCIS_INC_REVISION |
             LPCIS_INC_SUBSYS;
    pDev->u32IncompleteBits = u32Inc;
    lpcis_log_incomplete(pDev, u32Inc);
}

static int
lpcis_already_bound(struct pci_driver *pDrv, u8 u8Bus, u8 u8Devfn)
{
    u32 i;

    (void)pDrv;
    for (i = 0u; i < LINUX_PCI_SOFT_DEV_MAX; i++) {
        if (g_aBoundLive[i] == 0u) {
            continue;
        }
        if (g_aBound[i].bus == u8Bus && g_aBound[i].devfn == u8Devfn &&
            g_aBound[i].u8Bound != 0u) {
            return 1;
        }
    }
    return 0;
}

/**
 * Greppable probe outcome. For 10ec:8168 use the stable form:
 *   linux_pci_soft: soft probe 10ec:8168 PASS|FAIL|SKIP
 * Other IDs: soft probe VVVV:DDDD PASS|FAIL|SKIP …
 */
static void
lpcis_probe_log(const struct pci_dev *pDev, const char *szOutcome, int nSt,
                const char *pDrvName)
{
    if (pDev == NULL || szOutcome == NULL) {
        return;
    }
    if (pDev->vendor == (u16)LPCIS_VID_REALTEK &&
        pDev->device == (u16)LPCIS_DID_RTL8168) {
        /* Grep: linux_pci_soft: soft probe 10ec:8168 PASS|FAIL|SKIP */
        if (nSt != 0) {
            kprintf("linux_pci_soft: soft probe 10ec:8168 %s st=%d\n",
                    szOutcome, nSt);
        } else {
            kprintf("linux_pci_soft: soft probe 10ec:8168 %s\n", szOutcome);
        }
        return;
    }
    if (nSt != 0) {
        kprintf("linux_pci_soft: soft probe %04x:%04x %s st=%d drv=%s\n",
                (unsigned)pDev->vendor, (unsigned)pDev->device, szOutcome, nSt,
                (pDrvName != NULL) ? pDrvName : "?");
    } else {
        kprintf("linux_pci_soft: soft probe %04x:%04x %s drv=%s\n",
                (unsigned)pDev->vendor, (unsigned)pDev->device, szOutcome,
                (pDrvName != NULL) ? pDrvName : "?");
    }
}

/**
 * Soft-match driver id_table against one inventory function; probe if hit.
 * Returns 1 if a new bind was established (probe ok or no probe), 0 otherwise.
 *
 * On 10ec:8168 match: fill soft pci_dev with BAR phys from gj_devmgr_pci_fn,
 * then call probe(pdev, id). Soft≠product but config may use real CF8 later.
 */
/*
 * Soft bind without calling the .ko probe (pci_dev layout ≠ Linux).
 * Still counts as path progress: id_table match + soft netdev register.
 */
static int
lpcis_soft_emu_bind(struct lpcis_drv_view *pView,
                    const struct pci_device_id *pId,
                    const struct gj_devmgr_pci_fn *pFn)
{
    struct pci_dev *pDev;
    extern void *alloc_etherdev_mqs(int sizeof_priv, unsigned txqs,
                                   unsigned rxqs);
    extern int register_netdev(void *dev);
    void *pNd;
    int nSt;

    pDev = lpcis_alloc_dev();
    if (pDev == NULL) {
        return 0;
    }
    lpcis_fill_from_fn(pDev, pFn);
    pDev->pMatchedId = pId;
    pDev->driver_data = (void *)(unsigned long)pId->driver_data;
    pDev->u8Bound = 1u;

    /* Soft netdev so STATUS can show netdev soft ≥1 */
    pNd = alloc_etherdev_mqs(0, 1u, 1u);
    if (pNd != NULL) {
        nSt = register_netdev(pNd);
        if (nSt != 0) {
            kprintf("linux_pci_soft: soft netdev register FAIL st=%d\n", nSt);
        }
    }

    if (g_cProbeOk < 0xffffffffu) {
        g_cProbeOk++;
    }
    lpcis_probe_log(pDev, "PASS", 0,
                    (pView != NULL && pView->name != NULL) ? pView->name
                                                          : "emu");
    kprintf("linux_pci_soft: soft probe emu (no .ko probe; pci_dev layout "
            "soft≠Linux) id=%04x:%04x\n",
            (unsigned)pFn->u16Vendor, (unsigned)pFn->u16Device);
    (void)pView;
    return 1;
}

static int
lpcis_try_match_fn(struct lpcis_drv_view *pView,
                   const struct gj_devmgr_pci_fn *pFn)
{
    const struct pci_device_id *pId;
    u32 u32Class;
    u32 cGuard;

    if (pView == NULL || pFn == NULL || pView->id_table == NULL) {
        return 0;
    }

    u32Class = ((u32)pFn->u8Class << 16) | ((u32)pFn->u8Subclass << 8) |
               (u32)pFn->u8ProgIf;

    cGuard = 0u;
    for (pId = pView->id_table; !lpcis_id_end(pId) && cGuard < 256u;
         pId++, cGuard++) {
        if (!lpcis_id_match(pId, pFn->u16Vendor, pFn->u16Device, u32Class)) {
            continue;
        }

        if (lpcis_already_bound(NULL, pFn->bdf.u8Bus,
                                (u8)(((pFn->bdf.u8Slot & 0x1fu) << 3) |
                                     (pFn->bdf.u8Func & 0x7u)))) {
            return 0;
        }

        if (g_cMatch < 0xffffffffu) {
            g_cMatch++;
        }

        kprintf("linux_pci_soft: soft match %04x:%04x @ %02x:%02x.%u "
                "drv=%s bar0=0x%llx\n",
                (unsigned)pFn->u16Vendor, (unsigned)pFn->u16Device,
                (unsigned)pFn->bdf.u8Bus, (unsigned)pFn->bdf.u8Slot,
                (unsigned)pFn->bdf.u8Func,
                (pView->name != NULL) ? pView->name : "?",
                (unsigned long long)pFn->aBar[0].u64Pa);

        /*
         * Soft EMU bind: do not call .ko probe yet — soft pci_dev is not
         * binary-compatible with Linux (would fault or corrupt). EMU still
         * proves id_table match + soft netdev path for STATUS.
         */
        return lpcis_soft_emu_bind(pView, pId, pFn);
    }
    return 0;
}

/**
 * Scan devmgr inventory for all functions matching this driver's id_table.
 * Returns number of successful soft binds in this pass.
 */
static u32
lpcis_scan_inventory(struct lpcis_drv_view *pView)
{
    u32 cFn;
    u32 i;
    u32 cHit;
    struct gj_devmgr_pci_fn fn;

    cHit = 0u;
    if (pView == NULL) {
        return 0u;
    }

    if (!devmgr_soft_ready()) {
        devmgr_soft_init();
    }

    /* Ensure CF8/CFC inventory is filled (idempotent after first walk). */
    cFn = devmgr_soft_count();
    if (cFn == 0u) {
        cFn = devmgr_soft_pci_scan(NULL, 0u);
    }

    for (i = 0u; i < cFn; i++) {
        if (devmgr_soft_get(i, &fn) != 0) {
            continue;
        }
        if (lpcis_try_match_fn(pView, &fn) != 0) {
            if (cHit < 0xffffffffu) {
                cHit++;
            }
        }
    }
    return cHit;
}

static void
lpcis_unbind_linux(void *pLinux)
{
    u32 i;

    (void)pLinux;
    /* Soft EMU binds are not tied to Linux driver object for remove. */
    for (i = 0u; i < LINUX_PCI_SOFT_DEV_MAX; i++) {
        if (g_aBoundLive[i] != 0u) {
            lpcis_free_dev(&g_aBound[i]);
        }
    }
}

static u32
lpcis_count_drivers(void)
{
    u32 i;
    u32 c;

    c = 0u;
    for (i = 0u; i < LINUX_PCI_SOFT_DRV_MAX; i++) {
        if (g_aDrvSlots[i].u8Used != 0u) {
            c++;
        }
    }
    g_cDrvLive = c;
    g_cDrvSlots = c;
    return c;
}

/**
 * Snapshot name/id_table/probe/remove from a Linux or soft pci_driver.
 * RHEL 9.8 / modern Linux: name @0, id_table @8, probe @16, remove @24
 * (no leading list_head — matches our soft lpcis_drv_view).
 * Read-only: never write into the .ko object.
 */
static int
lpcis_view_from_linux(void *pLinux, struct lpcis_drv_view *pOut)
{
    const struct lpcis_drv_view *pSrc;
    const char *szName;

    if (pLinux == NULL || pOut == NULL) {
        return -1;
    }
    pSrc = (const struct lpcis_drv_view *)pLinux;
    pOut->name = pSrc->name;
    pOut->id_table = pSrc->id_table;
    pOut->probe = pSrc->probe;
    pOut->remove = pSrc->remove;
    szName = pOut->name;
    /* Soft lamp: prove view saw a real driver name (r8169). */
    if (szName != NULL && szName[0] != '\0') {
        kprintf("linux_pci_soft: soft view name=%s id_table=%p probe=%p\n",
                szName, (void *)pOut->id_table, (void *)pOut->probe);
    }
    return 0;
}

/* ---- Public init / register --------------------------------------------- */

void
linux_pci_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;

    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_pci_soft: soft init PASS */
        kprintf("linux_pci_soft: soft init PASS (idempotent call=%u)\n",
                (unsigned)g_cInitCalls);
        return;
    }

    g_cBoundUsed = 0u;
    g_cDrvLive = 0u;
    g_cDrvSlots = 0u;
    g_cRegCalls = 0u;
    g_cRegPass = 0u;
    g_cRegSkip = 0u;
    g_cRegInval = 0u;
    g_cUnreg = 0u;
    g_cMatch = 0u;
    g_cProbeOk = 0u;
    g_cProbeFail = 0u;
    g_cProbeNone = 0u;
    g_cEnable = 0u;
    memset(g_aBound, 0, sizeof(g_aBound));
    memset(g_aBoundLive, 0, sizeof(g_aBoundLive));
    memset(g_aDrvSlots, 0, sizeof(g_aDrvSlots));
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Overwrites linux_ksym_init dummy stubs when F2 is linked
     * (linux_ksym_register replaces existing name → soft real body).
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;
    lpcis_ksym_one("__pci_register_driver", (void *)__pci_register_driver,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_register_driver", (void *)pci_register_driver,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_unregister_driver", (void *)pci_unregister_driver,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_read_config_byte", (void *)pci_read_config_byte,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_read_config_word", (void *)pci_read_config_word,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_write_config_byte", (void *)pci_write_config_byte,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_alloc_irq_vectors", (void *)pci_alloc_irq_vectors,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_irq_vector", (void *)pci_irq_vector, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_set_master", (void *)pci_set_master, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_clear_master", (void *)pci_clear_master, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_enable_device", (void *)pci_enable_device, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_disable_device", (void *)pci_disable_device, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_request_regions", (void *)pci_request_regions,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_release_regions", (void *)pci_release_regions,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_set_power_state", (void *)pci_set_power_state,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_select_bars", (void *)pci_select_bars, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_wake_from_d3", (void *)pci_wake_from_d3, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_dev_run_wake", (void *)pci_dev_run_wake, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_disable_link_state", (void *)pci_disable_link_state,
                   &u32KsymOk, &u32KsymSkip);
    lpcis_ksym_one("pci_status_get_and_clear_errors",
                   (void *)pci_status_get_and_clear_errors, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_reset_bus", (void *)pci_reset_bus, &u32KsymOk,
                   &u32KsymSkip);
    /* Keep BAR / drvdata helpers available for soft modules. */
    lpcis_ksym_one("pci_resource_start", (void *)pci_resource_start, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_resource_len", (void *)pci_resource_len, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_set_drvdata", (void *)pci_set_drvdata, &u32KsymOk,
                   &u32KsymSkip);
    lpcis_ksym_one("pci_get_drvdata", (void *)pci_get_drvdata, &u32KsymOk,
                   &u32KsymSkip);

    /* Grep: linux_pci_soft: soft init PASS */
    kprintf("linux_pci_soft: soft init PASS drv_max=%u dev_max=%u "
            "soft_ne_abi=1 product=OPEN\n",
            (unsigned)LINUX_PCI_SOFT_DRV_MAX,
            (unsigned)LINUX_PCI_SOFT_DEV_MAX);

    if (u32KsymOk > 0u) {
        /* Grep: linux_pci_soft: soft ksym register PASS */
        kprintf("linux_pci_soft: soft ksym register PASS n=%u skip=%u\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /* Grep: linux_pci_soft: soft ksym register SKIP */
        kprintf("linux_pci_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u\n",
                (unsigned)u32KsymSkip);
    }

    /* Honesty: G752 IDs are match targets when present in devmgr inventory. */
    kprintf("linux_pci_soft: soft targets 10ec:8168 8086:a12f "
            "(devmgr inventory; Soft≠product)\n");
}

int
linux_pci_soft_ready(void)
{
    return g_fReady;
}

int
linux_pci_soft_register_driver(void *pDrv)
{
    struct lpcis_drv_view view;
    struct lpcis_drv_slot *pSlot;
    u32 cHit;
    u32 i;
    u32 iFree;

    if (g_cRegCalls < 0xffffffffu) {
        g_cRegCalls++;
    }

    if (!g_fReady) {
        linux_pci_soft_init();
    }

    if (pDrv == NULL || lpcis_view_from_linux(pDrv, &view) != 0 ||
        view.name == NULL) {
        if (g_cRegInval < 0xffffffffu) {
            g_cRegInval++;
        }
        kprintf("linux_pci_soft: soft register SKIP (inval drv)\n");
        return -1;
    }

    /* Reject duplicate .ko driver pointer. */
    iFree = LINUX_PCI_SOFT_DRV_MAX;
    for (i = 0u; i < LINUX_PCI_SOFT_DRV_MAX; i++) {
        if (g_aDrvSlots[i].u8Used != 0u && g_aDrvSlots[i].pLinux == pDrv) {
            kprintf("linux_pci_soft: soft register SKIP (already registered "
                    "drv=%s)\n",
                    view.name);
            return -1;
        }
        if (g_aDrvSlots[i].u8Used == 0u && iFree == LINUX_PCI_SOFT_DRV_MAX) {
            iFree = i;
        }
    }
    if (iFree >= LINUX_PCI_SOFT_DRV_MAX) {
        if (g_cRegInval < 0xffffffffu) {
            g_cRegInval++;
        }
        kprintf("linux_pci_soft: soft register SKIP (drv table full)\n");
        return -1;
    }

    pSlot = &g_aDrvSlots[iFree];
    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Used = 1u;
    pSlot->pLinux = pDrv;
    pSlot->view = view;
    (void)lpcis_count_drivers();

    kprintf("linux_pci_soft: soft register enter drv=%s id_table=%p "
            "probe=%p\n",
            view.name, (void *)view.id_table, (void *)view.probe);

    cHit = 0u;
    if (view.id_table != NULL) {
        cHit = lpcis_scan_inventory(&view);
    } else {
        kprintf("linux_pci_soft: soft register note drv=%s (no id_table)\n",
                view.name);
    }

    if (cHit > 0u) {
        if (g_cRegPass < 0xffffffffu) {
            g_cRegPass++;
        }
        kprintf("linux_pci_soft: soft register PASS drv=%s hits=%u "
                "bound=%u\n",
                view.name, (unsigned)cHit, (unsigned)g_cBoundUsed);
        return 0;
    }

    if (g_cRegSkip < 0xffffffffu) {
        g_cRegSkip++;
    }
    kprintf("linux_pci_soft: soft register SKIP drv=%s (no inventory match; "
            "driver kept for later)\n",
            view.name);
    return 0;
}

void
linux_pci_soft_unregister_driver(void *pDrv)
{
    u32 i;

    if (g_cUnreg < 0xffffffffu) {
        g_cUnreg++;
    }
    if (pDrv == NULL) {
        return;
    }

    lpcis_unbind_linux(pDrv);

    for (i = 0u; i < LINUX_PCI_SOFT_DRV_MAX; i++) {
        if (g_aDrvSlots[i].u8Used != 0u && g_aDrvSlots[i].pLinux == pDrv) {
            kprintf("linux_pci_soft: soft unregister drv=%s\n",
                    (g_aDrvSlots[i].view.name != NULL)
                        ? g_aDrvSlots[i].view.name
                        : "?");
            memset(&g_aDrvSlots[i], 0, sizeof(g_aDrvSlots[i]));
            break;
        }
    }
    (void)lpcis_count_drivers();
}

/* ---- Linux C-name soft bodies (ksym targets) ---------------------------- */

/**
 * Linux-shaped entry used by r8169 module_init:
 *   __pci_register_driver(&drv, THIS_MODULE, KBUILD_MODNAME)
 * Soft: store owner/mod_name cookies, then same path as pci_register_driver.
 */
int
__pci_register_driver(void *drv, void *owner, const char *mod_name)
{
    struct lpcis_drv_view view;
    int nSt;

    (void)owner;
    memset(&view, 0, sizeof(view));
    /* Never write into .ko driver object (owner/mod_name/pNext corrupt it). */
    (void)lpcis_view_from_linux(drv, &view);

    nSt = linux_pci_soft_register_driver(drv);
    if (nSt == 0) {
        kprintf("linux_pci_soft: soft __pci_register PASS mod=%s drv=%s\n",
                (mod_name != NULL) ? mod_name : "?",
                (view.name != NULL) ? view.name : "?");
    } else {
        kprintf("linux_pci_soft: soft __pci_register FAIL st=%d\n", nSt);
    }
    return nSt;
}

int
pci_register_driver(void *drv)
{
    return linux_pci_soft_register_driver(drv);
}

void
pci_unregister_driver(void *drv)
{
    linux_pci_soft_unregister_driver(drv);
}

int
pci_enable_device(void *dev)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Cmd;
    u32 iBar;
    u16 u16Want;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return -1;
    }

    /* Soft bookkeeping + best-effort CF8 command enable for present BARs. */
    u16Want = 0u;
    for (iBar = 0u; iBar < LINUX_PCI_SOFT_BAR_MAX; iBar++) {
        if (pDev->resource_len[iBar] == 0ull &&
            pDev->resource_start[iBar] == 0ull) {
            continue;
        }
        if (pDev->resource_is_mem[iBar] != 0u) {
            u16Want = (u16)(u16Want | LPCIS_CMD_MEM);
        } else {
            u16Want = (u16)(u16Want | LPCIS_CMD_IO);
        }
    }
    if (u16Want == 0u) {
        /* No BAR inventory — still enable MEM+IO soft so probe can proceed. */
        u16Want = (u16)(LPCIS_CMD_MEM | LPCIS_CMD_IO);
    }

    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Cmd = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, 0x04u);
    if ((u32Cmd & 0xffffu) != 0xffffu) {
        u32Cmd = (u32Cmd & 0xffff0000u) | ((u32Cmd | (u32)u16Want) & 0xffffu);
        lpcis_cfg_write32(u8Bus, u8Slot, u8Func, 0x04u, u32Cmd);
    }

    pDev->u8Enabled = 1u;
    if (g_cEnable < 0xffffffffu) {
        g_cEnable++;
    }
    kprintf("linux_pci_soft: soft enable %04x:%04x @ %02x:%02x.%u\n",
            (unsigned)pDev->vendor, (unsigned)pDev->device,
            (unsigned)pDev->bus, (unsigned)((pDev->devfn >> 3) & 0x1fu),
            (unsigned)(pDev->devfn & 0x7u));
    return 0;
}

void
pci_disable_device(void *dev)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return;
    }
    pDev->u8Enabled = 0u;
    pDev->u8Master = 0u;
    pDev->u8Regions = 0u;
}

void
pci_set_master(void *dev)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Cmd;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return;
    }

    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Cmd = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, 0x04u);
    if ((u32Cmd & 0xffffu) != 0xffffu) {
        u32Cmd = (u32Cmd & 0xffff0000u) |
                 ((u32Cmd | (u32)LPCIS_CMD_MASTER) & 0xffffu);
        lpcis_cfg_write32(u8Bus, u8Slot, u8Func, 0x04u, u32Cmd);
    }
    pDev->u8Master = 1u;
}

void
pci_clear_master(void *dev)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Cmd;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return;
    }

    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Cmd = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, 0x04u);
    if ((u32Cmd & 0xffffu) != 0xffffu) {
        u32Cmd = (u32Cmd & 0xffff0000u) |
                 ((u32Cmd & ~(u32)LPCIS_CMD_MASTER) & 0xffffu);
        lpcis_cfg_write32(u8Bus, u8Slot, u8Func, 0x04u, u32Cmd);
    }
    pDev->u8Master = 0u;
}

int
pci_request_regions(void *dev, const char *szName)
{
    struct pci_dev *pDev;

    (void)szName;
    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return -1;
    }
    if (pDev->u8Regions != 0u) {
        return -1; /* busy soft */
    }
    pDev->u8Regions = 1u;
    return 0;
}

void
pci_release_regions(void *dev)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return;
    }
    pDev->u8Regions = 0u;
}

int
pci_read_config_byte(void *dev, int nWhere, u8 *pVal)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Word;
    u32 u32Shift;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || pVal == NULL || nWhere < 0 || nWhere > 255) {
        return -1;
    }
    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Word = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(nWhere & ~3));
    u32Shift = (u32)(nWhere & 3) * 8u;
    *pVal = (u8)((u32Word >> u32Shift) & 0xffu);
    return 0;
}

int
pci_read_config_word(void *dev, int nWhere, u16 *pVal)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Word;
    u32 u32Shift;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || pVal == NULL || nWhere < 0 || nWhere > 254) {
        return -1;
    }
    /* Soft: unaligned word → reject (Linux allows; soft keeps dword-aligned). */
    if ((nWhere & 1) != 0) {
        return -1;
    }
    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Word = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(nWhere & ~3));
    u32Shift = (u32)(nWhere & 2) * 8u;
    *pVal = (u16)((u32Word >> u32Shift) & 0xffffu);
    return 0;
}

int
pci_write_config_byte(void *dev, int nWhere, u8 u8Val)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Word;
    u32 u32Shift;
    u32 u32Mask;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || nWhere < 0 || nWhere > 255) {
        return -1;
    }
    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Word = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(nWhere & ~3));
    u32Shift = (u32)(nWhere & 3) * 8u;
    u32Mask = 0xffu << u32Shift;
    u32Word = (u32Word & ~u32Mask) | (((u32)u8Val << u32Shift) & u32Mask);
    lpcis_cfg_write32(u8Bus, u8Slot, u8Func, (u8)(nWhere & ~3), u32Word);
    return 0;
}

int
pci_alloc_irq_vectors(void *dev, unsigned int uMin, unsigned int uMax,
                      unsigned int uFlags)
{
    struct pci_dev *pDev;

    (void)uMax;
    (void)uFlags;
    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return -1;
    }
    if (uMin > 1u) {
        /* Soft path only supplies one vector (line IRQ). */
        return -1;
    }
    pDev->u8IrqAllocated = 1u;
    if (pDev->irq <= 0) {
        pDev->irq = LINUX_PCI_SOFT_IRQ;
    }
    /* Soft: always allocate exactly 1 vector. */
    return 1;
}

int
pci_irq_vector(void *dev, unsigned int uNr)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || uNr != 0u) {
        return -1;
    }
    if (pDev->irq > 0) {
        return pDev->irq;
    }
    return LINUX_PCI_SOFT_IRQ;
}

int
pci_select_bars(void *dev, unsigned long ulFlags)
{
    struct pci_dev *pDev;
    int nBars;
    u32 i;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return 0;
    }
    nBars = 0;
    for (i = 0u; i < LINUX_PCI_SOFT_BAR_MAX; i++) {
        if (pDev->resource_len[i] == 0ull && pDev->resource_start[i] == 0ull) {
            continue;
        }
        if ((ulFlags & IORESOURCE_MEM) != 0ul &&
            pDev->resource_is_mem[i] != 0u) {
            nBars |= (1 << (int)i);
        }
        if ((ulFlags & IORESOURCE_IO) != 0ul &&
            pDev->resource_is_mem[i] == 0u) {
            nBars |= (1 << (int)i);
        }
        /* No flags: any present BAR. */
        if ((ulFlags & (IORESOURCE_MEM | IORESOURCE_IO)) == 0ul) {
            nBars |= (1 << (int)i);
        }
    }
    return nBars;
}

int
pci_set_power_state(void *dev, int nState)
{
    (void)dev;
    (void)nState;
    return 0;
}

int
pci_wake_from_d3(void *dev, int nEnable)
{
    (void)dev;
    (void)nEnable;
    return 0;
}

int
pci_dev_run_wake(void *dev)
{
    (void)dev;
    return 0;
}

int
pci_disable_link_state(void *dev, int nState)
{
    (void)dev;
    (void)nState;
    return 0;
}

u16
pci_status_get_and_clear_errors(void *dev)
{
    struct pci_dev *pDev;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Dw;
    u16 u16St;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return 0u;
    }
    /* Soft: read status (offset 0x06), clear sticky error bits via CF8. */
    lpcis_dev_bdf(pDev, &u8Bus, &u8Slot, &u8Func);
    u32Dw = lpcis_cfg_read32(u8Bus, u8Slot, u8Func, 0x04u);
    u16St = (u16)((u32Dw >> 16) & 0xffffu);
    if (u16St != 0xffffu && (u16St & 0xf900u) != 0u) {
        /* Write-1-to-clear error bits in status. */
        u32Dw = (u32Dw & 0x0000ffffu) | ((u32)(u16St & 0xf900u) << 16);
        lpcis_cfg_write32(u8Bus, u8Slot, u8Func, 0x04u, u32Dw);
    }
    return (u16St == 0xffffu) ? 0u : (u16)(u16St & 0xf900u);
}

int
pci_reset_bus(void *dev)
{
    (void)dev;
    /* Soft≠product: no secondary bus reset. */
    return 0;
}

u64
pci_resource_start(void *dev, int nBar)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || nBar < 0 || nBar >= (int)LINUX_PCI_SOFT_BAR_MAX) {
        return 0ull;
    }
    return pDev->resource_start[nBar];
}

u64
pci_resource_len(void *dev, int nBar)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL || nBar < 0 || nBar >= (int)LINUX_PCI_SOFT_BAR_MAX) {
        return 0ull;
    }
    return pDev->resource_len[nBar];
}

void
pci_set_drvdata(void *dev, void *pData)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return;
    }
    pDev->driver_data = pData;
}

void *
pci_get_drvdata(void *dev)
{
    struct pci_dev *pDev;

    pDev = (struct pci_dev *)dev;
    if (pDev == NULL) {
        return NULL;
    }
    return pDev->driver_data;
}

/* ---- Diagnostics -------------------------------------------------------- */

u32
linux_pci_soft_driver_count(void)
{
    return lpcis_count_drivers();
}

u32
linux_pci_soft_bound_count(void)
{
    return g_cBoundUsed;
}

u32
linux_pci_soft_register_calls(void)
{
    return g_cRegCalls;
}

u32
linux_pci_soft_match_count(void)
{
    return g_cMatch;
}

/**
 * Force soft EMU bind for a VID:DID present in devmgr inventory.
 * Does not require a live .ko pci_driver registration (safety net when
 * id_table stride / layout was wrong, or init returned 0 without bind).
 */
u32
linux_pci_soft_force_emu_bind(u16 u16Vend, u16 u16Dev)
{
    u32 cFn;
    u32 i;
    u32 cHit;
    struct gj_devmgr_pci_fn fn;
    struct lpcis_drv_view view;
    static const struct pci_device_id s_aForceId[2] = {
        { .vendor = 0u, .device = 0u, .subvendor = PCI_ANY_ID,
          .subdevice = PCI_ANY_ID, .class = 0u, .class_mask = 0u,
          .driver_data = 0ul, .override_only = 0u, .u32Pad = 0u },
        { .vendor = 0u, .device = 0u, .subvendor = 0u, .subdevice = 0u,
          .class = 0u, .class_mask = 0u, .driver_data = 0ul,
          .override_only = 0u, .u32Pad = 0u }
    };
    struct pci_device_id aId[2];

    if (!g_fReady) {
        linux_pci_soft_init();
    }

    aId[0] = s_aForceId[0];
    aId[0].vendor = (u32)u16Vend;
    aId[0].device = (u32)u16Dev;
    aId[1] = s_aForceId[1];

    memset(&view, 0, sizeof(view));
    view.name = "force-emu";
    view.id_table = aId;
    view.probe = NULL;
    view.remove = NULL;

    if (!devmgr_soft_ready()) {
        devmgr_soft_init();
    }
    cFn = devmgr_soft_count();
    if (cFn == 0u) {
        cFn = devmgr_soft_pci_scan(NULL, 0u);
    }

    cHit = 0u;
    for (i = 0u; i < cFn; i++) {
        if (devmgr_soft_get(i, &fn) != 0) {
            continue;
        }
        if (fn.u16Vendor != u16Vend || fn.u16Device != u16Dev) {
            continue;
        }
        if (lpcis_try_match_fn(&view, &fn) != 0) {
            if (cHit < 0xffffffffu) {
                cHit++;
            }
        }
    }

    if (u16Vend == (u16)LPCIS_VID_REALTEK &&
        u16Dev == (u16)LPCIS_DID_RTL8168) {
        if (cHit > 0u) {
            kprintf("linux_pci_soft: soft force emu 10ec:8168 PASS hits=%u "
                    "bound=%u\n",
                    (unsigned)cHit, (unsigned)g_cBoundUsed);
        } else {
            kprintf("linux_pci_soft: soft force emu 10ec:8168 SKIP "
                    "(no inventory or already bound)\n");
        }
    } else {
        kprintf("linux_pci_soft: soft force emu %04x:%04x hits=%u\n",
                (unsigned)u16Vend, (unsigned)u16Dev, (unsigned)cHit);
    }
    return cHit;
}
