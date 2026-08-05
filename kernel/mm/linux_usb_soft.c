/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft USB core + SCSI mid ksym seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_usb_soft_init: ready lamp + linux_ksym_register of fail-closed bodies
 *   - usb_* register/URB/control/sg/autopm (usb-storage reloc surface)
 *   - scsi_host_* / scsi mid helpers for load only (not real stick)
 *
 * Cap: high-value usb_* + scsi_* for el9 usb-storage.ko (~33 names).
 * Soft ≠ product: load / INIT=0 without real HC ≠ stick datapath.
 *
 * Greppable markers (keep stable):
 *   linux_usb_soft: soft init PASS n=
 *   linux_usb_soft: soft usb_register …
 *   linux_usb_soft: soft submit_urb …
 */
#include <gj/klog.h>
#include <gj/linux_usb_soft.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft errno-shaped returns (Linux negative errno mental model). */
#define LUSB_ENODEV   (-19)
#define LUSB_ENOMEM   (-12)
#define LUSB_EINVAL   (-22)
#define LUSB_ENOENT   (-2)

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
static u32 g_cScsiHostAlloc;

static int g_fLogRegister;
static int g_fLogSubmit;

/* Soft static objects (never free; zeroed once). Soft≠ABI-stable. */
static u8 g_aSoftUrb[LINUX_USB_SOFT_URB_POOL][LINUX_USB_SOFT_URB_CB];
static u8 g_aSoftUrbUsed[LINUX_USB_SOFT_URB_POOL];
static u8 g_aSoftHost[LINUX_USB_SOFT_HOST_CB];
static u8 g_fSoftHostLive;
static u8 g_aSoftCoh[256];
static u8 g_fSoftObjsInit;

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
    memset(g_aSoftCoh, 0, sizeof(g_aSoftCoh));
    g_fSoftObjsInit = 1u;
}

/* ---- usb_* soft bodies (fail-closed / no-op) --------------------------- */

/*
 * Module init path: soft-success register so reloc+init can complete without
 * a real HC. Soft≠product — no probe, no stick. Grep once.
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
                "soft=1 product=0 (no HC; ≠ stick)\n",
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

    (void)nIsoPackets;
    (void)nMemFlags;
    lusb_inc(&g_cAllocUrb);
    lusb_soft_objs_once();
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        if (g_aSoftUrbUsed[i] == 0u) {
            g_aSoftUrbUsed[i] = 1u;
            memset(g_aSoftUrb[i], 0, LINUX_USB_SOFT_URB_CB);
            return (void *)g_aSoftUrb[i];
        }
    }
    /* Pool exhausted — fail closed (no unbounded soft alloc). */
    return NULL;
}

void
usb_free_urb(void *pUrb)
{
    u32 i;

    if (pUrb == NULL) {
        return;
    }
    for (i = 0u; i < LINUX_USB_SOFT_URB_POOL; i++) {
        if ((void *)g_aSoftUrb[i] == pUrb) {
            g_aSoftUrbUsed[i] = 0u;
            return;
        }
    }
}

int
usb_submit_urb(void *pUrb, int nMemFlags)
{
    (void)pUrb;
    (void)nMemFlags;
    lusb_inc(&g_cSubmitUrb);
    if (g_fLogSubmit == 0) {
        g_fLogSubmit = 1;
        kprintf("linux_usb_soft: soft submit_urb n=%u ENODEV soft=1 product=0 "
                "(no HC; ≠ stick)\n",
                (unsigned)g_cSubmitUrb);
    }
    return LUSB_ENODEV;
}

int
usb_unlink_urb(void *pUrb)
{
    (void)pUrb;
    return LUSB_ENODEV;
}

void
usb_kill_urb(void *pUrb)
{
    (void)pUrb;
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
usb_free_coherent(void *pDev, unsigned long cbSize, void *pAddr, unsigned long dma)
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

/* ---- scsi_* soft bodies (load only; midlayer not product) -------------- */

/*
 * Attribute / host-class checks: fail closed (not a soft host device).
 * First reloc miss on el9 usb-storage.ko after generic leaf stubs.
 */
int
scsi_is_host_device(void *pDev)
{
    (void)pDev;
    return 0;
}

void *
scsi_host_alloc(void *pSht, int nPrivSize)
{
    (void)pSht;
    (void)nPrivSize;
    lusb_inc(&g_cScsiHostAlloc);
    lusb_soft_objs_once();
    /* Single soft host slot; second alloc fails closed. */
    if (g_fSoftHostLive != 0u) {
        return NULL;
    }
    memset(g_aSoftHost, 0, LINUX_USB_SOFT_HOST_CB);
    g_fSoftHostLive = 1u;
    return (void *)g_aSoftHost;
}

int
scsi_add_host_with_dma(void *pShost, void *pDev, void *pDmaDev)
{
    (void)pShost;
    (void)pDev;
    (void)pDmaDev;
    return LUSB_ENODEV;
}

void
scsi_scan_host(void *pShost)
{
    (void)pShost;
}

void
scsi_host_put(void *pShost)
{
    if (pShost == (void *)g_aSoftHost) {
        g_fSoftHostLive = 0u;
    }
}

void
scsi_remove_host(void *pShost)
{
    (void)pShost;
}

void
scsi_done_direct(void *pCmd)
{
    (void)pCmd;
}

void
scsi_report_bus_reset(void *pShost, int nChannel)
{
    (void)pShost;
    (void)nChannel;
}

void
scsi_report_device_reset(void *pShost, int nChannel, int nId)
{
    (void)pShost;
    (void)nChannel;
    (void)nId;
}

void
scsi_eh_prep_cmnd(void *pScmd, void *pSes, unsigned char *pCmnd,
                  int nCmndSize, unsigned int uSenseBytes)
{
    (void)pScmd;
    (void)pSes;
    (void)pCmnd;
    (void)nCmndSize;
    (void)uSenseBytes;
}

void
scsi_eh_restore_cmnd(void *pScmd, void *pSes)
{
    (void)pScmd;
    (void)pSes;
}

int
scsi_normalize_sense(const unsigned char *pSense, int nSenseLen, void *pSshdr)
{
    (void)pSense;
    (void)nSenseLen;
    (void)pSshdr;
    return 0;
}

void *
scsi_sense_desc_find(const unsigned char *pSense, int nSenseLen, int nDescType)
{
    (void)pSense;
    (void)nSenseLen;
    (void)nDescType;
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
    g_cScsiHostAlloc = 0u;
    g_fLogRegister = 0;
    g_fLogSubmit = 0;
    g_fSoftObjsInit = 0u;
    lusb_soft_objs_once();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Replaces any prior zero stubs for these names when present.
     * Weak linux_ksym_register: count as skip when F2 not linked yet.
     *
     * Stub budget: 20 usb_* + 13 scsi_* = 33 (cap 20–40). Soft≠product.
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

    /* scsi mid host / EH / sense (13) — load only */
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

    /* Grep: linux_usb_soft: soft init PASS n= */
    kprintf("linux_usb_soft: soft init PASS n=%u skip=%u stubs=%u "
            "usb=20 scsi=13 soft=1 product=0 (seed; ≠ stick; need=HC OPEN)\n",
            (unsigned)g_cKsymOk, (unsigned)g_cKsymSkip,
            (unsigned)g_cStubReg);
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
