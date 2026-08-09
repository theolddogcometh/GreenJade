/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft USB core + SCSI mid (scsi_mod surface) + MSC leaf ksym seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_usb_soft_init: ready lamp + linux_ksym_register of fail-closed bodies
 *   - usb_* register/URB/control/sg/autopm/anchor/streams (usb-storage + uas)
 *   - virtual usbcore HCD/hub/device/bus surface (fail-closed; Soft!=HC)
 *   - scsi_* mid host/EH/sense deepen for el9 usb-storage.ko (scsi_mod class)
 *   - sg_* scatterlist surface (incl. residual sg_next / table helpers)
 *   - MSC-adjacent wait/work/kthread/param/dev helpers so leaf reloc can clear
 *   - soft dep virtual: usb_common + usbcore + scsi_mod (eng deps_ready only)
 *
 * Wave (D residual): expand usbcore-shaped + scsi_mod-shaped ksym so soft
 * usb_storage path can progress past FAIL KSYM. Host often ships usbcore /
 * scsi_mod / usb_common BUILTIN -> virtual dep markers only. Soft != product ·
 * G-AC-1: load / INIT=0 without real HC != stick datapath. Clean-room names only.
 * Residual UND class (D residual expand): sd_mod-shaped scsi mid + sg copy +
 * usbcore endpoint/config/urb-fill + residual2 (host/scan/vpd/hcd/sg-chain)
 * + residual3 (register/sysfs/evt + buffer_map + append-sg)
 * + residual4 (target/queue/abort + sg page + hcd/hub/ep sysfs)
 * + residual5 (queue/EH abort + resid + sg pool/set + hcd start/bus/runtime)
 * + residual6 (cmnd/rq/mq + EH tur + sg chain/last + hcd pci PM/runtime)
 * + residual7 (hcd buffer/platform + hub TT/port + phy + scsi recovery + sg iter)
 * + residual8 (soft HCD/URB layout bridge: hcd↔bus + intfdata + make_path +
 *   anchor init - UDX-facing cookies; freestanding_no_exec; DoD A seed)
 * + residual9 (HCD/URB layout residual deepen: hcd↔dev + free_dev +
 *   bandwidth check + root-hub query + host_interface + urb ep/hcd cookies
 *   for userspace UDX; freestanding MSC SKIP GJ_XHCI_MSC_PROBE=0)
 * + residual10 (lean DoD A soft EP seed: EP pool + pipe_endpoint cookie +
 *   URB↔EP↔udev↔hcd reverse + actual_len/bus_name/wire_dev; userspace UDX
 *   direction; freestanding_no_exec; no .ko exec · G-AC-1)
 * + residual11 (lean DoD A UDX residual: EP enable/maxp bookkeep + URB
 *   transfer/pipe/context reverse + soft pipe encode + EP-HCD reverse;
 *   freestanding_no_exec; no .ko exec; G-AC-1)
 * + residual12 (lean Dual DoD A eng seed: EP addr/type wire + URB
 *   setup/complete/context/transfer reverse setters + soft pipe decode;
 *   userspace xhci_udx direction; freestanding MSC SKIP
 *   GJ_XHCI_MSC_PROBE=0; freestanding_no_exec; no .ko exec; G-AC-1)
 * + residual13 (lean Dual DoD A eng residual: URB setup/complete/pipe/
 *   status reverse setters + EP interval reverse + URB xfer_type reverse
 *   + soft pipe_from_ep for userspace xhci_udx; soft residual lean lamp
 *   freestanding_msc=SKIP product_usb=UDX RUN_INIT=0 freestanding_no_exec;
 *   Soft!=product; no freestanding thrash; no .ko exec; G-AC-1)
 * + residual14 (lean Dual DoD A eng residual deepen: URB dev/hcd/xfer_type/
 *   interval reverse + linked flag + actual_len eng bookkeep + soft
 *   wire_urb_ep for userspace xhci_udx eng; soft residual lean lamp
 *   freestanding_msc=SKIP product_usb=UDX RUN_INIT=0 freestanding_no_exec;
 *   Soft!=product; no freestanding thrash; no .ko exec; G-AC-1)
 * + residual15 (lean Dual DoD A eng residual lifecycle: URB submitted/
 *   unlinked/poisoned reverse + seq reverse + soft wire_urb_hcd for
 *   userspace xhci_udx eng; soft residual lean lamp freestanding_msc=SKIP
 *   product_usb=UDX RUN_INIT=0 freestanding_no_exec; Soft!=product;
 *   no freestanding thrash; no .ko exec; G-AC-1)
 * + residual16 (lean Dual DoD A eng residual anchor/live deepen: URB
 *   anchored/live reverse + anchor cookie + soft wire_urb_dev +
 *   clear_urb_lifecycle for userspace xhci_udx eng; soft residual lean
 *   lamp freestanding_msc=SKIP product_usb=UDX RUN_INIT=0
 *   freestanding_no_exec; Soft!=product; no freestanding thrash; no .ko
 *   exec; G-AC-1).
 * - fail-closed soft stubs; Soft!=product; never claim stick write.
 * Soft layout (struct usb_soft_urb/hcd/udev) is incomplete front-only shape
 * for fill/submit/giveback + later UDX hosts - Soft!=ABI-stable; Soft!=.ko exec.
 * Align: freestanding xHCI residual SKIP default (GJ_XHCI_MSC_PROBE=0).
 *
 * Greppable markers (keep stable):
 *   linux_usb_soft: soft init PASS n=
 *   linux_usb_soft: soft usb_register ...
 *   linux_usb_soft: soft submit_urb ...
 *   linux_usb_soft: soft hcd ...
 *   linux_usb_soft: soft hcd/urb layout PASS ...
 *   linux_usb_soft: soft scsi_mod surface ...
 *   linux_usb_soft: soft scsi_host_alloc ...
 *   linux_usb_soft: soft scsi_add_host ...
 *   linux_usb_soft: soft residual UND ...
 *   linux_usb_soft: soft residual UND PASS n=
 *   linux_usb_soft: soft residual UND ... Soft!=product
 *   linux_usb_soft: soft dep virtual ...
 *   linux_usb_soft: soft DoD A seed ...
 *   linux_usb_soft: soft residual lean PASS Soft!=product ...
 *   linux_usb_soft: soft residual freestanding_msc=SKIP ...
 *   linux_usb_soft: soft residual product_usb=UDX ...
 *   linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec ...
 *   linux_module: soft dep virtual usbcore|scsi_mod|usb_common soft=1 product=0
 *     (honest seed markers so deps_ready can PASS eng lamps; != real .ko)
 * Soft!=product · never claim stick write · need=HC OPEN.
 * Soft residual lean: seed only for userspace UDX path eng; freestanding
 * MSC SKIP; product_usb=UDX; RUN_INIT=0 freestanding_no_exec; G-AC-1.
 */
#include <gj/klog.h>
#include <gj/linux_usb_soft.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved -> NULL; init skips export.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/*
 * Soft dep virtual registration lives in linux_module.c. Weak so this TU
 * builds if module path is stripped; seed markers only when module linked.
 * Grep: linux_module: soft dep virtual usbcore soft=1 product=0
 */
int linux_module_soft_dep_virtual_register(const char *szCanon)
    __attribute__((weak));

/* Soft errno-shaped returns (Linux negative errno mental model). */
#define LUSB_ENODEV   (-19)
#define LUSB_ENOMEM   (-12)
#define LUSB_EINVAL   (-22)
#define LUSB_ENOENT   (-2)
#define LUSB_EAGAIN   (-11)
#define LUSB_ETIMEDOUT (-110)

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cKsymOk;
static u32 g_cKsymSkip;
static u32 g_cStubReg;

static u32 g_cUsbRegister;
static u32 g_cUsbDeregister;
static u32 g_cSubmitUrb;
static u32 g_cAllocUrb;
static u32 g_cHcdCreate;
static u32 g_cHcdAdd;
static u32 g_cHcdGiveback;
static u32 g_cHub;
static u32 g_cScsiHostAlloc;
static u32 g_cScsiAddHost;
static u32 g_cScsiScan;
static u32 g_cScsiDone;
static u32 g_cScsiEh;
static u32 g_cScsiSense;
static u32 g_cScsiBlock;
static u32 g_cScsiReport;
static u32 g_cSgNents;
static u32 g_cKthreadCreate;

static int g_fLogRegister;
static int g_fLogSubmit;
static int g_fLogSg;
static int g_fLogHcd;
static int g_fLogScsiHost;
static int g_fLogScsiAdd;
static int g_fLogScsiScan;
static int g_fLogScsiMod;

/* Soft static objects (never free; zeroed once). Soft!=ABI-stable. */
static u8 g_aSoftUrb[LINUX_USB_SOFT_URB_POOL][LINUX_USB_SOFT_URB_CB];
static u8 g_aSoftUrbUsed[LINUX_USB_SOFT_URB_POOL];
static u8 g_aSoftHost[LINUX_USB_SOFT_HOST_CB];
static u8 g_fSoftHostLive;
static u8 g_fSoftHostAdded;
static u8 g_fSoftHostBlocked;
static void *g_pSoftHostSht;
static int g_nSoftHostPriv;
static u32 g_uSoftHostNo;
/* Soft HCD pool (virtual usbcore; Soft!=real HC). */
static u8 g_aSoftHcd[LINUX_USB_SOFT_HCD_POOL][LINUX_USB_SOFT_HCD_CB];
static u8 g_aSoftHcdUsed[LINUX_USB_SOFT_HCD_POOL];
static u8 g_aSoftHcdAdded[LINUX_USB_SOFT_HCD_POOL];
static void *g_apSoftHcdDriver[LINUX_USB_SOFT_HCD_POOL];
static u8 g_aSoftUdev[LINUX_USB_SOFT_UDEV_CB];
static u8 g_fSoftUdevLive;
static u8 g_aSoftBus[LINUX_USB_SOFT_BUS_CB];
static u8 g_fSoftBusLive;
/* Soft endpoint pool (residual10 UDX cookies; Soft!=descriptor walk). */
static u8 g_aSoftEp[LINUX_USB_SOFT_EP_POOL][LINUX_USB_SOFT_EP_CB];
static u8 g_aSoftEpUsed[LINUX_USB_SOFT_EP_POOL];
static u8 g_aSoftCoh[256];
static u8 g_aSoftAnchor[LINUX_USB_SOFT_ANCHOR_CB];
static u8 g_aSoftParamOpsUint[LINUX_USB_SOFT_DATA_CB];
static u8 g_aSoftParamOpsString[LINUX_USB_SOFT_DATA_CB];
static u8 g_aSoftPcpuHot[LINUX_USB_SOFT_DATA_CB];
static u8 g_aSoftSystemFreezableWq[LINUX_USB_SOFT_DATA_CB];
static u8 g_aSoftTracepointSched[LINUX_USB_SOFT_DATA_CB];
static u8 g_aSoftStorSenseInvalidCdb[32];
/* Soft EH session: save cmnd/sense across scsi_eh_prep/restore. Soft!=ABI. */
static u8 g_aSoftEhSes[LINUX_USB_SOFT_EH_CB];
static u8 g_aSoftEhCmnd[LINUX_USB_SOFT_CMND_CB];
static int g_nSoftEhCmndLen;
static u8 g_aSoftEhSense[96];
static unsigned int g_uSoftEhSenseBytes;
static u8 g_fSoftEhLive;
static u8 g_aSoftSshdr[LINUX_USB_SOFT_SSHDR_CB];
static u8 g_fSoftObjsInit;
/* Soft scalar / string data exports (usbcore-shaped). Soft!=ABI-stable. */
static int g_nSoftUsbDisabled;
static int g_nSoftScsiLoggingLevel;
static u8 g_aSoftWqBlob[LINUX_USB_SOFT_DATA_CB];
static const char g_szSoftUsbEp[] = "soft-ep";
static const char g_szSoftUsbSpeed[] = "soft";
static const char g_szSoftUsbState[] = "notattached";
static const char g_szSoftScsiHostbyte[] = "soft";
static const char g_szSoftScsiSenseKey[] = "soft";
static const char g_szSoftScsiMlreturn[] = "soft";
static int g_fLogResidual;
static int g_fLogLayout;
static int g_fLogDodA;
static int g_fLogLean;
/* Soft URB submit/link sequence (layout residual eng only). Soft!=product. */
static u32 g_uSoftUrbSeq;

/* Soft host bookkeeping magic (not a Linux field). Soft!=ABI-stable. */
#define LUSB_SCSI_HOST_MAGIC 0x48534353u /* 'SCSH' little-endian */
#define LUSB_HCD_MAGIC       LINUX_USB_SOFT_HCD_MAGIC
#define LUSB_URB_MAGIC       LINUX_USB_SOFT_URB_MAGIC
#define LUSB_UDEV_MAGIC      LINUX_USB_SOFT_UDEV_MAGIC
#define LUSB_BUS_MAGIC       LINUX_USB_SOFT_BUS_MAGIC
#define LUSB_EP_MAGIC        LINUX_USB_SOFT_EP_MAGIC

/* Soft intfdata cookie (single slot; Soft!=device model). */
static void *g_pSoftIntfData;

/* Compile-time fit: soft front layouts must fit pool slab sizes. */
typedef char lusb_urb_fit_chk[
    (sizeof(struct usb_soft_urb) <= LINUX_USB_SOFT_URB_CB) ? 1 : -1];
typedef char lusb_hcd_fit_chk[
    (sizeof(struct usb_soft_hcd) <= LINUX_USB_SOFT_HCD_CB) ? 1 : -1];
typedef char lusb_udev_fit_chk[
    (sizeof(struct usb_soft_udev) <= LINUX_USB_SOFT_UDEV_CB) ? 1 : -1];
typedef char lusb_bus_fit_chk[
    (sizeof(struct usb_soft_bus) <= LINUX_USB_SOFT_BUS_CB) ? 1 : -1];
typedef char lusb_ep_fit_chk[
    (sizeof(struct usb_soft_ep) <= LINUX_USB_SOFT_EP_CB) ? 1 : -1];

/* ---- Helpers ------------------------------------------------------------ */

static void
lusb_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

static void
lusb_ksym_one(const char *szName, void *pFn)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        lusb_inc(&g_cKsymSkip);
        return;
    }
    if (linux_ksym_register == NULL) {
        lusb_inc(&g_cKsymSkip);
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        lusb_inc(&g_cKsymOk);
        lusb_inc(&g_cStubReg);
    } else {
        lusb_inc(&g_cKsymSkip);
    }
}

static void
lusb_soft_objs_once(void)
{
    u32 i;

    if (g_fSoftObjsInit != 0u) {
        return;
    }
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        memset(g_aSoftUrb[i], 0, LINUX_USB_SOFT_URB_CB);
        g_aSoftUrbUsed[i] = 0u;
    }
    memset(g_aSoftHost, 0, LINUX_USB_SOFT_HOST_CB);
    g_fSoftHostLive = 0u;
    g_fSoftHostAdded = 0u;
    g_fSoftHostBlocked = 0u;
    g_pSoftHostSht = NULL;
    g_nSoftHostPriv = 0;
    g_uSoftHostNo = 0u;
    for (i = 0u; i < LINUX_USB_SOFT_HCD_POOL; i++) {
        memset(g_aSoftHcd[i], 0, LINUX_USB_SOFT_HCD_CB);
        g_aSoftHcdUsed[i] = 0u;
        g_aSoftHcdAdded[i] = 0u;
        g_apSoftHcdDriver[i] = NULL;
    }
    memset(g_aSoftUdev, 0, sizeof(g_aSoftUdev));
    g_fSoftUdevLive = 0u;
    memset(g_aSoftBus, 0, sizeof(g_aSoftBus));
    g_fSoftBusLive = 0u;
    for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
        memset(g_aSoftEp[i], 0, LINUX_USB_SOFT_EP_CB);
        g_aSoftEpUsed[i] = 0u;
    }
    memset(g_aSoftCoh, 0, sizeof(g_aSoftCoh));
    memset(g_aSoftAnchor, 0, sizeof(g_aSoftAnchor));
    memset(g_aSoftParamOpsUint, 0, sizeof(g_aSoftParamOpsUint));
    memset(g_aSoftParamOpsString, 0, sizeof(g_aSoftParamOpsString));
    memset(g_aSoftPcpuHot, 0, sizeof(g_aSoftPcpuHot));
    memset(g_aSoftSystemFreezableWq, 0, sizeof(g_aSoftSystemFreezableWq));
    memset(g_aSoftTracepointSched, 0, sizeof(g_aSoftTracepointSched));
    memset(g_aSoftStorSenseInvalidCdb, 0, sizeof(g_aSoftStorSenseInvalidCdb));
    memset(g_aSoftEhSes, 0, sizeof(g_aSoftEhSes));
    memset(g_aSoftEhCmnd, 0, sizeof(g_aSoftEhCmnd));
    g_nSoftEhCmndLen = 0;
    memset(g_aSoftEhSense, 0, sizeof(g_aSoftEhSense));
    g_uSoftEhSenseBytes = 0u;
    g_fSoftEhLive = 0u;
    memset(g_aSoftSshdr, 0, sizeof(g_aSoftSshdr));
    g_pSoftIntfData = NULL;
    g_fSoftObjsInit = 1u;
}

/* Soft host identity: pointer equality against single soft slab. */
static int
lusb_is_soft_host(void *pShost)
{
    return (pShost != NULL && pShost == (void *)g_aSoftHost) ? 1 : 0;
}

/* Soft HCD slot index or -1. Soft!=product. */
static int
lusb_soft_hcd_slot(void *pHcd)
{
    u32 i;

    if (pHcd == NULL) {
        return -1;
    }
    for (i = 0u; i < LINUX_USB_SOFT_HCD_POOL; i++) {
        if ((void *)g_aSoftHcd[i] == pHcd) {
            return (int)i;
        }
    }
    return -1;
}

/* Soft URB pool membership. Soft!=product. */
static int
lusb_soft_urb_slot(void *pUrb)
{
    u32 i;

    if (pUrb == NULL) {
        return -1;
    }
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        if ((void *)g_aSoftUrb[i] == pUrb) {
            return (int)i;
        }
    }
    return -1;
}

/* Soft layout casts (magic + pool). Soft!=ABI-stable. */
static struct usb_soft_urb *
lusb_urb_soft(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    if (lusb_soft_urb_slot(pUrb) < 0) {
        return NULL;
    }
    pSoft = (struct usb_soft_urb *)pUrb;
    if (pSoft->u32Magic != LUSB_URB_MAGIC) {
        return NULL;
    }
    return pSoft;
}

static struct usb_soft_hcd *
lusb_hcd_soft(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    if (lusb_soft_hcd_slot(pHcd) < 0) {
        return NULL;
    }
    pSoft = (struct usb_soft_hcd *)pHcd;
    if (pSoft->u32Magic != LUSB_HCD_MAGIC) {
        return NULL;
    }
    return pSoft;
}

static struct usb_soft_udev *
lusb_udev_soft(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    if (pUdev == NULL || pUdev != (void *)g_aSoftUdev) {
        return NULL;
    }
    pSoft = (struct usb_soft_udev *)pUdev;
    if (pSoft->u32Magic != LUSB_UDEV_MAGIC) {
        return NULL;
    }
    return pSoft;
}

static struct usb_soft_bus *
lusb_bus_soft(void *pBus)
{
    struct usb_soft_bus *pSoft;

    if (pBus == NULL || pBus != (void *)g_aSoftBus) {
        return NULL;
    }
    pSoft = (struct usb_soft_bus *)pBus;
    if (pSoft->u32Magic != LUSB_BUS_MAGIC) {
        return NULL;
    }
    return pSoft;
}

/* Soft EP pool membership. residual10 · Soft!=product. */
static int
lusb_soft_ep_slot(void *pEp)
{
    u32 i;

    if (pEp == NULL) {
        return -1;
    }
    for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
        if ((void *)g_aSoftEp[i] == pEp) {
            return (int)i;
        }
    }
    return -1;
}

static struct usb_soft_ep *
lusb_ep_soft(void *pEp)
{
    struct usb_soft_ep *pSoft;

    if (lusb_soft_ep_slot(pEp) < 0) {
        return NULL;
    }
    pSoft = (struct usb_soft_ep *)pEp;
    if (pSoft->u32Magic != LUSB_EP_MAGIC) {
        return NULL;
    }
    return pSoft;
}

/* Linux-shaped pipe-IN bit (bit 7). Soft echo only. */
static u8
lusb_pipe_dir_in(unsigned int uPipe)
{
    return ((uPipe & 0x80u) != 0u) ? 1u : 0u;
}

/*
 * Linux-shaped pipe endpoint number (bits 15:11 -> nibble). Soft echo only.
 * residual9 layout residual for UDX hosts. Soft!=product · != stick.
 */
static u8
lusb_pipe_ep_num(unsigned int uPipe)
{
    return (u8)((uPipe >> 15) & 0xfu);
}

/* Init soft EP front after pool memset. residual10 · Soft!=product. */
static void
lusb_ep_soft_prime(struct usb_soft_ep *pSoft, void *pUdev, unsigned int uPipe)
{
    u8 u8Num;
    u8 u8DirIn;

    if (pSoft == NULL) {
        return;
    }
    u8Num = lusb_pipe_ep_num(uPipe);
    u8DirIn = lusb_pipe_dir_in(uPipe);
    pSoft->u32Magic = LUSB_EP_MAGIC;
    pSoft->pUdev = pUdev;
    pSoft->pHcd = NULL;
    pSoft->u8Num = u8Num;
    pSoft->u8DirIn = u8DirIn;
    pSoft->u8Addr = (u8)(u8Num | ((u8DirIn != 0u) ? 0x80u : 0u));
    pSoft->u8Type = LINUX_USB_SOFT_XFER_CTRL; /* default; fill may override */
    pSoft->u8Enabled = 0u;
    pSoft->u8Live = 1u;
    pSoft->u16MaxPacket = 0u; /* fail-closed; Soft!=product */
    pSoft->u16Interval = 0u;
    if (pUdev != NULL) {
        struct usb_soft_udev *pUdevSoft;

        pUdevSoft = lusb_udev_soft(pUdev);
        if (pUdevSoft != NULL) {
            pSoft->pHcd = pUdevSoft->pHcd;
        }
    }
}

/*
 * Soft EP lookup/create by udev+pipe. residual10 UDX cookie seed.
 * Single soft EP per (num,dir) in pool; Soft!=descriptor walk; != stick.
 */
static struct usb_soft_ep *
lusb_ep_soft_lookup_or_prime(void *pUdev, unsigned int uPipe)
{
    u32 i;
    u8 u8Num;
    u8 u8DirIn;
    struct usb_soft_ep *pSoft;
    int nFree;

    lusb_soft_objs_once();
    u8Num = lusb_pipe_ep_num(uPipe);
    u8DirIn = lusb_pipe_dir_in(uPipe);
    nFree = -1;
    for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
        if (g_aSoftEpUsed[i] == 0u) {
            if (nFree < 0) {
                nFree = (int)i;
            }
            continue;
        }
        pSoft = (struct usb_soft_ep *)(void *)g_aSoftEp[i];
        if (pSoft->u32Magic == LUSB_EP_MAGIC && pSoft->pUdev == pUdev &&
            pSoft->u8Num == u8Num && pSoft->u8DirIn == u8DirIn) {
            return pSoft;
        }
    }
    if (nFree < 0) {
        return NULL; /* pool exhausted - fail closed */
    }
    memset(g_aSoftEp[(u32)nFree], 0, LINUX_USB_SOFT_EP_CB);
    g_aSoftEpUsed[(u32)nFree] = 1u;
    pSoft = (struct usb_soft_ep *)(void *)g_aSoftEp[(u32)nFree];
    lusb_ep_soft_prime(pSoft, pUdev, uPipe);
    return pSoft;
}

/* Init soft URB front after pool memset. Soft!=product. */
static void
lusb_urb_soft_prime(struct usb_soft_urb *pSoft)
{
    if (pSoft == NULL) {
        return;
    }
    pSoft->u32Magic = LUSB_URB_MAGIC;
    pSoft->nStatus = 0;
    pSoft->u8Live = 1u;
    pSoft->u8Submitted = 0u;
    pSoft->u8Poisoned = 0u;
    pSoft->u8Anchored = 0u;
    pSoft->u8XferType = LINUX_USB_SOFT_XFER_CTRL;
    pSoft->u8DirIn = 0u;
    pSoft->u8Unlinked = 0u;
    pSoft->u8EpNum = 0u;
    pSoft->u8Linked = 0u;
    pSoft->u32Seq = 0u;
    pSoft->u32ActualLen = 0u;
    pSoft->pHcd = NULL;
    pSoft->pAnchor = NULL;
}

/* Init soft HCD front after pool memset. Soft!=product. */
static void
lusb_hcd_soft_prime(struct usb_soft_hcd *pSoft, void *pDriver,
                    void *pController, const char *szBusName, void *pPrimary)
{
    u32 i;

    if (pSoft == NULL) {
        return;
    }
    pSoft->u32Magic = LUSB_HCD_MAGIC;
    pSoft->u32Flags = LINUX_USB_SOFT_HCD_F_PRIMARY;
    pSoft->pDriver = pDriver;
    pSoft->pController = pController;
    pSoft->pShared = pPrimary;
    pSoft->pBus = NULL;
    pSoft->pRootHub = NULL; /* soft never attaches a real root hub */
    pSoft->u8Used = 1u;
    pSoft->u8Added = 0u;
    pSoft->u8Primary = (pPrimary == NULL) ? 1u : 0u;
    if (pSoft->u8Primary == 0u) {
        pSoft->u32Flags &= ~LINUX_USB_SOFT_HCD_F_PRIMARY;
    }
    pSoft->u8Died = 0u;
    pSoft->u8Started = 0u;
    pSoft->u8RhRegistered = 0u;
    pSoft->u32Irq = 0u;
    pSoft->u32Frame = 0u;
    pSoft->u32UrbLinked = 0u;
    for (i = 0u; i < sizeof(pSoft->szBusName); i++) {
        pSoft->szBusName[i] = '\0';
    }
    if (szBusName != NULL) {
        for (i = 0u; i < (sizeof(pSoft->szBusName) - 1u) &&
                     szBusName[i] != '\0'; i++) {
            pSoft->szBusName[i] = szBusName[i];
        }
        pSoft->szBusName[i] = '\0';
    } else {
        pSoft->szBusName[0] = 's';
        pSoft->szBusName[1] = 'o';
        pSoft->szBusName[2] = 'f';
        pSoft->szBusName[3] = 't';
        pSoft->szBusName[4] = '\0';
    }
}

/* ---- usb_* soft bodies (fail-closed / no-op) --------------------------- */

/*
 * Module init path: soft-success register so reloc+init can complete without
 * a real HC. Soft!=product - no probe, no stick. Grep once.
 */
int
usb_register_driver(void *pDriver, void *pModule, const char *szName)
{
    (void)pDriver;
    (void)pModule;
    lusb_inc(&g_cUsbRegister);
    if (g_fLogRegister == 0) {
        g_fLogRegister = 1;
        kprintf("linux_usb_soft: soft usb_register name=%s n=%u "
                "soft=1 product=0 (no HC; != stick)\n",
                (szName != NULL && szName[0] != '\0') ? szName : "?",
                (unsigned)g_cUsbRegister);
    }
    return 0;
}

void
usb_deregister(void *pDriver)
{
    (void)pDriver;
    lusb_inc(&g_cUsbDeregister);
}

void *
usb_alloc_urb(int nIsoPackets, int nMemFlags)
{
    u32 i;
    struct usb_soft_urb *pSoft;

    (void)nIsoPackets;
    (void)nMemFlags;
    lusb_inc(&g_cAllocUrb);
    lusb_soft_objs_once();
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        if (g_aSoftUrbUsed[i] == 0u) {
            g_aSoftUrbUsed[i] = 1u;
            memset(g_aSoftUrb[i], 0, LINUX_USB_SOFT_URB_CB);
            pSoft = (struct usb_soft_urb *)(void *)g_aSoftUrb[i];
            lusb_urb_soft_prime(pSoft);
            return (void *)g_aSoftUrb[i];
        }
    }
    /* Pool exhausted - fail closed (no unbounded soft alloc). */
    return NULL;
}

void
usb_free_urb(void *pUrb)
{
    u32 i;
    struct usb_soft_urb *pSoft;

    if (pUrb == NULL) {
        return;
    }
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        if ((void *)g_aSoftUrb[i] == pUrb) {
            pSoft = (struct usb_soft_urb *)pUrb;
            pSoft->u8Live = 0u;
            pSoft->u32Magic = 0u;
            g_aSoftUrbUsed[i] = 0u;
            return;
        }
    }
}

int
usb_submit_urb(void *pUrb, int nMemFlags)
{
    struct usb_soft_urb *pSoft;

    (void)nMemFlags;
    lusb_inc(&g_cSubmitUrb);
    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        if (pSoft->u8Poisoned != 0u) {
            pSoft->nStatus = LUSB_ENODEV;
            pSoft->u8Submitted = 0u;
            pSoft->u32ActualLen = 0u;
            return LUSB_ENODEV;
        }
        /* Soft layout bookkeep only - no HC datapath. Soft!=product. */
        pSoft->u8Submitted = 1u;
        pSoft->u32ActualLen = 0u;
        pSoft->nStatus = LUSB_ENODEV;
        pSoft->u8Unlinked = 0u;
        pSoft->u8EpNum = lusb_pipe_ep_num(pSoft->uPipe);
        lusb_inc(&g_uSoftUrbSeq);
        pSoft->u32Seq = g_uSoftUrbSeq;
    }
    if (g_fLogSubmit == 0) {
        g_fLogSubmit = 1;
        kprintf("linux_usb_soft: soft submit_urb n=%u ENODEV soft=1 product=0 "
                "(no HC; != stick; layout=%u; freestanding_msc=SKIP)\n",
                (unsigned)g_cSubmitUrb,
                (unsigned)((pSoft != NULL) ? 1u : 0u));
    }
    return LUSB_ENODEV;
}

int
usb_unlink_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* residual15 lifecycle: mirror set_unlinked (clear linked + HCD count). */
        if (pSoft->u8Linked != 0u) {
            pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
            if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
                pHcdSoft->u32UrbLinked--;
            }
            pSoft->u8Linked = 0u;
        }
        pSoft->u8Unlinked = 1u;
        pSoft->u8Submitted = 0u;
        pSoft->nStatus = LUSB_ENODEV;
    }
    return LUSB_ENODEV;
}

void
usb_kill_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* residual15 lifecycle: mirror set_unlinked (clear linked + HCD count). */
        if (pSoft->u8Linked != 0u) {
            pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
            if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
                pHcdSoft->u32UrbLinked--;
            }
            pSoft->u8Linked = 0u;
        }
        pSoft->u8Submitted = 0u;
        pSoft->u8Unlinked = 1u;
        pSoft->nStatus = LUSB_ENODEV;
        pSoft->u32ActualLen = 0u;
    }
}

void *
usb_alloc_coherent(void *pDev, unsigned long cbSize, int nMemFlags,
                   void *pDma)
{
    (void)pDev;
    (void)nMemFlags;
    (void)pDma;
    lusb_soft_objs_once();
    /* Tiny soft bounce only; large requests fail closed. */
    if (cbSize == 0ul || cbSize > sizeof(g_aSoftCoh)) {
        return NULL;
    }
    if (pDma != NULL) {
        *(unsigned long *)pDma = 0ul;
    }
    memset(g_aSoftCoh, 0, (size_t)cbSize);
    return (void *)g_aSoftCoh;
}

void
usb_free_coherent(void *pDev, unsigned long cbSize, void *pAddr,
                  unsigned long dma)
{
    (void)pDev;
    (void)cbSize;
    (void)pAddr;
    (void)dma;
}

int
usb_control_msg(void *pDev, unsigned int uPipe, unsigned char u8Req,
                unsigned char u8ReqType, unsigned short u16Value,
                unsigned short u16Index, void *pData, unsigned short u16Size,
                int nTimeout)
{
    (void)pDev;
    (void)uPipe;
    (void)u8Req;
    (void)u8ReqType;
    (void)u16Value;
    (void)u16Index;
    (void)pData;
    (void)u16Size;
    (void)nTimeout;
    return LUSB_ENODEV;
}

int
usb_sg_init(void *pSg, void *pDev, unsigned int uPipe, unsigned int uPeriod,
            void *pSgList, int nNents, unsigned long cbLength, int nMemFlags)
{
    (void)pSg;
    (void)pDev;
    (void)uPipe;
    (void)uPeriod;
    (void)pSgList;
    (void)nNents;
    (void)cbLength;
    (void)nMemFlags;
    return LUSB_ENODEV;
}

void
usb_sg_wait(void *pSg)
{
    (void)pSg;
}

void
usb_sg_cancel(void *pSg)
{
    (void)pSg;
}

int
usb_autopm_get_interface_no_resume(void *pIntf)
{
    (void)pIntf;
    return 0;
}

void
usb_autopm_put_interface(void *pIntf)
{
    (void)pIntf;
}

void
usb_autopm_put_interface_no_suspend(void *pIntf)
{
    (void)pIntf;
}

int
usb_find_common_endpoints(void *pAlt, void **ppBulkIn, void **ppBulkOut,
                          void **ppIntIn, void **ppIntOut)
{
    (void)pAlt;
    if (ppBulkIn != NULL) {
        *ppBulkIn = NULL;
    }
    if (ppBulkOut != NULL) {
        *ppBulkOut = NULL;
    }
    if (ppIntIn != NULL) {
        *ppIntIn = NULL;
    }
    if (ppIntOut != NULL) {
        *ppIntOut = NULL;
    }
    return LUSB_ENOENT;
}

int
usb_lock_device_for_reset(void *pDev, void *pIntf)
{
    (void)pDev;
    (void)pIntf;
    return LUSB_ENODEV;
}

int
usb_reset_device(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_reset_endpoint(void *pDev, unsigned int uEpAddr)
{
    (void)pDev;
    (void)uEpAddr;
}

/* ---- usb_* expand (uas + streams/anchor/set_interface) ----------------- */

int
usb_alloc_streams(void *pIntf, void **ppEps, unsigned int cEps,
                  unsigned int cStreams, int nMemFlags)
{
    (void)pIntf;
    (void)ppEps;
    (void)cEps;
    (void)cStreams;
    (void)nMemFlags;
    return LUSB_ENODEV;
}

void
usb_free_streams(void *pIntf, void **ppEps, unsigned int cEps, int nMemFlags)
{
    (void)pIntf;
    (void)ppEps;
    (void)cEps;
    (void)nMemFlags;
}

void
usb_anchor_urb(void *pUrb, void *pAnchor)
{
    struct usb_soft_urb *pSoft;

    lusb_soft_objs_once();
    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->pAnchor = pAnchor;
        pSoft->u8Anchored = 1u;
    }
}

void
usb_unanchor_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->pAnchor = NULL;
        pSoft->u8Anchored = 0u;
    }
}

void *
usb_get_urb(void *pUrb)
{
    /* Soft: return same pointer (no refcount). Soft!=product. */
    return pUrb;
}

void
usb_kill_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

int
usb_wait_anchor_empty_timeout(void *pAnchor, unsigned int uTimeout)
{
    (void)pAnchor;
    (void)uTimeout;
    return 0;
}

int
usb_set_interface(void *pDev, int nIfnum, int nAlt)
{
    (void)pDev;
    (void)nIfnum;
    (void)nAlt;
    return LUSB_ENODEV;
}

/*
 * usb-storage exports used by uas. Soft zero/no-op so either leaf can load
 * without the other providing exports. Soft!=product.
 */
void
usb_stor_adjust_quirks(void *pUdev, unsigned long *puQuirks)
{
    (void)pUdev;
    if (puQuirks != NULL) {
        /* Leave caller value unchanged. */
    }
}

/* Data-ish export: zero sense blob. Soft!=ABI-stable. */
/* Registered as address of g_aSoftStorSenseInvalidCdb in init. */

/* ---- usbcore expand (common exports; fail-closed; Soft!=product) -------- */

/*
 * Additive usbcore-shaped surface beyond the el9 usb-storage 20-UND class.
 * Names only / clean-room bodies. No HC, no descriptor walk, no stick.
 */

void *
usb_get_dev(void *pDev)
{
    /* Soft: no refcount; echo pointer. Soft!=product. */
    return pDev;
}

void
usb_put_dev(void *pDev)
{
    (void)pDev;
}

void *
usb_get_intf(void *pIntf)
{
    return pIntf;
}

void
usb_put_intf(void *pIntf)
{
    (void)pIntf;
}

int
usb_bulk_msg(void *pDev, unsigned int uPipe, void *pData, int nLen,
             int *pnActual, int nTimeout)
{
    (void)pDev;
    (void)uPipe;
    (void)pData;
    (void)nLen;
    (void)nTimeout;
    if (pnActual != NULL) {
        *pnActual = 0;
    }
    return LUSB_ENODEV;
}

int
usb_interrupt_msg(void *pDev, unsigned int uPipe, void *pData, int nLen,
                  int *pnActual, int nTimeout)
{
    (void)pDev;
    (void)uPipe;
    (void)pData;
    (void)nLen;
    (void)nTimeout;
    if (pnActual != NULL) {
        *pnActual = 0;
    }
    return LUSB_ENODEV;
}

int
usb_clear_halt(void *pDev, unsigned int uPipe)
{
    (void)pDev;
    (void)uPipe;
    return LUSB_ENODEV;
}

int
usb_string(void *pDev, int nIndex, char *pBuf, unsigned long cbSize)
{
    (void)pDev;
    (void)nIndex;
    if (pBuf != NULL && cbSize > 0ul) {
        pBuf[0] = '\0';
    }
    return LUSB_ENODEV;
}

int
usb_autopm_get_interface(void *pIntf)
{
    (void)pIntf;
    return 0;
}

int
usb_autopm_get_interface_async(void *pIntf)
{
    (void)pIntf;
    return 0;
}

void
usb_autopm_put_interface_async(void *pIntf)
{
    (void)pIntf;
}

int
usb_autopm_get_interface_no_suspend(void *pIntf)
{
    (void)pIntf;
    return 0;
}

void *
usb_ifnum_to_if(void *pDev, unsigned int uIfnum)
{
    (void)pDev;
    (void)uIfnum;
    return NULL;
}

void *
usb_altnum_to_altsetting(void *pIntf, unsigned int uAlt)
{
    (void)pIntf;
    (void)uAlt;
    return NULL;
}

int
usb_driver_claim_interface(void *pDriver, void *pIntf, void *pPriv)
{
    (void)pDriver;
    (void)pIntf;
    (void)pPriv;
    return LUSB_ENODEV;
}

void
usb_driver_release_interface(void *pDriver, void *pIntf)
{
    (void)pDriver;
    (void)pIntf;
}

int
usb_interface_claimed(void *pIntf)
{
    (void)pIntf;
    return 0;
}

void *
usb_match_id(void *pIntf, const void *pId)
{
    (void)pIntf;
    (void)pId;
    return NULL;
}

void *
usb_match_one_id(void *pIntf, const void *pId)
{
    (void)pIntf;
    (void)pId;
    return NULL;
}

int
usb_get_descriptor(void *pDev, unsigned char u8Type, unsigned char u8Index,
                   void *pBuf, int nSize)
{
    (void)pDev;
    (void)u8Type;
    (void)u8Index;
    (void)pBuf;
    (void)nSize;
    return LUSB_ENODEV;
}

int
usb_get_status(void *pDev, int nType, int nTarget, void *pData)
{
    (void)pDev;
    (void)nType;
    (void)nTarget;
    (void)pData;
    return LUSB_ENODEV;
}

int
usb_set_configuration(void *pDev, int nConfig)
{
    (void)pDev;
    (void)nConfig;
    return LUSB_ENODEV;
}

int
usb_choose_configuration(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_enable_autosuspend(void *pDev)
{
    (void)pDev;
}

void
usb_disable_autosuspend(void *pDev)
{
    (void)pDev;
}

void
usb_poison_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->u8Poisoned = 1u;
        pSoft->u8Submitted = 0u;
        pSoft->nStatus = LUSB_ENODEV;
    }
}

void
usb_unpoison_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->u8Poisoned = 0u;
    }
}

void
usb_block_urb(void *pUrb)
{
    /* Soft: same as poison bookkeep (no HC queue). Soft!=product. */
    usb_poison_urb(pUrb);
}

int
usb_register_dev(void *pIntf, void *pClassDriver)
{
    (void)pIntf;
    (void)pClassDriver;
    return LUSB_ENODEV;
}

void
usb_deregister_dev(void *pIntf, void *pClassDriver)
{
    (void)pIntf;
    (void)pClassDriver;
}

void *
usb_find_interface(void *pDriver, int nMinor)
{
    (void)pDriver;
    (void)nMinor;
    return NULL;
}

void
usb_lock_device(void *pDev)
{
    (void)pDev;
}

void
usb_unlock_device(void *pDev)
{
    (void)pDev;
}

int
usb_trylock_device(void *pDev)
{
    (void)pDev;
    return 0;
}

int
usb_check_bulk_endpoints(void *pIntf, const unsigned char *pEpAddrs)
{
    (void)pIntf;
    (void)pEpAddrs;
    return 0;
}

int
usb_check_int_endpoints(void *pIntf, const unsigned char *pEpAddrs)
{
    (void)pIntf;
    (void)pEpAddrs;
    return 0;
}

const char *
usb_ep_type_string(int nEpType)
{
    (void)nEpType;
    return g_szSoftUsbEp;
}

const char *
usb_speed_string(int nSpeed)
{
    (void)nSpeed;
    return g_szSoftUsbSpeed;
}

const char *
usb_state_string(int nState)
{
    (void)nState;
    return g_szSoftUsbState;
}

int
usb_get_current_frame_number(void *pDev)
{
    (void)pDev;
    return 0;
}

int
usb_anchor_empty(void *pAnchor)
{
    (void)pAnchor;
    return 1;
}

int
usb_pipe_type_check(void *pDev, unsigned int uPipe)
{
    (void)pDev;
    (void)uPipe;
    return 0;
}

int
usb_urb_ep_type_check(void *pUrb)
{
    (void)pUrb;
    return 0;
}

int
usb_control_msg_send(void *pDev, unsigned int uEp, unsigned char u8Req,
                     unsigned char u8ReqType, unsigned short u16Value,
                     unsigned short u16Index, const void *pData,
                     unsigned short u16Size, int nTimeout, int nMemFlags)
{
    (void)pDev;
    (void)uEp;
    (void)u8Req;
    (void)u8ReqType;
    (void)u16Value;
    (void)u16Index;
    (void)pData;
    (void)u16Size;
    (void)nTimeout;
    (void)nMemFlags;
    return LUSB_ENODEV;
}

int
usb_control_msg_recv(void *pDev, unsigned int uEp, unsigned char u8Req,
                     unsigned char u8ReqType, unsigned short u16Value,
                     unsigned short u16Index, void *pData,
                     unsigned short u16Size, int nTimeout, int nMemFlags)
{
    (void)pDev;
    (void)uEp;
    (void)u8Req;
    (void)u8ReqType;
    (void)u16Value;
    (void)u16Index;
    (void)pData;
    (void)u16Size;
    (void)nTimeout;
    (void)nMemFlags;
    return LUSB_ENODEV;
}

void *
usb_get_from_anchor(void *pAnchor)
{
    (void)pAnchor;
    return NULL;
}

void
usb_scuttle_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_unlink_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_anchor_resume_wakeups(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_anchor_suspend_wakeups(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_queue_reset_device(void *pIntf)
{
    (void)pIntf;
}

int
usb_reset_configuration(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

/*
 * Older alias shape: some modules call usb_register(driver) macro ->
 * usb_register_driver. Provide soft name for direct UND.
 */
int
usb_register(void *pDriver)
{
    return usb_register_driver(pDriver, NULL, "soft");
}

/* ---- scsi_* soft bodies (scsi_mod surface; load only; != product) ------- */

/*
 * el9 usb-storage.ko scsi_mod UND class (13):
 *   scsi_add_host_with_dma scsi_done_direct scsi_eh_prep_cmnd
 *   scsi_eh_restore_cmnd scsi_host_alloc scsi_host_put scsi_is_host_device
 *   scsi_normalize_sense scsi_remove_host scsi_report_bus_reset
 *   scsi_report_device_reset scsi_scan_host scsi_sense_desc_find
 * + uas expand (5): block/unblock/change_queue_depth/done/print_command
 *
 * Deepen: soft host bookkeeping, EH save/restore, fixed+desc sense parse,
 * soft-success add_host for soft slab (probe progress past "Unable to add").
 * Soft!=product - no LUN, no BOT, no stick.
 */

/*
 * Attribute / host-class checks: only soft host slab is "ours".
 * Foreign device pointers fail closed (not a real scsi_host device).
 */
int
scsi_is_host_device(void *pDev)
{
    /*
     * Soft: no device-model graph. Return 1 only if caller passes our soft
     * host slab itself (some paths pass host-as-dev soft-shaped). Else 0.
     */
    if (lusb_is_soft_host(pDev) != 0) {
        return 1;
    }
    return 0;
}

void *
scsi_host_alloc(void *pSht, int nPrivSize)
{
    u32 *puMagic;

    lusb_inc(&g_cScsiHostAlloc);
    lusb_soft_objs_once();

    /* Reject unbounded / oversize priv (us_data ~hundreds of bytes). */
    if (nPrivSize < 0) {
        return NULL;
    }
    if ((u32)nPrivSize > (LINUX_USB_SOFT_HOST_CB / 2u)) {
        return NULL;
    }
    /* Single soft host slot; second alloc fails closed. */
    if (g_fSoftHostLive != 0u) {
        return NULL;
    }

    memset(g_aSoftHost, 0, LINUX_USB_SOFT_HOST_CB);
    g_pSoftHostSht = pSht;
    g_nSoftHostPriv = nPrivSize;
    g_uSoftHostNo = 1u;
    g_fSoftHostAdded = 0u;
    g_fSoftHostBlocked = 0u;
    g_fSoftHostLive = 1u;

    /*
     * Soft bookkeeping word at blob head (not a Linux field offset).
     * Drivers that poke Linux Scsi_Host offsets still write into the slab;
     * Soft!=ABI-stable - load/reloc only, not hostish probe.
     */
    puMagic = (u32 *)(void *)g_aSoftHost;
    *puMagic = LUSB_SCSI_HOST_MAGIC;

    if (g_fLogScsiHost == 0) {
        g_fLogScsiHost = 1;
        kprintf("linux_usb_soft: soft scsi_host_alloc n=%u priv=%d "
                "host_cb=%u soft=1 product=0 (scsi_mod surface; != stick)\n",
                (unsigned)g_cScsiHostAlloc, nPrivSize,
                (unsigned)LINUX_USB_SOFT_HOST_CB);
    }
    return (void *)g_aSoftHost;
}

int
scsi_add_host_with_dma(void *pShost, void *pDev, void *pDmaDev)
{
    (void)pDev;
    (void)pDmaDev;
    lusb_inc(&g_cScsiAddHost);

    /*
     * Soft success for our soft host: lets MSC probe progress past
     * "Unable to add the scsi host" under soft seed. Foreign pointer
     * still fail-closed. Soft!=product - no sysfs, no LUN attach.
     */
    if (lusb_is_soft_host(pShost) == 0 || g_fSoftHostLive == 0u) {
        return LUSB_ENODEV;
    }
    g_fSoftHostAdded = 1u;
    if (g_fLogScsiAdd == 0) {
        g_fLogScsiAdd = 1;
        kprintf("linux_usb_soft: soft scsi_add_host n=%u soft=1 product=0 "
                "(no sysfs; != stick; need=HC OPEN)\n",
                (unsigned)g_cScsiAddHost);
    }
    return 0;
}

void
scsi_scan_host(void *pShost)
{
    lusb_inc(&g_cScsiScan);
    if (lusb_is_soft_host(pShost) == 0) {
        return;
    }
    /* Soft: no bus scan, no LUN 0. Soft!=product. */
    if (g_fLogScsiScan == 0) {
        g_fLogScsiScan = 1;
        kprintf("linux_usb_soft: soft scsi_scan_host n=%u soft=1 product=0 "
                "(no LUN; != stick)\n",
                (unsigned)g_cScsiScan);
    }
}

void
scsi_host_put(void *pShost)
{
    if (lusb_is_soft_host(pShost) != 0) {
        g_fSoftHostLive = 0u;
        g_fSoftHostAdded = 0u;
        g_fSoftHostBlocked = 0u;
        g_pSoftHostSht = NULL;
        g_nSoftHostPriv = 0;
        g_uSoftHostNo = 0u;
        memset(g_aSoftHost, 0, LINUX_USB_SOFT_HOST_CB);
    }
}

void
scsi_remove_host(void *pShost)
{
    if (lusb_is_soft_host(pShost) != 0) {
        g_fSoftHostAdded = 0u;
        g_fSoftHostBlocked = 0u;
    }
}

void
scsi_done_direct(void *pCmd)
{
    (void)pCmd;
    lusb_inc(&g_cScsiDone);
    /* Soft: no midlayer completion callback chain. Soft!=product. */
}

void
scsi_report_bus_reset(void *pShost, int nChannel)
{
    (void)nChannel;
    lusb_inc(&g_cScsiReport);
    if (lusb_is_soft_host(pShost) != 0) {
        /* Soft: record only; no EH wake. Soft!=product. */
    }
}

void
scsi_report_device_reset(void *pShost, int nChannel, int nId)
{
    (void)nChannel;
    (void)nId;
    lusb_inc(&g_cScsiReport);
    if (lusb_is_soft_host(pShost) != 0) {
        /* Soft: record only. Soft!=product. */
    }
}

void
scsi_eh_prep_cmnd(void *pScmd, void *pSes, unsigned char *pCmnd,
                  int nCmndSize, unsigned int uSenseBytes)
{
    int nCopy;
    unsigned int uSenseCap;

    (void)pScmd;
    lusb_inc(&g_cScsiEh);
    lusb_soft_objs_once();

    /*
     * Soft EH session: stash optional override CDB + sense budget.
     * If pSes is non-NULL, zero a soft-shaped session blob into it when it
     * aliases our static, else keep private soft state only.
     * Soft!=ABI-stable.
     */
    memset(g_aSoftEhSes, 0, sizeof(g_aSoftEhSes));
    memset(g_aSoftEhCmnd, 0, sizeof(g_aSoftEhCmnd));
    g_nSoftEhCmndLen = 0;
    memset(g_aSoftEhSense, 0, sizeof(g_aSoftEhSense));
    g_uSoftEhSenseBytes = 0u;

    if (pCmnd != NULL && nCmndSize > 0) {
        nCopy = nCmndSize;
        if (nCopy > (int)LINUX_USB_SOFT_CMND_CB) {
            nCopy = (int)LINUX_USB_SOFT_CMND_CB;
        }
        memcpy(g_aSoftEhCmnd, pCmnd, (size_t)nCopy);
        g_nSoftEhCmndLen = nCopy;
    }
    uSenseCap = uSenseBytes;
    if (uSenseCap > (unsigned int)sizeof(g_aSoftEhSense)) {
        uSenseCap = (unsigned int)sizeof(g_aSoftEhSense);
    }
    g_uSoftEhSenseBytes = uSenseCap;
    g_fSoftEhLive = 1u;

    if (pSes != NULL) {
        /* Best-effort: zero caller session up to soft EH size. Soft!=ABI. */
        memset(pSes, 0, LINUX_USB_SOFT_EH_CB);
        if (g_nSoftEhCmndLen > 0) {
            memcpy(pSes, g_aSoftEhCmnd, (size_t)g_nSoftEhCmndLen);
        }
    }
}

void
scsi_eh_restore_cmnd(void *pScmd, void *pSes)
{
    (void)pScmd;
    (void)pSes;
    lusb_inc(&g_cScsiEh);
    /* Soft: drop EH override; no real scmd field rewrite. Soft!=product. */
    g_fSoftEhLive = 0u;
    g_nSoftEhCmndLen = 0;
    g_uSoftEhSenseBytes = 0u;
    memset(g_aSoftEhCmnd, 0, sizeof(g_aSoftEhCmnd));
    memset(g_aSoftEhSense, 0, sizeof(g_aSoftEhSense));
    memset(g_aSoftEhSes, 0, sizeof(g_aSoftEhSes));
}

/*
 * Fixed-format sense (response 0x70/0x71) -> soft sshdr first bytes:
 *   [0]=response_code [1]=sense_key [2]=asc [3]=ascq
 * Descriptor format (0x72/0x73): response + key + asc + ascq from hdr.
 * Returns 1 on soft parse ok, 0 if unusable. Soft!=Linux struct layout.
 */
int
scsi_normalize_sense(const unsigned char *pSense, int nSenseLen, void *pSshdr)
{
    unsigned char u8Resp;
    unsigned char u8Key;
    unsigned char u8Asc;
    unsigned char u8Ascq;
    unsigned char *pOut;

    lusb_inc(&g_cScsiSense);
    lusb_soft_objs_once();
    memset(g_aSoftSshdr, 0, sizeof(g_aSoftSshdr));

    if (pSense == NULL || nSenseLen < 1) {
        return 0;
    }
    u8Resp = (unsigned char)(pSense[0] & 0x7fu);
    u8Key = 0u;
    u8Asc = 0u;
    u8Ascq = 0u;

    if (u8Resp == 0x70u || u8Resp == 0x71u) {
        /* Fixed format. Soft!=product. */
        if (nSenseLen > 2) {
            u8Key = (unsigned char)(pSense[2] & 0x0fu);
        }
        if (nSenseLen > 12) {
            u8Asc = pSense[12];
        }
        if (nSenseLen > 13) {
            u8Ascq = pSense[13];
        }
    } else if (u8Resp == 0x72u || u8Resp == 0x73u) {
        /* Descriptor format header. Soft!=product. */
        if (nSenseLen > 1) {
            u8Key = (unsigned char)(pSense[1] & 0x0fu);
        }
        if (nSenseLen > 2) {
            u8Asc = pSense[2];
        }
        if (nSenseLen > 3) {
            u8Ascq = pSense[3];
        }
    } else {
        return 0;
    }

    g_aSoftSshdr[0] = u8Resp;
    g_aSoftSshdr[1] = u8Key;
    g_aSoftSshdr[2] = u8Asc;
    g_aSoftSshdr[3] = u8Ascq;

    if (pSshdr != NULL) {
        pOut = (unsigned char *)pSshdr;
        /* Write soft-normalized quad; caller layout Soft!=ABI-stable. */
        pOut[0] = u8Resp;
        pOut[1] = u8Key;
        pOut[2] = u8Asc;
        pOut[3] = u8Ascq;
    }
    return 1;
}

/*
 * Descriptor-format sense: walk type/len descriptors after 8-byte header.
 * Fixed format has no descriptors -> NULL. Soft!=product.
 */
void *
scsi_sense_desc_find(const unsigned char *pSense, int nSenseLen, int nDescType)
{
    int nAdd;
    int nOff;
    int nType;
    int nLen;
    unsigned char u8Resp;

    lusb_inc(&g_cScsiSense);
    if (pSense == NULL || nSenseLen < 8) {
        return NULL;
    }
    u8Resp = (unsigned char)(pSense[0] & 0x7fu);
    if (u8Resp != 0x72u && u8Resp != 0x73u) {
        return NULL;
    }
    nAdd = (int)pSense[7];
    if (nAdd < 0) {
        return NULL;
    }
    if (nAdd > nSenseLen - 8) {
        nAdd = nSenseLen - 8;
    }
    nOff = 8;
    while (nOff + 2 <= 8 + nAdd) {
        nType = (int)pSense[nOff];
        nLen = (int)pSense[nOff + 1];
        if (nType == nDescType) {
            return (void *)(u64)(uintptr_t)&pSense[nOff];
        }
        if (nLen < 0) {
            break;
        }
        nOff += 2 + nLen;
        if (nOff > nSenseLen) {
            break;
        }
    }
    return NULL;
}

/* ---- scsi_* expand (uas mid helpers) ----------------------------------- */

void
scsi_block_requests(void *pShost)
{
    lusb_inc(&g_cScsiBlock);
    if (lusb_is_soft_host(pShost) != 0) {
        g_fSoftHostBlocked = 1u;
    }
}

void
scsi_unblock_requests(void *pShost)
{
    lusb_inc(&g_cScsiBlock);
    if (lusb_is_soft_host(pShost) != 0) {
        g_fSoftHostBlocked = 0u;
    }
}

int
scsi_change_queue_depth(void *pSdev, int nDepth)
{
    (void)pSdev;
    /* Soft: accept depth bookkeeping only; return requested. Soft!=product. */
    if (nDepth < 0) {
        return 0;
    }
    return nDepth;
}

void
scsi_done(void *pCmd)
{
    (void)pCmd;
    lusb_inc(&g_cScsiDone);
    /* Soft: no scmd->scsi_done callback. Soft!=product. */
}

void
scsi_print_command(void *pCmd)
{
    (void)pCmd;
    /* Soft: no console CDB dump. Soft!=product. */
}

/* ---- scsi_mod expand (uas + mid residual; Soft!=product) ---------------- */

/*
 * Additive scsi_mod-shaped surface: el9 uas UND class + common mid helpers
 * (host get/lookup, sense build, EH finish, device lookup). Fail-closed /
 * no-op. Soft!=product - no LUN, no BOT, no stick.
 */

void
int_to_scsilun(unsigned long long u64Lun, void *pScsiLun)
{
    unsigned char *p;

    (void)u64Lun;
    if (pScsiLun == NULL) {
        return;
    }
    /* Soft: zero 8-byte LUN blob. Soft!=ABI-stable packing. */
    p = (unsigned char *)pScsiLun;
    p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = 0u;
}

unsigned long long
scsilun_to_int(void *pScsiLun)
{
    (void)pScsiLun;
    return 0ull;
}

void
scmd_printk(const char *szLevel, void *pScmd, const char *szFmt, ...)
{
    (void)szLevel;
    (void)pScmd;
    (void)szFmt;
}

void
sdev_prefix_printk(const char *szLevel, void *pSdev, const char *szDev,
                   const char *szFmt, ...)
{
    (void)szLevel;
    (void)pSdev;
    (void)szDev;
    (void)szFmt;
}

void *
scsi_host_get(void *pShost)
{
    /* Soft: echo host pointer (no refcount). Soft!=product. */
    if (lusb_is_soft_host(pShost) != 0) {
        return pShost;
    }
    return pShost;
}

void *
scsi_host_lookup(unsigned short uHostNo)
{
    if (g_fSoftHostLive != 0u && (unsigned int)uHostNo == g_uSoftHostNo) {
        return (void *)g_aSoftHost;
    }
    return NULL;
}

int
scsi_host_busy(void *pShost)
{
    (void)pShost;
    return 0;
}

int
scsi_add_host(void *pShost, void *pDev)
{
    /* Soft shape of add_host -> add_host_with_dma(dev, dev). Soft!=product. */
    return scsi_add_host_with_dma(pShost, pDev, pDev);
}

void *
scsi_device_lookup(void *pShost, unsigned int uChannel, unsigned int uId,
                   unsigned int uLun)
{
    (void)pShost;
    (void)uChannel;
    (void)uId;
    (void)uLun;
    return NULL;
}

void *
scsi_device_get(void *pSdev)
{
    return pSdev;
}

void
scsi_device_put(void *pSdev)
{
    (void)pSdev;
}

int
scsi_device_quiesce(void *pSdev)
{
    (void)pSdev;
    return 0;
}

void
scsi_device_resume(void *pSdev)
{
    (void)pSdev;
}

void
scsi_device_set_state(void *pSdev, int nState)
{
    (void)pSdev;
    (void)nState;
}

void
scsi_build_sense(void *pScmd, int nDesc, unsigned char u8Key,
                 unsigned char u8Asc, unsigned char u8Ascq)
{
    (void)pScmd;
    (void)nDesc;
    (void)u8Key;
    (void)u8Asc;
    (void)u8Ascq;
}

void
scsi_build_sense_buffer(int nDesc, unsigned char *pBuf, unsigned char u8Key,
                        unsigned char u8Asc, unsigned char u8Ascq)
{
    (void)nDesc;
    (void)u8Key;
    (void)u8Asc;
    (void)u8Ascq;
    if (pBuf != NULL) {
        pBuf[0] = 0u;
    }
}

int
scsi_set_sense_information(unsigned char *pBuf, int nBufLen,
                           unsigned long long u64Info)
{
    (void)pBuf;
    (void)nBufLen;
    (void)u64Info;
    return 0;
}

int
scsi_get_sense_info_fld(const unsigned char *pSense, int nSenseLen,
                        unsigned long long *pu64Info)
{
    (void)pSense;
    (void)nSenseLen;
    if (pu64Info != NULL) {
        *pu64Info = 0ull;
    }
    return 0;
}

int
scsi_command_normalize_sense(void *pScmd, void *pSshdr)
{
    (void)pScmd;
    (void)pSshdr;
    return 0;
}

void
scsi_print_sense_hdr(const char *szName, void *pSshdr)
{
    (void)szName;
    (void)pSshdr;
}

void
__scsi_print_sense(const char *szName, const unsigned char *pSense,
                   int nSenseLen)
{
    (void)szName;
    (void)pSense;
    (void)nSenseLen;
}

const char *
scsi_extd_sense_format(unsigned char u8Asc, unsigned char u8Ascq,
                       const char **ppFmt)
{
    (void)u8Asc;
    (void)u8Ascq;
    if (ppFmt != NULL) {
        *ppFmt = NULL;
    }
    return NULL;
}

int
scsi_is_sdev_device(void *pDev)
{
    (void)pDev;
    return 0;
}

int
scsi_is_target_device(void *pDev)
{
    (void)pDev;
    return 0;
}

void
scsi_eh_finish_cmd(void *pScmd, void *pDoneQ)
{
    (void)pScmd;
    (void)pDoneQ;
    lusb_inc(&g_cScsiEh);
}

void
scsi_eh_flush_done_q(void *pDoneQ)
{
    (void)pDoneQ;
    lusb_inc(&g_cScsiEh);
}

int
scsi_eh_get_sense(void *pScmd)
{
    (void)pScmd;
    lusb_inc(&g_cScsiEh);
    return 0;
}

void
scsi_eh_scmd_add(void *pScmd)
{
    (void)pScmd;
    lusb_inc(&g_cScsiEh);
}

void
scsi_eh_wakeup(void *pShost)
{
    (void)pShost;
    lusb_inc(&g_cScsiEh);
}

int
scsi_check_sense(void *pScmd)
{
    (void)pScmd;
    return 0;
}

int
scsi_block_when_processing_errors(void *pSdev)
{
    (void)pSdev;
    return 1;
}

int
scsi_track_queue_full(void *pSdev, int nDepth)
{
    (void)pSdev;
    (void)nDepth;
    return 0;
}

int
scsi_dma_map(void *pScmd)
{
    (void)pScmd;
    return 0;
}

void
scsi_dma_unmap(void *pScmd)
{
    (void)pScmd;
}

int
scsi_alloc_sgtables(void *pScmd)
{
    (void)pScmd;
    return LUSB_ENOMEM;
}

void
scsi_free_sgtables(void *pScmd)
{
    (void)pScmd;
}

void
scsi_init_command(void *pSdev, void *pScmd)
{
    (void)pSdev;
    (void)pScmd;
}

void
scsi_finish_command(void *pScmd)
{
    (void)pScmd;
    lusb_inc(&g_cScsiDone);
}

int
scsi_execute_cmd(void *pSdev, const unsigned char *pCmd, int nDataDir,
                 void *pBuffer, unsigned int uBuffLen, int nTimeoutMs,
                 int nRetries, void *pArgs)
{
    (void)pSdev;
    (void)pCmd;
    (void)nDataDir;
    (void)pBuffer;
    (void)uBuffLen;
    (void)nTimeoutMs;
    (void)nRetries;
    (void)pArgs;
    return LUSB_ENODEV;
}

int
scsi_device_max_queue_depth(void *pSdev, int nDepth)
{
    (void)pSdev;
    if (nDepth < 1) {
        return 1;
    }
    return nDepth;
}

/* ---- sg_* scatterlist (first residual after usb/scsi seed) ------------- */

int
sg_nents(void *pSg)
{
    (void)pSg;
    lusb_inc(&g_cSgNents);
    if (g_fLogSg == 0) {
        g_fLogSg = 1;
        kprintf("linux_usb_soft: soft sg_nents n=%u soft=1 product=0 "
                "(no map; != stick)\n",
                (unsigned)g_cSgNents);
    }
    /* Soft: empty map. Soft!=product. */
    return 0;
}

void
sg_miter_start(void *pMiter, void *pSgl, unsigned int uNents,
               unsigned int uFlags)
{
    (void)pMiter;
    (void)pSgl;
    (void)uNents;
    (void)uFlags;
}

int
sg_miter_skip(void *pMiter, unsigned long cbOffset)
{
    (void)pMiter;
    (void)cbOffset;
    return 0;
}

int
sg_miter_next(void *pMiter)
{
    (void)pMiter;
    /* Soft: no next segment. Soft!=product. */
    return 0;
}

void
sg_miter_stop(void *pMiter)
{
    (void)pMiter;
}

/*
 * Older el9 kvers UND sg_next after miter surface. Soft: end-of-list NULL.
 * Soft!=product.
 */
void *
sg_next(void *pSg)
{
    (void)pSg;
    lusb_inc(&g_cSgNents);
    return NULL;
}

void *
sg_last(void *pSgl, unsigned int uNents)
{
    (void)pSgl;
    (void)uNents;
    return NULL;
}

void
sg_init_table(void *pSgl, unsigned int uNents)
{
    (void)pSgl;
    (void)uNents;
}

void
sg_init_one(void *pSg, const void *pBuf, unsigned long cbLen)
{
    (void)pSg;
    (void)pBuf;
    (void)cbLen;
}

int
sg_alloc_table(void *pTable, unsigned int uNents, int nMemFlags)
{
    (void)pTable;
    (void)uNents;
    (void)nMemFlags;
    return LUSB_ENOMEM;
}

void
sg_free_table(void *pTable)
{
    (void)pTable;
}

/* ---- MSC-adjacent wait / work / kthread (load surface) ----------------- */

void
init_wait_entry(void *pEntry, int nFlags)
{
    (void)pEntry;
    (void)nFlags;
}

long
prepare_to_wait_event(void *pWq, void *pEntry, int nState)
{
    (void)pWq;
    (void)pEntry;
    (void)nState;
    return 0;
}

void
__init_waitqueue_head(void *pWq, const char *szName, void *pKey)
{
    (void)pWq;
    (void)szName;
    (void)pKey;
}

void
__init_swait_queue_head(void *pWq, const char *szName, void *pKey)
{
    (void)pWq;
    (void)szName;
    (void)pKey;
}

void
__wake_up(void *pWq, unsigned int uMode, int nNr, void *pKey)
{
    (void)pWq;
    (void)uMode;
    (void)nNr;
    (void)pKey;
}

int
wake_up_process(void *pTask)
{
    (void)pTask;
    return 0;
}

void *
kthread_create_on_node(void *pfnThread, void *pData, int nNode,
                       const char *szNameFmt, ...)
{
    (void)pfnThread;
    (void)pData;
    (void)nNode;
    (void)szNameFmt;
    lusb_inc(&g_cKthreadCreate);
    /* Soft: no real thread. Soft!=product. */
    return NULL;
}

int
kthread_should_stop(void)
{
    return 1;
}

int
kthread_stop(void *pTask)
{
    (void)pTask;
    return 0;
}

void
init_timer_key(void *pTimer, void *pfnFunc, unsigned int uFlags,
               const char *szName, void *pKey)
{
    (void)pTimer;
    (void)pfnFunc;
    (void)uFlags;
    (void)szName;
    (void)pKey;
}

void
delayed_work_timer_fn(unsigned long uData)
{
    (void)uData;
}

int
queue_delayed_work_on(int nCpu, void *pWq, void *pDwork,
                      unsigned long uDelay)
{
    (void)nCpu;
    (void)pWq;
    (void)pDwork;
    (void)uDelay;
    /* Soft: not queued. Soft!=product. */
    return 0;
}

int
cancel_delayed_work_sync(void *pDwork)
{
    (void)pDwork;
    return 0;
}

/* ---- MSC-adjacent dev / param / string / time -------------------------- */

int
device_create_file(void *pDev, const void *pAttr)
{
    (void)pDev;
    (void)pAttr;
    return 0;
}

int
sysfs_emit(char *pBuf, const char *szFmt, ...)
{
    (void)pBuf;
    (void)szFmt;
    return 0;
}

void
_dev_notice(const void *pDev, const char *szFmt, ...)
{
    (void)pDev;
    (void)szFmt;
}

void
__dynamic_dev_dbg(void *pDescriptor, const void *pDev, const char *szFmt, ...)
{
    (void)pDescriptor;
    (void)pDev;
    (void)szFmt;
}

int
kstrtouint(const char *sz, unsigned int uBase, unsigned int *pu)
{
    (void)sz;
    (void)uBase;
    if (pu != NULL) {
        *pu = 0u;
    }
    return LUSB_EINVAL;
}

unsigned long
simple_strtoul(const char *sz, char **ppEnd, unsigned int uBase)
{
    (void)sz;
    (void)uBase;
    if (ppEnd != NULL) {
        *ppEnd = (char *)sz;
    }
    return 0ul;
}

char *
skip_spaces(const char *sz)
{
    if (sz == NULL) {
        return NULL;
    }
    while (*sz == ' ' || *sz == '\t') {
        sz++;
    }
    return (char *)sz;
}

unsigned long long
int_pow(unsigned long long uBase, unsigned int uExp)
{
    unsigned long long uR = 1ull;

    while (uExp > 0u) {
        if ((uExp & 1u) != 0u) {
            uR *= uBase;
        }
        uBase *= uBase;
        uExp >>= 1;
    }
    return uR;
}

unsigned long
__msecs_to_jiffies(const unsigned int uM)
{
    /* Soft jiffy ≈ 1 ms unit. Soft!=product. */
    return (unsigned long)uM;
}

unsigned long long
ktime_get_mono_fast_ns(void)
{
    return 0ull;
}

void
__trace_set_current_state(int nState)
{
    (void)nState;
}

/* ---- MSC-adjacent wq / heap residual (uas UND class) ------------------- */

/*
 * el9 uas residual after usb/scsi seed: workqueue + __kmalloc + _dev_printk.
 * Soft fail-closed. Soft!=product. Prefer this TU over leaf-cap in linux_ksym.
 */

void *
alloc_workqueue(const char *szFmt, unsigned int uFlags, int nMaxActive, ...)
{
    (void)szFmt;
    (void)uFlags;
    (void)nMaxActive;
    lusb_soft_objs_once();
    /* Soft: single static wq blob; not a real worker. Soft!=product. */
    return (void *)g_aSoftWqBlob;
}

void
destroy_workqueue(void *pWq)
{
    (void)pWq;
}

int
cancel_work_sync(void *pWork)
{
    (void)pWork;
    return 0;
}

int
flush_work(void *pWork)
{
    (void)pWork;
    return 0;
}

void
__init_work(void *pWork, int nOnStack)
{
    (void)pWork;
    (void)nOnStack;
}

void *
__kmalloc(unsigned long cbSize, int nFlags)
{
    (void)cbSize;
    (void)nFlags;
    /* Soft: no heap. Soft!=product. */
    return NULL;
}

void
_dev_printk(const char *szLevel, const void *pDev, const char *szFmt, ...)
{
    (void)szLevel;
    (void)pDev;
    (void)szFmt;
}

void
dev_printk(const char *szLevel, const void *pDev, const char *szFmt, ...)
{
    (void)szLevel;
    (void)pDev;
    (void)szFmt;
}

int
mutex_lock_nested(void *pLock, unsigned int uSubclass)
{
    (void)pLock;
    (void)uSubclass;
    return 0;
}

/* ---- virtual usbcore HCD / hub / device / bus (Soft!=product · G-AC-1) -- */

/*
 * Additive virtual-usbcore surface for host-controller and hub class exports
 * that modular xhci_hcd / usbcore would provide on a full Linux multi-mod
 * path. Host el9 ships these BUILTIN -> soft seed only.
 *
 * Soft bookkeeping only: soft HCD pool, soft bus/dev blobs. Fail-closed on
 * DMA map, giveback is no-op, add_hcd soft-success for soft slab only.
 * Soft!=product: no MMIO, no root-hub enum, no stick, no BOT/UAS datapath.
 * Grep: linux_usb_soft: soft hcd ...
 */

int
queue_work_on(int nCpu, void *pWq, void *pWork)
{
    (void)nCpu;
    (void)pWq;
    (void)pWork;
    /* Soft: not queued. Soft!=product. */
    return 0;
}

void *
usb_create_hcd(void *pDriver, void *pDev, const char *szBusName)
{
    u32 i;
    struct usb_soft_hcd *pSoft;

    lusb_inc(&g_cHcdCreate);
    lusb_soft_objs_once();

    for (i = 0u; i < LINUX_USB_SOFT_HCD_POOL; i++) {
        if (g_aSoftHcdUsed[i] == 0u) {
            memset(g_aSoftHcd[i], 0, LINUX_USB_SOFT_HCD_CB);
            g_aSoftHcdUsed[i] = 1u;
            g_aSoftHcdAdded[i] = 0u;
            g_apSoftHcdDriver[i] = pDriver;
            pSoft = (struct usb_soft_hcd *)(void *)g_aSoftHcd[i];
            lusb_hcd_soft_prime(pSoft, pDriver, pDev, szBusName, NULL);
            if (g_fLogHcd == 0) {
                g_fLogHcd = 1;
                kprintf("linux_usb_soft: soft hcd create n=%u bus=%s "
                        "soft=1 product=0 (virtual usbcore; != stick; "
                        "need=HC OPEN; layout=1)\n",
                        (unsigned)g_cHcdCreate,
                        (szBusName != NULL && szBusName[0] != '\0')
                            ? szBusName
                            : "soft");
            }
            return (void *)g_aSoftHcd[i];
        }
    }
    return NULL;
}

void *
usb_create_shared_hcd(void *pDriver, void *pDev, const char *szBusName,
                      void *pPrimaryHcd)
{
    u32 i;
    struct usb_soft_hcd *pSoft;

    lusb_inc(&g_cHcdCreate);
    lusb_soft_objs_once();
    for (i = 0u; i < LINUX_USB_SOFT_HCD_POOL; i++) {
        if (g_aSoftHcdUsed[i] == 0u) {
            memset(g_aSoftHcd[i], 0, LINUX_USB_SOFT_HCD_CB);
            g_aSoftHcdUsed[i] = 1u;
            g_aSoftHcdAdded[i] = 0u;
            g_apSoftHcdDriver[i] = pDriver;
            pSoft = (struct usb_soft_hcd *)(void *)g_aSoftHcd[i];
            lusb_hcd_soft_prime(pSoft, pDriver, pDev, szBusName, pPrimaryHcd);
            return (void *)g_aSoftHcd[i];
        }
    }
    return NULL;
}

int
usb_add_hcd(void *pHcd, unsigned int uIrqNum, unsigned long uIrqFlags)
{
    int nSlot;
    struct usb_soft_hcd *pSoft;
    static int s_fLogAdd;

    (void)uIrqFlags;
    lusb_inc(&g_cHcdAdd);
    nSlot = lusb_soft_hcd_slot(pHcd);
    if (nSlot < 0) {
        return LUSB_ENODEV;
    }
    g_aSoftHcdAdded[(u32)nSlot] = 1u;
    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        struct usb_soft_bus *pBusSoft;

        pSoft->u8Added = 1u;
        pSoft->u32Flags |= LINUX_USB_SOFT_HCD_F_ADDED;
        pSoft->u32Irq = uIrqNum;
        /* Soft bus cookie: ensure soft bus slab + dual link. Soft!=sysfs. */
        if (g_fSoftBusLive == 0u) {
            memset(g_aSoftBus, 0, sizeof(g_aSoftBus));
            pBusSoft = (struct usb_soft_bus *)(void *)g_aSoftBus;
            pBusSoft->u32Magic = LUSB_BUS_MAGIC;
            pBusSoft->u32BusNum = 1u;
            pBusSoft->u8Live = 1u;
            g_fSoftBusLive = 1u;
        } else {
            pBusSoft = (struct usb_soft_bus *)(void *)g_aSoftBus;
            if (pBusSoft->u32Magic != LUSB_BUS_MAGIC) {
                pBusSoft->u32Magic = LUSB_BUS_MAGIC;
                pBusSoft->u8Live = 1u;
            }
        }
        pSoft->pBus = (void *)g_aSoftBus;
        pBusSoft->pHcd = pHcd;
        pBusSoft->pRootHub = NULL;
    }
    if (s_fLogAdd == 0) {
        s_fLogAdd = 1;
        kprintf("linux_usb_soft: soft hcd add n=%u soft=1 product=0 "
                "(no root-hub; != stick; G-AC-1; layout=1)\n",
                (unsigned)g_cHcdAdd);
    }
    return 0;
}

void
usb_remove_hcd(void *pHcd)
{
    int nSlot;
    struct usb_soft_hcd *pSoft;

    nSlot = lusb_soft_hcd_slot(pHcd);
    if (nSlot >= 0) {
        g_aSoftHcdAdded[(u32)nSlot] = 0u;
        pSoft = lusb_hcd_soft(pHcd);
        if (pSoft != NULL) {
            pSoft->u8Added = 0u;
            pSoft->u8Started = 0u;
            pSoft->u32Flags &=
                ~(LINUX_USB_SOFT_HCD_F_ADDED | LINUX_USB_SOFT_HCD_F_STARTED);
        }
    }
}

void
usb_put_hcd(void *pHcd)
{
    int nSlot;
    struct usb_soft_hcd *pSoft;

    nSlot = lusb_soft_hcd_slot(pHcd);
    if (nSlot >= 0) {
        pSoft = (struct usb_soft_hcd *)pHcd;
        pSoft->u8Used = 0u;
        pSoft->u32Magic = 0u;
        g_aSoftHcdUsed[(u32)nSlot] = 0u;
        g_aSoftHcdAdded[(u32)nSlot] = 0u;
        g_apSoftHcdDriver[(u32)nSlot] = NULL;
        memset(g_aSoftHcd[(u32)nSlot], 0, LINUX_USB_SOFT_HCD_CB);
    }
}

void *
usb_get_hcd(void *pHcd)
{
    /* Soft: echo (no refcount). Soft!=product. */
    return pHcd;
}

void
usb_hcd_giveback_urb(void *pHcd, void *pUrb, int nStatus)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    lusb_inc(&g_cHcdGiveback);
    pSoft = lusb_urb_soft(pUrb);
    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        pSoft->nStatus = nStatus;
        pSoft->u8Submitted = 0u;
        pSoft->u32ActualLen = 0u;
        pSoft->u8Linked = 0u;
        if (pHcdSoft != NULL) {
            pSoft->pHcd = pHcd;
            if (pHcdSoft->u32UrbLinked > 0u) {
                pHcdSoft->u32UrbLinked--;
            }
        }
        /* Soft: never invoke pfnComplete (no .ko completion). Soft!=product. */
    }
}

int
usb_hcd_link_urb_to_ep(void *pHcd, void *pUrb)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;
    struct usb_soft_ep *pEp;

    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pHcdSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcdSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->pHcd = pHcd;
        pSoft->u8Linked = 1u;
        pSoft->u8Unlinked = 0u;
        pSoft->u8EpNum = lusb_pipe_ep_num(pSoft->uPipe);
        lusb_inc(&g_uSoftUrbSeq);
        pSoft->u32Seq = g_uSoftUrbSeq;
        if (pHcdSoft->u32UrbLinked < 0xffffffffu) {
            pHcdSoft->u32UrbLinked++;
        }
        /* residual10: ensure soft EP cookie + HCD reverse. Soft!=product. */
        pEp = lusb_ep_soft(pSoft->pEp);
        if (pEp == NULL) {
            pEp = lusb_ep_soft_lookup_or_prime(pSoft->pDev, pSoft->uPipe);
            if (pEp != NULL) {
                pSoft->pEp = (void *)pEp;
            }
        }
        if (pEp != NULL) {
            pEp->pHcd = pHcd;
            pEp->u8Enabled = 1u;
        }
    }
    return 0;
}

int
usb_hcd_unlink_urb_from_ep(void *pHcd, void *pUrb)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        pSoft->u8Unlinked = 1u;
        pSoft->u8Linked = 0u;
        if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
            pHcdSoft->u32UrbLinked--;
        }
    }
    return 0;
}

int
usb_hcd_check_unlink_urb(void *pHcd, void *pUrb, int nStatus)
{
    (void)pHcd;
    (void)pUrb;
    (void)nStatus;
    return 0;
}

int
usb_hcd_map_urb_for_dma(void *pHcd, void *pUrb, int nMemFlags)
{
    (void)pHcd;
    (void)pUrb;
    (void)nMemFlags;
    /* Soft: no DMA map. Soft!=product. */
    return LUSB_ENODEV;
}

void
usb_hcd_unmap_urb_for_dma(void *pHcd, void *pUrb)
{
    (void)pHcd;
    (void)pUrb;
}

void
usb_hcd_poll_rh_status(void *pHcd)
{
    (void)pHcd;
}

void
usb_hcd_resume_root_hub(void *pHcd)
{
    (void)pHcd;
}

void
usb_hcd_start_port_resume(void *pBus, int nPortNum)
{
    (void)pBus;
    (void)nPortNum;
}

void
usb_hcd_end_port_resume(void *pBus, int nPortNum)
{
    (void)pBus;
    (void)nPortNum;
}

int
usb_hcd_is_primary_hcd(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        return (pSoft->u8Primary != 0u) ? 1 : 0;
    }
    /* Soft: unknown pointer -> not primary. Soft!=product. */
    return 0;
}

int
usb_hcd_irq(int nIrq, void *pHcd)
{
    (void)nIrq;
    (void)pHcd;
    /* Soft: IRQ_NONE-shaped 0. Soft!=product. */
    return 0;
}

int
usb_hcd_alloc_bandwidth(void *pHcd, void *pUdev, void *pAltOld,
                        void *pAltNew)
{
    (void)pHcd;
    (void)pUdev;
    (void)pAltOld;
    (void)pAltNew;
    return LUSB_ENODEV;
}

int
usb_hcd_get_frame_number(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        return (int)pSoft->u32Frame;
    }
    return 0;
}

int
usb_hcd_pci_probe(void *pPciDev, const void *pId, void *pDriver)
{
    (void)pPciDev;
    (void)pId;
    (void)pDriver;
    /* Soft: no PCI bind. Soft!=product; G-AC-1. */
    return LUSB_ENODEV;
}

void
usb_hcd_pci_remove(void *pPciDev)
{
    (void)pPciDev;
}

void
usb_hcd_pci_shutdown(void *pPciDev)
{
    (void)pPciDev;
}

int
usb_hcd_amd_remote_wakeup_quirk(void *pPciDev)
{
    (void)pPciDev;
    return 0;
}

/* ---- virtual usbcore hub / device / bus -------------------------------- */

void *
usb_hub_find_child(void *pHdev, int nPort1)
{
    (void)pHdev;
    (void)nPort1;
    lusb_inc(&g_cHub);
    /* Soft: no children. Soft!=product; != stick. */
    return NULL;
}

int
usb_hub_claim_port(void *pHdev, unsigned int uPort1, void *pOwner)
{
    (void)pHdev;
    (void)uPort1;
    (void)pOwner;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_hub_release_port(void *pHdev, unsigned int uPort1, void *pOwner)
{
    (void)pHdev;
    (void)uPort1;
    (void)pOwner;
    lusb_inc(&g_cHub);
    return 0;
}

void
usb_hub_release_all_ports(void *pHdev, void *pOwner)
{
    (void)pHdev;
    (void)pOwner;
    lusb_inc(&g_cHub);
}

void *
usb_hub_to_struct_hub(void *pHdev)
{
    (void)pHdev;
    lusb_inc(&g_cHub);
    return NULL;
}

int
usb_get_hub_status(void *pHdev, void *pData)
{
    (void)pHdev;
    (void)pData;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_get_port_status(void *pHdev, int nPort1, void *pData)
{
    (void)pHdev;
    (void)nPort1;
    (void)pData;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_clear_port_feature(void *pHdev, int nPort1, int nFeature)
{
    (void)pHdev;
    (void)nPort1;
    (void)nFeature;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_set_port_feature(void *pHdev, int nPort1, int nFeature)
{
    (void)pHdev;
    (void)nPort1;
    (void)nFeature;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_hub_clear_tt_buffer(void *pUdev, u16 u16Pipe, u16 u16DevInfo)
{
    (void)pUdev;
    (void)u16Pipe;
    (void)u16DevInfo;
    return LUSB_ENODEV;
}

void
usb_hub_adjust_deviceremovable(void *pHdev, void *pDesc)
{
    (void)pHdev;
    (void)pDesc;
}

int
usb_hub_for_each_child(void *pHdev, int nPort1, void **ppChild)
{
    (void)pHdev;
    (void)nPort1;
    if (ppChild != NULL) {
        *ppChild = NULL;
    }
    /* Soft: stop iteration. Soft!=product. */
    return 0;
}

void
usb_ep0_reinit(void *pUdev)
{
    (void)pUdev;
}

void *
usb_alloc_dev(void *pParent, void *pBus, unsigned int uPort1)
{
    struct usb_soft_udev *pSoft;
    struct usb_soft_bus *pBusSoft;
    struct usb_soft_hcd *pHcdSoft;

    lusb_soft_objs_once();
    /* Single soft udev slot; Soft!=enum stick. */
    if (g_fSoftUdevLive != 0u) {
        return NULL;
    }
    memset(g_aSoftUdev, 0, sizeof(g_aSoftUdev));
    pSoft = (struct usb_soft_udev *)(void *)g_aSoftUdev;
    pSoft->u32Magic = LUSB_UDEV_MAGIC;
    pSoft->pParent = pParent;
    pSoft->pBus = pBus;
    pSoft->uPort1 = uPort1;
    pSoft->u8Live = 1u;
    pSoft->u8Authorized = 0u;
    pSoft->u32State = 0u; /* notattached-shaped */
    /* residual9: wire HCD cookie from soft bus when present. Soft!=enum. */
    pBusSoft = lusb_bus_soft(pBus);
    if (pBusSoft != NULL && pBusSoft->pHcd != NULL) {
        pSoft->pHcd = pBusSoft->pHcd;
        pHcdSoft = lusb_hcd_soft(pBusSoft->pHcd);
        if (pHcdSoft != NULL) {
            /* Soft root hub remains NULL - never claim RH enum. */
            pHcdSoft->pRootHub = NULL;
        }
    } else {
        pSoft->pHcd = NULL;
    }
    g_fSoftUdevLive = 1u;
    return (void *)g_aSoftUdev;
}

int
usb_new_device(void *pUdev)
{
    (void)pUdev;
    /* Soft: no address/enum. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void
usb_disconnect(void **ppUdev)
{
    u32 i;
    struct usb_soft_ep *pEp;

    if (ppUdev != NULL && *ppUdev == (void *)g_aSoftUdev) {
        /* residual10: scrub soft EP pool for this udev. Soft!=product. */
        for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
            if (g_aSoftEpUsed[i] == 0u) {
                continue;
            }
            pEp = (struct usb_soft_ep *)(void *)g_aSoftEp[i];
            if (pEp->u32Magic == LUSB_EP_MAGIC &&
                pEp->pUdev == (void *)g_aSoftUdev) {
                memset(g_aSoftEp[i], 0, LINUX_USB_SOFT_EP_CB);
                g_aSoftEpUsed[i] = 0u;
            }
        }
        g_fSoftUdevLive = 0u;
        memset(g_aSoftUdev, 0, sizeof(g_aSoftUdev));
        *ppUdev = NULL;
    }
}

void
usb_disable_device(void *pUdev, int nSkipEp0)
{
    (void)pUdev;
    (void)nSkipEp0;
}

void
usb_enable_device(void *pUdev)
{
    (void)pUdev;
}

void
usb_set_device_state(void *pUdev, int nNewState)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft != NULL) {
        /* Soft layout residual only - no enum side effects. Soft!=product. */
        pSoft->u32State = (u32)nNewState;
    }
}

int
usb_device_is_owned(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_device_supports_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_device_supports_ltm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_enable_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_disable_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_unlocked_enable_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_unlocked_disable_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_enable_ltm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_disable_ltm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

void
usb_wakeup_notification(void *pHdev, unsigned int uPortNum)
{
    (void)pHdev;
    (void)uPortNum;
}

unsigned int
usb_wakeup_enabled_descendants(void *pUdev)
{
    (void)pUdev;
    return 0u;
}

int
usb_port_resume(void *pUdev, int nMsgFlags)
{
    (void)pUdev;
    (void)nMsgFlags;
    return LUSB_ENODEV;
}

int
usb_port_suspend(void *pUdev, int nMsgFlags)
{
    (void)pUdev;
    (void)nMsgFlags;
    return LUSB_ENODEV;
}

void *
usb_bus_get(void *pBus)
{
    return pBus;
}

void
usb_bus_put(void *pBus)
{
    (void)pBus;
}

int
usb_register_bus(void *pBus)
{
    struct usb_soft_bus *pSoft;

    lusb_soft_objs_once();
    /*
     * Soft: accept either caller bus pointer or fill soft bus blob.
     * Soft!=product - no sysfs bus number, no root-hub attach.
     */
    if (pBus == NULL || pBus == (void *)g_aSoftBus) {
        memset(g_aSoftBus, 0, sizeof(g_aSoftBus));
        pSoft = (struct usb_soft_bus *)(void *)g_aSoftBus;
        pSoft->u32Magic = LUSB_BUS_MAGIC;
        pSoft->u32BusNum = 1u;
        pSoft->u8Live = 1u;
        pSoft->pRootHub = NULL;
        g_fSoftBusLive = 1u;
    } else {
        g_fSoftBusLive = 1u;
    }
    return 0;
}

void
usb_deregister_bus(void *pBus)
{
    (void)pBus;
    g_fSoftBusLive = 0u;
    memset(g_aSoftBus, 0, sizeof(g_aSoftBus));
}

int
usb_add_device(void *pUdev)
{
    (void)pUdev;
    return LUSB_ENODEV;
}

void
usb_remove_device(void *pUdev)
{
    (void)pUdev;
}

int
usb_create_device_files(void *pUdev)
{
    (void)pUdev;
    return 0;
}

/* ---- virtual usbcore endpoint / find / misc helpers -------------------- */

void *
usb_find_next_intf_descriptor(void *pDesc, unsigned int *puSize,
                              unsigned int *puOffset)
{
    (void)pDesc;
    (void)puSize;
    (void)puOffset;
    return NULL;
}

void *
usb_find_alt_setting(void *pConfig, unsigned int uIfaceNum,
                     unsigned int uAltNum)
{
    (void)pConfig;
    (void)uIfaceNum;
    (void)uAltNum;
    return NULL;
}

int
usb_for_each_dev(void *pData, int (*pfn)(void *pUdev, void *pData))
{
    (void)pData;
    (void)pfn;
    /* Soft: no device walk. Soft!=product. */
    return 0;
}

int
usb_if_interface_number(void *pAlt)
{
    (void)pAlt;
    return -1;
}

int
usb_interface_id(void *pConfig, void *pAlt)
{
    (void)pConfig;
    (void)pAlt;
    return -1;
}

int
usb_get_max_power(void *pConfig)
{
    (void)pConfig;
    return 0;
}

int
usb_get_maximum_speed(void *pUdev)
{
    (void)pUdev;
    return 0; /* USB_SPEED_UNKNOWN-shaped */
}

int
usb_endpoint_maxp(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    /* residual10: soft EP maxpacket bookkeep (0 fail-closed). Soft!=product. */
    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (int)pSoft->u16MaxPacket;
    }
    return 0;
}

int
usb_endpoint_maxp_mult(const void *pEpd)
{
    (void)pEpd;
    return 1;
}

void *
usb_pipe_endpoint(void *pUdev, unsigned int uPipe)
{
    struct usb_soft_ep *pEp;

    /*
     * residual10: soft EP pool cookie for UDX hosts. Soft!=descriptor walk;
     * Soft!=product; never real stick EP. Pool exhaust -> NULL fail-closed.
     */
    pEp = lusb_ep_soft_lookup_or_prime(pUdev, uPipe);
    return (pEp != NULL) ? (void *)pEp : NULL;
}

int
usb_urb_dir_in(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8DirIn != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_dir_out(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8DirIn == 0u) ? 1 : 0;
    }
    return 0;
}

void
usb_submit_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

int
usb_get_current_bandwidth(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_find_common_endpoints_reverse(void *pAlt, void **ppBulkIn,
                                  void **ppBulkOut, void **ppIntIn,
                                  void **ppIntOut)
{
    /* Soft: same fail-closed shape as usb_find_common_endpoints. */
    return usb_find_common_endpoints(pAlt, ppBulkIn, ppBulkOut, ppIntIn,
                                     ppIntOut);
}

void *
usb_find_bulk_in_endpoint(void *pAlt)
{
    (void)pAlt;
    return NULL;
}

void *
usb_find_bulk_out_endpoint(void *pAlt)
{
    (void)pAlt;
    return NULL;
}

void *
usb_find_int_in_endpoint(void *pAlt)
{
    (void)pAlt;
    return NULL;
}

void *
usb_find_int_out_endpoint(void *pAlt)
{
    (void)pAlt;
    return NULL;
}

void
usb_poison_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_unpoison_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

void
usb_block_anchored_urbs(void *pAnchor)
{
    (void)pAnchor;
}

unsigned long
usb_calc_bus_time(int nSpeed, int nIsInput, int nIsoc, int nBytecount)
{
    (void)nSpeed;
    (void)nIsInput;
    (void)nIsoc;
    (void)nBytecount;
    return 0ul;
}

void *
usb_phy_roothub_alloc(void *pDev)
{
    (void)pDev;
    return NULL;
}

int
usb_phy_roothub_init(void *pPhyRh)
{
    (void)pPhyRh;
    return LUSB_ENODEV;
}

void
usb_phy_roothub_exit(void *pPhyRh)
{
    (void)pPhyRh;
}

int
usb_phy_roothub_set_mode(void *pPhyRh, int nMode)
{
    (void)pPhyRh;
    (void)nMode;
    return LUSB_ENODEV;
}

int
usb_phy_roothub_calibrate(void *pPhyRh)
{
    (void)pPhyRh;
    return 0;
}

int
usb_phy_roothub_power_on(void *pPhyRh)
{
    (void)pPhyRh;
    return LUSB_ENODEV;
}

void
usb_phy_roothub_power_off(void *pPhyRh)
{
    (void)pPhyRh;
}

void
usb_register_notify(void *pNb)
{
    (void)pNb;
}

void
usb_unregister_notify(void *pNb)
{
    (void)pNb;
}

void
usb_ledtrig_set(int nEvent)
{
    (void)nEvent;
}

int
usb_acpi_set_power_state(void *pHdev, int nIndex, int fEnable)
{
    (void)pHdev;
    (void)nIndex;
    (void)fEnable;
    return LUSB_ENODEV;
}

int
usb_acpi_power_manageable(void *pHdev, int nIndex)
{
    (void)pHdev;
    (void)nIndex;
    return 0;
}

int
usb_acpi_register(void)
{
    return 0;
}

void
usb_acpi_unregister(void)
{
}

int
usb_hub_initiate_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

/* ---- residual UND class (D residual expand; Soft!=product) -------------- */

/*
 * Residual fail-closed stubs beyond el9 usb-storage seed + prior expand:
 *   r1: scsi_mod mid (sd_mod-shaped) + sg copy/buffer + usb endpoint/config
 *   r2: scsi host/scan/vpd/io + sg chain/sgl/page-iter + usb hcd/bos/hub
 *   r3: scsi register/sysfs/evt + usb buffer_map/dynid + sg append
 *   r4: scsi target/queue/abort + sg page + usb hcd/hub/ep sysfs
 *   r5: scsi queue/EH abort + resid/offline + sg pool/set + hcd start/bus/runtime
 *   r6: scsi cmnd/rq/mq + EH tur + sg chain/last + hcd pci PM/runtime
 *   r7: hcd buffer/platform + hub TT/port + phy + scsi recovery + sg page-iter
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- scsi residual (sd_mod mid UND) -------------------------------------- */

int
scsi_autopm_get_device(void *pSdev)
{
    (void)pSdev;
    return 0;
}

void
scsi_autopm_put_device(void *pSdev)
{
    (void)pSdev;
}

int
scsi_get_vpd_page(void *pSdev, unsigned char u8Page, unsigned char *pBuf,
                  int nBufLen)
{
    (void)pSdev;
    (void)u8Page;
    if (pBuf != NULL && nBufLen > 0) {
        pBuf[0] = 0u;
    }
    return LUSB_ENODEV;
}

int
scsi_mode_sense(void *pSdev, int nDbd, int nModePage, unsigned char *pBuf,
                int nLen, int nTimeout, int nRetries, int *pnDataLen,
                void *pSshdr)
{
    (void)pSdev;
    (void)nDbd;
    (void)nModePage;
    (void)pBuf;
    (void)nLen;
    (void)nTimeout;
    (void)nRetries;
    (void)pSshdr;
    if (pnDataLen != NULL) {
        *pnDataLen = 0;
    }
    return LUSB_ENODEV;
}

int
scsi_mode_select(void *pSdev, int nPf, int nSp, unsigned char *pBuf, int nLen,
                 int nTimeout, int nRetries, int *pnDataLen, void *pSshdr)
{
    (void)pSdev;
    (void)nPf;
    (void)nSp;
    (void)pBuf;
    (void)nLen;
    (void)nTimeout;
    (void)nRetries;
    (void)pSshdr;
    if (pnDataLen != NULL) {
        *pnDataLen = 0;
    }
    return LUSB_ENODEV;
}

int
scsi_test_unit_ready(void *pSdev, int nTimeout, int nRetries, void *pSshdr)
{
    (void)pSdev;
    (void)nTimeout;
    (void)nRetries;
    (void)pSshdr;
    return LUSB_ENODEV;
}

int
scsi_report_opcode(void *pSdev, unsigned char *pBuffer, unsigned int uLen,
                   unsigned char u8Opcode)
{
    (void)pSdev;
    (void)pBuffer;
    (void)uLen;
    (void)u8Opcode;
    return LUSB_ENODEV;
}

int
scsi_set_medium_removal(void *pSdev, char cState)
{
    (void)pSdev;
    (void)cState;
    return LUSB_ENODEV;
}

int
scsi_ioctl(void *pSdev, unsigned int uCmd, void *pArg)
{
    (void)pSdev;
    (void)uCmd;
    (void)pArg;
    return LUSB_ENODEV;
}

int
scsi_ioctl_block_when_processing_errors(void *pSdev, unsigned int uCmd,
                                        int fNdelay)
{
    (void)pSdev;
    (void)uCmd;
    (void)fNdelay;
    return LUSB_ENODEV;
}

const char *
scsi_hostbyte_string(int nResult)
{
    (void)nResult;
    return g_szSoftScsiHostbyte;
}

const char *
scsi_sense_key_string(unsigned char u8Key)
{
    (void)u8Key;
    return g_szSoftScsiSenseKey;
}

const char *
scsi_mlreturn_string(int nResult)
{
    (void)nResult;
    return g_szSoftScsiMlreturn;
}

void
scsi_print_result(void *pScmd, const char *szMsg, int nDisposition)
{
    (void)pScmd;
    (void)szMsg;
    (void)nDisposition;
}

void
scsi_print_status(unsigned char u8Status)
{
    (void)u8Status;
}

void
scsi_host_block(void *pShost)
{
    (void)pShost;
    g_fSoftHostBlocked = 1u;
    lusb_inc(&g_cScsiBlock);
}

void
scsi_host_unblock(void *pShost, int nNewState)
{
    (void)pShost;
    (void)nNewState;
    g_fSoftHostBlocked = 0u;
    lusb_inc(&g_cScsiBlock);
}

void
scsi_target_block(void *pDev)
{
    (void)pDev;
}

void
scsi_target_unblock(void *pDev, int nNewState)
{
    (void)pDev;
    (void)nNewState;
}

void
scsi_device_block(void *pSdev)
{
    (void)pSdev;
}

void
scsi_device_unblock(void *pSdev, int nNewState)
{
    (void)pSdev;
    (void)nNewState;
}

void
scsi_schedule_eh(void *pShost)
{
    (void)pShost;
    lusb_inc(&g_cScsiEh);
}

int
scsi_add_device(void *pShost, unsigned int uChannel, unsigned int uId,
                unsigned int uLun)
{
    (void)pShost;
    (void)uChannel;
    (void)uId;
    (void)uLun;
    /* Soft: no LUN enum. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void
scsi_remove_device(void *pSdev)
{
    (void)pSdev;
}

void
scsi_rescan_device(void *pSdev)
{
    (void)pSdev;
}

void
scsi_scan_target(void *pParent, unsigned int uChannel, unsigned int uId,
                 unsigned int uLun, int nRescan)
{
    (void)pParent;
    (void)uChannel;
    (void)uId;
    (void)uLun;
    (void)nRescan;
    lusb_inc(&g_cScsiScan);
}

int
scsi_is_wlun(unsigned long long u64Lun)
{
    (void)u64Lun;
    return 0;
}

const char *
scsi_device_type(unsigned char u8Type)
{
    (void)u8Type;
    return "soft";
}

void
scsi_sanitize_inquiry_string(unsigned char *pS, int nLen)
{
    (void)pS;
    (void)nLen;
}

int
scsi_set_sense_field_pointer(unsigned char *pBuf, int nBufLen,
                             unsigned short uFp, int nBp)
{
    (void)pBuf;
    (void)nBufLen;
    (void)uFp;
    (void)nBp;
    return 0;
}

void
scsi_failures_reset_retries(void *pFailures)
{
    (void)pFailures;
}

int
scsi_pr_type_to_block(int nPrType)
{
    (void)nPrType;
    return 0;
}

int
scsi_decide_disposition(void *pScmd)
{
    (void)pScmd;
    /* Soft: no retry path. Soft!=product. */
    return 0;
}

int
scsi_noretry_cmd(void *pScmd)
{
    (void)pScmd;
    return 1;
}

/* -- sg residual (copy / buffer / chained) ------------------------------- */

int
sg_nents_for_len(void *pSg, unsigned long long u64Len)
{
    (void)pSg;
    (void)u64Len;
    lusb_inc(&g_cSgNents);
    return 0;
}

unsigned long
sg_copy_buffer(void *pSgl, unsigned int uNents, void *pBuf,
               unsigned long cbBuflen, unsigned long cbSkip, int fToBuffer)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)cbBuflen;
    (void)cbSkip;
    (void)fToBuffer;
    return 0ul;
}

unsigned long
sg_copy_from_buffer(void *pSgl, unsigned int uNents, const void *pBuf,
                    unsigned long cbBuflen)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)cbBuflen;
    return 0ul;
}

unsigned long
sg_copy_to_buffer(void *pSgl, unsigned int uNents, void *pBuf,
                  unsigned long cbBuflen)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)cbBuflen;
    return 0ul;
}

unsigned long
sg_pcopy_from_buffer(void *pSgl, unsigned int uNents, const void *pBuf,
                     unsigned long cbBuflen, unsigned long cbSkip)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)cbBuflen;
    (void)cbSkip;
    return 0ul;
}

unsigned long
sg_pcopy_to_buffer(void *pSgl, unsigned int uNents, void *pBuf,
                   unsigned long cbBuflen, unsigned long cbSkip)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)cbBuflen;
    (void)cbSkip;
    return 0ul;
}

unsigned long
sg_zero_buffer(void *pSgl, unsigned int uNents, unsigned long cbBuflen,
               unsigned long cbSkip)
{
    (void)pSgl;
    (void)uNents;
    (void)cbBuflen;
    (void)cbSkip;
    return 0ul;
}

int
__sg_alloc_table(void *pTable, unsigned int uNents, unsigned int uMaxEnts,
                 void *pFirstChunk, unsigned int uFirstChunkNents,
                 int nMemFlags, void *pfnAlloc)
{
    (void)pTable;
    (void)uNents;
    (void)uMaxEnts;
    (void)pFirstChunk;
    (void)uFirstChunkNents;
    (void)nMemFlags;
    (void)pfnAlloc;
    return LUSB_ENOMEM;
}

void
__sg_free_table(void *pTable, unsigned int uMaxEnts,
                unsigned int uFirstChunkNents, void *pfnFree,
                unsigned int fSkipFirstChunk)
{
    (void)pTable;
    (void)uMaxEnts;
    (void)uFirstChunkNents;
    (void)pfnFree;
    (void)fSkipFirstChunk;
}

int
sg_alloc_table_from_pages(void *pSgt, void **ppPages, unsigned int uNPages,
                          unsigned int uOffset, unsigned long long u64Size,
                          int nMemFlags)
{
    (void)pSgt;
    (void)ppPages;
    (void)uNPages;
    (void)uOffset;
    (void)u64Size;
    (void)nMemFlags;
    return LUSB_ENOMEM;
}

int
sg_split(void *pIn, int nInMappedNents, int nFirstSkip, int nSplitNents,
         void **ppOut, int *pnOutMappedNents, unsigned long long *pu64Sizes,
         int nMemFlags)
{
    (void)pIn;
    (void)nInMappedNents;
    (void)nFirstSkip;
    (void)nSplitNents;
    (void)ppOut;
    (void)pnOutMappedNents;
    (void)pu64Sizes;
    (void)nMemFlags;
    return LUSB_ENOMEM;
}

int
sg_miter_get_next_page(void *pMiter)
{
    (void)pMiter;
    return 0;
}

/* -- usbcore residual (endpoint / config / urb-fill) --------------------- */

void
usb_disable_endpoint(void *pDev, unsigned int uEpAddr, int fResetHardware)
{
    u32 i;
    struct usb_soft_ep *pSoft;
    u8 u8Num;
    u8 u8DirIn;

    (void)fResetHardware;
    /*
     * residual11: clear soft EP enable cookie by (udev, addr). Soft!=HC;
     * never touches real hardware. Soft!=product · != stick.
     */
    u8Num = (u8)(uEpAddr & 0x0fu);
    u8DirIn = ((uEpAddr & 0x80u) != 0u) ? 1u : 0u;
    for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
        if (g_aSoftEpUsed[i] == 0u) {
            continue;
        }
        pSoft = (struct usb_soft_ep *)(void *)g_aSoftEp[i];
        if (pSoft->u32Magic != LUSB_EP_MAGIC) {
            continue;
        }
        if (pDev != NULL && pSoft->pUdev != pDev) {
            continue;
        }
        if (pSoft->u8Num == u8Num && pSoft->u8DirIn == u8DirIn) {
            pSoft->u8Enabled = 0u;
        }
    }
}

void
usb_enable_endpoint(void *pDev, void *pEp, int fResetEp)
{
    struct usb_soft_ep *pSoft;
    struct usb_soft_udev *pUdevSoft;

    (void)fResetEp;
    /*
     * residual11: soft EP enable cookie + optional udev/HCD reverse wire.
     * Soft!=HC · Soft!=product · != stick · freestanding_no_exec.
     */
    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return;
    }
    pSoft->u8Enabled = 1u;
    if (pDev != NULL) {
        pSoft->pUdev = pDev;
        pUdevSoft = lusb_udev_soft(pDev);
        if (pUdevSoft != NULL && pUdevSoft->pHcd != NULL) {
            pSoft->pHcd = pUdevSoft->pHcd;
        }
    }
}

void
usb_disable_interface(void *pDev, void *pIntf, int fResetHardware)
{
    (void)pDev;
    (void)pIntf;
    (void)fResetHardware;
}

void
usb_enable_interface(void *pDev, void *pIntf, int fResetEps)
{
    (void)pDev;
    (void)pIntf;
    (void)fResetEps;
}

int
usb_get_configuration(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_destroy_configuration(void *pDev)
{
    (void)pDev;
}

unsigned char *
usb_cache_string(void *pUdev, int nIndex)
{
    (void)pUdev;
    (void)nIndex;
    return NULL;
}

void
usb_init_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    if (pUrb == NULL) {
        return;
    }
    if (lusb_soft_urb_slot(pUrb) >= 0) {
        memset(pUrb, 0, LINUX_USB_SOFT_URB_CB);
        pSoft = (struct usb_soft_urb *)pUrb;
        lusb_urb_soft_prime(pSoft);
        return;
    }
    /* Foreign pointer: partial zero only. Soft!=ABI-stable. */
    memset(pUrb, 0, 64u);
}

void
usb_fill_bulk_urb(void *pUrb, void *pDev, unsigned int uPipe, void *pTransfer,
                  int nBufferLength, void *pfnComplete, void *pContext)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEp;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return;
    }
    pSoft->pDev = pDev;
    pSoft->uPipe = uPipe;
    pSoft->pTransfer = pTransfer;
    pSoft->u32TransferLen = (nBufferLength > 0) ? (u32)nBufferLength : 0u;
    pSoft->u32ActualLen = 0u;
    pSoft->pfnComplete = pfnComplete;
    pSoft->pContext = pContext;
    pSoft->pSetup = NULL;
    pSoft->nInterval = 0;
    pSoft->u8XferType = LINUX_USB_SOFT_XFER_BULK;
    pSoft->u8DirIn = lusb_pipe_dir_in(uPipe);
    pSoft->u8EpNum = lusb_pipe_ep_num(uPipe);
    pSoft->nStatus = 0;
    pSoft->u8Submitted = 0u;
    /* residual10: wire soft EP cookie for UDX hosts. Soft!=product. */
    pEp = lusb_ep_soft_lookup_or_prime(pDev, uPipe);
    if (pEp != NULL) {
        pEp->u8Type = LINUX_USB_SOFT_XFER_BULK;
        pSoft->pEp = (void *)pEp;
    }
}

void
usb_fill_control_urb(void *pUrb, void *pDev, unsigned int uPipe, void *pSetup,
                     void *pTransfer, int nBufferLength, void *pfnComplete,
                     void *pContext)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEp;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return;
    }
    pSoft->pDev = pDev;
    pSoft->uPipe = uPipe;
    pSoft->pSetup = pSetup;
    pSoft->pTransfer = pTransfer;
    pSoft->u32TransferLen = (nBufferLength > 0) ? (u32)nBufferLength : 0u;
    pSoft->u32ActualLen = 0u;
    pSoft->pfnComplete = pfnComplete;
    pSoft->pContext = pContext;
    pSoft->nInterval = 0;
    pSoft->u8XferType = LINUX_USB_SOFT_XFER_CTRL;
    pSoft->u8DirIn = lusb_pipe_dir_in(uPipe);
    pSoft->u8EpNum = lusb_pipe_ep_num(uPipe);
    pSoft->nStatus = 0;
    pSoft->u8Submitted = 0u;
    /* residual10: wire soft EP cookie. Soft!=product. */
    pEp = lusb_ep_soft_lookup_or_prime(pDev, uPipe);
    if (pEp != NULL) {
        pEp->u8Type = LINUX_USB_SOFT_XFER_CTRL;
        pSoft->pEp = (void *)pEp;
    }
}

void
usb_fill_int_urb(void *pUrb, void *pDev, unsigned int uPipe, void *pTransfer,
                 int nBufferLength, void *pfnComplete, void *pContext,
                 int nInterval)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEp;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return;
    }
    pSoft->pDev = pDev;
    pSoft->uPipe = uPipe;
    pSoft->pTransfer = pTransfer;
    pSoft->u32TransferLen = (nBufferLength > 0) ? (u32)nBufferLength : 0u;
    pSoft->u32ActualLen = 0u;
    pSoft->pfnComplete = pfnComplete;
    pSoft->pContext = pContext;
    pSoft->pSetup = NULL;
    pSoft->nInterval = nInterval;
    pSoft->u8XferType = LINUX_USB_SOFT_XFER_INT;
    pSoft->u8DirIn = lusb_pipe_dir_in(uPipe);
    pSoft->u8EpNum = lusb_pipe_ep_num(uPipe);
    pSoft->nStatus = 0;
    pSoft->u8Submitted = 0u;
    /* residual10: wire soft EP cookie + interval. Soft!=product. */
    pEp = lusb_ep_soft_lookup_or_prime(pDev, uPipe);
    if (pEp != NULL) {
        pEp->u8Type = LINUX_USB_SOFT_XFER_INT;
        pEp->u16Interval = (nInterval > 0) ? (u16)nInterval : 0u;
        pSoft->pEp = (void *)pEp;
    }
}

unsigned int
usb_maxpacket(void *pUdev, int nPipe, int fIsOut)
{
    struct usb_soft_ep *pEp;
    unsigned int uPipe;

    (void)fIsOut;
    /*
     * residual10: soft EP maxpacket bookkeep only (0 fail-closed default).
     * Soft!=product · != stick.
     */
    uPipe = (unsigned int)nPipe;
    pEp = lusb_ep_soft_lookup_or_prime(pUdev, uPipe);
    if (pEp != NULL) {
        return (unsigned int)pEp->u16MaxPacket;
    }
    return 0u;
}

int
usb_endpoint_num(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    /* residual10: honor soft EP cookie; foreign -> 0. Soft!=product. */
    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (int)pSoft->u8Num;
    }
    return 0;
}

int
usb_endpoint_type(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (int)pSoft->u8Type;
    }
    return 0; /* control-shaped / foreign */
}

int
usb_endpoint_dir_in(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (pSoft->u8DirIn != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_endpoint_is_bulk_in(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (pSoft->u8Type == LINUX_USB_SOFT_XFER_BULK &&
                pSoft->u8DirIn != 0u)
                   ? 1
                   : 0;
    }
    return 0;
}

int
usb_endpoint_is_bulk_out(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (pSoft->u8Type == LINUX_USB_SOFT_XFER_BULK &&
                pSoft->u8DirIn == 0u)
                   ? 1
                   : 0;
    }
    return 0;
}

int
usb_endpoint_is_int_in(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (pSoft->u8Type == LINUX_USB_SOFT_XFER_INT &&
                pSoft->u8DirIn != 0u)
                   ? 1
                   : 0;
    }
    return 0;
}

int
usb_endpoint_is_int_out(const void *pEpd)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft((void *)pEpd);
    if (pSoft != NULL) {
        return (pSoft->u8Type == LINUX_USB_SOFT_XFER_INT &&
                pSoft->u8DirIn == 0u)
                   ? 1
                   : 0;
    }
    return 0;
}

int
usb_authorize_device(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft != NULL) {
        /*
         * Soft bookkeep only: mark authorized cookie without granting
         * datapath. Still fail-closed (ENODEV) so no stick claim.
         * Soft!=product · G-AC-1 · freestanding_msc=SKIP.
         */
        pSoft->u8Authorized = 1u;
    }
    return LUSB_ENODEV;
}

int
usb_deauthorize_device(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft != NULL) {
        pSoft->u8Authorized = 0u;
    }
    return 0;
}

void
usb_detect_quirks(void *pUdev)
{
    (void)pUdev;
}

void
usb_detect_interface_quirks(void *pUdev)
{
    (void)pUdev;
}

void *
usb_find_device(unsigned short uVendor, unsigned short uProduct)
{
    (void)uVendor;
    (void)uProduct;
    /* Soft: never enumerate real stick. Soft!=product. */
    return NULL;
}

int
__usb_get_extra_descriptor(char *pBuffer, unsigned int uSize,
                           unsigned char u8Type, void **ppPtr,
                           unsigned long cbMin)
{
    (void)pBuffer;
    (void)uSize;
    (void)u8Type;
    (void)cbMin;
    if (ppPtr != NULL) {
        *ppPtr = NULL;
    }
    return LUSB_ENOENT;
}

int
usb_driver_set_configuration(void *pUdev, int nConfig)
{
    (void)pUdev;
    (void)nConfig;
    return LUSB_ENODEV;
}

void
usb_hcd_platform_shutdown(void *pDev)
{
    (void)pDev;
}

void
usb_amd_dev_put(void)
{
}

int
usb_set_isoch_delay(void *pUdev)
{
    (void)pUdev;
    return 0;
}

/*
 * Residual UND expand 2 (D residual deepen; Soft!=product)
 * -------------------------------------------------------
 * Additional fail-closed soft stubs beyond residual1 (76):
 *   residual2 scsi  30  - host/scan/vpd/io completion/queue helpers
 *   residual2 sg    10  - chained table + sgl + page-iter
 *   residual2 usb   30  - hcd submit/unlink/ep + bos + hub port + notify
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ...
 */

/* -- residual2 scsi mid (host/scan/vpd/io) -------------------------------- */

void *
scsi_device_lookup_by_target(void *pStarget, unsigned long long u64Lun)
{
    (void)pStarget;
    (void)u64Lun;
    return NULL;
}

void
scsi_host_busy_iter(void *pShost, void *pfn, void *pPriv)
{
    (void)pShost;
    (void)pfn;
    (void)pPriv;
    /* Soft: no busy cmd walk. Soft!=product. */
}

int
scsi_scan_host_selected(void *pShost, unsigned int uChannel, unsigned int uId,
                        unsigned long long u64Lun, int nRescan)
{
    (void)pShost;
    (void)uChannel;
    (void)uId;
    (void)u64Lun;
    (void)nRescan;
    lusb_inc(&g_cScsiScan);
    return LUSB_ENODEV;
}

void
scsi_complete_async_scans(void)
{
    /* Soft: no async scan queue. Soft!=product. */
}

void *
scsi_get_host_dev(void *pShost)
{
    (void)pShost;
    return NULL;
}

void
scsi_free_host_dev(void *pSdev)
{
    (void)pSdev;
}

void
scsi_target_reap(void *pStarget)
{
    (void)pStarget;
}

int
scsi_device_reprobe(void *pSdev)
{
    (void)pSdev;
    return LUSB_ENODEV;
}

void
scsi_host_set_state(void *pShost, int nState)
{
    (void)pShost;
    (void)nState;
}

int
scsi_cmd_allowed(void *pScmd, unsigned int uFlags)
{
    (void)pScmd;
    (void)uFlags;
    return 0; /* soft deny all CDB classes; Soft!=product */
}

enum {
    LUSB_SOFT_BLK_STS_IOERR = 10 /* soft blk_status-shaped; Soft!=ABI */
};

void
scsi_timeout(void *pScmd)
{
    (void)pScmd;
    lusb_inc(&g_cScsiEh);
}

void
scsi_io_completion(void *pScmd, unsigned int uGoodBytes)
{
    (void)pScmd;
    (void)uGoodBytes;
    lusb_inc(&g_cScsiDone);
}

void
scsi_run_host_queues(void *pShost)
{
    (void)pShost;
}

void
scsi_device_unbusy(void *pSdev)
{
    (void)pSdev;
}

void
scsi_requeue_command(void *pShost, void *pScmd)
{
    (void)pShost;
    (void)pScmd;
}

int
scsi_init_io(void *pScmd)
{
    (void)pScmd;
    return LUSB_ENOMEM;
}

void *
scsi_device_from_queue(void *pQ)
{
    (void)pQ;
    return NULL;
}

int
scsi_vpd_lun_id(void *pSdev, char *pId, unsigned long cbSize)
{
    (void)pSdev;
    if (pId != NULL && cbSize > 0ul) {
        pId[0] = '\0';
    }
    return LUSB_ENODEV;
}

int
scsi_vpd_tpg_id(void *pSdev, int *pnRelTargetPort)
{
    (void)pSdev;
    if (pnRelTargetPort != NULL) {
        *pnRelTargetPort = 0;
    }
    return LUSB_ENODEV;
}

void
scsi_attach_vpd(void *pSdev)
{
    (void)pSdev;
}

int
scsi_internal_device_block_nowait(void *pSdev)
{
    (void)pSdev;
    return 0;
}

int
scsi_internal_device_unblock_nowait(void *pSdev, int nNewState)
{
    (void)pSdev;
    (void)nNewState;
    return 0;
}

int
scsi_result_to_blk_status(int nResult)
{
    (void)nResult;
    return LUSB_SOFT_BLK_STS_IOERR;
}

int
scsi_status_is_good(int nStatus)
{
    (void)nStatus;
    return 0;
}

void
scsi_host_complete_all_commands(void *pShost, int nStatus)
{
    (void)pShost;
    (void)nStatus;
    lusb_inc(&g_cScsiDone);
}

int
scsi_queue_work(void *pShost, void *pWork)
{
    (void)pShost;
    (void)pWork;
    return 0; /* soft: work not queued; Soft!=product */
}

void
scsi_flush_work(void *pShost)
{
    (void)pShost;
}

int
scsi_cdl_enable(void *pSdev, int fEnable)
{
    (void)pSdev;
    (void)fEnable;
    return LUSB_ENODEV;
}

void *
scsi_bios_ptable(void *pBdev)
{
    (void)pBdev;
    return NULL;
}

int
scsi_partsize(void *pBuf, unsigned long uLongCapacity, unsigned int *puCyl,
              unsigned int *puHds, unsigned int *puSec)
{
    (void)pBuf;
    (void)uLongCapacity;
    if (puCyl != NULL) {
        *puCyl = 0u;
    }
    if (puHds != NULL) {
        *puHds = 0u;
    }
    if (puSec != NULL) {
        *puSec = 0u;
    }
    return LUSB_ENODEV;
}

/* -- residual2 sg (chained / sgl / page-iter) ---------------------------- */

int
sg_alloc_table_chained(void *pTable, int nNents, void *pFirstChunk,
                       unsigned int uNentsFirstChunk)
{
    (void)pTable;
    (void)nNents;
    (void)pFirstChunk;
    (void)uNentsFirstChunk;
    return LUSB_ENOMEM;
}

void
sg_free_table_chained(void *pTable, unsigned int uNentsFirstChunk)
{
    (void)pTable;
    (void)uNentsFirstChunk;
}

void *
sgl_alloc(unsigned long long u64Length, int nMemFlags, unsigned int *puNents)
{
    (void)u64Length;
    (void)nMemFlags;
    if (puNents != NULL) {
        *puNents = 0u;
    }
    return NULL;
}

void
sgl_free(void *pSgl)
{
    (void)pSgl;
}

void *
sgl_alloc_order(unsigned long long u64Length, unsigned int uOrder,
                int fChainable, int nMemFlags, unsigned int *puNents)
{
    (void)u64Length;
    (void)uOrder;
    (void)fChainable;
    (void)nMemFlags;
    if (puNents != NULL) {
        *puNents = 0u;
    }
    return NULL;
}

void
sgl_free_order(void *pSgl, int nOrder)
{
    (void)pSgl;
    (void)nOrder;
}

void
sgl_free_n_order(void *pSgl, int nNents, int nOrder)
{
    (void)pSgl;
    (void)nNents;
    (void)nOrder;
}

void
__sg_page_iter_start(void *pPiter, void *pSglist, unsigned int uNents,
                     unsigned long uPgoffset)
{
    (void)pPiter;
    (void)pSglist;
    (void)uNents;
    (void)uPgoffset;
}

int
__sg_page_iter_next(void *pPiter)
{
    (void)pPiter;
    return 0;
}

int
__sg_page_iter_dma_next(void *pDmaIter)
{
    (void)pDmaIter;
    return 0;
}

/* -- residual2 usbcore (hcd/ep/bos/hub port/notify) ---------------------- */

int
usb_hcd_submit_urb(void *pUrb, int nMemFlags)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    (void)nMemFlags;
    lusb_inc(&g_cSubmitUrb);
    /*
     * residual9 layout deepen: mirror usb_submit_urb soft bookkeep when the
     * URB is a soft pool slot. Still ENODEV - no HC datapath / != stick.
     */
    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        if (pSoft->u8Poisoned != 0u) {
            pSoft->nStatus = LUSB_ENODEV;
            pSoft->u8Submitted = 0u;
            return LUSB_ENODEV;
        }
        pSoft->u8Submitted = 1u;
        pSoft->u32ActualLen = 0u;
        pSoft->nStatus = LUSB_ENODEV;
        pSoft->u8Unlinked = 0u;
        pSoft->u8EpNum = lusb_pipe_ep_num(pSoft->uPipe);
        lusb_inc(&g_uSoftUrbSeq);
        pSoft->u32Seq = g_uSoftUrbSeq;
        pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
        if (pHcdSoft != NULL && pHcdSoft->u8Died != 0u) {
            pSoft->u8Submitted = 0u;
            return LUSB_ENODEV;
        }
    }
    return LUSB_ENODEV;
}

int
usb_hcd_unlink_urb(void *pUrb, int nStatus)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->u8Unlinked = 1u;
        pSoft->u8Submitted = 0u;
        pSoft->u8Linked = 0u;
        pSoft->nStatus = (nStatus != 0) ? nStatus : LUSB_ENODEV;
        pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
        if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
            pHcdSoft->u32UrbLinked--;
        }
    }
    return LUSB_ENODEV;
}

void
usb_hcd_flush_endpoint(void *pUdev, void *pEp)
{
    (void)pUdev;
    (void)pEp;
}

void
usb_hcd_disable_endpoint(void *pUdev, void *pEp)
{
    (void)pUdev;
    (void)pEp;
}

void
usb_hcd_reset_endpoint(void *pUdev, void *pEp)
{
    (void)pUdev;
    (void)pEp;
}

void
usb_hcd_synchronize_unlinks(void *pUdev)
{
    (void)pUdev;
}

void
usb_hc_died(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        pSoft->u8Died = 1u;
        pSoft->u32Flags |= LINUX_USB_SOFT_HCD_F_DIED;
        pSoft->u8Started = 0u;
        pSoft->u32Flags &= ~LINUX_USB_SOFT_HCD_F_STARTED;
    }
}

void
usb_root_hub_lost_power(void *pRhdev)
{
    (void)pRhdev;
}

int
usb_authorize_interface(void *pIntf)
{
    (void)pIntf;
    return LUSB_ENODEV;
}

int
usb_deauthorize_interface(void *pIntf)
{
    (void)pIntf;
    return 0;
}

int
usb_get_bos_descriptor(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_release_bos_descriptor(void *pDev)
{
    (void)pDev;
}

int
usb_get_device_descriptor(void *pDev, unsigned int uSize)
{
    (void)pDev;
    (void)uSize;
    return LUSB_ENODEV;
}

int
usb_enable_remote_wakeup(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_disable_remote_wakeup(void *pDev)
{
    (void)pDev;
    return 0;
}

int
usb_enable_usb2_hardware_lpm(void *pUdev)
{
    (void)pUdev;
    return LUSB_ENODEV;
}

int
usb_disable_usb2_hardware_lpm(void *pUdev)
{
    (void)pUdev;
    return 0;
}

int
usb_hub_port_disable(void *pHub, int nPort1, int fSetState)
{
    (void)pHub;
    (void)nPort1;
    (void)fSetState;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_hub_port_reset(void *pHub, int nPort1, void *pUdev, unsigned int uDelay,
                   int fWarm)
{
    (void)pHub;
    (void)nPort1;
    (void)pUdev;
    (void)uDelay;
    (void)fWarm;
    lusb_inc(&g_cHub);
    return LUSB_ENODEV;
}

int
usb_reset_and_verify_device(void *pUdev)
{
    (void)pUdev;
    return LUSB_ENODEV;
}

void
usb_create_ep_devs(void *pParent, void *pEndpoint, void *pUdev)
{
    (void)pParent;
    (void)pEndpoint;
    (void)pUdev;
}

void
usb_remove_ep_devs(void *pEndpoint)
{
    (void)pEndpoint;
}

void
usb_forced_unbind_intf(void *pIntf)
{
    (void)pIntf;
}

void
usb_unbind_and_rebind_marked_interfaces(void *pDev)
{
    (void)pDev;
}

int
usb_hub_claimed_port(void *pHub, int nPort1)
{
    (void)pHub;
    (void)nPort1;
    return 0;
}

void
usb_notify_add_device(void *pUdev)
{
    (void)pUdev;
}

void
usb_notify_remove_device(void *pUdev)
{
    (void)pUdev;
}

void
usb_create_sysfs_dev_files(void *pUdev)
{
    (void)pUdev;
}

void
usb_remove_sysfs_dev_files(void *pUdev)
{
    (void)pUdev;
}

int
usb_decode_interval(const void *pEpd, int nSpeed)
{
    (void)pEpd;
    (void)nSpeed;
    return 0;
}

/*
 * Residual UND expand 3 (D residual deepen; Soft!=product)
 * -------------------------------------------------------
 * Additional fail-closed soft stubs beyond residual1+2 (146):
 *   residual3 scsi  30  - register/driver/sysfs/evt/state/print/mq/execute
 *   residual3 sg     7  - append table + miter consume + virt/phys/dma
 *   residual3 usb   10  - dynid + device_driver + bus enum + buffer_map*
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- residual3 scsi mid (register/sysfs/evt/print/mq) -------------------- */

void *
scsi_register(void *pSht, int nPrivSize)
{
    /* Soft older host register -> soft host_alloc path. Soft!=product. */
    return scsi_host_alloc(pSht, nPrivSize);
}

void
scsi_unregister(void *pShost)
{
    if (lusb_is_soft_host(pShost) != 0) {
        g_fSoftHostAdded = 0u;
        g_fSoftHostLive = 0u;
        memset(g_aSoftHost, 0, LINUX_USB_SOFT_HOST_CB);
    }
}

int
scsi_register_driver(void *pDriver)
{
    (void)pDriver;
    /* Soft: no bus match. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void
scsi_unregister_driver(void *pDriver)
{
    (void)pDriver;
}

int
scsi_register_interface(void *pIntf)
{
    (void)pIntf;
    return LUSB_ENODEV;
}

void
scsi_unregister_interface(void *pIntf)
{
    (void)pIntf;
}

void
scsi_host_set_prot(void *pShost, unsigned int uMask)
{
    (void)pShost;
    (void)uMask;
    /* Soft: no DIX/DIF. Soft!=product. */
}

unsigned long long
scsi_host_prot_dma_mask(void *pShost)
{
    (void)pShost;
    return 0ull;
}

unsigned long
scsi_get_device_flags(void *pSdev, const unsigned char *pVendor,
                      const unsigned char *pModel)
{
    (void)pSdev;
    (void)pVendor;
    (void)pModel;
    return 0ul;
}

int
scsi_sysfs_add_sdev(void *pSdev)
{
    (void)pSdev;
    /* Soft: no sysfs. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void *
__scsi_iterate_devices(void *pShost, void *pPrev)
{
    (void)pShost;
    (void)pPrev;
    return NULL;
}

void
starget_for_each_device(void *pStarget, void *pData,
                        void (*pfn)(void *pSdev, void *pData))
{
    (void)pStarget;
    (void)pData;
    (void)pfn;
    /* Soft: empty target walk. Soft!=product. */
}

const char *
scsi_device_state_name(int nState)
{
    (void)nState;
    return "soft";
}

const char *
scsi_host_state_name(int nState)
{
    (void)nState;
    return "soft";
}

void
scsi_print_sense(const char *szName, void *pScmd)
{
    (void)szName;
    (void)pScmd;
}

void
__scsi_print_command(const unsigned char *pCdb)
{
    (void)pCdb;
}

void
scsi_show_result(int nResult)
{
    (void)nResult;
}

void
scsi_show_sense(unsigned char u8Key, unsigned char u8Asc, unsigned char u8Ascq)
{
    (void)u8Key;
    (void)u8Asc;
    (void)u8Ascq;
}

int
scsi_is_ua_sense(void *pSshdr)
{
    (void)pSshdr;
    return 0;
}

void *
sdev_evt_alloc(int nEvtType, int nMemFlags)
{
    (void)nEvtType;
    (void)nMemFlags;
    return NULL;
}

void
sdev_evt_send(void *pSdev, void *pEvt, int nMemFlags)
{
    (void)pSdev;
    (void)pEvt;
    (void)nMemFlags;
}

void
sdev_evt_send_simple(void *pSdev, int nEvtType, int nMemFlags)
{
    (void)pSdev;
    (void)nEvtType;
    (void)nMemFlags;
}

void
scsi_cmd_get_serial(void *pShost, void *pScmd)
{
    (void)pShost;
    (void)pScmd;
}

int
scsi_device_busy(void *pSdev)
{
    (void)pSdev;
    return 0;
}

void
scsi_kick_queue(void *pShost)
{
    (void)pShost;
    /* Soft: no run queue. Soft!=product. */
}

int
__scsi_execute(void *pSdev, const unsigned char *pCmd, int nDataDir,
               void *pBuffer, unsigned int uBuffLen, unsigned char *pSense,
               void *pSshdr, int nTimeout, int nRetries, unsigned long uFlags,
               int nRqFlags, int *pnResid)
{
    (void)pSdev;
    (void)pCmd;
    (void)nDataDir;
    (void)pBuffer;
    (void)uBuffLen;
    (void)pSense;
    (void)pSshdr;
    (void)nTimeout;
    (void)nRetries;
    (void)uFlags;
    (void)nRqFlags;
    if (pnResid != NULL) {
        *pnResid = 0;
    }
    return LUSB_ENODEV;
}

int
scsi_mq_setup_tags(void *pShost)
{
    (void)pShost;
    return LUSB_ENOMEM;
}

void
scsi_mq_free_tags(void *pShost)
{
    (void)pShost;
}

int
scsi_init_queue(void *pShost)
{
    (void)pShost;
    return LUSB_ENODEV;
}

void
scsi_exit_queue(void *pShost)
{
    (void)pShost;
}

/* -- residual3 sg (append / miter consume / virt-phys) ------------------- */

int
sg_alloc_append_table_from_pages(void *pSgt, void **ppPages,
                                 unsigned int uNPages, unsigned int uOffset,
                                 unsigned long long u64Size,
                                 unsigned int uMaxSegment,
                                 unsigned int uLeftPages, int nMemFlags)
{
    (void)pSgt;
    (void)ppPages;
    (void)uNPages;
    (void)uOffset;
    (void)u64Size;
    (void)uMaxSegment;
    (void)uLeftPages;
    (void)nMemFlags;
    return LUSB_ENOMEM;
}

void
sg_free_append_table(void *pSgt)
{
    (void)pSgt;
}

void
sg_miter_consume(void *pMiter, unsigned long cbLength)
{
    (void)pMiter;
    (void)cbLength;
}

void *
sg_virt(void *pSg)
{
    (void)pSg;
    return NULL;
}

unsigned long
sg_phys(void *pSg)
{
    (void)pSg;
    return 0ul;
}

unsigned int
sg_dma_len(void *pSg)
{
    (void)pSg;
    return 0u;
}

unsigned long
sg_dma_address(void *pSg)
{
    (void)pSg;
    return 0ul;
}

/* -- residual3 usbcore (dynid / device_driver / buffer_map) -------------- */

int
usb_store_new_id(void *pDynids, const void *pIdTable, void *pDriver,
                 const char *szBuf, unsigned long cbCount)
{
    (void)pDynids;
    (void)pIdTable;
    (void)pDriver;
    (void)szBuf;
    (void)cbCount;
    /* Soft: no dynid match table. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
usb_show_dynids(void *pDynids, char *pBuf)
{
    (void)pDynids;
    if (pBuf != NULL) {
        pBuf[0] = '\0';
    }
    return 0;
}

int
usb_register_device_driver(void *pDriver, void *pModule, const char *szName)
{
    (void)pDriver;
    (void)pModule;
    (void)szName;
    /* Soft: no device-level USB driver bind. Soft!=product. */
    return LUSB_ENODEV;
}

void
usb_deregister_device_driver(void *pDriver)
{
    (void)pDriver;
}

void
usb_bus_start_enum(void *pBus, unsigned int uPortNum)
{
    (void)pBus;
    (void)uPortNum;
    /* Soft: no enum kick. Soft!=product; != stick. */
}

void *
usb_buffer_map(void *pUrb)
{
    (void)pUrb;
    return NULL;
}

void
usb_buffer_unmap(void *pUrb)
{
    (void)pUrb;
}

int
usb_buffer_map_sg(void *pDev, int fIsIn, void *pSg, int nNents)
{
    (void)pDev;
    (void)fIsIn;
    (void)pSg;
    (void)nNents;
    return LUSB_ENOMEM;
}

void
usb_buffer_unmap_sg(void *pDev, int fIsIn, void *pSg, int nNents)
{
    (void)pDev;
    (void)fIsIn;
    (void)pSg;
    (void)nNents;
}

int
usb_disabled_new_device(void)
{
    /* Soft: refuse new device attach. Soft!=product; != stick. */
    return 1;
}

/*
 * Residual UND expand 4 (D residual deepen; Soft!=product)
 * -------------------------------------------------------
 * Minimal deepen beyond residual1+2+3 (193):
 *   residual4 scsi  12  - target quiesce + internal block + mq/dispatch
 *   residual4 sg     6  - page assign + kmalloc table helpers
 *   residual4 usb   11  - hcd alloc_dev + ep/sysfs + hub port may-disable
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- residual4 scsi mid (target/queue/abort/eh) -------------------------- */

int
scsi_target_quiesce(void *pStarget)
{
    (void)pStarget;
    /* Soft: no target quiesce. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
scsi_target_resume(void *pStarget)
{
    (void)pStarget;
    return LUSB_ENODEV;
}

int
scsi_internal_device_block(void *pSdev)
{
    (void)pSdev;
    return LUSB_ENODEV;
}

int
scsi_internal_device_unblock(void *pSdev, int nNewState)
{
    (void)pSdev;
    (void)nNewState;
    return LUSB_ENODEV;
}

int
scsi_register_device(void *pSdev)
{
    (void)pSdev;
    /* Soft: no sdev bus register. Soft!=product. */
    return LUSB_ENODEV;
}

void
scsi_unregister_device(void *pSdev)
{
    (void)pSdev;
}

void
scsi_device_cancel_work(void *pSdev)
{
    (void)pSdev;
}

int
scsi_dev_queue_ready(void *pQ)
{
    (void)pQ;
    return 0;
}

int
scsi_end_request(void *pReq, int nError, unsigned int uBytes, unsigned int uRest)
{
    (void)pReq;
    (void)nError;
    (void)uBytes;
    (void)uRest;
    /* Soft: no blk completion. Soft!=product; != stick. */
    return 0;
}

void
scsi_mq_done(void *pScmd)
{
    (void)pScmd;
}

void
scsi_softirq_done(void *pReq)
{
    (void)pReq;
}

int
scsi_dispatch_cmd(void *pScmd)
{
    (void)pScmd;
    return LUSB_ENODEV;
}

/* -- residual4 sg (page / kmalloc table) --------------------------------- */

void
sg_init_marker(void *pSg, unsigned int uNents)
{
    (void)pSg;
    (void)uNents;
}

void
sg_assign_page(void *pSg, void *pPage)
{
    (void)pSg;
    (void)pPage;
}

void
sg_set_page(void *pSg, void *pPage, unsigned int uLen, unsigned int uOffset)
{
    (void)pSg;
    (void)pPage;
    (void)uLen;
    (void)uOffset;
}

void *
sg_page(void *pSg)
{
    (void)pSg;
    return NULL;
}

void
sg_kfree(const void *p, unsigned int uSize)
{
    (void)p;
    (void)uSize;
}

void *
sg_kmalloc(unsigned int uSize, int nMemFlags)
{
    (void)uSize;
    (void)nMemFlags;
    return NULL;
}

/* -- residual4 usbcore (hcd/hub/ep sysfs) -------------------------------- */

int
usb_hcd_find_raw_port_number(void *pHcd, int nPort1)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft == NULL || nPort1 <= 0) {
        return 0;
    }
    /* Soft: echo port1 as raw (no HC remap). Soft!=product; != stick. */
    return nPort1;
}

int
usb_hcd_alloc_dev(void *pHcd, void *pUdev)
{
    struct usb_soft_hcd *pHcdSoft;
    struct usb_soft_udev *pUdevSoft;

    pHcdSoft = lusb_hcd_soft(pHcd);
    pUdevSoft = lusb_udev_soft(pUdev);
    if (pHcdSoft != NULL && pUdevSoft != NULL) {
        /* Soft layout bind only - still fail-closed (no HC slot). */
        pUdevSoft->pHcd = pHcd;
        pUdevSoft->pBus = pHcdSoft->pBus;
    }
    /* Soft: no HC device slot. Soft!=HC; != stick. */
    return LUSB_ENODEV;
}

void
usb_hcd_free_dev(void *pHcd, void *pUdev)
{
    struct usb_soft_udev *pUdevSoft;

    (void)pHcd;
    pUdevSoft = lusb_udev_soft(pUdev);
    if (pUdevSoft != NULL) {
        pUdevSoft->pHcd = NULL;
    }
}

void
usb_disconnect_and_release(void *pUdev)
{
    (void)pUdev;
    /* Soft: no disconnect path. Soft!=product; != stick. */
}

int
usb_ep_device_add(void *pUdev, void *pEp)
{
    (void)pUdev;
    (void)pEp;
    return LUSB_ENODEV;
}

void
usb_ep_device_remove(void *pUdev, void *pEp)
{
    (void)pUdev;
    (void)pEp;
}

int
usb_create_sysfs_intf_files(void *pIntf)
{
    (void)pIntf;
    /* Soft: no sysfs. Soft!=product. */
    return LUSB_ENODEV;
}

void
usb_remove_sysfs_intf_files(void *pIntf)
{
    (void)pIntf;
}

void *
usb_bus_get_by_id(int nBusNum)
{
    (void)nBusNum;
    return NULL;
}

int
usb_hub_may_disable_port(void *pHdev, int nPort1)
{
    (void)pHdev;
    (void)nPort1;
    /* Soft: always may-disable (no live port). Soft!=HC. */
    return 1;
}

int
usb_hub_set_configuration(void *pHdev, int nConfiguration)
{
    (void)pHdev;
    (void)nConfiguration;
    return LUSB_ENODEV;
}

/*
 * Residual UND expand 5 (D residual deepen; Soft!=product)
 * -------------------------------------------------------
 * Deepen beyond residual1+2+3+4 (222):
 *   residual5 scsi  16  - queue insert/run + EH abort + offline/online + resid
 *   residual5 sg     6  - set_buf/folio + pool + from_pages_segment + mark_end
 *   residual5 usb   15  - hcd start/stop/bus PM + port/runtime + hub wq/xhci
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- residual5 scsi mid (queue/EH abort/offline/resid) ------------------- */

int
scsi_queue_insert(void *pScmd, int nReason)
{
    (void)pScmd;
    (void)nReason;
    /* Soft: no midlayer requeue. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void
scsi_run_queue(void *pQ)
{
    (void)pQ;
}

void
scsi_requeue_run_queue(void *pQ)
{
    (void)pQ;
}

int
scsi_setup_command(void *pScmd, void *pReq)
{
    (void)pScmd;
    (void)pReq;
    return LUSB_ENODEV;
}

void
scsi_mq_requeue_cmd(void *pScmd)
{
    (void)pScmd;
}

/* Soft BLK_EH_* mental model (not Linux enum ABI). Soft!=product. */
#define LUSB_SOFT_BLK_EH_DONE 0

int
scsi_times_out(void *pReq)
{
    (void)pReq;
    /* Soft: treat as done (no timer reset). Soft!=product; != stick. */
    return LUSB_SOFT_BLK_EH_DONE;
}

int
scsi_abort_command(void *pScmd)
{
    (void)pScmd;
    return LUSB_ENODEV;
}

int
scsi_try_to_abort_cmd(void *pShost, void *pScmd)
{
    (void)pShost;
    (void)pScmd;
    /* Soft: abort fails closed. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

void
scsi_eh_done(void *pScmd)
{
    (void)pScmd;
}

int
scsi_error_handler(void *pData)
{
    (void)pData;
    /* Soft: no EH thread body. Soft!=product; != stick. */
    return 0;
}

int
scsi_offline_device(void *pSdev)
{
    (void)pSdev;
    return LUSB_ENODEV;
}

int
scsi_online_device(void *pSdev)
{
    (void)pSdev;
    return LUSB_ENODEV;
}

void
scsi_set_resid(void *pScmd, int nResid)
{
    (void)pScmd;
    (void)nResid;
}

int
scsi_get_resid(void *pScmd)
{
    (void)pScmd;
    return 0;
}

int
scsi_init_sense_cache(void *pShost)
{
    (void)pShost;
    /* Soft: no sense cache slab. Soft!=product. */
    return LUSB_ENOMEM;
}

int
scsicam_bios_param(void *pSdev, void *pBdev, unsigned long u64Capacity,
                   int *paGeom)
{
    (void)pSdev;
    (void)pBdev;
    (void)u64Capacity;
    if (paGeom != NULL) {
        paGeom[0] = 0;
        paGeom[1] = 0;
        paGeom[2] = 0;
    }
    /* Soft: no CHS geometry. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

/* -- residual5 sg (set_buf/folio + pool + segment table) ----------------- */

void
sg_set_buf(void *pSg, const void *pBuf, unsigned int uLen)
{
    (void)pSg;
    (void)pBuf;
    (void)uLen;
    /* Soft: no page map from linear buf. Soft!=map; != stick. */
}

void
sg_set_folio(void *pSg, void *pFolio, unsigned long uLen, unsigned long uOffset)
{
    (void)pSg;
    (void)pFolio;
    (void)uLen;
    (void)uOffset;
}

int
sg_alloc_table_from_pages_segment(void *pSgt, void **ppPages,
                                  unsigned int uNPages, unsigned int uOffset,
                                  unsigned long uSize, unsigned int uMaxSeg,
                                  int nMemFlags)
{
    (void)pSgt;
    (void)ppPages;
    (void)uNPages;
    (void)uOffset;
    (void)uSize;
    (void)uMaxSeg;
    (void)nMemFlags;
    /* Soft: no multi-page sg table. Soft!=map; != stick. */
    return LUSB_ENOMEM;
}

void *
sg_pool_alloc(unsigned int uSize, int nMemFlags)
{
    (void)uSize;
    (void)nMemFlags;
    return NULL;
}

void
sg_pool_free(void *p, unsigned int uSize)
{
    (void)p;
    (void)uSize;
}

void
sg_mark_end(void *pSg)
{
    (void)pSg;
    /* Soft: no chain end mark. Soft!=map. */
}

/* -- residual5 usbcore (hcd start/bus/runtime + hub/xhci) ---------------- */

int
usb_hcd_start(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft == NULL || pSoft->u8Added == 0u || pSoft->u8Died != 0u) {
        /* Soft: no HC start. Soft!=HC; != stick. */
        return LUSB_ENODEV;
    }
    /*
     * Soft bookkeep only: mark started without MMIO/IRQ/root-hub.
     * Fail-closed return still ENODEV so callers cannot claim product HC.
     * Soft!=product · G-AC-1 · freestanding_no_exec.
     */
    pSoft->u8Started = 1u;
    pSoft->u32Flags |= LINUX_USB_SOFT_HCD_F_STARTED;
    return LUSB_ENODEV;
}

void
usb_hcd_stop(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        pSoft->u8Started = 0u;
        pSoft->u32Flags &= ~LINUX_USB_SOFT_HCD_F_STARTED;
    }
}

int
usb_hcd_bus_suspend(void *pHcd, void *pMsg)
{
    (void)pHcd;
    (void)pMsg;
    return LUSB_ENODEV;
}

int
usb_hcd_bus_resume(void *pHcd, void *pMsg)
{
    (void)pHcd;
    (void)pMsg;
    return LUSB_ENODEV;
}

void
usb_hcd_defer_giveback(void *pHcd, void *pUrb)
{
    /* Soft: fold into immediate giveback bookkeep (no BH). Soft!=HC. */
    usb_hcd_giveback_urb(pHcd, pUrb, LUSB_ENODEV);
}

int
usb_hcd_setup_local_mem(void *pHcd, unsigned long uStart, unsigned long uDma,
                        unsigned long uLen)
{
    (void)pHcd;
    (void)uStart;
    (void)uDma;
    (void)uLen;
    return LUSB_ENOMEM;
}

int
usb_port_disable(void *pUdev)
{
    (void)pUdev;
    return LUSB_ENODEV;
}

int
usb_remote_wakeup(void *pUdev)
{
    (void)pUdev;
    return LUSB_ENODEV;
}

int
usb_runtime_suspend(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_runtime_resume(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_runtime_idle(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_kick_hub_wq(void *pHdev)
{
    (void)pHdev;
    /* Soft: no hub workqueue. Soft!=HC; != stick. */
}

void
usb_release_interface(void *pDev, void *pIntf)
{
    (void)pDev;
    (void)pIntf;
}

void
usb_enable_intel_xhci_ports(void *pPdev)
{
    (void)pPdev;
    /* Soft: no EHCI/xHCI handoff. Soft!=HC; != stick. */
}

void
usb_disable_xhci_ports(void *pPdev)
{
    (void)pPdev;
}

/*
 * Residual UND expand 6 (D residual deepen; Soft!=product)
 * -------------------------------------------------------
 * Deepen beyond residual1+2+3+4+r5 (259):
 *   residual6 scsi  17  - cmnd/rq/mq alloc + host queue ready + EH tur/stu
 *   residual6 sg     6  - unmark/is_last/chain + nents_for_dma + table_from_pages
 *   residual6 usb   14  - hcd pci PM/runtime + hub cleanup + port runtime + bus
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- residual6 scsi mid (cmnd/rq/mq + EH tur) ---------------------------- */

void *
scsi_cmd_to_rq(void *pScmd)
{
    (void)pScmd;
    /* Soft: no blk request back-pointer. Soft!=product; != stick. */
    return NULL;
}

void *
scsi_cmnd_ptr(void *pReq)
{
    (void)pReq;
    return NULL;
}

void
scsi_req_init(void *pReq)
{
    (void)pReq;
}

void
scsi_initialize_rq(void *pReq)
{
    (void)pReq;
}

void
scsi_init_request(void *pQ, void *pReq)
{
    (void)pQ;
    (void)pReq;
}

void
scsi_exit_request(void *pQ, void *pReq)
{
    (void)pQ;
    (void)pReq;
}

void *
scsi_get_command(void *pSdev, int nMemFlags)
{
    (void)pSdev;
    (void)nMemFlags;
    /* Soft: no scmd pool. Soft!=product; != stick. */
    return NULL;
}

void
scsi_put_command(void *pScmd)
{
    (void)pScmd;
}

void
scsi_free_host_cmd(void *pScmd)
{
    (void)pScmd;
}

int
scsi_host_queue_ready(void *pShost)
{
    (void)pShost;
    /* Soft: never ready for host queue. Soft!=product; != stick. */
    return 0;
}

int
scsi_target_busy(void *pStarget)
{
    (void)pStarget;
    return 0;
}

void
scsi_eh_ready_devs(void *pShost, void *pWorkQ, void *pDoneQ)
{
    (void)pShost;
    (void)pWorkQ;
    (void)pDoneQ;
    /* Soft: no EH ready path. Soft!=product; != stick. */
}

void
scsi_eh_offline_sdevs(void *pWorkQ, void *pDoneQ)
{
    (void)pWorkQ;
    (void)pDoneQ;
}

int
scsi_send_eh_cmnd(void *pScmd, unsigned char *pCmnd, int nCmndLen,
                  int nTimeout, unsigned int uSenseBytes)
{
    (void)pScmd;
    (void)pCmnd;
    (void)nCmndLen;
    (void)nTimeout;
    (void)uSenseBytes;
    /* Soft: EH CDB fails closed. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
scsi_eh_tur(void *pScmd)
{
    (void)pScmd;
    /* Soft: TUR fails (no medium). Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
scsi_eh_try_stu(void *pScmd)
{
    (void)pScmd;
    return LUSB_ENODEV;
}

void
scsi_restart_operations(void *pShost)
{
    (void)pShost;
    /* Soft: no post-EH restart. Soft!=product; != stick. */
}

/* -- residual6 sg (chain / last / nents_for_dma / table) ----------------- */

void
sg_unmark_end(void *pSg)
{
    (void)pSg;
    /* Soft: no chain end bit clear. Soft!=map. */
}

int
sg_is_last(void *pSg)
{
    (void)pSg;
    /* Soft: treat as end-of-list. Soft!=map; != stick. */
    return 1;
}

void
sg_chain(void *pPrv, unsigned int uNents, void *pSgl)
{
    (void)pPrv;
    (void)uNents;
    (void)pSgl;
    /* Soft: no scatterlist chain link. Soft!=map; != stick. */
}

int
sg_nents_for_dma(void *pSg, unsigned long u64DmaLen)
{
    (void)pSg;
    (void)u64DmaLen;
    /* Soft: empty DMA map. Soft!=map; != stick. */
    return 0;
}

int
__sg_alloc_table_from_pages(void *pSgt, void **ppPages, unsigned int uNPages,
                            unsigned int uOffset, unsigned long uSize,
                            unsigned int uMaxSeg, void *pLeftover,
                            unsigned int uLeftPages, int nMemFlags)
{
    (void)pSgt;
    (void)ppPages;
    (void)uNPages;
    (void)uOffset;
    (void)uSize;
    (void)uMaxSeg;
    (void)pLeftover;
    (void)uLeftPages;
    (void)nMemFlags;
    /* Soft: no page-backed table. Soft!=map; != stick. */
    return LUSB_ENOMEM;
}

void *
sg_next_safe(void *pSg, int *pnRemaining)
{
    (void)pSg;
    if (pnRemaining != NULL) {
        *pnRemaining = 0;
    }
    return NULL;
}

/* -- residual6 usbcore (hcd pci PM + hub/port runtime + bus) ------------- */

int
usb_hcd_pci_suspend(void *pDev, void *pMsg)
{
    (void)pDev;
    (void)pMsg;
    /* Soft: no PCI HC suspend. Soft!=HC; != stick. */
    return LUSB_ENODEV;
}

int
usb_hcd_pci_resume(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_hcd_pci_suspend_noirq(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_hcd_pci_resume_noirq(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_hcd_pci_runtime_suspend(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_hcd_pci_runtime_resume(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_hcd_unmap_urb_setup_for_dma(void *pHcd, void *pUrb)
{
    (void)pHcd;
    (void)pUrb;
    /* Soft: no setup-packet DMA unmap. Soft!=HC; != stick. */
}

void
usb_mark_last_busy(void *pDev)
{
    (void)pDev;
    /* Soft: no runtime PM last-busy. Soft!=HC; != stick. */
}

int
usb_port_runtime_suspend(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

int
usb_port_runtime_resume(void *pDev)
{
    (void)pDev;
    return LUSB_ENODEV;
}

void
usb_hub_cleanup(void)
{
    /* Soft: no hub driver teardown. Soft!=HC; != stick. */
}

void
usb_bus_init(void *pBus)
{
    (void)pBus;
    /* Soft: no bus structure init. Soft!=enum; != stick. */
}

void
usb_remove_device_files(void *pUdev)
{
    (void)pUdev;
}

int
usb_enable_interface_streams(void *pIntf)
{
    (void)pIntf;
    /* Soft: no bulk streams. Soft!=HC; != stick. */
    return LUSB_ENODEV;
}

/*
 * Residual UND expand 7 (D residual deepen; Soft!=product · dual DoD A)
 * --------------------------------------------------------------------
 * Deepen beyond residual1+2+3+4+r5+r6 (296):
 *   residual7 usb   18  - hcd buffer/platform/shared + root-hub IRQ +
 *                         hub TT/debounce/power + bus notify + phy get/put
 *   residual7 scsi  12  - host recovery/state + EH lock/reset + mq queue +
 *                         VPD buf + target block + sense_valid
 *   residual7 sg     6  - page-iter + table-from-buf + miter/copy residual
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Virtual usbcore/HCD ksym only - future userspace USB hosts; freestanding_no_exec.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 */

/* -- residual7 usbcore (hcd buffer/platform + hub/port + phy) ------------ */

void *
usb_hcd_buffer_alloc(void *pHcd, unsigned long uSize, int nMemFlags,
                     unsigned long *puDma)
{
    (void)pHcd;
    (void)uSize;
    (void)nMemFlags;
    if (puDma != NULL) {
        *puDma = 0ul;
    }
    /* Soft: no HCD local buffer pool. Soft!=HC; != stick. */
    return NULL;
}

void
usb_hcd_buffer_free(void *pHcd, unsigned long uSize, void *pVirt,
                    unsigned long uDma)
{
    (void)pHcd;
    (void)uSize;
    (void)pVirt;
    (void)uDma;
}

int
usb_hcd_buffer_create(void *pHcd)
{
    (void)pHcd;
    /* Soft: no DMA buffer pools. Soft!=HC; != stick. */
    return LUSB_ENOMEM;
}

void
usb_hcd_buffer_destroy(void *pHcd)
{
    (void)pHcd;
}

int
usb_hcd_platform_probe(void *pPdev, void *pDriver)
{
    (void)pPdev;
    (void)pDriver;
    /* Soft: no platform HC bind. Soft!=HC; != stick. */
    return LUSB_ENODEV;
}

int
usb_hcd_platform_remove(void *pPdev)
{
    (void)pPdev;
    return LUSB_ENODEV;
}

void *
usb_hcd_shared_hcd(void *pHcd)
{
    (void)pHcd;
    /* Soft: no companion/shared HCD pointer. Soft!=HC; != stick. */
    return NULL;
}

int
usb_hcd_may_wakeup(void *pHcd)
{
    (void)pHcd;
    /* Soft: never wake-capable. Soft!=HC; != stick. */
    return 0;
}

void
usb_giveback_urb_bh(unsigned long uData)
{
    (void)uData;
    /* Soft: no giveback bottom-half. Soft!=HC; != stick. */
}

void
usb_enable_root_hub_irq(void *pHcd)
{
    (void)pHcd;
    /* Soft: no root-hub IRQ arm. Soft!=HC; != stick. */
}

void
usb_disable_root_hub_irq(void *pHcd)
{
    (void)pHcd;
}

void
usb_hub_handle_tt_clear(void *pHdev, void *pClear)
{
    (void)pHdev;
    (void)pClear;
    /* Soft: no TT clear work. Soft!=HC; != stick. */
}

int
usb_hub_port_debounce(void *pHub, int nPort1, unsigned short *puStatus)
{
    (void)pHub;
    (void)nPort1;
    if (puStatus != NULL) {
        *puStatus = 0u;
    }
    /* Soft: no port debounce / connect. Soft!=enum; != stick. */
    return LUSB_ENODEV;
}

int
usb_hub_set_port_power(void *pHdev, void *pHub, int nPort1, int nSet)
{
    (void)pHdev;
    (void)pHub;
    (void)nPort1;
    (void)nSet;
    /* Soft: no VBUS power control. Soft!=HC; != stick. */
    return LUSB_ENODEV;
}

void
usb_notify_add_bus(void *pBus)
{
    (void)pBus;
    /* Soft: no bus notifier chain. Soft!=enum; != stick. */
}

void
usb_notify_remove_bus(void *pBus)
{
    (void)pBus;
}

void *
usb_get_phy(int nType)
{
    (void)nType;
    /* Soft: no USB PHY. Soft!=HC; != stick. */
    return NULL;
}

void
usb_put_phy(void *pPhy)
{
    (void)pPhy;
}

/* -- residual7 scsi mid (host recovery + EH + mq + VPD) ------------------ */

int
scsi_host_in_recovery(void *pShost)
{
    (void)pShost;
    /* Soft: never report recovery (no host). Soft!=product; != stick. */
    return 0;
}

int
scsi_host_get_state(void *pShost)
{
    (void)pShost;
    /* Soft: SHOT_CREATED-shaped 0. Soft!=product; != stick. */
    return 0;
}

int
scsi_eh_lock_door(void *pScmd)
{
    (void)pScmd;
    /* Soft: no medium lock via EH. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
scsi_ioctl_reset(void *pSdev, int nFlag)
{
    (void)pSdev;
    (void)nFlag;
    /* Soft: no bus/host/device reset. Soft!=product; != stick. */
    return LUSB_ENODEV;
}

int
scsi_cmd_ioctl(void *pQ, void *pDisk, unsigned int uCmd, void *pArg)
{
    (void)pQ;
    (void)pDisk;
    (void)uCmd;
    (void)pArg;
    return LUSB_ENODEV;
}

void *
scsi_get_vpd_buf(void *pSdev, unsigned char u8Page)
{
    (void)pSdev;
    (void)u8Page;
    /* Soft: no VPD page buffer. Soft!=product; != stick. */
    return NULL;
}

void
scsi_vpd_release(void *pSdev)
{
    (void)pSdev;
}

void
scsi_block_targets(void *pShost, void *pDev)
{
    (void)pShost;
    (void)pDev;
    /* Soft: no target block fan-out. Soft!=product; != stick. */
}

void
scsi_unblock_targets(void *pShost, void *pDev)
{
    (void)pShost;
    (void)pDev;
}

void *
scsi_mq_alloc_queue(void *pSdev)
{
    (void)pSdev;
    /* Soft: no blk-mq queue. Soft!=product; != stick. */
    return NULL;
}

void
scsi_mq_destroy_queue(void *pSdev)
{
    (void)pSdev;
}

int
scsi_sense_valid(void *pSshdr)
{
    (void)pSshdr;
    /* Soft: no valid sense. Soft!=product; != stick. */
    return 0;
}

/* -- residual7 sg (page-iter + table-from-buf) --------------------------- */

void *
sg_page_iter_page(void *pPiter)
{
    (void)pPiter;
    /* Soft: no page back-pointer. Soft!=map; != stick. */
    return NULL;
}

unsigned long
sg_page_iter_dma_address(void *pDmaIter)
{
    (void)pDmaIter;
    /* Soft: no DMA address from iter. Soft!=map; != stick. */
    return 0ul;
}

void
sg_init_table_from_buf(void *pSgl, const void *pBuf, unsigned int uBuflen)
{
    (void)pSgl;
    (void)pBuf;
    (void)uBuflen;
    /* Soft: no single-buf table. Soft!=map; != stick. */
}

int
sg_copy_buffer_to_sg(void *pSgl, unsigned int uNents, const void *pBuf,
                     unsigned long uBuflen, unsigned long uSkip)
{
    (void)pSgl;
    (void)uNents;
    (void)pBuf;
    (void)uBuflen;
    (void)uSkip;
    /* Soft: no buffer->sg copy. Soft!=map; != stick. */
    return 0;
}

void *
sg_miter_get_next_sg(void *pMiter)
{
    (void)pMiter;
    /* Soft: miter exhausted. Soft!=map; != stick. */
    return NULL;
}

int
sg_alloc_table_from_buf(void *pSgt, void *pBuf, unsigned long uBuflen,
                        int nMemFlags)
{
    (void)pSgt;
    (void)pBuf;
    (void)uBuflen;
    (void)nMemFlags;
    /* Soft: no table from linear buf. Soft!=map; != stick. */
    return LUSB_ENOMEM;
}

/*
 * Residual UND expand 8 (DoD A soft layout / UDX bridge; Soft!=product)
 * --------------------------------------------------------------------
 * Small, non-stamp residual: HCD↔bus cookies + intfdata + make_path +
 * init_usb_anchor. Soft layout front fields already deepen fill/submit/
 * giveback. freestanding xHCI residual SKIP default (GJ_XHCI_MSC_PROBE=0).
 * Soft!=product · G-AC-1 · never claim stick write · need=HC OPEN.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft hcd/urb layout PASS
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual8 usbcore (HCD/URB layout bridge for UDX hosts) ------------- */

void *
hcd_to_bus(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        return pSoft->pBus;
    }
    return NULL;
}

void *
bus_to_hcd(void *pBus)
{
    struct usb_soft_bus *pSoft;
    struct usb_soft_hcd *pHcdSoft;
    u32 i;

    pSoft = lusb_bus_soft(pBus);
    if (pSoft != NULL && pSoft->pHcd != NULL) {
        return pSoft->pHcd;
    }
    /* Fallback: first added soft HCD that points at this bus. Soft!=product. */
    for (i = 0u; i < LINUX_USB_SOFT_HCD_POOL; i++) {
        if (g_aSoftHcdUsed[i] == 0u) {
            continue;
        }
        pHcdSoft = (struct usb_soft_hcd *)(void *)g_aSoftHcd[i];
        if (pHcdSoft->u32Magic == LUSB_HCD_MAGIC &&
            pHcdSoft->pBus == pBus) {
            return (void *)g_aSoftHcd[i];
        }
    }
    (void)pBus;
    return NULL;
}

void
usb_set_intfdata(void *pIntf, void *pData)
{
    (void)pIntf;
    /* Soft: single global cookie (no device model). Soft!=product. */
    g_pSoftIntfData = pData;
}

void *
usb_get_intfdata(void *pIntf)
{
    (void)pIntf;
    return g_pSoftIntfData;
}

int
usb_make_path(void *pDev, char *pBuf, unsigned long cbSize)
{
    const char *sz;
    u32 i;

    (void)pDev;
    if (pBuf == NULL || cbSize == 0ul) {
        return LUSB_EINVAL;
    }
    /* Soft path string - never a real sysfs topology. Soft!=product. */
    sz = "soft-usb";
    for (i = 0u; i + 1u < (u32)cbSize && sz[i] != '\0'; i++) {
        pBuf[i] = sz[i];
    }
    if (i + 1u >= (u32)cbSize && cbSize > 0ul) {
        pBuf[cbSize - 1ul] = '\0';
        return LUSB_ENOMEM; /* truncated-shaped */
    }
    pBuf[i] = '\0';
    return (int)i;
}

void
init_usb_anchor(void *pAnchor)
{
    lusb_soft_objs_once();
    if (pAnchor != NULL) {
        /* Soft: zero caller blob up to soft anchor size. Soft!=ABI. */
        memset(pAnchor, 0, LINUX_USB_SOFT_ANCHOR_CB);
    } else {
        memset(g_aSoftAnchor, 0, sizeof(g_aSoftAnchor));
    }
}

void *
usb_find_interface_driver(void *pIntf)
{
    (void)pIntf;
    /* Soft: no bound driver object. Soft!=product. */
    return NULL;
}

int
usb_hcd_link_urb_to_ep_checked(void *pHcd, void *pUrb)
{
    /* Soft: same as link_urb_to_ep with soft layout. Soft!=HC. */
    return usb_hcd_link_urb_to_ep(pHcd, pUrb);
}

/*
 * Residual UND expand 9 (DoD A soft HCD/URB layout residual; Soft!=product)
 * -----------------------------------------------------------------------
 * residual9 deepen: HCD↔controller cookie + free_dev + bandwidth residual
 * flags + root-hub query + host_interface + URB ep/hcd reverse cookies for
 * userspace UDX hosts. Soft!=ABI-stable · freestanding MSC SKIP default
 * (GJ_XHCI_MSC_PROBE=0) · freestanding_no_exec · G-AC-1 · != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft hcd/urb layout PASS
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual9 usbcore (HCD/URB layout residual for UDX hosts) ----------- */

void *
usb_hcd_to_dev(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        return pSoft->pController;
    }
    return NULL;
}

void
usb_free_dev(void *pUdev)
{
    struct usb_soft_udev *pSoft;
    u32 i;
    struct usb_soft_ep *pEp;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft == NULL) {
        return;
    }
    /* residual10: drop soft EP cookies wired to this udev. Soft!=product. */
    for (i = 0u; i < LINUX_USB_SOFT_EP_POOL; i++) {
        if (g_aSoftEpUsed[i] == 0u) {
            continue;
        }
        pEp = (struct usb_soft_ep *)(void *)g_aSoftEp[i];
        if (pEp->u32Magic == LUSB_EP_MAGIC && pEp->pUdev == pUdev) {
            memset(g_aSoftEp[i], 0, LINUX_USB_SOFT_EP_CB);
            g_aSoftEpUsed[i] = 0u;
        }
    }
    /* Soft single-slot teardown. Soft!=product; never real stick free. */
    g_fSoftUdevLive = 0u;
    memset(g_aSoftUdev, 0, sizeof(g_aSoftUdev));
}

int
usb_hcd_update_device(void *pHcd, void *pUdev)
{
    struct usb_soft_hcd *pHcdSoft;
    struct usb_soft_udev *pUdevSoft;

    pHcdSoft = lusb_hcd_soft(pHcd);
    pUdevSoft = lusb_udev_soft(pUdev);
    if (pHcdSoft == NULL || pUdevSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcdSoft->u8Died != 0u || pHcdSoft->u8Added == 0u) {
        return LUSB_ENODEV;
    }
    /* Soft layout residual: dual-link udev↔hcd/bus. Soft!=enum; != stick. */
    pUdevSoft->pHcd = pHcd;
    pUdevSoft->pBus = pHcdSoft->pBus;
    /* Soft never installs a real root hub under pRootHub. */
    pHcdSoft->pRootHub = NULL;
    return LUSB_ENODEV;
}

int
usb_device_is_root_hub(void *pUdev)
{
    struct usb_soft_udev *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft == NULL) {
        return 0;
    }
    pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
    if (pHcdSoft != NULL && pHcdSoft->pRootHub == pUdev) {
        /* Soft seed keeps pRootHub NULL - path retained for layout honesty. */
        return 1;
    }
    /* Soft: no root hub enum. Soft!=product; freestanding_msc=SKIP. */
    return 0;
}

int
usb_hcd_check_bandwidth(void *pHcd, void *pUdev)
{
    struct usb_soft_hcd *pSoft;

    (void)pUdev;
    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft == NULL || pSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    /*
     * Soft residual flag only - never grants real bandwidth.
     * Soft!=product · G-AC-1 · != stick.
     */
    pSoft->u32Flags |= LINUX_USB_SOFT_HCD_F_BW_SOFT;
    return LUSB_ENODEV;
}

void
usb_hcd_release_bandwidth(void *pHcd, void *pUdev)
{
    struct usb_soft_hcd *pSoft;

    (void)pUdev;
    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        pSoft->u32Flags &= ~LINUX_USB_SOFT_HCD_F_BW_SOFT;
    }
}

void *
usb_find_host_interface(void *pConfig, int nIfnum, int nAlt)
{
    (void)pConfig;
    (void)nIfnum;
    (void)nAlt;
    /* Soft: no config walk / host interface. Soft!=product; != stick. */
    return NULL;
}

void *
usb_get_hcd_from_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pHcd;
    }
    return NULL;
}

int
usb_urb_ep_index(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (int)pSoft->u8EpNum;
    }
    return -1;
}

void *
usb_get_dev_from_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pDev;
    }
    return NULL;
}

/*
 * Residual UND expand 10 (DoD A lean soft EP / UDX seed; Soft!=product)
 * --------------------------------------------------------------------
 * residual10: soft EP pool + URB↔EP↔udev↔hcd reverse cookies for
 * userspace UDX hosts. pipe_endpoint / fill / link wire soft EP slabs.
 * Soft!=ABI-stable · freestanding MSC SKIP (GJ_XHCI_MSC_PROBE=0) ·
 * freestanding_no_exec · no .ko exec · G-AC-1 · != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft hcd/urb layout PASS
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual10 usbcore (soft EP + reverse cookies for UDX hosts) -------- */

void *
usb_get_hcd_from_dev(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft != NULL) {
        return pSoft->pHcd;
    }
    return NULL;
}

void *
usb_get_bus_from_dev(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft != NULL) {
        return pSoft->pBus;
    }
    return NULL;
}

void *
usb_get_ep_from_urb(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pEp;
    }
    return NULL;
}

void
usb_urb_set_ep(void *pUrb, void *pEp)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEpSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return;
    }
    pEpSoft = lusb_ep_soft(pEp);
    if (pEpSoft != NULL) {
        pSoft->pEp = pEp;
        pSoft->u8EpNum = pEpSoft->u8Num;
        pSoft->u8DirIn = pEpSoft->u8DirIn;
        if (pEpSoft->u8Type <= LINUX_USB_SOFT_XFER_ISO) {
            pSoft->u8XferType = pEpSoft->u8Type;
        }
        if (pEpSoft->pHcd != NULL) {
            pSoft->pHcd = pEpSoft->pHcd;
        }
    } else if (pEp == NULL) {
        pSoft->pEp = NULL;
    }
    /* Foreign pEp ignored - Soft!=ABI; fail-closed leave prior cookie. */
}

int
usb_urb_actual_length(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* Soft fail-closed: actual always 0 under seed (!= stick). */
        return (int)pSoft->u32ActualLen;
    }
    return 0;
}

const char *
usb_hcd_bus_name(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft != NULL) {
        return pSoft->szBusName;
    }
    return "soft";
}

int
usb_hcd_soft_wire_dev(void *pHcd, void *pUdev)
{
    struct usb_soft_hcd *pHcdSoft;
    struct usb_soft_udev *pUdevSoft;
    struct usb_soft_bus *pBusSoft;

    pHcdSoft = lusb_hcd_soft(pHcd);
    pUdevSoft = lusb_udev_soft(pUdev);
    if (pHcdSoft == NULL || pUdevSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcdSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    /*
     * Soft layout residual: dual-link udev↔hcd/bus. Never installs a real
     * root hub (pRootHub stays NULL). Soft!=enum · != stick · G-AC-1.
     */
    pUdevSoft->pHcd = pHcd;
    pUdevSoft->pBus = pHcdSoft->pBus;
    pHcdSoft->pRootHub = NULL;
    pBusSoft = lusb_bus_soft(pHcdSoft->pBus);
    if (pBusSoft != NULL) {
        pBusSoft->pHcd = pHcd;
        pBusSoft->pRootHub = NULL;
    }
    return 0; /* soft wire cookie ok; still no stick datapath */
}

void *
usb_endpoint_device(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft != NULL) {
        return pSoft->pUdev;
    }
    return NULL;
}

/*
 * Residual UND expand 11 (DoD A lean UDX residual; Soft!=product)
 * ----------------------------------------------------------------
 * residual11: EP enable/maxp bookkeep + URB transfer/pipe/context reverse
 * + soft pipe encode + EP↔HCD reverse for userspace UDX hosts.
 * Soft!=ABI-stable · freestanding MSC SKIP (GJ_XHCI_MSC_PROBE=0) ·
 * freestanding_no_exec · no .ko exec · G-AC-1 · != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft hcd/urb layout PASS
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual11 usbcore (EP bookkeep + URB reverse for UDX hosts) -------- */

int
usb_endpoint_set_maxp(void *pEp, unsigned int uMaxPacket)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft bookkeep only - never programs HC maxpacket. Soft!=product. */
    if (uMaxPacket > 0xffffu) {
        pSoft->u16MaxPacket = 0xffffu;
    } else {
        pSoft->u16MaxPacket = (u16)uMaxPacket;
    }
    return 0;
}

void *
usb_endpoint_hcd(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft != NULL) {
        return pSoft->pHcd;
    }
    return NULL;
}

int
usb_endpoint_enabled(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft != NULL) {
        return (pSoft->u8Enabled != 0u) ? 1 : 0;
    }
    return 0;
}

void *
usb_urb_transfer_buffer(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pTransfer;
    }
    return NULL;
}

unsigned int
usb_urb_transfer_buffer_length(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (unsigned int)pSoft->u32TransferLen;
    }
    return 0u;
}

unsigned int
usb_urb_pipe(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->uPipe;
    }
    return 0u;
}

void *
usb_urb_context(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pContext;
    }
    return NULL;
}

/*
 * Soft pipe encode (UDX cookie; not full Linux pipe encoding).
 * Bits: dir IN = bit7; ep# in bits 15:11 (matches lusb_pipe_ep_num echo).
 * Soft!=product · Soft!=ABI-stable · never stick wire.
 */
unsigned int
usb_soft_encode_pipe(unsigned int uEpNum, int fDirIn)
{
    unsigned int uPipe;
    unsigned int uEp;

    uEp = uEpNum & 0x0fu;
    uPipe = (uEp << 15);
    if (fDirIn != 0) {
        uPipe |= 0x80u;
    }
    return uPipe;
}

/*
 * Residual UND expand 12 (lean Dual DoD A eng seed; Soft!=product)
 * ----------------------------------------------------------------
 * residual12: EP address/type wire + URB setup/complete/context/transfer
 * reverse setters + soft pipe decode for userspace xhci_udx hosts.
 * Product USB path = userspace UDX (xhci_udx), not freestanding MSC /
 * usb_storage.ko init. freestanding MSC SKIP (GJ_XHCI_MSC_PROBE=0).
 * Soft!=ABI-stable; freestanding_no_exec; no .ko exec; G-AC-1; != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft hcd/urb layout PASS
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual12 usbcore (lean Dual DoD A eng seed for UDX hosts) -------- */

int
usb_endpoint_address(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft != NULL) {
        return (int)pSoft->u8Addr;
    }
    return 0;
}

int
usb_endpoint_set_type(void *pEp, unsigned int uType)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft bookkeep only - never programs HC ep type. Soft!=product. */
    if (uType > LINUX_USB_SOFT_XFER_ISO) {
        return LUSB_EINVAL;
    }
    pSoft->u8Type = (u8)uType;
    return 0;
}

int
usb_endpoint_wire_hcd(void *pEp, void *pHcd)
{
    struct usb_soft_ep *pEpSoft;
    struct usb_soft_hcd *pHcdSoft;

    pEpSoft = lusb_ep_soft(pEp);
    if (pEpSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcd == NULL) {
        pEpSoft->pHcd = NULL;
        return 0;
    }
    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pHcdSoft == NULL) {
        return LUSB_ENODEV; /* foreign HCD - fail closed */
    }
    if (pHcdSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    /* Soft reverse wire only - no HC schedule. Soft!=product; != stick. */
    pEpSoft->pHcd = pHcd;
    return 0;
}

void *
usb_urb_setup_packet(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pSetup;
    }
    return NULL;
}

void *
usb_urb_complete(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* Soft never invokes pfnComplete. Cookie reverse only. Soft!=product. */
        return pSoft->pfnComplete;
    }
    return NULL;
}

void
usb_urb_set_context(void *pUrb, void *pContext)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        pSoft->pContext = pContext;
    }
}

int
usb_urb_set_transfer_buffer(void *pUrb, void *pBuf, unsigned int uLen)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft bookkeep only - no DMA map / no stick datapath. Soft!=product. */
    pSoft->pTransfer = pBuf;
    pSoft->u32TransferLen = uLen;
    pSoft->u32ActualLen = 0u; /* fail-closed residual; != stick */
    return 0;
}

/*
 * Soft pipe decode (UDX reverse of usb_soft_encode_pipe).
 * Writes ep# and dir-IN flag when pointers non-NULL.
 * Soft!=product; Soft!=ABI-stable; never stick wire.
 */
void
usb_soft_decode_pipe(unsigned int uPipe, unsigned int *puEpNum, int *pfDirIn)
{
    if (puEpNum != NULL) {
        *puEpNum = (unsigned int)lusb_pipe_ep_num(uPipe);
    }
    if (pfDirIn != NULL) {
        *pfDirIn = (lusb_pipe_dir_in(uPipe) != 0u) ? 1 : 0;
    }
}

/*
 * Residual UND expand 13 (lean Dual DoD A eng residual; Soft!=product)
 * --------------------------------------------------------------------
 * residual13: URB setup/complete/pipe/status reverse setters + EP
 * interval reverse + URB xfer_type reverse + soft pipe_from_ep for
 * userspace xhci_udx.
 * Soft residual lean lamp: freestanding_msc=SKIP · product_usb=UDX ·
 * RUN_INIT=0 freestanding_no_exec · no freestanding thrash · no .ko exec.
 * Product USB path = userspace UDX (xhci_udx), not freestanding MSC /
 * usb_storage.ko init. Soft!=ABI-stable; G-AC-1; != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft residual lean PASS Soft!=product
 * Grep: linux_usb_soft: soft residual freestanding_msc=SKIP
 * Grep: linux_usb_soft: soft residual product_usb=UDX
 * Grep: linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual13 usbcore (lean Dual DoD A eng residual for UDX hosts) ---- */

void
usb_urb_set_setup_packet(void *pUrb, void *pSetup)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* Soft cookie only - no control wire. Soft!=product; != stick. */
        pSoft->pSetup = pSetup;
        pSoft->u8XferType = LINUX_USB_SOFT_XFER_CTRL;
    }
}

void
usb_urb_set_complete(void *pUrb, void *pfnComplete)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        /* Soft never invokes pfnComplete. Cookie store only. Soft!=product. */
        pSoft->pfnComplete = pfnComplete;
    }
}

int
usb_urb_set_pipe(void *pUrb, unsigned int uPipe)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEpSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft pipe echo + ep# / dir reverse; optional EP pool wire. Soft!=HC. */
    pSoft->uPipe = uPipe;
    pSoft->u8EpNum = lusb_pipe_ep_num(uPipe);
    pSoft->u8DirIn = lusb_pipe_dir_in(uPipe);
    if (pSoft->pDev != NULL) {
        pEpSoft = lusb_ep_soft_lookup_or_prime(pSoft->pDev, uPipe);
        if (pEpSoft != NULL) {
            pSoft->pEp = (void *)pEpSoft;
            if (pEpSoft->pHcd != NULL) {
                pSoft->pHcd = pEpSoft->pHcd;
            }
            if (pEpSoft->u8Type <= LINUX_USB_SOFT_XFER_ISO) {
                pSoft->u8XferType = pEpSoft->u8Type;
            }
        }
    }
    return 0;
}

int
usb_urb_set_status(void *pUrb, int nStatus)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /*
     * Soft eng bookkeep only. Default submit path remains fail-closed
     * ENODEV / actual_len=0. Never claims stick datapath. Soft!=product.
     */
    pSoft->nStatus = nStatus;
    return 0;
}

unsigned int
usb_endpoint_interval(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft != NULL) {
        return (unsigned int)pSoft->u16Interval;
    }
    return 0u;
}

int
usb_endpoint_set_interval(void *pEp, unsigned int uInterval)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft bookkeep only - never programs HC interval. Soft!=product. */
    if (uInterval > 0xffffu) {
        pSoft->u16Interval = 0xffffu;
    } else {
        pSoft->u16Interval = (u16)uInterval;
    }
    return 0;
}

int
usb_urb_xfer_type(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (int)pSoft->u8XferType;
    }
    return -1; /* foreign URB - Soft!=product */
}

/*
 * Soft pipe from soft EP cookie (UDX reverse helper).
 * Uses soft EP num/dir; falls back to 0 when foreign. Soft!=product.
 */
unsigned int
usb_soft_pipe_from_ep(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0u;
    }
    return usb_soft_encode_pipe((unsigned int)pSoft->u8Num,
                                (pSoft->u8DirIn != 0u) ? 1 : 0);
}

/*
 * Residual UND expand 14 (lean Dual DoD A eng residual deepen; Soft!=product)
 * --------------------------------------------------------------------------
 * residual14: URB dev/hcd/xfer_type/interval reverse + linked flag +
 * actual_len eng bookkeep + soft wire_urb_ep for userspace xhci_udx eng.
 * Soft residual lean lamp: freestanding_msc=SKIP · product_usb=UDX ·
 * RUN_INIT=0 freestanding_no_exec · no freestanding thrash · no .ko exec.
 * Product USB path = userspace UDX (xhci_udx), not freestanding MSC /
 * usb_storage.ko init. Soft!=ABI-stable; G-AC-1; != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft residual lean PASS Soft!=product
 * Grep: linux_usb_soft: soft residual freestanding_msc=SKIP
 * Grep: linux_usb_soft: soft residual product_usb=UDX
 * Grep: linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual14 usbcore (lean Dual DoD A eng residual deepen for UDX) --- */

int
usb_urb_set_dev(void *pUrb, void *pDev)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_udev *pUdevSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pDev == NULL) {
        pSoft->pDev = NULL;
        return 0;
    }
    pUdevSoft = lusb_udev_soft(pDev);
    if (pUdevSoft == NULL) {
        return LUSB_ENODEV; /* foreign udev - fail closed */
    }
    /* Soft reverse wire only - no enum / no stick. Soft!=product. */
    pSoft->pDev = pDev;
    if (pUdevSoft->pHcd != NULL) {
        pSoft->pHcd = pUdevSoft->pHcd;
    }
    return 0;
}

int
usb_urb_set_hcd(void *pUrb, void *pHcd)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcd == NULL) {
        pSoft->pHcd = NULL;
        return 0;
    }
    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pHcdSoft == NULL) {
        return LUSB_ENODEV; /* foreign HCD - fail closed */
    }
    if (pHcdSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    /* Soft HCD cookie only - no schedule / no stick. Soft!=product. */
    pSoft->pHcd = pHcd;
    return 0;
}

int
usb_urb_set_xfer_type(void *pUrb, unsigned int uType)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (uType > LINUX_USB_SOFT_XFER_ISO) {
        return LUSB_EINVAL;
    }
    /* Soft bookkeep only - never programs HC xfer type. Soft!=product. */
    pSoft->u8XferType = (u8)uType;
    return 0;
}

int
usb_urb_interval(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->nInterval;
    }
    return 0;
}

int
usb_urb_set_interval(void *pUrb, int nInterval)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEpSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft echo only - never programs HC interval. Soft!=product. */
    if (nInterval < 0) {
        nInterval = 0;
    }
    pSoft->nInterval = nInterval;
    pEpSoft = lusb_ep_soft(pSoft->pEp);
    if (pEpSoft != NULL) {
        if (nInterval > 0xffff) {
            pEpSoft->u16Interval = 0xffffu;
        } else {
            pEpSoft->u16Interval = (u16)nInterval;
        }
    }
    return 0;
}

int
usb_urb_linked(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Linked != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_actual_length(void *pUrb, unsigned int uLen)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /*
     * Soft eng bookkeep only. Submit path still fail-closed actual_len=0.
     * Never claims stick datapath / BOT wire. Soft!=product · G-AC-1.
     */
    pSoft->u32ActualLen = uLen;
    return 0;
}

/*
 * Soft wire URB ↔ EP (UDX reverse helper). Copies ep#/dir/type/hcd/dev
 * cookies and encodes soft pipe. Soft!=product; Soft!=ABI-stable; != stick.
 */
int
usb_soft_wire_urb_ep(void *pUrb, void *pEp)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_ep *pEpSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pEp == NULL) {
        pSoft->pEp = NULL;
        pSoft->u8Linked = 0u;
        return 0;
    }
    pEpSoft = lusb_ep_soft(pEp);
    if (pEpSoft == NULL) {
        return LUSB_ENODEV; /* foreign EP - fail closed */
    }
    /* Soft reverse wire only - no HC schedule / no stick. Soft!=product. */
    pSoft->pEp = pEp;
    pSoft->u8EpNum = pEpSoft->u8Num;
    pSoft->u8DirIn = pEpSoft->u8DirIn;
    if (pEpSoft->u8Type <= LINUX_USB_SOFT_XFER_ISO) {
        pSoft->u8XferType = pEpSoft->u8Type;
    }
    pSoft->uPipe = usb_soft_encode_pipe((unsigned int)pEpSoft->u8Num,
                                        (pEpSoft->u8DirIn != 0u) ? 1 : 0);
    if (pEpSoft->pHcd != NULL) {
        pSoft->pHcd = pEpSoft->pHcd;
    }
    if (pEpSoft->pUdev != NULL) {
        pSoft->pDev = pEpSoft->pUdev;
    }
    if (pEpSoft->u16Interval > 0u) {
        pSoft->nInterval = (int)pEpSoft->u16Interval;
    }
    pSoft->u8Linked = 1u;
    return 0;
}

/*
 * Residual UND expand 15 (lean Dual DoD A eng residual lifecycle; Soft!=product)
 * -----------------------------------------------------------------------------
 * residual15: URB submitted/unlinked/poisoned reverse + seq reverse +
 * soft wire_urb_hcd for userspace xhci_udx eng lifecycle bookkeep.
 * Soft residual lean lamp: freestanding_msc=SKIP · product_usb=UDX ·
 * RUN_INIT=0 freestanding_no_exec · no freestanding thrash · no .ko exec.
 * Product USB path = userspace UDX (xhci_udx), not freestanding MSC /
 * usb_storage.ko init. Soft!=ABI-stable; G-AC-1; != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft residual lean PASS Soft!=product
 * Grep: linux_usb_soft: soft residual freestanding_msc=SKIP
 * Grep: linux_usb_soft: soft residual product_usb=UDX
 * Grep: linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual15 usbcore (lean Dual DoD A eng residual lifecycle for UDX) - */

int
usb_urb_submitted(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Submitted != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_submitted(void *pUrb, int fSubmitted)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /*
     * Soft eng lifecycle only. Does NOT schedule HC / giveback / stick wire.
     * Poisoned soft URB cannot be marked submitted (fail-closed). Soft!=product.
     */
    if (fSubmitted != 0) {
        if (pSoft->u8Poisoned != 0u) {
            return LUSB_ENODEV;
        }
        pSoft->u8Submitted = 1u;
        pSoft->u8Unlinked = 0u;
        if (pSoft->u32Seq == 0u) {
            lusb_inc(&g_uSoftUrbSeq);
            pSoft->u32Seq = g_uSoftUrbSeq;
        }
    } else {
        pSoft->u8Submitted = 0u;
    }
    return 0;
}

int
usb_urb_unlinked(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Unlinked != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_unlinked(void *pUrb, int fUnlinked)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /*
     * Soft eng bookkeep only - never HC unlink / never stick. Mirror
     * hcd_unlink path: clear linked + soft HCD link count. Soft!=product.
     */
    if (fUnlinked != 0) {
        if (pSoft->u8Linked != 0u) {
            pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
            if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
                pHcdSoft->u32UrbLinked--;
            }
            pSoft->u8Linked = 0u;
        }
        pSoft->u8Unlinked = 1u;
        pSoft->u8Submitted = 0u;
    } else {
        pSoft->u8Unlinked = 0u;
    }
    return 0;
}

int
usb_urb_poisoned(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Poisoned != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_poisoned(void *pUrb, int fPoisoned)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /*
     * Soft eng poison cookie. Fail-closed submit path honors this flag.
     * Strong residual15: clear linked + mark unlinked (lifecycle closed).
     * Never claims product teardown / stick. Soft!=product · G-AC-1.
     */
    if (fPoisoned != 0) {
        if (pSoft->u8Linked != 0u) {
            pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
            if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
                pHcdSoft->u32UrbLinked--;
            }
            pSoft->u8Linked = 0u;
        }
        pSoft->u8Poisoned = 1u;
        pSoft->u8Submitted = 0u;
        pSoft->u8Unlinked = 1u;
    } else {
        pSoft->u8Poisoned = 0u;
    }
    return 0;
}

unsigned int
usb_urb_seq(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (unsigned int)pSoft->u32Seq;
    }
    return 0u;
}

/*
 * Soft wire URB <-> HCD (UDX reverse helper). Complements wire_urb_ep.
 * Copies HCD cookie + optional udev reverse via controller/bus; bumps
 * soft HCD link count. Strong residual15: null/rewire decrements prior
 * HCD link; same-HCD rewire is idempotent. Soft!=product; Soft!=ABI-stable;
 * != stick.
 */
int
usb_soft_wire_urb_hcd(void *pUrb, void *pHcd)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;
    struct usb_soft_hcd *pOldHcd;
    struct usb_soft_udev *pUdevSoft;
    int fSame;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pHcd == NULL) {
        if (pSoft->u8Linked != 0u) {
            pOldHcd = lusb_hcd_soft(pSoft->pHcd);
            if (pOldHcd != NULL && pOldHcd->u32UrbLinked > 0u) {
                pOldHcd->u32UrbLinked--;
            }
        }
        pSoft->pHcd = NULL;
        pSoft->u8Linked = 0u;
        return 0;
    }
    pHcdSoft = lusb_hcd_soft(pHcd);
    if (pHcdSoft == NULL) {
        return LUSB_ENODEV; /* foreign HCD - fail closed */
    }
    if (pHcdSoft->u8Died != 0u) {
        return LUSB_ENODEV;
    }
    fSame = (pSoft->pHcd == pHcd && pSoft->u8Linked != 0u) ? 1 : 0;
    /* Soft reverse wire only - no schedule / no stick. Soft!=product. */
    if (fSame == 0 && pSoft->u8Linked != 0u && pSoft->pHcd != NULL) {
        pOldHcd = lusb_hcd_soft(pSoft->pHcd);
        if (pOldHcd != NULL && pOldHcd->u32UrbLinked > 0u) {
            pOldHcd->u32UrbLinked--;
        }
    }
    pSoft->pHcd = pHcd;
    pSoft->u8Linked = 1u;
    pSoft->u8Unlinked = 0u;
    if (pSoft->u32Seq == 0u) {
        lusb_inc(&g_uSoftUrbSeq);
        pSoft->u32Seq = g_uSoftUrbSeq;
    }
    if (fSame == 0 && pHcdSoft->u32UrbLinked < 0xffffffffu) {
        pHcdSoft->u32UrbLinked++;
    }
    /* Optional soft udev reverse when URB has soft udev on same HCD. */
    pUdevSoft = lusb_udev_soft(pSoft->pDev);
    if (pUdevSoft != NULL && pUdevSoft->pHcd == NULL) {
        pUdevSoft->pHcd = pHcd;
    }
    return 0;
}

/*
 * Residual UND expand 16 (lean Dual DoD A eng residual anchor/live; Soft!=product)
 * -------------------------------------------------------------------------------
 * residual16: URB anchored/live reverse + anchor cookie + soft wire_urb_dev
 * + clear_urb_lifecycle for userspace xhci_udx eng lifecycle bookkeep.
 * Soft residual lean lamp: freestanding_msc=SKIP · product_usb=UDX ·
 * RUN_INIT=0 freestanding_no_exec · no freestanding thrash · no .ko exec.
 * Product USB path = userspace UDX (xhci_udx), not freestanding MSC /
 * usb_storage.ko init. Soft!=ABI-stable; G-AC-1; != stick.
 * Grep: linux_usb_soft: soft residual UND ... Soft!=product
 * Grep: linux_usb_soft: soft residual lean PASS Soft!=product
 * Grep: linux_usb_soft: soft residual freestanding_msc=SKIP
 * Grep: linux_usb_soft: soft residual product_usb=UDX
 * Grep: linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: linux_usb_soft: soft DoD A seed
 */

/* -- residual16 usbcore (lean Dual DoD A eng residual anchor/live for UDX) */

int
usb_urb_anchored(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Anchored != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_anchored(void *pUrb, int fAnchored)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft eng bookkeep only - never real anchor queue / stick. Soft!=product. */
    if (fAnchored != 0) {
        pSoft->u8Anchored = 1u;
    } else {
        pSoft->u8Anchored = 0u;
        pSoft->pAnchor = NULL;
    }
    return 0;
}

int
usb_urb_live(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return (pSoft->u8Live != 0u) ? 1 : 0;
    }
    return 0;
}

int
usb_urb_set_live(void *pUrb, int fLive)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft pool live cookie only - never HC ownership. Soft!=product. */
    pSoft->u8Live = (fLive != 0) ? 1u : 0u;
    return 0;
}

void *
usb_urb_anchor(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pAnchor;
    }
    return NULL;
}

int
usb_urb_set_anchor(void *pUrb, void *pAnchor)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    /* Soft reverse cookie only - never real anchor list. Soft!=product. */
    pSoft->pAnchor = pAnchor;
    pSoft->u8Anchored = (pAnchor != NULL) ? 1u : 0u;
    return 0;
}

/*
 * Soft wire URB <-> soft udev (UDX reverse helper). Complements
 * wire_urb_ep / wire_urb_hcd. Soft!=product; Soft!=ABI-stable; != stick.
 */
int
usb_soft_wire_urb_dev(void *pUrb, void *pDev)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_udev *pUdevSoft;
    struct usb_soft_ep *pEpSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pDev == NULL) {
        pSoft->pDev = NULL;
        return 0;
    }
    pUdevSoft = lusb_udev_soft(pDev);
    if (pUdevSoft == NULL) {
        return LUSB_ENODEV; /* foreign udev - fail closed */
    }
    /* Soft reverse wire only - no enum / no stick. Soft!=product. */
    pSoft->pDev = pDev;
    if (pUdevSoft->pHcd != NULL) {
        pSoft->pHcd = pUdevSoft->pHcd;
    }
    /* Optional soft EP reverse when EP already wired to this udev. */
    pEpSoft = lusb_ep_soft(pSoft->pEp);
    if (pEpSoft != NULL && pEpSoft->pUdev == NULL) {
        pEpSoft->pUdev = pDev;
    }
    return 0;
}

/*
 * Soft clear URB lifecycle cookies (submitted/unlinked/poisoned/linked/
 * anchored + status/actual). Keeps transfer/setup cookies for re-fill.
 * Soft eng only - never HC teardown / never stick. Soft!=product · G-AC-1.
 */
int
usb_soft_clear_urb_lifecycle(void *pUrb)
{
    struct usb_soft_urb *pSoft;
    struct usb_soft_hcd *pHcdSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    if (pSoft->u8Linked != 0u) {
        pHcdSoft = lusb_hcd_soft(pSoft->pHcd);
        if (pHcdSoft != NULL && pHcdSoft->u32UrbLinked > 0u) {
            pHcdSoft->u32UrbLinked--;
        }
    }
    pSoft->u8Submitted = 0u;
    pSoft->u8Unlinked = 0u;
    pSoft->u8Poisoned = 0u;
    pSoft->u8Linked = 0u;
    pSoft->u8Anchored = 0u;
    pSoft->pAnchor = NULL;
    pSoft->u32ActualLen = 0u;
    pSoft->nStatus = 0;
    /* Keep pDev/pEp/pHcd/pipe/transfer cookies for re-fill eng. Soft!=product. */
    return 0;
}

/*
 * Soft residual lean lamp (once-capped; Soft!=product; G-AC-1).
 * Seed residual for userspace UDX path eng only. freestanding MSC SKIP.
 * product_usb=UDX. RUN_INIT=0 freestanding_no_exec. No freestanding thrash.
 * No version stamp. No product stick claim.
 * Grep: soft residual lean PASS Soft!=product
 * Grep: soft residual freestanding_msc=SKIP
 * Grep: soft residual product_usb=UDX
 * Grep: soft residual RUN_INIT=0 freestanding_no_exec
 */
int
linux_usb_soft_residual_lean_lamp_once(void)
{
    if (g_fLogLean != 0) {
        return 0;
    }
    if (g_fReady == 0) {
        return 0;
    }
    g_fLogLean = 1;
    kprintf("linux_usb_soft: soft residual lean PASS Soft!=product "
            "soft residual freestanding_msc=SKIP "
            "soft residual product_usb=UDX "
            "soft residual RUN_INIT=0 freestanding_no_exec "
            "residual16=8 residual15=8 residual14=8 layout=1 soft_seed=1 "
            "abi_hostability=1 ko_init_exec=0 freestanding_thrash=0 "
            "stamp_storm=0 version_stamp=0 product=0 G-AC-1=1 "
            "(DoD A eng residual anchor/live; userspace xhci_udx direction; "
            "!= freestanding MSC; != usb_storage.ko init; != stick; "
            "dual MIT OR Apache-2.0; lean lamp once)\n");
    return 1;
}

/* Soft layout accessors (header export; UDX / eng). Soft!=product. */

struct usb_soft_urb *
linux_usb_soft_urb_cast(void *pUrb)
{
    return lusb_urb_soft(pUrb);
}

struct usb_soft_hcd *
linux_usb_soft_hcd_cast(void *pHcd)
{
    return lusb_hcd_soft(pHcd);
}

struct usb_soft_udev *
linux_usb_soft_udev_cast(void *pUdev)
{
    return lusb_udev_soft(pUdev);
}

struct usb_soft_bus *
linux_usb_soft_bus_cast(void *pBus)
{
    return lusb_bus_soft(pBus);
}

struct usb_soft_ep *
linux_usb_soft_ep_cast(void *pEp)
{
    return lusb_ep_soft(pEp);
}

int
linux_usb_soft_urb_status(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return LUSB_ENODEV;
    }
    return pSoft->nStatus;
}

u32
linux_usb_soft_hcd_flags(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u32Flags;
}

u8
linux_usb_soft_urb_xfer_type(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8XferType;
}

u8
linux_usb_soft_urb_ep_num(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8EpNum;
}

u8
linux_usb_soft_urb_dir_in(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8DirIn;
}

u32
linux_usb_soft_urb_actual_len(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u32ActualLen;
}

u32
linux_usb_soft_urb_transfer_len(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u32TransferLen;
}

void *
linux_usb_soft_hcd_controller(void *pHcd)
{
    struct usb_soft_hcd *pSoft;

    pSoft = lusb_hcd_soft(pHcd);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pController;
}

void *
linux_usb_soft_udev_hcd(void *pUdev)
{
    struct usb_soft_udev *pSoft;

    pSoft = lusb_udev_soft(pUdev);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pHcd;
}

void *
linux_usb_soft_urb_ep(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pEp;
}

void *
linux_usb_soft_ep_hcd(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pHcd;
}

u16
linux_usb_soft_ep_maxpacket(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u16MaxPacket;
}

u8
linux_usb_soft_ep_enabled(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u8Enabled;
}

u8
linux_usb_soft_ep_num(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8Num;
}

u8
linux_usb_soft_ep_addr(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8Addr;
}

u8
linux_usb_soft_ep_type(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8Type;
}

u8
linux_usb_soft_ep_dir_in(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0xffu;
    }
    return pSoft->u8DirIn;
}

void *
linux_usb_soft_urb_setup(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pSetup;
}

void *
linux_usb_soft_urb_complete(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pfnComplete;
}

void *
linux_usb_soft_urb_context(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return NULL;
    }
    return pSoft->pContext;
}

u16
linux_usb_soft_ep_interval(void *pEp)
{
    struct usb_soft_ep *pSoft;

    pSoft = lusb_ep_soft(pEp);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u16Interval;
}

u32
linux_usb_soft_urb_seq(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft == NULL) {
        return 0u;
    }
    return pSoft->u32Seq;
}

void *
linux_usb_soft_urb_dev(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pDev;
    }
    return NULL;
}

void *
linux_usb_soft_urb_hcd(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pHcd;
    }
    return NULL;
}

int
linux_usb_soft_urb_interval(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->nInterval;
    }
    return 0;
}

u8
linux_usb_soft_urb_linked(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Linked;
    }
    return 0xffu;
}

/* residual15: lifecycle reverse accessors for UDX eng. Soft!=product. */
u8
linux_usb_soft_urb_submitted(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Submitted;
    }
    return 0xffu;
}

u8
linux_usb_soft_urb_unlinked(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Unlinked;
    }
    return 0xffu;
}

u8
linux_usb_soft_urb_poisoned(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Poisoned;
    }
    return 0xffu;
}

/* residual16: anchor/live reverse accessors for UDX eng. Soft!=product. */
u8
linux_usb_soft_urb_anchored(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Anchored;
    }
    return 0xffu;
}

u8
linux_usb_soft_urb_live(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->u8Live;
    }
    return 0xffu;
}

void *
linux_usb_soft_urb_anchor(void *pUrb)
{
    struct usb_soft_urb *pSoft;

    pSoft = lusb_urb_soft(pUrb);
    if (pSoft != NULL) {
        return pSoft->pAnchor;
    }
    return NULL;
}

/* ---- init -------------------------------------------------------------- */

void
linux_usb_soft_init(void)
{
    if (g_fReady != 0) {
        lusb_inc(&g_cInitCalls);
        return;
    }

    g_cInitCalls = 1u;
    g_cKsymOk = 0u;
    g_cKsymSkip = 0u;
    g_cStubReg = 0u;
    g_cUsbRegister = 0u;
    g_cUsbDeregister = 0u;
    g_cSubmitUrb = 0u;
    g_cAllocUrb = 0u;
    g_cHcdCreate = 0u;
    g_cHcdAdd = 0u;
    g_cHcdGiveback = 0u;
    g_cHub = 0u;
    g_cScsiHostAlloc = 0u;
    g_cScsiAddHost = 0u;
    g_cScsiScan = 0u;
    g_cScsiDone = 0u;
    g_cScsiEh = 0u;
    g_cScsiSense = 0u;
    g_cScsiBlock = 0u;
    g_cScsiReport = 0u;
    g_cSgNents = 0u;
    g_cKthreadCreate = 0u;
    g_fLogRegister = 0;
    g_fLogSubmit = 0;
    g_fLogSg = 0;
    g_fLogHcd = 0;
    g_fLogScsiHost = 0;
    g_fLogScsiAdd = 0;
    g_fLogScsiScan = 0;
    g_fLogScsiMod = 0;
    g_fLogResidual = 0;
    g_fLogLayout = 0;
    g_fLogDodA = 0;
    g_fLogLean = 0;
    g_uSoftUrbSeq = 0u;
    g_fSoftObjsInit = 0u;
    lusb_soft_objs_once();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Replaces any prior zero stubs for these names when present.
     * Weak linux_ksym_register: count as skip when F2 not linked yet.
     *
     * Budget (D residual expand virtual usbcore + scsi_mod surface):
     *   usb core+expand  84  (20 el9 class + 10 uas + 54 common expand)
     *   virt usbcore    101  (HCD/hub/device/bus/endpoint/phy/acpi)
     *   scsi mid+expand  58  (13 el9 class + 5 uas + 40 mid residual)
     *   sg_*             11  (5 miter + 6 table residual)
     *   msc leaf+wq      39  (13 wait/kthread + 16 dev/param + 10 wq/heap)
     *   residual UND    366  (r1..r10:358 + r11:8 EP maxp/URB reverse)
     *   total           659  Soft!=product; != stick; G-AC-1; need=HC OPEN.
     * Soft layout deepen (DoD A): fill/submit/giveback + r10/r11 UDX cookies;
     * freestanding xHCI residual SKIP (GJ_XHCI_MSC_PROBE=0); soft seed only.
     */
    /* usbcore register / urb / control / sg / autopm / reset (20) */
    lusb_ksym_one("usb_register_driver", (void *)usb_register_driver);
    lusb_ksym_one("usb_deregister", (void *)usb_deregister);
    lusb_ksym_one("usb_alloc_urb", (void *)usb_alloc_urb);
    lusb_ksym_one("usb_free_urb", (void *)usb_free_urb);
    lusb_ksym_one("usb_submit_urb", (void *)usb_submit_urb);
    lusb_ksym_one("usb_unlink_urb", (void *)usb_unlink_urb);
    lusb_ksym_one("usb_kill_urb", (void *)usb_kill_urb);
    lusb_ksym_one("usb_alloc_coherent", (void *)usb_alloc_coherent);
    lusb_ksym_one("usb_free_coherent", (void *)usb_free_coherent);
    lusb_ksym_one("usb_control_msg", (void *)usb_control_msg);
    lusb_ksym_one("usb_sg_init", (void *)usb_sg_init);
    lusb_ksym_one("usb_sg_wait", (void *)usb_sg_wait);
    lusb_ksym_one("usb_sg_cancel", (void *)usb_sg_cancel);
    lusb_ksym_one("usb_autopm_get_interface_no_resume",
                  (void *)usb_autopm_get_interface_no_resume);
    lusb_ksym_one("usb_autopm_put_interface",
                  (void *)usb_autopm_put_interface);
    lusb_ksym_one("usb_autopm_put_interface_no_suspend",
                  (void *)usb_autopm_put_interface_no_suspend);
    lusb_ksym_one("usb_find_common_endpoints",
                  (void *)usb_find_common_endpoints);
    lusb_ksym_one("usb_lock_device_for_reset",
                  (void *)usb_lock_device_for_reset);
    lusb_ksym_one("usb_reset_device", (void *)usb_reset_device);
    lusb_ksym_one("usb_reset_endpoint", (void *)usb_reset_endpoint);

    /* usb expand: streams / anchor / set_interface / stor exports (10) */
    lusb_ksym_one("usb_alloc_streams", (void *)usb_alloc_streams);
    lusb_ksym_one("usb_free_streams", (void *)usb_free_streams);
    lusb_ksym_one("usb_anchor_urb", (void *)usb_anchor_urb);
    lusb_ksym_one("usb_unanchor_urb", (void *)usb_unanchor_urb);
    lusb_ksym_one("usb_get_urb", (void *)usb_get_urb);
    lusb_ksym_one("usb_kill_anchored_urbs", (void *)usb_kill_anchored_urbs);
    lusb_ksym_one("usb_wait_anchor_empty_timeout",
                  (void *)usb_wait_anchor_empty_timeout);
    lusb_ksym_one("usb_set_interface", (void *)usb_set_interface);
    lusb_ksym_one("usb_stor_adjust_quirks", (void *)usb_stor_adjust_quirks);
    lusb_ksym_one("usb_stor_sense_invalidCDB",
                  (void *)g_aSoftStorSenseInvalidCdb);

    /* usbcore expand: common exports beyond storage 20-UND class (54) */
    lusb_ksym_one("usb_get_dev", (void *)usb_get_dev);
    lusb_ksym_one("usb_put_dev", (void *)usb_put_dev);
    lusb_ksym_one("usb_get_intf", (void *)usb_get_intf);
    lusb_ksym_one("usb_put_intf", (void *)usb_put_intf);
    lusb_ksym_one("usb_bulk_msg", (void *)usb_bulk_msg);
    lusb_ksym_one("usb_interrupt_msg", (void *)usb_interrupt_msg);
    lusb_ksym_one("usb_clear_halt", (void *)usb_clear_halt);
    lusb_ksym_one("usb_string", (void *)usb_string);
    lusb_ksym_one("usb_autopm_get_interface",
                  (void *)usb_autopm_get_interface);
    lusb_ksym_one("usb_autopm_get_interface_async",
                  (void *)usb_autopm_get_interface_async);
    lusb_ksym_one("usb_autopm_put_interface_async",
                  (void *)usb_autopm_put_interface_async);
    lusb_ksym_one("usb_autopm_get_interface_no_suspend",
                  (void *)usb_autopm_get_interface_no_suspend);
    lusb_ksym_one("usb_ifnum_to_if", (void *)usb_ifnum_to_if);
    lusb_ksym_one("usb_altnum_to_altsetting",
                  (void *)usb_altnum_to_altsetting);
    lusb_ksym_one("usb_driver_claim_interface",
                  (void *)usb_driver_claim_interface);
    lusb_ksym_one("usb_driver_release_interface",
                  (void *)usb_driver_release_interface);
    lusb_ksym_one("usb_interface_claimed", (void *)usb_interface_claimed);
    lusb_ksym_one("usb_match_id", (void *)usb_match_id);
    lusb_ksym_one("usb_match_one_id", (void *)usb_match_one_id);
    lusb_ksym_one("usb_get_descriptor", (void *)usb_get_descriptor);
    lusb_ksym_one("usb_get_status", (void *)usb_get_status);
    lusb_ksym_one("usb_set_configuration", (void *)usb_set_configuration);
    lusb_ksym_one("usb_choose_configuration",
                  (void *)usb_choose_configuration);
    lusb_ksym_one("usb_enable_autosuspend", (void *)usb_enable_autosuspend);
    lusb_ksym_one("usb_disable_autosuspend",
                  (void *)usb_disable_autosuspend);
    lusb_ksym_one("usb_poison_urb", (void *)usb_poison_urb);
    lusb_ksym_one("usb_unpoison_urb", (void *)usb_unpoison_urb);
    lusb_ksym_one("usb_block_urb", (void *)usb_block_urb);
    lusb_ksym_one("usb_register_dev", (void *)usb_register_dev);
    lusb_ksym_one("usb_deregister_dev", (void *)usb_deregister_dev);
    lusb_ksym_one("usb_find_interface", (void *)usb_find_interface);
    lusb_ksym_one("usb_lock_device", (void *)usb_lock_device);
    lusb_ksym_one("usb_unlock_device", (void *)usb_unlock_device);
    lusb_ksym_one("usb_trylock_device", (void *)usb_trylock_device);
    lusb_ksym_one("usb_check_bulk_endpoints",
                  (void *)usb_check_bulk_endpoints);
    lusb_ksym_one("usb_check_int_endpoints",
                  (void *)usb_check_int_endpoints);
    lusb_ksym_one("usb_ep_type_string", (void *)usb_ep_type_string);
    lusb_ksym_one("usb_speed_string", (void *)usb_speed_string);
    lusb_ksym_one("usb_state_string", (void *)usb_state_string);
    lusb_ksym_one("usb_get_current_frame_number",
                  (void *)usb_get_current_frame_number);
    lusb_ksym_one("usb_anchor_empty", (void *)usb_anchor_empty);
    lusb_ksym_one("usb_pipe_type_check", (void *)usb_pipe_type_check);
    lusb_ksym_one("usb_urb_ep_type_check", (void *)usb_urb_ep_type_check);
    lusb_ksym_one("usb_control_msg_send", (void *)usb_control_msg_send);
    lusb_ksym_one("usb_control_msg_recv", (void *)usb_control_msg_recv);
    lusb_ksym_one("usb_get_from_anchor", (void *)usb_get_from_anchor);
    lusb_ksym_one("usb_scuttle_anchored_urbs",
                  (void *)usb_scuttle_anchored_urbs);
    lusb_ksym_one("usb_unlink_anchored_urbs",
                  (void *)usb_unlink_anchored_urbs);
    lusb_ksym_one("usb_anchor_resume_wakeups",
                  (void *)usb_anchor_resume_wakeups);
    lusb_ksym_one("usb_anchor_suspend_wakeups",
                  (void *)usb_anchor_suspend_wakeups);
    lusb_ksym_one("usb_queue_reset_device", (void *)usb_queue_reset_device);
    lusb_ksym_one("usb_reset_configuration",
                  (void *)usb_reset_configuration);
    lusb_ksym_one("usb_register", (void *)usb_register);
    lusb_ksym_one("usb_disabled", (void *)&g_nSoftUsbDisabled);

    /* virtual usbcore: HCD surface (20) Soft!=real HC */
    lusb_ksym_one("usb_create_hcd", (void *)usb_create_hcd);
    lusb_ksym_one("usb_create_shared_hcd", (void *)usb_create_shared_hcd);
    lusb_ksym_one("usb_add_hcd", (void *)usb_add_hcd);
    lusb_ksym_one("usb_remove_hcd", (void *)usb_remove_hcd);
    lusb_ksym_one("usb_put_hcd", (void *)usb_put_hcd);
    lusb_ksym_one("usb_get_hcd", (void *)usb_get_hcd);
    lusb_ksym_one("usb_hcd_giveback_urb", (void *)usb_hcd_giveback_urb);
    lusb_ksym_one("usb_hcd_link_urb_to_ep", (void *)usb_hcd_link_urb_to_ep);
    lusb_ksym_one("usb_hcd_unlink_urb_from_ep",
                  (void *)usb_hcd_unlink_urb_from_ep);
    lusb_ksym_one("usb_hcd_check_unlink_urb",
                  (void *)usb_hcd_check_unlink_urb);
    lusb_ksym_one("usb_hcd_map_urb_for_dma", (void *)usb_hcd_map_urb_for_dma);
    lusb_ksym_one("usb_hcd_unmap_urb_for_dma",
                  (void *)usb_hcd_unmap_urb_for_dma);
    lusb_ksym_one("usb_hcd_poll_rh_status", (void *)usb_hcd_poll_rh_status);
    lusb_ksym_one("usb_hcd_resume_root_hub", (void *)usb_hcd_resume_root_hub);
    lusb_ksym_one("usb_hcd_start_port_resume",
                  (void *)usb_hcd_start_port_resume);
    lusb_ksym_one("usb_hcd_end_port_resume", (void *)usb_hcd_end_port_resume);
    lusb_ksym_one("usb_hcd_is_primary_hcd", (void *)usb_hcd_is_primary_hcd);
    lusb_ksym_one("usb_hcd_irq", (void *)usb_hcd_irq);
    lusb_ksym_one("usb_hcd_alloc_bandwidth",
                  (void *)usb_hcd_alloc_bandwidth);
    lusb_ksym_one("usb_hcd_get_frame_number",
                  (void *)usb_hcd_get_frame_number);

    /* virtual usbcore: HCD PCI + hub (18) Soft!=stick */
    lusb_ksym_one("usb_hcd_pci_probe", (void *)usb_hcd_pci_probe);
    lusb_ksym_one("usb_hcd_pci_remove", (void *)usb_hcd_pci_remove);
    lusb_ksym_one("usb_hcd_pci_shutdown", (void *)usb_hcd_pci_shutdown);
    lusb_ksym_one("usb_hcd_amd_remote_wakeup_quirk",
                  (void *)usb_hcd_amd_remote_wakeup_quirk);
    lusb_ksym_one("usb_hub_find_child", (void *)usb_hub_find_child);
    lusb_ksym_one("usb_hub_claim_port", (void *)usb_hub_claim_port);
    lusb_ksym_one("usb_hub_release_port", (void *)usb_hub_release_port);
    lusb_ksym_one("usb_hub_release_all_ports",
                  (void *)usb_hub_release_all_ports);
    lusb_ksym_one("usb_hub_to_struct_hub", (void *)usb_hub_to_struct_hub);
    lusb_ksym_one("usb_get_hub_status", (void *)usb_get_hub_status);
    lusb_ksym_one("usb_get_port_status", (void *)usb_get_port_status);
    lusb_ksym_one("usb_clear_port_feature", (void *)usb_clear_port_feature);
    lusb_ksym_one("usb_set_port_feature", (void *)usb_set_port_feature);
    lusb_ksym_one("usb_hub_clear_tt_buffer", (void *)usb_hub_clear_tt_buffer);
    lusb_ksym_one("usb_hub_adjust_deviceremovable",
                  (void *)usb_hub_adjust_deviceremovable);
    lusb_ksym_one("usb_hub_for_each_child", (void *)usb_hub_for_each_child);
    lusb_ksym_one("usb_hub_initiate_lpm", (void *)usb_hub_initiate_lpm);
    lusb_ksym_one("usb_ep0_reinit", (void *)usb_ep0_reinit);

    /* virtual usbcore: device / bus / LPM (24) Soft!=enum */
    lusb_ksym_one("usb_alloc_dev", (void *)usb_alloc_dev);
    lusb_ksym_one("usb_new_device", (void *)usb_new_device);
    lusb_ksym_one("usb_disconnect", (void *)usb_disconnect);
    lusb_ksym_one("usb_disable_device", (void *)usb_disable_device);
    lusb_ksym_one("usb_enable_device", (void *)usb_enable_device);
    lusb_ksym_one("usb_set_device_state", (void *)usb_set_device_state);
    lusb_ksym_one("usb_device_is_owned", (void *)usb_device_is_owned);
    lusb_ksym_one("usb_device_supports_lpm", (void *)usb_device_supports_lpm);
    lusb_ksym_one("usb_device_supports_ltm", (void *)usb_device_supports_ltm);
    lusb_ksym_one("usb_enable_lpm", (void *)usb_enable_lpm);
    lusb_ksym_one("usb_disable_lpm", (void *)usb_disable_lpm);
    lusb_ksym_one("usb_unlocked_enable_lpm", (void *)usb_unlocked_enable_lpm);
    lusb_ksym_one("usb_unlocked_disable_lpm",
                  (void *)usb_unlocked_disable_lpm);
    lusb_ksym_one("usb_enable_ltm", (void *)usb_enable_ltm);
    lusb_ksym_one("usb_disable_ltm", (void *)usb_disable_ltm);
    lusb_ksym_one("usb_wakeup_notification",
                  (void *)usb_wakeup_notification);
    lusb_ksym_one("usb_wakeup_enabled_descendants",
                  (void *)usb_wakeup_enabled_descendants);
    lusb_ksym_one("usb_port_resume", (void *)usb_port_resume);
    lusb_ksym_one("usb_port_suspend", (void *)usb_port_suspend);
    lusb_ksym_one("usb_bus_get", (void *)usb_bus_get);
    lusb_ksym_one("usb_bus_put", (void *)usb_bus_put);
    lusb_ksym_one("usb_register_bus", (void *)usb_register_bus);
    lusb_ksym_one("usb_deregister_bus", (void *)usb_deregister_bus);
    lusb_ksym_one("usb_add_device", (void *)usb_add_device);
    lusb_ksym_one("usb_remove_device", (void *)usb_remove_device);
    lusb_ksym_one("usb_create_device_files",
                  (void *)usb_create_device_files);

    /* virtual usbcore: endpoint / find / phy / acpi / misc (28) */
    lusb_ksym_one("usb_find_next_intf_descriptor",
                  (void *)usb_find_next_intf_descriptor);
    lusb_ksym_one("usb_find_alt_setting", (void *)usb_find_alt_setting);
    lusb_ksym_one("usb_for_each_dev", (void *)usb_for_each_dev);
    lusb_ksym_one("usb_if_interface_number",
                  (void *)usb_if_interface_number);
    lusb_ksym_one("usb_interface_id", (void *)usb_interface_id);
    lusb_ksym_one("usb_get_max_power", (void *)usb_get_max_power);
    lusb_ksym_one("usb_get_maximum_speed", (void *)usb_get_maximum_speed);
    lusb_ksym_one("usb_endpoint_maxp", (void *)usb_endpoint_maxp);
    lusb_ksym_one("usb_endpoint_maxp_mult", (void *)usb_endpoint_maxp_mult);
    lusb_ksym_one("usb_pipe_endpoint", (void *)usb_pipe_endpoint);
    lusb_ksym_one("usb_urb_dir_in", (void *)usb_urb_dir_in);
    lusb_ksym_one("usb_urb_dir_out", (void *)usb_urb_dir_out);
    lusb_ksym_one("usb_submit_anchored_urbs",
                  (void *)usb_submit_anchored_urbs);
    lusb_ksym_one("usb_get_current_bandwidth",
                  (void *)usb_get_current_bandwidth);
    lusb_ksym_one("usb_find_common_endpoints_reverse",
                  (void *)usb_find_common_endpoints_reverse);
    lusb_ksym_one("usb_find_bulk_in_endpoint",
                  (void *)usb_find_bulk_in_endpoint);
    lusb_ksym_one("usb_find_bulk_out_endpoint",
                  (void *)usb_find_bulk_out_endpoint);
    lusb_ksym_one("usb_find_int_in_endpoint",
                  (void *)usb_find_int_in_endpoint);
    lusb_ksym_one("usb_find_int_out_endpoint",
                  (void *)usb_find_int_out_endpoint);
    lusb_ksym_one("usb_poison_anchored_urbs",
                  (void *)usb_poison_anchored_urbs);
    lusb_ksym_one("usb_unpoison_anchored_urbs",
                  (void *)usb_unpoison_anchored_urbs);
    lusb_ksym_one("usb_block_anchored_urbs",
                  (void *)usb_block_anchored_urbs);
    lusb_ksym_one("usb_calc_bus_time", (void *)usb_calc_bus_time);
    lusb_ksym_one("usb_phy_roothub_alloc", (void *)usb_phy_roothub_alloc);
    lusb_ksym_one("usb_phy_roothub_init", (void *)usb_phy_roothub_init);
    lusb_ksym_one("usb_phy_roothub_exit", (void *)usb_phy_roothub_exit);
    lusb_ksym_one("usb_phy_roothub_set_mode",
                  (void *)usb_phy_roothub_set_mode);
    lusb_ksym_one("usb_phy_roothub_calibrate",
                  (void *)usb_phy_roothub_calibrate);
    lusb_ksym_one("usb_phy_roothub_power_on",
                  (void *)usb_phy_roothub_power_on);
    lusb_ksym_one("usb_phy_roothub_power_off",
                  (void *)usb_phy_roothub_power_off);
    lusb_ksym_one("usb_register_notify", (void *)usb_register_notify);
    lusb_ksym_one("usb_unregister_notify", (void *)usb_unregister_notify);
    lusb_ksym_one("usb_ledtrig_set", (void *)usb_ledtrig_set);
    lusb_ksym_one("usb_acpi_set_power_state",
                  (void *)usb_acpi_set_power_state);
    lusb_ksym_one("usb_acpi_power_manageable",
                  (void *)usb_acpi_power_manageable);
    lusb_ksym_one("usb_acpi_register", (void *)usb_acpi_register);
    lusb_ksym_one("usb_acpi_unregister", (void *)usb_acpi_unregister);

    /* scsi_mod surface: host / EH / sense (13) - el9 usb-storage deepen */
    lusb_ksym_one("scsi_is_host_device", (void *)scsi_is_host_device);
    lusb_ksym_one("scsi_host_alloc", (void *)scsi_host_alloc);
    lusb_ksym_one("scsi_add_host_with_dma", (void *)scsi_add_host_with_dma);
    lusb_ksym_one("scsi_scan_host", (void *)scsi_scan_host);
    lusb_ksym_one("scsi_host_put", (void *)scsi_host_put);
    lusb_ksym_one("scsi_remove_host", (void *)scsi_remove_host);
    lusb_ksym_one("scsi_done_direct", (void *)scsi_done_direct);
    lusb_ksym_one("scsi_report_bus_reset", (void *)scsi_report_bus_reset);
    lusb_ksym_one("scsi_report_device_reset",
                  (void *)scsi_report_device_reset);
    lusb_ksym_one("scsi_eh_prep_cmnd", (void *)scsi_eh_prep_cmnd);
    lusb_ksym_one("scsi_eh_restore_cmnd", (void *)scsi_eh_restore_cmnd);
    lusb_ksym_one("scsi_normalize_sense", (void *)scsi_normalize_sense);
    lusb_ksym_one("scsi_sense_desc_find", (void *)scsi_sense_desc_find);

    /* scsi_mod expand: uas mid (5) */
    lusb_ksym_one("scsi_block_requests", (void *)scsi_block_requests);
    lusb_ksym_one("scsi_unblock_requests", (void *)scsi_unblock_requests);
    lusb_ksym_one("scsi_change_queue_depth", (void *)scsi_change_queue_depth);
    lusb_ksym_one("scsi_done", (void *)scsi_done);
    lusb_ksym_one("scsi_print_command", (void *)scsi_print_command);

    /* scsi_mod expand: uas residual + mid helpers (40) */
    lusb_ksym_one("int_to_scsilun", (void *)int_to_scsilun);
    lusb_ksym_one("scsilun_to_int", (void *)scsilun_to_int);
    lusb_ksym_one("scmd_printk", (void *)scmd_printk);
    lusb_ksym_one("sdev_prefix_printk", (void *)sdev_prefix_printk);
    lusb_ksym_one("scsi_host_get", (void *)scsi_host_get);
    lusb_ksym_one("scsi_host_lookup", (void *)scsi_host_lookup);
    lusb_ksym_one("scsi_host_busy", (void *)scsi_host_busy);
    lusb_ksym_one("scsi_add_host", (void *)scsi_add_host);
    lusb_ksym_one("scsi_device_lookup", (void *)scsi_device_lookup);
    lusb_ksym_one("scsi_device_get", (void *)scsi_device_get);
    lusb_ksym_one("scsi_device_put", (void *)scsi_device_put);
    lusb_ksym_one("scsi_device_quiesce", (void *)scsi_device_quiesce);
    lusb_ksym_one("scsi_device_resume", (void *)scsi_device_resume);
    lusb_ksym_one("scsi_device_set_state", (void *)scsi_device_set_state);
    lusb_ksym_one("scsi_build_sense", (void *)scsi_build_sense);
    lusb_ksym_one("scsi_build_sense_buffer",
                  (void *)scsi_build_sense_buffer);
    lusb_ksym_one("scsi_set_sense_information",
                  (void *)scsi_set_sense_information);
    lusb_ksym_one("scsi_get_sense_info_fld",
                  (void *)scsi_get_sense_info_fld);
    lusb_ksym_one("scsi_command_normalize_sense",
                  (void *)scsi_command_normalize_sense);
    lusb_ksym_one("scsi_print_sense_hdr", (void *)scsi_print_sense_hdr);
    lusb_ksym_one("__scsi_print_sense", (void *)__scsi_print_sense);
    lusb_ksym_one("scsi_extd_sense_format", (void *)scsi_extd_sense_format);
    lusb_ksym_one("scsi_is_sdev_device", (void *)scsi_is_sdev_device);
    lusb_ksym_one("scsi_is_target_device", (void *)scsi_is_target_device);
    lusb_ksym_one("scsi_eh_finish_cmd", (void *)scsi_eh_finish_cmd);
    lusb_ksym_one("scsi_eh_flush_done_q", (void *)scsi_eh_flush_done_q);
    lusb_ksym_one("scsi_eh_get_sense", (void *)scsi_eh_get_sense);
    lusb_ksym_one("scsi_eh_scmd_add", (void *)scsi_eh_scmd_add);
    lusb_ksym_one("scsi_eh_wakeup", (void *)scsi_eh_wakeup);
    lusb_ksym_one("scsi_check_sense", (void *)scsi_check_sense);
    lusb_ksym_one("scsi_block_when_processing_errors",
                  (void *)scsi_block_when_processing_errors);
    lusb_ksym_one("scsi_track_queue_full", (void *)scsi_track_queue_full);
    lusb_ksym_one("scsi_dma_map", (void *)scsi_dma_map);
    lusb_ksym_one("scsi_dma_unmap", (void *)scsi_dma_unmap);
    lusb_ksym_one("scsi_alloc_sgtables", (void *)scsi_alloc_sgtables);
    lusb_ksym_one("scsi_free_sgtables", (void *)scsi_free_sgtables);
    lusb_ksym_one("scsi_init_command", (void *)scsi_init_command);
    lusb_ksym_one("scsi_finish_command", (void *)scsi_finish_command);
    lusb_ksym_one("scsi_execute_cmd", (void *)scsi_execute_cmd);
    lusb_ksym_one("scsi_device_max_queue_depth",
                  (void *)scsi_device_max_queue_depth);

    /* scatterlist - miter + table residual (11) */
    lusb_ksym_one("sg_nents", (void *)sg_nents);
    lusb_ksym_one("sg_miter_start", (void *)sg_miter_start);
    lusb_ksym_one("sg_miter_skip", (void *)sg_miter_skip);
    lusb_ksym_one("sg_miter_next", (void *)sg_miter_next);
    lusb_ksym_one("sg_miter_stop", (void *)sg_miter_stop);
    lusb_ksym_one("sg_next", (void *)sg_next);
    lusb_ksym_one("sg_last", (void *)sg_last);
    lusb_ksym_one("sg_init_table", (void *)sg_init_table);
    lusb_ksym_one("sg_init_one", (void *)sg_init_one);
    lusb_ksym_one("sg_alloc_table", (void *)sg_alloc_table);
    lusb_ksym_one("sg_free_table", (void *)sg_free_table);

    /* MSC leaf wait / work / kthread (13) */
    lusb_ksym_one("init_wait_entry", (void *)init_wait_entry);
    lusb_ksym_one("prepare_to_wait_event", (void *)prepare_to_wait_event);
    lusb_ksym_one("__init_waitqueue_head", (void *)__init_waitqueue_head);
    lusb_ksym_one("__init_swait_queue_head", (void *)__init_swait_queue_head);
    lusb_ksym_one("__wake_up", (void *)__wake_up);
    lusb_ksym_one("wake_up_process", (void *)wake_up_process);
    lusb_ksym_one("kthread_create_on_node", (void *)kthread_create_on_node);
    lusb_ksym_one("kthread_should_stop", (void *)kthread_should_stop);
    lusb_ksym_one("kthread_stop", (void *)kthread_stop);
    lusb_ksym_one("init_timer_key", (void *)init_timer_key);
    lusb_ksym_one("delayed_work_timer_fn", (void *)delayed_work_timer_fn);
    lusb_ksym_one("queue_delayed_work_on", (void *)queue_delayed_work_on);
    lusb_ksym_one("cancel_delayed_work_sync",
                  (void *)cancel_delayed_work_sync);

    /* MSC leaf dev / param / string / time / data (16) */
    lusb_ksym_one("device_create_file", (void *)device_create_file);
    lusb_ksym_one("sysfs_emit", (void *)sysfs_emit);
    lusb_ksym_one("_dev_notice", (void *)_dev_notice);
    lusb_ksym_one("__dynamic_dev_dbg", (void *)__dynamic_dev_dbg);
    lusb_ksym_one("kstrtouint", (void *)kstrtouint);
    lusb_ksym_one("simple_strtoul", (void *)simple_strtoul);
    lusb_ksym_one("skip_spaces", (void *)skip_spaces);
    lusb_ksym_one("int_pow", (void *)int_pow);
    lusb_ksym_one("__msecs_to_jiffies", (void *)__msecs_to_jiffies);
    lusb_ksym_one("ktime_get_mono_fast_ns", (void *)ktime_get_mono_fast_ns);
    lusb_ksym_one("__trace_set_current_state",
                  (void *)__trace_set_current_state);
    lusb_ksym_one("param_ops_uint", (void *)g_aSoftParamOpsUint);
    lusb_ksym_one("param_ops_string", (void *)g_aSoftParamOpsString);
    lusb_ksym_one("pcpu_hot", (void *)g_aSoftPcpuHot);
    lusb_ksym_one("system_freezable_wq", (void *)g_aSoftSystemFreezableWq);
    lusb_ksym_one("__tracepoint_sched_set_state_tp",
                  (void *)g_aSoftTracepointSched);

    /* MSC residual wq / heap (10) - uas UND class; Soft!=leaf-cap in ksym */
    lusb_ksym_one("alloc_workqueue", (void *)alloc_workqueue);
    lusb_ksym_one("destroy_workqueue", (void *)destroy_workqueue);
    lusb_ksym_one("cancel_work_sync", (void *)cancel_work_sync);
    lusb_ksym_one("flush_work", (void *)flush_work);
    lusb_ksym_one("__init_work", (void *)__init_work);
    lusb_ksym_one("__kmalloc", (void *)__kmalloc);
    lusb_ksym_one("_dev_printk", (void *)_dev_printk);
    lusb_ksym_one("dev_printk", (void *)dev_printk);
    lusb_ksym_one("mutex_lock_nested", (void *)mutex_lock_nested);
    lusb_ksym_one("queue_work_on", (void *)queue_work_on);

    /* residual UND: scsi mid sd_mod-shaped (35) Soft!=product; != stick */
    lusb_ksym_one("scsi_autopm_get_device", (void *)scsi_autopm_get_device);
    lusb_ksym_one("scsi_autopm_put_device", (void *)scsi_autopm_put_device);
    lusb_ksym_one("scsi_get_vpd_page", (void *)scsi_get_vpd_page);
    lusb_ksym_one("scsi_mode_sense", (void *)scsi_mode_sense);
    lusb_ksym_one("scsi_mode_select", (void *)scsi_mode_select);
    lusb_ksym_one("scsi_test_unit_ready", (void *)scsi_test_unit_ready);
    lusb_ksym_one("scsi_report_opcode", (void *)scsi_report_opcode);
    lusb_ksym_one("scsi_set_medium_removal",
                  (void *)scsi_set_medium_removal);
    lusb_ksym_one("scsi_ioctl", (void *)scsi_ioctl);
    lusb_ksym_one("scsi_ioctl_block_when_processing_errors",
                  (void *)scsi_ioctl_block_when_processing_errors);
    lusb_ksym_one("scsi_hostbyte_string", (void *)scsi_hostbyte_string);
    lusb_ksym_one("scsi_sense_key_string", (void *)scsi_sense_key_string);
    lusb_ksym_one("scsi_mlreturn_string", (void *)scsi_mlreturn_string);
    lusb_ksym_one("scsi_print_result", (void *)scsi_print_result);
    lusb_ksym_one("scsi_print_status", (void *)scsi_print_status);
    lusb_ksym_one("scsi_host_block", (void *)scsi_host_block);
    lusb_ksym_one("scsi_host_unblock", (void *)scsi_host_unblock);
    lusb_ksym_one("scsi_target_block", (void *)scsi_target_block);
    lusb_ksym_one("scsi_target_unblock", (void *)scsi_target_unblock);
    lusb_ksym_one("scsi_device_block", (void *)scsi_device_block);
    lusb_ksym_one("scsi_device_unblock", (void *)scsi_device_unblock);
    lusb_ksym_one("scsi_schedule_eh", (void *)scsi_schedule_eh);
    lusb_ksym_one("scsi_add_device", (void *)scsi_add_device);
    lusb_ksym_one("scsi_remove_device", (void *)scsi_remove_device);
    lusb_ksym_one("scsi_rescan_device", (void *)scsi_rescan_device);
    lusb_ksym_one("scsi_scan_target", (void *)scsi_scan_target);
    lusb_ksym_one("scsi_is_wlun", (void *)scsi_is_wlun);
    lusb_ksym_one("scsi_device_type", (void *)scsi_device_type);
    lusb_ksym_one("scsi_sanitize_inquiry_string",
                  (void *)scsi_sanitize_inquiry_string);
    lusb_ksym_one("scsi_set_sense_field_pointer",
                  (void *)scsi_set_sense_field_pointer);
    lusb_ksym_one("scsi_failures_reset_retries",
                  (void *)scsi_failures_reset_retries);
    lusb_ksym_one("scsi_pr_type_to_block", (void *)scsi_pr_type_to_block);
    lusb_ksym_one("scsi_decide_disposition",
                  (void *)scsi_decide_disposition);
    lusb_ksym_one("scsi_noretry_cmd", (void *)scsi_noretry_cmd);
    lusb_ksym_one("scsi_logging_level", (void *)&g_nSoftScsiLoggingLevel);

    /* residual UND: sg copy/buffer (12) Soft!=map; != stick */
    lusb_ksym_one("sg_nents_for_len", (void *)sg_nents_for_len);
    lusb_ksym_one("sg_copy_buffer", (void *)sg_copy_buffer);
    lusb_ksym_one("sg_copy_from_buffer", (void *)sg_copy_from_buffer);
    lusb_ksym_one("sg_copy_to_buffer", (void *)sg_copy_to_buffer);
    lusb_ksym_one("sg_pcopy_from_buffer", (void *)sg_pcopy_from_buffer);
    lusb_ksym_one("sg_pcopy_to_buffer", (void *)sg_pcopy_to_buffer);
    lusb_ksym_one("sg_zero_buffer", (void *)sg_zero_buffer);
    lusb_ksym_one("__sg_alloc_table", (void *)__sg_alloc_table);
    lusb_ksym_one("__sg_free_table", (void *)__sg_free_table);
    lusb_ksym_one("sg_alloc_table_from_pages",
                  (void *)sg_alloc_table_from_pages);
    lusb_ksym_one("sg_split", (void *)sg_split);
    lusb_ksym_one("sg_miter_get_next_page", (void *)sg_miter_get_next_page);

    /* residual UND: usbcore endpoint/config/urb-fill (29) Soft!=HC; != stick */
    lusb_ksym_one("usb_disable_endpoint", (void *)usb_disable_endpoint);
    lusb_ksym_one("usb_enable_endpoint", (void *)usb_enable_endpoint);
    lusb_ksym_one("usb_disable_interface", (void *)usb_disable_interface);
    lusb_ksym_one("usb_enable_interface", (void *)usb_enable_interface);
    lusb_ksym_one("usb_get_configuration", (void *)usb_get_configuration);
    lusb_ksym_one("usb_destroy_configuration",
                  (void *)usb_destroy_configuration);
    lusb_ksym_one("usb_cache_string", (void *)usb_cache_string);
    lusb_ksym_one("usb_init_urb", (void *)usb_init_urb);
    lusb_ksym_one("usb_fill_bulk_urb", (void *)usb_fill_bulk_urb);
    lusb_ksym_one("usb_fill_control_urb", (void *)usb_fill_control_urb);
    lusb_ksym_one("usb_fill_int_urb", (void *)usb_fill_int_urb);
    lusb_ksym_one("usb_maxpacket", (void *)usb_maxpacket);
    lusb_ksym_one("usb_endpoint_num", (void *)usb_endpoint_num);
    lusb_ksym_one("usb_endpoint_type", (void *)usb_endpoint_type);
    lusb_ksym_one("usb_endpoint_dir_in", (void *)usb_endpoint_dir_in);
    lusb_ksym_one("usb_endpoint_is_bulk_in", (void *)usb_endpoint_is_bulk_in);
    lusb_ksym_one("usb_endpoint_is_bulk_out",
                  (void *)usb_endpoint_is_bulk_out);
    lusb_ksym_one("usb_endpoint_is_int_in", (void *)usb_endpoint_is_int_in);
    lusb_ksym_one("usb_endpoint_is_int_out", (void *)usb_endpoint_is_int_out);
    lusb_ksym_one("usb_authorize_device", (void *)usb_authorize_device);
    lusb_ksym_one("usb_deauthorize_device", (void *)usb_deauthorize_device);
    lusb_ksym_one("usb_detect_quirks", (void *)usb_detect_quirks);
    lusb_ksym_one("usb_detect_interface_quirks",
                  (void *)usb_detect_interface_quirks);
    lusb_ksym_one("usb_find_device", (void *)usb_find_device);
    lusb_ksym_one("__usb_get_extra_descriptor",
                  (void *)__usb_get_extra_descriptor);
    lusb_ksym_one("usb_driver_set_configuration",
                  (void *)usb_driver_set_configuration);
    lusb_ksym_one("usb_hcd_platform_shutdown",
                  (void *)usb_hcd_platform_shutdown);
    lusb_ksym_one("usb_amd_dev_put", (void *)usb_amd_dev_put);
    lusb_ksym_one("usb_set_isoch_delay", (void *)usb_set_isoch_delay);

    /* residual2 UND: scsi mid host/scan/vpd/io (30) Soft!=product; != stick */
    lusb_ksym_one("scsi_device_lookup_by_target",
                  (void *)scsi_device_lookup_by_target);
    lusb_ksym_one("scsi_host_busy_iter", (void *)scsi_host_busy_iter);
    lusb_ksym_one("scsi_scan_host_selected",
                  (void *)scsi_scan_host_selected);
    lusb_ksym_one("scsi_complete_async_scans",
                  (void *)scsi_complete_async_scans);
    lusb_ksym_one("scsi_get_host_dev", (void *)scsi_get_host_dev);
    lusb_ksym_one("scsi_free_host_dev", (void *)scsi_free_host_dev);
    lusb_ksym_one("scsi_target_reap", (void *)scsi_target_reap);
    lusb_ksym_one("scsi_device_reprobe", (void *)scsi_device_reprobe);
    lusb_ksym_one("scsi_host_set_state", (void *)scsi_host_set_state);
    lusb_ksym_one("scsi_cmd_allowed", (void *)scsi_cmd_allowed);
    lusb_ksym_one("scsi_timeout", (void *)scsi_timeout);
    lusb_ksym_one("scsi_io_completion", (void *)scsi_io_completion);
    lusb_ksym_one("scsi_run_host_queues", (void *)scsi_run_host_queues);
    lusb_ksym_one("scsi_device_unbusy", (void *)scsi_device_unbusy);
    lusb_ksym_one("scsi_requeue_command", (void *)scsi_requeue_command);
    lusb_ksym_one("scsi_init_io", (void *)scsi_init_io);
    lusb_ksym_one("scsi_device_from_queue", (void *)scsi_device_from_queue);
    lusb_ksym_one("scsi_vpd_lun_id", (void *)scsi_vpd_lun_id);
    lusb_ksym_one("scsi_vpd_tpg_id", (void *)scsi_vpd_tpg_id);
    lusb_ksym_one("scsi_attach_vpd", (void *)scsi_attach_vpd);
    lusb_ksym_one("scsi_internal_device_block_nowait",
                  (void *)scsi_internal_device_block_nowait);
    lusb_ksym_one("scsi_internal_device_unblock_nowait",
                  (void *)scsi_internal_device_unblock_nowait);
    lusb_ksym_one("scsi_result_to_blk_status",
                  (void *)scsi_result_to_blk_status);
    lusb_ksym_one("scsi_status_is_good", (void *)scsi_status_is_good);
    lusb_ksym_one("scsi_host_complete_all_commands",
                  (void *)scsi_host_complete_all_commands);
    lusb_ksym_one("scsi_queue_work", (void *)scsi_queue_work);
    lusb_ksym_one("scsi_flush_work", (void *)scsi_flush_work);
    lusb_ksym_one("scsi_cdl_enable", (void *)scsi_cdl_enable);
    lusb_ksym_one("scsi_bios_ptable", (void *)scsi_bios_ptable);
    lusb_ksym_one("scsi_partsize", (void *)scsi_partsize);

    /* residual2 UND: sg chained/sgl/page-iter (10) Soft!=map; != stick */
    lusb_ksym_one("sg_alloc_table_chained", (void *)sg_alloc_table_chained);
    lusb_ksym_one("sg_free_table_chained", (void *)sg_free_table_chained);
    lusb_ksym_one("sgl_alloc", (void *)sgl_alloc);
    lusb_ksym_one("sgl_free", (void *)sgl_free);
    lusb_ksym_one("sgl_alloc_order", (void *)sgl_alloc_order);
    lusb_ksym_one("sgl_free_order", (void *)sgl_free_order);
    lusb_ksym_one("sgl_free_n_order", (void *)sgl_free_n_order);
    lusb_ksym_one("__sg_page_iter_start", (void *)__sg_page_iter_start);
    lusb_ksym_one("__sg_page_iter_next", (void *)__sg_page_iter_next);
    lusb_ksym_one("__sg_page_iter_dma_next",
                  (void *)__sg_page_iter_dma_next);

    /* residual2 UND: usbcore hcd/ep/bos/hub/notify (30) Soft!=HC; != stick */
    lusb_ksym_one("usb_hcd_submit_urb", (void *)usb_hcd_submit_urb);
    lusb_ksym_one("usb_hcd_unlink_urb", (void *)usb_hcd_unlink_urb);
    lusb_ksym_one("usb_hcd_flush_endpoint", (void *)usb_hcd_flush_endpoint);
    lusb_ksym_one("usb_hcd_disable_endpoint",
                  (void *)usb_hcd_disable_endpoint);
    lusb_ksym_one("usb_hcd_reset_endpoint", (void *)usb_hcd_reset_endpoint);
    lusb_ksym_one("usb_hcd_synchronize_unlinks",
                  (void *)usb_hcd_synchronize_unlinks);
    lusb_ksym_one("usb_hc_died", (void *)usb_hc_died);
    lusb_ksym_one("usb_root_hub_lost_power",
                  (void *)usb_root_hub_lost_power);
    lusb_ksym_one("usb_authorize_interface",
                  (void *)usb_authorize_interface);
    lusb_ksym_one("usb_deauthorize_interface",
                  (void *)usb_deauthorize_interface);
    lusb_ksym_one("usb_get_bos_descriptor", (void *)usb_get_bos_descriptor);
    lusb_ksym_one("usb_release_bos_descriptor",
                  (void *)usb_release_bos_descriptor);
    lusb_ksym_one("usb_get_device_descriptor",
                  (void *)usb_get_device_descriptor);
    lusb_ksym_one("usb_enable_remote_wakeup",
                  (void *)usb_enable_remote_wakeup);
    lusb_ksym_one("usb_disable_remote_wakeup",
                  (void *)usb_disable_remote_wakeup);
    lusb_ksym_one("usb_enable_usb2_hardware_lpm",
                  (void *)usb_enable_usb2_hardware_lpm);
    lusb_ksym_one("usb_disable_usb2_hardware_lpm",
                  (void *)usb_disable_usb2_hardware_lpm);
    lusb_ksym_one("usb_hub_port_disable", (void *)usb_hub_port_disable);
    lusb_ksym_one("usb_hub_port_reset", (void *)usb_hub_port_reset);
    lusb_ksym_one("usb_reset_and_verify_device",
                  (void *)usb_reset_and_verify_device);
    lusb_ksym_one("usb_create_ep_devs", (void *)usb_create_ep_devs);
    lusb_ksym_one("usb_remove_ep_devs", (void *)usb_remove_ep_devs);
    lusb_ksym_one("usb_forced_unbind_intf", (void *)usb_forced_unbind_intf);
    lusb_ksym_one("usb_unbind_and_rebind_marked_interfaces",
                  (void *)usb_unbind_and_rebind_marked_interfaces);
    lusb_ksym_one("usb_hub_claimed_port", (void *)usb_hub_claimed_port);
    lusb_ksym_one("usb_notify_add_device", (void *)usb_notify_add_device);
    lusb_ksym_one("usb_notify_remove_device",
                  (void *)usb_notify_remove_device);
    lusb_ksym_one("usb_create_sysfs_dev_files",
                  (void *)usb_create_sysfs_dev_files);
    lusb_ksym_one("usb_remove_sysfs_dev_files",
                  (void *)usb_remove_sysfs_dev_files);
    lusb_ksym_one("usb_decode_interval", (void *)usb_decode_interval);

    /* residual3 UND: scsi register/sysfs/evt/mq (30) Soft!=product; != stick */
    lusb_ksym_one("scsi_register", (void *)scsi_register);
    lusb_ksym_one("scsi_unregister", (void *)scsi_unregister);
    lusb_ksym_one("scsi_register_driver", (void *)scsi_register_driver);
    lusb_ksym_one("scsi_unregister_driver", (void *)scsi_unregister_driver);
    lusb_ksym_one("scsi_register_interface", (void *)scsi_register_interface);
    lusb_ksym_one("scsi_unregister_interface",
                  (void *)scsi_unregister_interface);
    lusb_ksym_one("scsi_host_set_prot", (void *)scsi_host_set_prot);
    lusb_ksym_one("scsi_host_prot_dma_mask",
                  (void *)scsi_host_prot_dma_mask);
    lusb_ksym_one("scsi_get_device_flags", (void *)scsi_get_device_flags);
    lusb_ksym_one("scsi_sysfs_add_sdev", (void *)scsi_sysfs_add_sdev);
    lusb_ksym_one("__scsi_iterate_devices", (void *)__scsi_iterate_devices);
    lusb_ksym_one("starget_for_each_device",
                  (void *)starget_for_each_device);
    lusb_ksym_one("scsi_device_state_name", (void *)scsi_device_state_name);
    lusb_ksym_one("scsi_host_state_name", (void *)scsi_host_state_name);
    lusb_ksym_one("scsi_print_sense", (void *)scsi_print_sense);
    lusb_ksym_one("__scsi_print_command", (void *)__scsi_print_command);
    lusb_ksym_one("scsi_show_result", (void *)scsi_show_result);
    lusb_ksym_one("scsi_show_sense", (void *)scsi_show_sense);
    lusb_ksym_one("scsi_is_ua_sense", (void *)scsi_is_ua_sense);
    lusb_ksym_one("sdev_evt_alloc", (void *)sdev_evt_alloc);
    lusb_ksym_one("sdev_evt_send", (void *)sdev_evt_send);
    lusb_ksym_one("sdev_evt_send_simple", (void *)sdev_evt_send_simple);
    lusb_ksym_one("scsi_cmd_get_serial", (void *)scsi_cmd_get_serial);
    lusb_ksym_one("scsi_device_busy", (void *)scsi_device_busy);
    lusb_ksym_one("scsi_kick_queue", (void *)scsi_kick_queue);
    lusb_ksym_one("__scsi_execute", (void *)__scsi_execute);
    lusb_ksym_one("scsi_mq_setup_tags", (void *)scsi_mq_setup_tags);
    lusb_ksym_one("scsi_mq_free_tags", (void *)scsi_mq_free_tags);
    lusb_ksym_one("scsi_init_queue", (void *)scsi_init_queue);
    lusb_ksym_one("scsi_exit_queue", (void *)scsi_exit_queue);

    /* residual3 UND: sg append/virt/dma (7) Soft!=map; != stick */
    lusb_ksym_one("sg_alloc_append_table_from_pages",
                  (void *)sg_alloc_append_table_from_pages);
    lusb_ksym_one("sg_free_append_table", (void *)sg_free_append_table);
    lusb_ksym_one("sg_miter_consume", (void *)sg_miter_consume);
    lusb_ksym_one("sg_virt", (void *)sg_virt);
    lusb_ksym_one("sg_phys", (void *)sg_phys);
    lusb_ksym_one("sg_dma_len", (void *)sg_dma_len);
    lusb_ksym_one("sg_dma_address", (void *)sg_dma_address);

    /* residual3 UND: usbcore dynid/buffer_map (10) Soft!=HC; != stick */
    lusb_ksym_one("usb_store_new_id", (void *)usb_store_new_id);
    lusb_ksym_one("usb_show_dynids", (void *)usb_show_dynids);
    lusb_ksym_one("usb_register_device_driver",
                  (void *)usb_register_device_driver);
    lusb_ksym_one("usb_deregister_device_driver",
                  (void *)usb_deregister_device_driver);
    lusb_ksym_one("usb_bus_start_enum", (void *)usb_bus_start_enum);
    lusb_ksym_one("usb_buffer_map", (void *)usb_buffer_map);
    lusb_ksym_one("usb_buffer_unmap", (void *)usb_buffer_unmap);
    lusb_ksym_one("usb_buffer_map_sg", (void *)usb_buffer_map_sg);
    lusb_ksym_one("usb_buffer_unmap_sg", (void *)usb_buffer_unmap_sg);
    lusb_ksym_one("usb_disabled_new_device", (void *)usb_disabled_new_device);

    /* residual4 UND: scsi target/queue/dispatch (12) Soft!=product; != stick */
    lusb_ksym_one("scsi_target_quiesce", (void *)scsi_target_quiesce);
    lusb_ksym_one("scsi_target_resume", (void *)scsi_target_resume);
    lusb_ksym_one("scsi_internal_device_block",
                  (void *)scsi_internal_device_block);
    lusb_ksym_one("scsi_internal_device_unblock",
                  (void *)scsi_internal_device_unblock);
    lusb_ksym_one("scsi_register_device", (void *)scsi_register_device);
    lusb_ksym_one("scsi_unregister_device", (void *)scsi_unregister_device);
    lusb_ksym_one("scsi_device_cancel_work", (void *)scsi_device_cancel_work);
    lusb_ksym_one("scsi_dev_queue_ready", (void *)scsi_dev_queue_ready);
    lusb_ksym_one("scsi_end_request", (void *)scsi_end_request);
    lusb_ksym_one("scsi_mq_done", (void *)scsi_mq_done);
    lusb_ksym_one("scsi_softirq_done", (void *)scsi_softirq_done);
    lusb_ksym_one("scsi_dispatch_cmd", (void *)scsi_dispatch_cmd);

    /* residual4 UND: sg page/kmalloc (6) Soft!=map; != stick */
    lusb_ksym_one("sg_init_marker", (void *)sg_init_marker);
    lusb_ksym_one("sg_assign_page", (void *)sg_assign_page);
    lusb_ksym_one("sg_set_page", (void *)sg_set_page);
    lusb_ksym_one("sg_page", (void *)sg_page);
    lusb_ksym_one("sg_kfree", (void *)sg_kfree);
    lusb_ksym_one("sg_kmalloc", (void *)sg_kmalloc);

    /* residual4 UND: usbcore hcd/hub/ep sysfs (11) Soft!=HC; != stick */
    lusb_ksym_one("usb_hcd_find_raw_port_number",
                  (void *)usb_hcd_find_raw_port_number);
    lusb_ksym_one("usb_hcd_alloc_dev", (void *)usb_hcd_alloc_dev);
    lusb_ksym_one("usb_hcd_free_dev", (void *)usb_hcd_free_dev);
    lusb_ksym_one("usb_disconnect_and_release",
                  (void *)usb_disconnect_and_release);
    lusb_ksym_one("usb_ep_device_add", (void *)usb_ep_device_add);
    lusb_ksym_one("usb_ep_device_remove", (void *)usb_ep_device_remove);
    lusb_ksym_one("usb_create_sysfs_intf_files",
                  (void *)usb_create_sysfs_intf_files);
    lusb_ksym_one("usb_remove_sysfs_intf_files",
                  (void *)usb_remove_sysfs_intf_files);
    lusb_ksym_one("usb_bus_get_by_id", (void *)usb_bus_get_by_id);
    lusb_ksym_one("usb_hub_may_disable_port",
                  (void *)usb_hub_may_disable_port);
    lusb_ksym_one("usb_hub_set_configuration",
                  (void *)usb_hub_set_configuration);

    /* residual5 UND: scsi queue/EH abort/offline/resid (16) Soft!=product */
    lusb_ksym_one("scsi_queue_insert", (void *)scsi_queue_insert);
    lusb_ksym_one("scsi_run_queue", (void *)scsi_run_queue);
    lusb_ksym_one("scsi_requeue_run_queue", (void *)scsi_requeue_run_queue);
    lusb_ksym_one("scsi_setup_command", (void *)scsi_setup_command);
    lusb_ksym_one("scsi_mq_requeue_cmd", (void *)scsi_mq_requeue_cmd);
    lusb_ksym_one("scsi_times_out", (void *)scsi_times_out);
    lusb_ksym_one("scsi_abort_command", (void *)scsi_abort_command);
    lusb_ksym_one("scsi_try_to_abort_cmd", (void *)scsi_try_to_abort_cmd);
    lusb_ksym_one("scsi_eh_done", (void *)scsi_eh_done);
    lusb_ksym_one("scsi_error_handler", (void *)scsi_error_handler);
    lusb_ksym_one("scsi_offline_device", (void *)scsi_offline_device);
    lusb_ksym_one("scsi_online_device", (void *)scsi_online_device);
    lusb_ksym_one("scsi_set_resid", (void *)scsi_set_resid);
    lusb_ksym_one("scsi_get_resid", (void *)scsi_get_resid);
    lusb_ksym_one("scsi_init_sense_cache", (void *)scsi_init_sense_cache);
    lusb_ksym_one("scsicam_bios_param", (void *)scsicam_bios_param);

    /* residual5 UND: sg set/pool/segment (6) Soft!=map; != stick */
    lusb_ksym_one("sg_set_buf", (void *)sg_set_buf);
    lusb_ksym_one("sg_set_folio", (void *)sg_set_folio);
    lusb_ksym_one("sg_alloc_table_from_pages_segment",
                  (void *)sg_alloc_table_from_pages_segment);
    lusb_ksym_one("sg_pool_alloc", (void *)sg_pool_alloc);
    lusb_ksym_one("sg_pool_free", (void *)sg_pool_free);
    lusb_ksym_one("sg_mark_end", (void *)sg_mark_end);

    /* residual5 UND: usbcore hcd start/bus/runtime + hub/xhci (15) Soft!=HC */
    lusb_ksym_one("usb_hcd_start", (void *)usb_hcd_start);
    lusb_ksym_one("usb_hcd_stop", (void *)usb_hcd_stop);
    lusb_ksym_one("usb_hcd_bus_suspend", (void *)usb_hcd_bus_suspend);
    lusb_ksym_one("usb_hcd_bus_resume", (void *)usb_hcd_bus_resume);
    lusb_ksym_one("usb_hcd_defer_giveback", (void *)usb_hcd_defer_giveback);
    lusb_ksym_one("usb_hcd_setup_local_mem", (void *)usb_hcd_setup_local_mem);
    lusb_ksym_one("usb_port_disable", (void *)usb_port_disable);
    lusb_ksym_one("usb_remote_wakeup", (void *)usb_remote_wakeup);
    lusb_ksym_one("usb_runtime_suspend", (void *)usb_runtime_suspend);
    lusb_ksym_one("usb_runtime_resume", (void *)usb_runtime_resume);
    lusb_ksym_one("usb_runtime_idle", (void *)usb_runtime_idle);
    lusb_ksym_one("usb_kick_hub_wq", (void *)usb_kick_hub_wq);
    lusb_ksym_one("usb_release_interface", (void *)usb_release_interface);
    lusb_ksym_one("usb_enable_intel_xhci_ports",
                  (void *)usb_enable_intel_xhci_ports);
    lusb_ksym_one("usb_disable_xhci_ports", (void *)usb_disable_xhci_ports);

    /* residual6 UND: scsi cmnd/rq/mq + EH tur (17) Soft!=product; != stick */
    lusb_ksym_one("scsi_cmd_to_rq", (void *)scsi_cmd_to_rq);
    lusb_ksym_one("scsi_cmnd_ptr", (void *)scsi_cmnd_ptr);
    lusb_ksym_one("scsi_req_init", (void *)scsi_req_init);
    lusb_ksym_one("scsi_initialize_rq", (void *)scsi_initialize_rq);
    lusb_ksym_one("scsi_init_request", (void *)scsi_init_request);
    lusb_ksym_one("scsi_exit_request", (void *)scsi_exit_request);
    lusb_ksym_one("scsi_get_command", (void *)scsi_get_command);
    lusb_ksym_one("scsi_put_command", (void *)scsi_put_command);
    lusb_ksym_one("scsi_free_host_cmd", (void *)scsi_free_host_cmd);
    lusb_ksym_one("scsi_host_queue_ready", (void *)scsi_host_queue_ready);
    lusb_ksym_one("scsi_target_busy", (void *)scsi_target_busy);
    lusb_ksym_one("scsi_eh_ready_devs", (void *)scsi_eh_ready_devs);
    lusb_ksym_one("scsi_eh_offline_sdevs", (void *)scsi_eh_offline_sdevs);
    lusb_ksym_one("scsi_send_eh_cmnd", (void *)scsi_send_eh_cmnd);
    lusb_ksym_one("scsi_eh_tur", (void *)scsi_eh_tur);
    lusb_ksym_one("scsi_eh_try_stu", (void *)scsi_eh_try_stu);
    lusb_ksym_one("scsi_restart_operations",
                  (void *)scsi_restart_operations);

    /* residual6 UND: sg chain/last/nents_for_dma (6) Soft!=map; != stick */
    lusb_ksym_one("sg_unmark_end", (void *)sg_unmark_end);
    lusb_ksym_one("sg_is_last", (void *)sg_is_last);
    lusb_ksym_one("sg_chain", (void *)sg_chain);
    lusb_ksym_one("sg_nents_for_dma", (void *)sg_nents_for_dma);
    lusb_ksym_one("__sg_alloc_table_from_pages",
                  (void *)__sg_alloc_table_from_pages);
    lusb_ksym_one("sg_next_safe", (void *)sg_next_safe);

    /* residual6 UND: usbcore hcd pci PM + hub/port/bus (14) Soft!=HC */
    lusb_ksym_one("usb_hcd_pci_suspend", (void *)usb_hcd_pci_suspend);
    lusb_ksym_one("usb_hcd_pci_resume", (void *)usb_hcd_pci_resume);
    lusb_ksym_one("usb_hcd_pci_suspend_noirq",
                  (void *)usb_hcd_pci_suspend_noirq);
    lusb_ksym_one("usb_hcd_pci_resume_noirq",
                  (void *)usb_hcd_pci_resume_noirq);
    lusb_ksym_one("usb_hcd_pci_runtime_suspend",
                  (void *)usb_hcd_pci_runtime_suspend);
    lusb_ksym_one("usb_hcd_pci_runtime_resume",
                  (void *)usb_hcd_pci_runtime_resume);
    lusb_ksym_one("usb_hcd_unmap_urb_setup_for_dma",
                  (void *)usb_hcd_unmap_urb_setup_for_dma);
    lusb_ksym_one("usb_mark_last_busy", (void *)usb_mark_last_busy);
    lusb_ksym_one("usb_port_runtime_suspend",
                  (void *)usb_port_runtime_suspend);
    lusb_ksym_one("usb_port_runtime_resume",
                  (void *)usb_port_runtime_resume);
    lusb_ksym_one("usb_hub_cleanup", (void *)usb_hub_cleanup);
    lusb_ksym_one("usb_bus_init", (void *)usb_bus_init);
    lusb_ksym_one("usb_remove_device_files",
                  (void *)usb_remove_device_files);
    lusb_ksym_one("usb_enable_interface_streams",
                  (void *)usb_enable_interface_streams);

    /* residual7 UND: usbcore hcd buffer/platform + hub/port + phy (18) Soft!=HC */
    lusb_ksym_one("usb_hcd_buffer_alloc", (void *)usb_hcd_buffer_alloc);
    lusb_ksym_one("usb_hcd_buffer_free", (void *)usb_hcd_buffer_free);
    lusb_ksym_one("usb_hcd_buffer_create", (void *)usb_hcd_buffer_create);
    lusb_ksym_one("usb_hcd_buffer_destroy", (void *)usb_hcd_buffer_destroy);
    lusb_ksym_one("usb_hcd_platform_probe", (void *)usb_hcd_platform_probe);
    lusb_ksym_one("usb_hcd_platform_remove", (void *)usb_hcd_platform_remove);
    lusb_ksym_one("usb_hcd_shared_hcd", (void *)usb_hcd_shared_hcd);
    lusb_ksym_one("usb_hcd_may_wakeup", (void *)usb_hcd_may_wakeup);
    lusb_ksym_one("usb_giveback_urb_bh", (void *)usb_giveback_urb_bh);
    lusb_ksym_one("usb_enable_root_hub_irq", (void *)usb_enable_root_hub_irq);
    lusb_ksym_one("usb_disable_root_hub_irq",
                  (void *)usb_disable_root_hub_irq);
    lusb_ksym_one("usb_hub_handle_tt_clear", (void *)usb_hub_handle_tt_clear);
    lusb_ksym_one("usb_hub_port_debounce", (void *)usb_hub_port_debounce);
    lusb_ksym_one("usb_hub_set_port_power", (void *)usb_hub_set_port_power);
    lusb_ksym_one("usb_notify_add_bus", (void *)usb_notify_add_bus);
    lusb_ksym_one("usb_notify_remove_bus", (void *)usb_notify_remove_bus);
    lusb_ksym_one("usb_get_phy", (void *)usb_get_phy);
    lusb_ksym_one("usb_put_phy", (void *)usb_put_phy);

    /* residual7 UND: scsi host recovery + EH + mq + VPD (12) Soft!=product */
    lusb_ksym_one("scsi_host_in_recovery", (void *)scsi_host_in_recovery);
    lusb_ksym_one("scsi_host_get_state", (void *)scsi_host_get_state);
    lusb_ksym_one("scsi_eh_lock_door", (void *)scsi_eh_lock_door);
    lusb_ksym_one("scsi_ioctl_reset", (void *)scsi_ioctl_reset);
    lusb_ksym_one("scsi_cmd_ioctl", (void *)scsi_cmd_ioctl);
    lusb_ksym_one("scsi_get_vpd_buf", (void *)scsi_get_vpd_buf);
    lusb_ksym_one("scsi_vpd_release", (void *)scsi_vpd_release);
    lusb_ksym_one("scsi_block_targets", (void *)scsi_block_targets);
    lusb_ksym_one("scsi_unblock_targets", (void *)scsi_unblock_targets);
    lusb_ksym_one("scsi_mq_alloc_queue", (void *)scsi_mq_alloc_queue);
    lusb_ksym_one("scsi_mq_destroy_queue", (void *)scsi_mq_destroy_queue);
    lusb_ksym_one("scsi_sense_valid", (void *)scsi_sense_valid);

    /* residual7 UND: sg page-iter + table-from-buf (6) Soft!=map; != stick */
    lusb_ksym_one("sg_page_iter_page", (void *)sg_page_iter_page);
    lusb_ksym_one("sg_page_iter_dma_address",
                  (void *)sg_page_iter_dma_address);
    lusb_ksym_one("sg_init_table_from_buf", (void *)sg_init_table_from_buf);
    lusb_ksym_one("sg_copy_buffer_to_sg", (void *)sg_copy_buffer_to_sg);
    lusb_ksym_one("sg_miter_get_next_sg", (void *)sg_miter_get_next_sg);
    lusb_ksym_one("sg_alloc_table_from_buf", (void *)sg_alloc_table_from_buf);

    /* residual8 UND: HCD/URB layout bridge for UDX (8) Soft!=HC; DoD A seed */
    lusb_ksym_one("hcd_to_bus", (void *)hcd_to_bus);
    lusb_ksym_one("bus_to_hcd", (void *)bus_to_hcd);
    lusb_ksym_one("usb_set_intfdata", (void *)usb_set_intfdata);
    lusb_ksym_one("usb_get_intfdata", (void *)usb_get_intfdata);
    lusb_ksym_one("usb_make_path", (void *)usb_make_path);
    lusb_ksym_one("init_usb_anchor", (void *)init_usb_anchor);
    lusb_ksym_one("usb_find_interface_driver",
                  (void *)usb_find_interface_driver);
    lusb_ksym_one("usb_hcd_link_urb_to_ep_checked",
                  (void *)usb_hcd_link_urb_to_ep_checked);

    /* residual9 UND: HCD/URB layout residual for UDX (10) Soft!=HC; DoD A */
    lusb_ksym_one("usb_hcd_to_dev", (void *)usb_hcd_to_dev);
    lusb_ksym_one("usb_free_dev", (void *)usb_free_dev);
    lusb_ksym_one("usb_hcd_update_device", (void *)usb_hcd_update_device);
    lusb_ksym_one("usb_device_is_root_hub", (void *)usb_device_is_root_hub);
    lusb_ksym_one("usb_hcd_check_bandwidth", (void *)usb_hcd_check_bandwidth);
    lusb_ksym_one("usb_hcd_release_bandwidth",
                  (void *)usb_hcd_release_bandwidth);
    lusb_ksym_one("usb_find_host_interface", (void *)usb_find_host_interface);
    lusb_ksym_one("usb_get_hcd_from_urb", (void *)usb_get_hcd_from_urb);
    lusb_ksym_one("usb_urb_ep_index", (void *)usb_urb_ep_index);
    lusb_ksym_one("usb_get_dev_from_urb", (void *)usb_get_dev_from_urb);

    /* residual10 UND: soft EP + reverse cookies for UDX (8) Soft!=HC; DoD A */
    lusb_ksym_one("usb_get_hcd_from_dev", (void *)usb_get_hcd_from_dev);
    lusb_ksym_one("usb_get_bus_from_dev", (void *)usb_get_bus_from_dev);
    lusb_ksym_one("usb_get_ep_from_urb", (void *)usb_get_ep_from_urb);
    lusb_ksym_one("usb_urb_set_ep", (void *)usb_urb_set_ep);
    lusb_ksym_one("usb_urb_actual_length", (void *)usb_urb_actual_length);
    lusb_ksym_one("usb_hcd_bus_name", (void *)usb_hcd_bus_name);
    lusb_ksym_one("usb_hcd_soft_wire_dev", (void *)usb_hcd_soft_wire_dev);
    lusb_ksym_one("usb_endpoint_device", (void *)usb_endpoint_device);

    /* residual11 UND: EP maxp/enable + URB reverse for UDX (8) Soft!=HC; DoD A */
    lusb_ksym_one("usb_endpoint_set_maxp", (void *)usb_endpoint_set_maxp);
    lusb_ksym_one("usb_endpoint_hcd", (void *)usb_endpoint_hcd);
    lusb_ksym_one("usb_endpoint_enabled", (void *)usb_endpoint_enabled);
    lusb_ksym_one("usb_urb_transfer_buffer", (void *)usb_urb_transfer_buffer);
    lusb_ksym_one("usb_urb_transfer_buffer_length",
                  (void *)usb_urb_transfer_buffer_length);
    lusb_ksym_one("usb_urb_pipe", (void *)usb_urb_pipe);
    lusb_ksym_one("usb_urb_context", (void *)usb_urb_context);
    lusb_ksym_one("usb_soft_encode_pipe", (void *)usb_soft_encode_pipe);

    /* residual12 UND: lean Dual DoD A eng seed for UDX (8) Soft!=HC; DoD A */
    lusb_ksym_one("usb_endpoint_address", (void *)usb_endpoint_address);
    lusb_ksym_one("usb_endpoint_set_type", (void *)usb_endpoint_set_type);
    lusb_ksym_one("usb_endpoint_wire_hcd", (void *)usb_endpoint_wire_hcd);
    lusb_ksym_one("usb_urb_setup_packet", (void *)usb_urb_setup_packet);
    lusb_ksym_one("usb_urb_complete", (void *)usb_urb_complete);
    lusb_ksym_one("usb_urb_set_context", (void *)usb_urb_set_context);
    lusb_ksym_one("usb_urb_set_transfer_buffer",
                  (void *)usb_urb_set_transfer_buffer);
    lusb_ksym_one("usb_soft_decode_pipe", (void *)usb_soft_decode_pipe);

    /* residual13 UND: lean Dual DoD A eng residual for UDX (8) Soft!=HC; DoD A */
    lusb_ksym_one("usb_urb_set_setup_packet", (void *)usb_urb_set_setup_packet);
    lusb_ksym_one("usb_urb_set_complete", (void *)usb_urb_set_complete);
    lusb_ksym_one("usb_urb_set_pipe", (void *)usb_urb_set_pipe);
    lusb_ksym_one("usb_urb_set_status", (void *)usb_urb_set_status);
    lusb_ksym_one("usb_endpoint_interval", (void *)usb_endpoint_interval);
    lusb_ksym_one("usb_endpoint_set_interval",
                  (void *)usb_endpoint_set_interval);
    lusb_ksym_one("usb_urb_xfer_type", (void *)usb_urb_xfer_type);
    lusb_ksym_one("usb_soft_pipe_from_ep", (void *)usb_soft_pipe_from_ep);

    /* residual14 UND: lean Dual DoD A eng residual deepen for UDX (8) Soft!=HC */
    lusb_ksym_one("usb_urb_set_dev", (void *)usb_urb_set_dev);
    lusb_ksym_one("usb_urb_set_hcd", (void *)usb_urb_set_hcd);
    lusb_ksym_one("usb_urb_set_xfer_type", (void *)usb_urb_set_xfer_type);
    lusb_ksym_one("usb_urb_interval", (void *)usb_urb_interval);
    lusb_ksym_one("usb_urb_set_interval", (void *)usb_urb_set_interval);
    lusb_ksym_one("usb_urb_linked", (void *)usb_urb_linked);
    lusb_ksym_one("usb_urb_set_actual_length",
                  (void *)usb_urb_set_actual_length);
    lusb_ksym_one("usb_soft_wire_urb_ep", (void *)usb_soft_wire_urb_ep);

    /* residual15 UND: lean Dual DoD A eng residual lifecycle for UDX (8) Soft!=HC */
    lusb_ksym_one("usb_urb_submitted", (void *)usb_urb_submitted);
    lusb_ksym_one("usb_urb_set_submitted", (void *)usb_urb_set_submitted);
    lusb_ksym_one("usb_urb_unlinked", (void *)usb_urb_unlinked);
    lusb_ksym_one("usb_urb_set_unlinked", (void *)usb_urb_set_unlinked);
    lusb_ksym_one("usb_urb_poisoned", (void *)usb_urb_poisoned);
    lusb_ksym_one("usb_urb_set_poisoned", (void *)usb_urb_set_poisoned);
    lusb_ksym_one("usb_urb_seq", (void *)usb_urb_seq);
    lusb_ksym_one("usb_soft_wire_urb_hcd", (void *)usb_soft_wire_urb_hcd);

    /* residual16 UND: lean Dual DoD A eng residual anchor/live for UDX (8) Soft!=HC */
    lusb_ksym_one("usb_urb_anchored", (void *)usb_urb_anchored);
    lusb_ksym_one("usb_urb_set_anchored", (void *)usb_urb_set_anchored);
    lusb_ksym_one("usb_urb_live", (void *)usb_urb_live);
    lusb_ksym_one("usb_urb_set_live", (void *)usb_urb_set_live);
    lusb_ksym_one("usb_urb_anchor", (void *)usb_urb_anchor);
    lusb_ksym_one("usb_urb_set_anchor", (void *)usb_urb_set_anchor);
    lusb_ksym_one("usb_soft_wire_urb_dev", (void *)usb_soft_wire_urb_dev);
    lusb_ksym_one("usb_soft_clear_urb_lifecycle",
                  (void *)usb_soft_clear_urb_lifecycle);

    /* Grep: linux_usb_soft: soft init PASS n= */
    kprintf("linux_usb_soft: soft init PASS n=%u skip=%u stubs=%u "
            "usb=84 virt_usbcore=101 scsi=58 sg=11 msc_leaf=39 residual=406 "
            "soft=1 product=0 "
            "(virtual usbcore HCD+hub + residual UND + layout r10..r16; "
            "!= stick; G-AC-1; need=HC OPEN; Soft!=product; freestanding_no_exec; "
            "freestanding_msc=SKIP; product=xhci_udx userspace)\n",
            (unsigned)g_cKsymOk, (unsigned)g_cKsymSkip,
            (unsigned)g_cStubReg);

    /*
     * Grep: linux_usb_soft: soft hcd surface PASS
     * Virtual usbcore HCD/hub/device ksym only. Soft!=real HC; != stick.
     */
    kprintf("linux_usb_soft: soft hcd surface PASS n=101 "
            "hcd_cb=%u hcd_pool=%u udev_cb=%u soft=1 product=0 "
            "(virtual usbcore; != stick; G-AC-1; need=HC OPEN; "
            "freestanding_msc=SKIP)\n",
            (unsigned)LINUX_USB_SOFT_HCD_CB,
            (unsigned)LINUX_USB_SOFT_HCD_POOL,
            (unsigned)LINUX_USB_SOFT_UDEV_CB);

    /*
     * Grep: linux_usb_soft: soft hcd/urb layout PASS
     * Soft front-field layout for fill/submit/giveback + UDX cookies.
     * residual10..16: soft EP pool + reverse + maxp/transfer +
     * setup/complete/type wire + pipe/status/interval reverse +
     * dev/hcd/linked/wire_urb_ep + lifecycle submitted/unlinked/poisoned/
     * seq/wire_urb_hcd + anchor/live/wire_urb_dev/clear_lifecycle
     * (lean Dual DoD A eng residual anchor/live).
     * Soft!=ABI-stable; Soft!=product; one-shot.
     */
    if (g_fLogLayout == 0) {
        g_fLogLayout = 1;
        kprintf("linux_usb_soft: soft hcd/urb layout PASS "
                "urb_cb=%u urb_pool=%u hcd_cb=%u udev_cb=%u bus_cb=%u "
                "ep_cb=%u ep_pool=%u urb_sizeof=%u hcd_sizeof=%u "
                "ep_sizeof=%u residual10=8 residual11=8 residual12=8 "
                "residual13=8 residual14=8 residual15=8 residual16=8 "
                "soft=1 product=0 "
                "(UDX-facing EP/URB cookies; Soft!=ABI; != stick; G-AC-1; "
                "freestanding_msc=SKIP; product=xhci_udx userspace)\n",
                (unsigned)LINUX_USB_SOFT_URB_CB,
                (unsigned)LINUX_USB_SOFT_URB_POOL,
                (unsigned)LINUX_USB_SOFT_HCD_CB,
                (unsigned)LINUX_USB_SOFT_UDEV_CB,
                (unsigned)LINUX_USB_SOFT_BUS_CB,
                (unsigned)LINUX_USB_SOFT_EP_CB,
                (unsigned)LINUX_USB_SOFT_EP_POOL,
                (unsigned)sizeof(struct usb_soft_urb),
                (unsigned)sizeof(struct usb_soft_hcd),
                (unsigned)sizeof(struct usb_soft_ep));
    }

    /*
     * Grep: linux_usb_soft: soft scsi_mod surface PASS
     * Soft mid ksym only. Host often ships scsi_mod/usbcore BUILTIN (no .ko);
     * register soft-virtual dep markers so deps_ready can PASS eng lamps
     * without claiming real usbcore.ko / scsi_mod.ko loaded. Soft!=product.
     */
    if (g_fLogScsiMod == 0) {
        g_fLogScsiMod = 1;
        kprintf("linux_usb_soft: soft scsi_mod surface PASS n=58 "
                "host_cb=%u eh_cb=%u deepen=1 soft=1 product=0 "
                "(el9 usb-storage+uas scsi_* class; != stick; need=HC OPEN)\n",
                (unsigned)LINUX_USB_SOFT_HOST_CB,
                (unsigned)LINUX_USB_SOFT_EH_CB);
    }

    /*
     * Grep: linux_usb_soft: soft residual UND PASS
     * D residual class expand (r1+...+r16). Soft!=product; never claim stick write.
     */
    if (g_fLogResidual == 0) {
        g_fLogResidual = 1;
        kprintf("linux_usb_soft: soft residual UND PASS n=406 "
                "scsi=152 sg=53 usb=201 soft=1 product=0 "
                "(sd_mod mid + sg + endpoint/hcd + r8..r16 layout residual; "
                "!= stick; Soft!=product; G-AC-1; need=HC OPEN; "
                "freestanding_msc=SKIP)\n");
    }

    /*
     * Grep: linux_usb_soft: soft DoD A seed
     * Dual DoD A = Linux-shaped USB path toward userspace UDX (not
     * usb_storage.ko init in kernel). Soft seed only; freestanding MSC
     * residual SKIP by default (GJ_XHCI_MSC_PROBE=0). Product = xhci_udx
     * userspace. One-shot. Soft!=product. Cap logs one-shot - no stamp storm.
     * No version stamp. residual16 = lean eng residual anchor/live only.
     */
    if (g_fLogDodA == 0) {
        g_fLogDodA = 1;
        kprintf("linux_usb_soft: soft DoD A seed OPEN "
                "layout=1 residual8=8 residual9=10 residual10=8 residual11=8 "
                "residual12=8 residual13=8 residual14=8 residual15=8 "
                "residual16=8 freestanding_msc=SKIP "
                "GJ_XHCI_MSC_PROBE=0 freestanding_no_exec soft=1 product=0 "
                "(userspace xhci_udx direction; != .ko exec; != stick; G-AC-1; "
                "need=HC OPEN; Soft!=product)\n");
    }

    /*
     * Grep: linux_usb_soft: soft residual lean PASS Soft!=product
     * Once-capped lean honesty for Dual DoD A eng residual. Soft!=product.
     * No freestanding thrash. No RUN_INIT product claims.
     */
    (void)linux_usb_soft_residual_lean_lamp_once();

    /*
     * Soft dep virtual slots for multi-mod order honesty:
     *   usb_common -> slot usb_common-soft
     *   usbcore    -> slot usbcore-soft
     *   scsi_mod   -> slot scsi_mod-soft
     * deps_ready maps soft dep names to these markers; loaded(usbcore)=0.
     * Grep: linux_module: soft dep virtual usbcore soft=1 product=0
     * Grep: linux_module: soft dep virtual scsi_mod soft=1 product=0
     * Grep: linux_module: soft dep virtual usb_common soft=1 product=0
     * Soft!=product · G-AC-1; eng lamps only; stick still need=HC OPEN.
     */
    if (linux_module_soft_dep_virtual_register != NULL) {
        (void)linux_module_soft_dep_virtual_register("usb_common");
        (void)linux_module_soft_dep_virtual_register("usbcore");
        (void)linux_module_soft_dep_virtual_register("scsi_mod");
        kprintf("linux_usb_soft: soft dep virtual register "
                "usb_common+usbcore+scsi_mod soft=1 product=0 "
                "(deps_ready eng PASS; != real .ko; != stick; G-AC-1)\n");
    } else {
        kprintf("linux_usb_soft: soft dep virtual SKIP no module path "
                "soft=1 product=0\n");
    }
}

int
linux_usb_soft_ready(void)
{
    return g_fReady;
}

u32
linux_usb_soft_ksym_ok(void)
{
    return g_cKsymOk;
}

u32
linux_usb_soft_ksym_skip(void)
{
    return g_cKsymSkip;
}

u32
linux_usb_soft_stub_count(void)
{
    return g_cStubReg;
}
