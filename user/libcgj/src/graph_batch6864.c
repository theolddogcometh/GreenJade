/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6864: mount_attr extended flag constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_idmap_6864(void);
 *     - Returns 0x00100000 (soft MOUNT_ATTR_IDMAP).
 *   uint32_t gj_mount_attr_nosymfollow_6864(void);
 *     - Returns 0x00200000 (soft MOUNT_ATTR_NOSYMFOLLOW).
 *   uint32_t gj_mount_attr_ext_mask_6864(void);
 *     - Returns 0x00300000 (IDMAP|NOSYMFOLLOW).
 *   uint32_t __gj_mount_attr_*_6864  (aliases)
 *   __libcgj_batch6864_marker = "libcgj-batch6864"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_*_6864 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6864_marker[] = "libcgj-batch6864";

/* Soft MOUNT_ATTR extended flags (idmap / nosymfollow). */
#define B6864_IDMAP         0x00100000u
#define B6864_NOSYMFOLLOW   0x00200000u
#define B6864_EXT_MASK      (B6864_IDMAP | B6864_NOSYMFOLLOW)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6864_idmap(void)
{
	return B6864_IDMAP;
}

static uint32_t
b6864_nosymfollow(void)
{
	return B6864_NOSYMFOLLOW;
}

static uint32_t
b6864_ext_mask(void)
{
	return B6864_EXT_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_idmap_6864 - soft MOUNT_ATTR_IDMAP bit.
 *
 * Always returns 0x00100000. Soft pure-data flag tag; no mount_setattr
 * and no userns_fd plumbing. No parent wires.
 */
uint32_t
gj_mount_attr_idmap_6864(void)
{
	(void)NULL;
	return b6864_idmap();
}

/*
 * gj_mount_attr_nosymfollow_6864 - soft MOUNT_ATTR_NOSYMFOLLOW bit.
 *
 * Always returns 0x00200000.
 */
uint32_t
gj_mount_attr_nosymfollow_6864(void)
{
	return b6864_nosymfollow();
}

/*
 * gj_mount_attr_ext_mask_6864 - OR of IDMAP|NOSYMFOLLOW.
 *
 * Always returns 0x00300000.
 */
uint32_t
gj_mount_attr_ext_mask_6864(void)
{
	return b6864_ext_mask();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_idmap_6864(void)
    __attribute__((alias("gj_mount_attr_idmap_6864")));

uint32_t __gj_mount_attr_nosymfollow_6864(void)
    __attribute__((alias("gj_mount_attr_nosymfollow_6864")));

uint32_t __gj_mount_attr_ext_mask_6864(void)
    __attribute__((alias("gj_mount_attr_ext_mask_6864")));
