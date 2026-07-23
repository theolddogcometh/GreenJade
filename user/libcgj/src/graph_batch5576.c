/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5576: GreenJade OS image version minor stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_os_image_ver_minor_u(void);
 *     - Returns 0 (stub OS image minor version). Soft compile-time
 *       product tag for GreenJade install continuum.
 *   uint32_t __gj_os_image_ver_minor_u  (alias)
 *   __libcgj_batch5576_marker = "libcgj-batch5576"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique
 * gj_os_image_ver_minor_u surface only; no multi-def. Distinct from
 * gj_os_image_ver_major_u (batch5575). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5576_marker[] = "libcgj-batch5576";

/* Stub OS image minor version. */
#define B5576_OS_IMG_MINOR  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5576_os_img_minor(void)
{
	return B5576_OS_IMG_MINOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_os_image_ver_minor_u - report stub OS image minor version.
 *
 * Always returns 0. Soft pure-data product tag only; does not read
 * image metadata. Does not call libc. No parent wires.
 */
uint32_t
gj_os_image_ver_minor_u(void)
{
	(void)NULL;
	return b5576_os_img_minor();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_os_image_ver_minor_u(void)
    __attribute__((alias("gj_os_image_ver_minor_u")));
