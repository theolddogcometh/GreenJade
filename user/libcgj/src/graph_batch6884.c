/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6884: statx mask STATX_UID|STATX_GID soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_uid_6884(void);
 *     - Returns soft STATX_UID bit (0x00000008).
 *   uint32_t gj_statx_mask_gid_6884(void);
 *     - Returns soft STATX_GID bit (0x00000010).
 *   uint32_t gj_statx_mask_uid_gid_6884(void);
 *     - Returns soft STATX_UID|STATX_GID (0x00000018).
 *   uint32_t __gj_statx_mask_uid_6884  (alias)
 *   uint32_t __gj_statx_mask_gid_6884  (alias)
 *   uint32_t __gj_statx_mask_uid_gid_6884  (alias)
 *   __libcgj_batch6884_marker = "libcgj-batch6884"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6884 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6884_marker[] = "libcgj-batch6884";

/* Soft Linux STATX_UID / STATX_GID (stx_uid / stx_gid present). */
#define B6884_STATX_UID  0x00000008u
#define B6884_STATX_GID  0x00000010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6884_uid(void)
{
	return B6884_STATX_UID;
}

static uint32_t
b6884_gid(void)
{
	return B6884_STATX_GID;
}

static uint32_t
b6884_uid_gid(void)
{
	return B6884_STATX_UID | B6884_STATX_GID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_uid_6884 - soft STATX_UID request bit.
 *
 * Always returns 0x00000008. Soft pure-data product tag; no statx.
 */
uint32_t
gj_statx_mask_uid_6884(void)
{
	(void)NULL;
	return b6884_uid();
}

/*
 * gj_statx_mask_gid_6884 - soft STATX_GID request bit.
 *
 * Always returns 0x00000010. Soft pure-data product tag; no statx.
 */
uint32_t
gj_statx_mask_gid_6884(void)
{
	return b6884_gid();
}

/*
 * gj_statx_mask_uid_gid_6884 - soft STATX_UID|STATX_GID merge.
 *
 * Always returns 0x00000018. Combined ownership mask request bits.
 */
uint32_t
gj_statx_mask_uid_gid_6884(void)
{
	return b6884_uid_gid();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_uid_6884(void)
    __attribute__((alias("gj_statx_mask_uid_6884")));

uint32_t __gj_statx_mask_gid_6884(void)
    __attribute__((alias("gj_statx_mask_gid_6884")));

uint32_t __gj_statx_mask_uid_gid_6884(void)
    __attribute__((alias("gj_statx_mask_uid_gid_6884")));
