/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5403: install media detect (core).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_media_detect_5403(void);
 *     - Returns 0 (soft: media not auto-detected at compile time).
 *   uint32_t gj_install_media_path_ok_5403(const char *path);
 *     - Return 1 if path is absolute with length >= 2, else 0.
 *   uint32_t gj_install_media_kind_usb_5403(void);
 *     - Returns 1 (USB install-media kind tag).
 *   uint32_t gj_install_media_kind_iso_5403(void);
 *     - Returns 2 (ISO install-media kind tag).
 *   uint32_t __gj_install_media_detect_5403  (alias)
 *   uint32_t __gj_install_media_path_ok_5403  (alias)
 *   uint32_t __gj_install_media_kind_usb_5403  (alias)
 *   uint32_t __gj_install_media_kind_iso_5403  (alias)
 *   __libcgj_batch5403_marker = "libcgj-batch5403"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Install media detect
 * toward bar #3 Steam Deck install. Unique batch-suffixed symbols;
 * distinct from gj_install_media_ok (batch2638). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5403_marker[] = "libcgj-batch5403";

#define B5403_NOT_DETECTED  0u
#define B5403_KIND_USB      1u
#define B5403_KIND_ISO      2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5403_detect(void)
{
	return B5403_NOT_DETECTED;
}

/*
 * Soft absolute path shape: leading '/' and at least one more char.
 * Does not open devices or call libc.
 */
static uint32_t
b5403_path_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0u;
	}
	if (szPath[0] != '/') {
		return 0u;
	}
	if (szPath[1] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_media_detect_5403 - soft install-media detect lamp.
 *
 * Always returns 0 (not detected at compile time). Product-path tag
 * only; does not scan block devices. No parent wires.
 */
uint32_t
gj_install_media_detect_5403(void)
{
	(void)NULL;
	return b5403_detect();
}

/*
 * gj_install_media_path_ok_5403 - soft absolute install-media path check.
 *
 * path: NUL-terminated path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 */
uint32_t
gj_install_media_path_ok_5403(const char *szPath)
{
	return b5403_path_ok(szPath);
}

/*
 * gj_install_media_kind_usb_5403 - USB media kind tag.
 *
 * Always returns 1.
 */
uint32_t
gj_install_media_kind_usb_5403(void)
{
	return B5403_KIND_USB;
}

/*
 * gj_install_media_kind_iso_5403 - ISO media kind tag.
 *
 * Always returns 2.
 */
uint32_t
gj_install_media_kind_iso_5403(void)
{
	return B5403_KIND_ISO;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_media_detect_5403(void)
    __attribute__((alias("gj_install_media_detect_5403")));

uint32_t __gj_install_media_path_ok_5403(const char *szPath)
    __attribute__((alias("gj_install_media_path_ok_5403")));

uint32_t __gj_install_media_kind_usb_5403(void)
    __attribute__((alias("gj_install_media_kind_usb_5403")));

uint32_t __gj_install_media_kind_iso_5403(void)
    __attribute__((alias("gj_install_media_kind_iso_5403")));
