/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft USB core + SCSI mid ksym seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Fail-closed / no-op soft bodies so a soft-loaded usb-storage.ko can
 * resolve usb_* / scsi_* SHN_UNDEF against the ksym surface (reloc load
 * progress past first usb/scsi miss). Complements F2 linux_ksym empty
 * generics; this TU owns USB register/URB + SCSI host seed symbols.
 *
 * Soft ≠ product
 * --------------
 * No real host controller, no BOT/UAS datapath, no stick write.
 * INIT=0 without real HC still ≠ stick. Host xHCI often BUILTIN → no
 * usbcore.ko to multi-mod; soft seed is the interim ksym surface only.
 *
 * Soft ≠ ABI-stable
 * -----------------
 * Soft object blobs are incomplete shapes. Field order/size are NOT
 * guaranteed to match any Linux kernel version.
 *
 * Greppable markers (keep stable):
 *   linux_usb_soft: soft init PASS n=
 *   linux_usb_soft: soft usb_register …
 *   linux_usb_soft: soft submit_urb …
 *
 * See docs/LINUX_MODULE_PATH.md §D8.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Soft object sizes (opaque blobs; not Linux struct layouts). */
#define LINUX_USB_SOFT_URB_CB     256u
#define LINUX_USB_SOFT_HOST_CB    512u
#define LINUX_USB_SOFT_URB_POOL   4u

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

#ifdef __cplusplus
}
#endif
