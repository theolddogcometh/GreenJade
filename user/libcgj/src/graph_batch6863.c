/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6863: mount_attr atime-family flag constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_atime_mask_6863(void);
 *     - Returns 0x70 (soft MOUNT_ATTR__ATIME field mask).
 *   uint32_t gj_mount_attr_noatime_6863(void);
 *     - Returns 0x10 (soft MOUNT_ATTR_NOATIME).
 *   uint32_t gj_mount_attr_strictatime_6863(void);
 *     - Returns 0x20 (soft MOUNT_ATTR_STRICTATIME).
 *   uint32_t gj_mount_attr_nodiratime_6863(void);
 *     - Returns 0x80 (soft MOUNT_ATTR_NODIRATIME).
 *   uint32_t __gj_mount_attr_*_6863  (aliases)
 *   __libcgj_batch6863_marker = "libcgj-batch6863"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_*_6863 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6863_marker[] = "libcgj-batch6863";

/* Soft MOUNT_ATTR atime-family flags. RELATIME is 0 (default). */
#define B6863_ATIME_MASK    0x00000070u
#define B6863_NOATIME       0x00000010u
#define B6863_STRICTATIME   0x00000020u
#define B6863_NODIRATIME    0x00000080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6863_atime_mask(void)
{
	return B6863_ATIME_MASK;
}

static uint32_t
b6863_noatime(void)
{
	return B6863_NOATIME;
}

static uint32_t
b6863_strictatime(void)
{
	return B6863_STRICTATIME;
}

static uint32_t
b6863_nodiratime(void)
{
	return B6863_NODIRATIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_atime_mask_6863 - soft MOUNT_ATTR__ATIME field mask.
 *
 * Always returns 0x70. Soft pure-data tag; no mount_setattr call.
 */
uint32_t
gj_mount_attr_atime_mask_6863(void)
{
	(void)NULL;
	return b6863_atime_mask();
}

/*
 * gj_mount_attr_noatime_6863 - soft MOUNT_ATTR_NOATIME bit.
 *
 * Always returns 0x10.
 */
uint32_t
gj_mount_attr_noatime_6863(void)
{
	return b6863_noatime();
}

/*
 * gj_mount_attr_strictatime_6863 - soft MOUNT_ATTR_STRICTATIME bit.
 *
 * Always returns 0x20.
 */
uint32_t
gj_mount_attr_strictatime_6863(void)
{
	return b6863_strictatime();
}

/*
 * gj_mount_attr_nodiratime_6863 - soft MOUNT_ATTR_NODIRATIME bit.
 *
 * Always returns 0x80.
 */
uint32_t
gj_mount_attr_nodiratime_6863(void)
{
	return b6863_nodiratime();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_atime_mask_6863(void)
    __attribute__((alias("gj_mount_attr_atime_mask_6863")));

uint32_t __gj_mount_attr_noatime_6863(void)
    __attribute__((alias("gj_mount_attr_noatime_6863")));

uint32_t __gj_mount_attr_strictatime_6863(void)
    __attribute__((alias("gj_mount_attr_strictatime_6863")));

uint32_t __gj_mount_attr_nodiratime_6863(void)
    __attribute__((alias("gj_mount_attr_nodiratime_6863")));
