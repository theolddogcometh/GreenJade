/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5575: GreenJade OS image version major stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_os_image_ver_major_u(void);
 *     - Returns 1 (stub OS image major version). Soft compile-time
 *       product tag for GreenJade install continuum.
 *   uint32_t __gj_os_image_ver_major_u  (alias)
 *   __libcgj_batch5575_marker = "libcgj-batch5575"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique
 * gj_os_image_ver_major_u surface only; no multi-def. Distinct from
 * gj_os_image_ver_minor_u (batch5576). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5575_marker[] = "libcgj-batch5575";

/* Stub OS image major version. */
#define B5575_OS_IMG_MAJOR  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5575_os_img_major(void)
{
	return B5575_OS_IMG_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_os_image_ver_major_u - report stub OS image major version.
 *
 * Always returns 1. Soft pure-data product tag only; does not read
 * /etc/os-release or image metadata. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_os_image_ver_major_u(void)
{
	(void)NULL;
	return b5575_os_img_major();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_os_image_ver_major_u(void)
    __attribute__((alias("gj_os_image_ver_major_u")));
