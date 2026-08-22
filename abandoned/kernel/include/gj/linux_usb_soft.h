/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft USB core + SCSI mid (scsi_mod surface) + MSC leaf ksym seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Fail-closed / no-op soft bodies so a soft-loaded usb-storage.ko (and
 * uas.ko) can resolve usb_* / scsi_* / sg_* SHN_UNDEF against the ksym
 * surface (reloc load progress past FAIL KSYM). Complements F2 linux_ksym
 * empty generics (leaf ~15); this TU owns usbcore-shaped + scsi_mod-shaped
 * exports (incl. virtual HCD/hub/device surface), scatterlist residual,
 * and MSC-adjacent wait/work/kthread/wq.
 *
 * Soft layout (DoD A residual · UDX direction)
 * --------------------------------------------
 * Incomplete usb_soft_urb / usb_soft_hcd / usb_soft_udev front fields live
 * here so soft fill/submit/giveback and later userspace UDX hosts share a
 * greppable clean-room shape. Soft!=ABI-stable - NOT a Linux kver layout;
 * do not pass these blobs to real .ko probe. Product USB path is userspace
 * UDX (xhci_udx / DDI), not in-kernel usb_storage.ko init (G-AC-1).
 * Freestanding xHCI residual is SKIP by default (GJ_XHCI_MSC_PROBE=0);
 * this seed is the interim ksym/layout surface only.
 *
 * Soft != product · G-AC-1
 * -----------------------
 * No real host controller, no BOT/UAS datapath, no stick write.
 * INIT=0 without real HC still != stick. Host xHCI often BUILTIN -> no
 * usbcore.ko to multi-mod; soft seed is the interim ksym surface only.
 * Virtual soft deps (usb_common/usbcore/scsi_mod) satisfy eng deps_ready
 * only - loaded(canon)=0; Soft != product AC. scsi_* deepen is load-only
 * soft midlayer - not product scsi_mid/virtio.
 * Residual UND (D residual r1+r2+r3+r4+r5+r6+r7+r8): fail-closed
 * usbcore/scsi/sg stubs only - Soft!=product; never claim stick write.
 * residual3 = register/sysfs/evt + buffer_map/dynid + append-sg;
 * residual4 = target/queue/abort + sg page + hcd/hub/ep sysfs;
 * residual5 = queue/EH abort + resid + sg pool/set + hcd start/bus/runtime;
 * residual6 = cmnd/rq/mq + EH tur + sg chain/last + hcd pci PM/runtime;
 * residual7 = hcd buffer/platform + hub TT/port + phy + scsi host recovery
 * + sg page-iter; residual8 = soft layout HCD/URB bridge (hcd↔bus +
 * intfdata + make_path + anchor init) for UDX hosts - load/reloc surface
 * only; != stick; freestanding_no_exec;
 * residual9 = HCD/URB layout residual deepen (hcd↔dev + free_dev +
 * bandwidth check/release + root-hub query + host_interface +
 * urb ep/hcd/dev reverse cookies) for userspace UDX direction -
 * Soft!=product; freestanding MSC SKIP (GJ_XHCI_MSC_PROBE=0);
 * freestanding_no_exec;
 * residual10 = soft EP pool + URB↔EP↔udev↔hcd reverse cookies
 * (pipe_endpoint soft slab, actual_len/bus_name/wire_dev) lean UDX
 * seed - Soft!=product; freestanding_no_exec; no .ko exec (G-AC-1);
 * residual11 = lean DoD A UDX residual: EP enable/maxp bookkeep +
 * URB transfer/pipe/context reverse + soft pipe encode + EP-HCD
 * reverse - Soft!=product; freestanding_no_exec; no .ko exec (G-AC-1);
 * residual12 = lean Dual DoD A eng seed: EP addr/type wire + URB
 * setup/complete/context/transfer setters + soft pipe decode reverse
 * for userspace xhci_udx - Soft!=product; freestanding MSC SKIP
 * (GJ_XHCI_MSC_PROBE=0); freestanding_no_exec; no .ko exec (G-AC-1);
 * residual13 = lean Dual DoD A eng residual: URB setup/complete/pipe/
 * status reverse setters + EP interval reverse + URB xfer_type reverse
 * + soft pipe_from_ep + soft residual lean lamp (freestanding_msc=SKIP
 * product_usb=UDX RUN_INIT=0 freestanding_no_exec) for userspace
 * xhci_udx eng - Soft!=product; no freestanding thrash; freestanding MSC
 * SKIP (GJ_XHCI_MSC_PROBE=0); freestanding_no_exec; no .ko exec (G-AC-1);
 * residual14 = lean Dual DoD A eng residual deepen: URB dev/hcd/xfer_type/
 * interval reverse + linked flag + actual_len eng bookkeep + soft
 * wire_urb_ep for userspace xhci_udx eng - Soft!=product; no freestanding
 * thrash; freestanding MSC SKIP (GJ_XHCI_MSC_PROBE=0); freestanding_no_exec;
 * no .ko exec (G-AC-1).
 * Grep lamps: soft=1 product=0; need=HC OPEN; != stick; Soft!=product.
 *
 * Soft != ABI-stable
 * -----------------
 * Soft object blobs are incomplete shapes. Field order/size are NOT
 * guaranteed to match any Linux kernel version.
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
 *   linux_usb_soft: soft residual UND ... Soft!=product
 *   linux_usb_soft: soft residual UND PASS n=
 *   linux_usb_soft: soft dep virtual ...
 *   linux_usb_soft: soft DoD A seed ...
 *   linux_usb_soft: soft residual lean PASS Soft!=product ...
 *   linux_usb_soft: soft residual freestanding_msc=SKIP ...
 *   linux_usb_soft: soft residual product_usb=UDX ...
 *   linux_usb_soft: soft residual RUN_INIT=0 freestanding_no_exec ...
 *   linux_module: soft dep virtual usbcore|scsi_mod|usb_common soft=1 product=0
 *     (seed -> virtual slot <canon>-soft; deps_ready eng PASS; != real .ko)
 * Soft!=product · G-AC-1 · never claim stick write · INIT=0 != stick.
 * Soft residual lean: Dual DoD A eng seed for userspace UDX path only;
 * freestanding MSC SKIP; product_usb=UDX; RUN_INIT=0 freestanding_no_exec.
 *
 * See docs/LINUX_MODULE_PATH.md §D8 · docs/TODO.md dual DoD A.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Soft object sizes (opaque blobs; not Linux struct layouts). */
#define LINUX_USB_SOFT_URB_CB     256u
/* Soft Scsi_Host-shaped slab + hostdata priv (el9 usb-storage us_data). */
#define LINUX_USB_SOFT_HOST_CB    2048u
/* Soft usb_hcd-shaped slab (virtual usbcore; Soft!=ABI-stable). */
#define LINUX_USB_SOFT_HCD_CB     2048u
#define LINUX_USB_SOFT_HCD_POOL   2u
/* Soft usb_device / bus blobs (virtual; never enumerate real stick). */
#define LINUX_USB_SOFT_UDEV_CB    512u
#define LINUX_USB_SOFT_BUS_CB     256u
#define LINUX_USB_SOFT_URB_POOL   8u
/* Soft endpoint pool (residual10 UDX cookies; Soft!=descriptor walk). */
#define LINUX_USB_SOFT_EP_CB      64u
#define LINUX_USB_SOFT_EP_POOL    8u
#define LINUX_USB_SOFT_DATA_CB    64u
#define LINUX_USB_SOFT_ANCHOR_CB  64u
/* Soft EH session + fixed sense hdr scratch (scsi_eh_* / normalize). */
#define LINUX_USB_SOFT_EH_CB      128u
#define LINUX_USB_SOFT_SSHDR_CB   32u
#define LINUX_USB_SOFT_CMND_CB    16u

/* Soft layout magics (little-endian fourCC; not Linux fields). */
#define LINUX_USB_SOFT_URB_MAGIC  0x42525553u /* 'SURB' */
#define LINUX_USB_SOFT_HCD_MAGIC  0x44434853u /* 'SHCD' */
#define LINUX_USB_SOFT_UDEV_MAGIC 0x56445553u /* 'SUDV' */
#define LINUX_USB_SOFT_BUS_MAGIC  0x53425553u /* 'SUBS' */
#define LINUX_USB_SOFT_EP_MAGIC   0x54504553u /* 'SEPT' */

/* Soft transfer type bookkeeping (not Linux urb->pipe encoding). */
#define LINUX_USB_SOFT_XFER_CTRL  0u
#define LINUX_USB_SOFT_XFER_BULK  1u
#define LINUX_USB_SOFT_XFER_INT   2u
#define LINUX_USB_SOFT_XFER_ISO   3u

/* Soft HCD flags (usb_soft_hcd.u32Flags). Soft!=product. */
#define LINUX_USB_SOFT_HCD_F_ADDED   (1u << 0)
#define LINUX_USB_SOFT_HCD_F_PRIMARY (1u << 1)
#define LINUX_USB_SOFT_HCD_F_DIED    (1u << 2)
#define LINUX_USB_SOFT_HCD_F_STARTED (1u << 3)
/* residual9: soft bandwidth residual marked (never grants real BW). */
#define LINUX_USB_SOFT_HCD_F_BW_SOFT (1u << 4)

/**
 * Soft URB front fields (Linux urb *shape*, not layout-stable).
 *
 * fill_bulk/control/int + submit/giveback write these when the pointer is a
 * soft pool slot. Remaining bytes of the 256-byte pool entry are zero pad.
 * Soft!=ABI-stable · Soft!=product · usable by later UDX hosts as a cookie.
 * residual9: u8EpNum / u8Linked / u32Seq deepen UDX-facing layout residual.
 */
struct usb_soft_urb {
    u32          u32Magic;       /* LINUX_USB_SOFT_URB_MAGIC */
    int          nStatus;        /* soft completion status (neg errno) */
    void        *pDev;           /* soft udev / device cookie */
    void        *pEp;            /* soft endpoint cookie */
    unsigned int uPipe;          /* soft pipe encoding echo */
    unsigned int uPadAlign;      /* keep 8-byte alignment on x86_64 */
    void        *pTransfer;      /* transfer buffer (no DMA map) */
    u32          u32TransferLen; /* requested length */
    u32          u32ActualLen;   /* always 0 under soft fail-closed */
    void        *pfnComplete;    /* completion cookie; soft never calls */
    void        *pContext;
    void        *pSetup;         /* control setup packet cookie */
    int          nInterval;      /* interrupt interval echo */
    u8           u8Live;
    u8           u8Submitted;
    u8           u8Poisoned;
    u8           u8Anchored;
    u8           u8XferType;     /* LINUX_USB_SOFT_XFER_* */
    u8           u8DirIn;        /* 1 if pipe/dir IN-shaped */
    u8           u8Unlinked;
    u8           u8EpNum;        /* soft pipe ep# echo (residual9) */
    u8           u8Linked;       /* 1 if soft-linked to ep/HCD */
    u8           u8Pad[3];
    u32          u32Seq;         /* soft submit/link sequence (eng) */
    void        *pAnchor;
    void        *pHcd;           /* last soft HCD that touched this URB */
};

/**
 * Soft HCD front fields (Linux usb_hcd *shape*, not layout-stable).
 * Soft pool slots are LINUX_USB_SOFT_HCD_CB bytes; only the front is typed.
 * Soft!=product - no MMIO, no root-hub enum, no stick.
 * residual9: u32UrbLinked + pRootHub (always NULL soft) for UDX cookies.
 */
struct usb_soft_hcd {
    u32   u32Magic;       /* LINUX_USB_SOFT_HCD_MAGIC */
    u32   u32Flags;       /* LINUX_USB_SOFT_HCD_F_* */
    void *pDriver;        /* hc_driver cookie */
    void *pController;    /* controller device cookie */
    void *pShared;        /* primary HCD when shared */
    void *pBus;           /* soft bus cookie */
    void *pRootHub;      /* always NULL under soft seed (!= stick) */
    char  szBusName[16];
    u8    u8Used;
    u8    u8Added;
    u8    u8Primary;
    u8    u8Died;
    u8    u8Started;
    u8    u8RhRegistered;
    u8    u8Pad[2];
    u32   u32Irq;
    u32   u32Frame;
    u32   u32UrbLinked;   /* soft link count (eng residual; != queue) */
};

/**
 * Soft usb_device front fields (virtual; never real stick enum).
 * Soft!=ABI-stable · Soft!=product.
 */
struct usb_soft_udev {
    u32          u32Magic; /* LINUX_USB_SOFT_UDEV_MAGIC */
    u32          u32State; /* soft state echo only */
    void        *pParent;
    void        *pBus;
    void        *pHcd;
    unsigned int uPort1;
    u8           u8Live;
    u8           u8Authorized;
    u8           u8Lpm;
    u8           u8Pad;
    u16          u16Vendor;
    u16          u16Product;
};

/**
 * Soft usb_bus front fields (virtual bus; Soft!=sysfs).
 */
struct usb_soft_bus {
    u32   u32Magic; /* LINUX_USB_SOFT_BUS_MAGIC */
    u32   u32BusNum;
    void *pHcd;
    void *pRootHub; /* always NULL under soft seed */
    u8    u8Live;
    u8    u8Pad[3];
};

/**
 * Soft endpoint front fields (residual10 UDX cookies; Soft!=descriptor).
 * Pool slots are LINUX_USB_SOFT_EP_CB; only the front is typed.
 * Soft!=ABI-stable · Soft!=product · never real stick EP enum.
 */
struct usb_soft_ep {
    u32   u32Magic;      /* LINUX_USB_SOFT_EP_MAGIC */
    void *pUdev;         /* soft udev cookie */
    void *pHcd;          /* soft HCD cookie when wired */
    u8    u8Addr;        /* address incl. dir bit (0x80 IN) */
    u8    u8Num;         /* endpoint number 0..15 */
    u8    u8Type;        /* LINUX_USB_SOFT_XFER_* */
    u8    u8DirIn;       /* 1 if IN-shaped */
    u8    u8Enabled;     /* soft enable cookie only */
    u8    u8Live;
    u16   u16MaxPacket;  /* soft bookkeep; 0 = fail-closed */
    u16   u16Interval;   /* soft interval echo */
};

/**
 * Soft init: idempotent ready lamp + linux_ksym_register of soft bodies.
 * Prefer call after linux_ksym_init(); weak-call safe if F2 absent.
 * Grep: linux_usb_soft: soft init PASS n=
 */
void linux_usb_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_usb_soft_ready(void);

/** Soft diagnostics. */
u32  linux_usb_soft_ksym_ok(void);
u32  linux_usb_soft_ksym_skip(void);
u32  linux_usb_soft_stub_count(void);

/**
 * Soft layout accessors for UDX / eng hosts (Soft!=ABI-stable).
 * Return non-NULL / status only when pointer is a soft pool object.
 * Never claim stick datapath. freestanding_no_exec · G-AC-1.
 * residual9: bus cast + urb xfer/ep + hcd controller cookies for UDX.
 * residual10: soft EP cast + actual_len/dir + udev->hcd + urb->ep.
 * residual11: EP maxp/enabled/hcd + URB transfer_len reverse for UDX.
 * residual12: EP num/addr/type/dir + URB setup/complete reverse for UDX.
 * residual13: EP interval + URB context/seq reverse + lean lamp for UDX.
 * residual14: URB dev/hcd/interval/linked reverse + wire_urb_ep for UDX.
 */
struct usb_soft_urb *linux_usb_soft_urb_cast(void *pUrb);
struct usb_soft_hcd *linux_usb_soft_hcd_cast(void *pHcd);
struct usb_soft_udev *linux_usb_soft_udev_cast(void *pUdev);
struct usb_soft_bus *linux_usb_soft_bus_cast(void *pBus);
struct usb_soft_ep *linux_usb_soft_ep_cast(void *pEp);
int  linux_usb_soft_urb_status(void *pUrb);
u32  linux_usb_soft_hcd_flags(void *pHcd);
u8   linux_usb_soft_urb_xfer_type(void *pUrb);
u8   linux_usb_soft_urb_ep_num(void *pUrb);
u8   linux_usb_soft_urb_dir_in(void *pUrb);
u32  linux_usb_soft_urb_actual_len(void *pUrb);
u32  linux_usb_soft_urb_transfer_len(void *pUrb);
void *linux_usb_soft_hcd_controller(void *pHcd);
void *linux_usb_soft_udev_hcd(void *pUdev);
void *linux_usb_soft_urb_ep(void *pUrb);
void *linux_usb_soft_ep_hcd(void *pEp);
u16  linux_usb_soft_ep_maxpacket(void *pEp);
u8   linux_usb_soft_ep_enabled(void *pEp);
u8   linux_usb_soft_ep_num(void *pEp);
u8   linux_usb_soft_ep_addr(void *pEp);
u8   linux_usb_soft_ep_type(void *pEp);
u8   linux_usb_soft_ep_dir_in(void *pEp);
void *linux_usb_soft_urb_setup(void *pUrb);
void *linux_usb_soft_urb_complete(void *pUrb);
void *linux_usb_soft_urb_context(void *pUrb);
u16  linux_usb_soft_ep_interval(void *pEp);
u32  linux_usb_soft_urb_seq(void *pUrb);
void *linux_usb_soft_urb_dev(void *pUrb);
void *linux_usb_soft_urb_hcd(void *pUrb);
int  linux_usb_soft_urb_interval(void *pUrb);
u8   linux_usb_soft_urb_linked(void *pUrb);

/**
 * Soft residual lean lamp (once-capped). Dual DoD A eng residual honesty.
 * Grep: soft residual lean PASS · freestanding_msc=SKIP · product_usb=UDX
 * · RUN_INIT=0 freestanding_no_exec. Soft!=product · G-AC-1 · != stick.
 * Returns 1 if lamp emitted, 0 if already lit or not ready.
 */
int  linux_usb_soft_residual_lean_lamp_once(void);

#ifdef __cplusplus
}
#endif
