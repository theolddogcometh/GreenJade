/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6862: mount_attr basic flag constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_rdonly_6862(void);
 *     - Returns 0x1 (soft MOUNT_ATTR_RDONLY).
 *   uint32_t gj_mount_attr_nosuid_6862(void);
 *     - Returns 0x2 (soft MOUNT_ATTR_NOSUID).
 *   uint32_t gj_mount_attr_nodev_6862(void);
 *     - Returns 0x4 (soft MOUNT_ATTR_NODEV).
 *   uint32_t gj_mount_attr_noexec_6862(void);
 *     - Returns 0x8 (soft MOUNT_ATTR_NOEXEC).
 *   uint32_t gj_mount_attr_basic_mask_6862(void);
 *     - Returns 0xF (RDONLY|NOSUID|NODEV|NOEXEC).
 *   uint32_t __gj_mount_attr_*_6862  (aliases)
 *   __libcgj_batch6862_marker = "libcgj-batch6862"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_*_6862 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6862_marker[] = "libcgj-batch6862";

/* Soft MOUNT_ATTR_* basic protection flags (low nibble). */
#define B6862_RDONLY  0x00000001u
#define B6862_NOSUID  0x00000002u
#define B6862_NODEV   0x00000004u
#define B6862_NOEXEC  0x00000008u
#define B6862_BASIC   (B6862_RDONLY | B6862_NOSUID | B6862_NODEV | B6862_NOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6862_rdonly(void)
{
	return B6862_RDONLY;
}

static uint32_t
b6862_nosuid(void)
{
	return B6862_NOSUID;
}

static uint32_t
b6862_nodev(void)
{
	return B6862_NODEV;
}

static uint32_t
b6862_noexec(void)
{
	return B6862_NOEXEC;
}

static uint32_t
b6862_basic(void)
{
	return B6862_BASIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_rdonly_6862 - soft MOUNT_ATTR_RDONLY bit.
 *
 * Always returns 0x1. Soft pure-data flag tag; no mount_setattr call.
 */
uint32_t
gj_mount_attr_rdonly_6862(void)
{
	(void)NULL;
	return b6862_rdonly();
}

/*
 * gj_mount_attr_nosuid_6862 - soft MOUNT_ATTR_NOSUID bit.
 *
 * Always returns 0x2.
 */
uint32_t
gj_mount_attr_nosuid_6862(void)
{
	return b6862_nosuid();
}

/*
 * gj_mount_attr_nodev_6862 - soft MOUNT_ATTR_NODEV bit.
 *
 * Always returns 0x4.
 */
uint32_t
gj_mount_attr_nodev_6862(void)
{
	return b6862_nodev();
}

/*
 * gj_mount_attr_noexec_6862 - soft MOUNT_ATTR_NOEXEC bit.
 *
 * Always returns 0x8.
 */
uint32_t
gj_mount_attr_noexec_6862(void)
{
	return b6862_noexec();
}

/*
 * gj_mount_attr_basic_mask_6862 - OR of RDONLY|NOSUID|NODEV|NOEXEC.
 *
 * Always returns 0xF. Soft baseline basic protection mask.
 */
uint32_t
gj_mount_attr_basic_mask_6862(void)
{
	return b6862_basic();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_rdonly_6862(void)
    __attribute__((alias("gj_mount_attr_rdonly_6862")));

uint32_t __gj_mount_attr_nosuid_6862(void)
    __attribute__((alias("gj_mount_attr_nosuid_6862")));

uint32_t __gj_mount_attr_nodev_6862(void)
    __attribute__((alias("gj_mount_attr_nodev_6862")));

uint32_t __gj_mount_attr_noexec_6862(void)
    __attribute__((alias("gj_mount_attr_noexec_6862")));

uint32_t __gj_mount_attr_basic_mask_6862(void)
    __attribute__((alias("gj_mount_attr_basic_mask_6862")));
