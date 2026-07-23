/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7318: ioctl request dir pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_pack_u_7318(uint32_t want_read,
 *                                     uint32_t want_write);
 *     - Pack soft READ/WRITE wants into a 2-bit dir class (0..3).
 *   uint32_t __gj_ioctl_dir_pack_u_7318  (alias)
 *   __libcgj_batch7318_marker = "libcgj-batch7318"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_pack_u_7318 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7318_marker[] = "libcgj-batch7318";

/* Soft pack bits matching _IOC_WRITE / _IOC_READ layout. */
#define B7318_IOC_WRITE ((uint32_t)1u)
#define B7318_IOC_READ  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7318_ioctl_dir_pack(uint32_t u32WantRead, uint32_t u32WantWrite)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantWrite != 0u)
		u32Pack |= B7318_IOC_WRITE;
	if (u32WantRead != 0u)
		u32Pack |= B7318_IOC_READ;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_pack_u_7318 - soft-pack ioctl dir class wants.
 *
 * want_read:  non-zero to set _IOC_READ (2)
 * want_write: non-zero to set _IOC_WRITE (1)
 *
 * Returns bitmask of requested dir bits (0=NONE, 1=W, 2=R, 3=RW).
 * Pure integer pack; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_dir_pack_u_7318(uint32_t u32WantRead, uint32_t u32WantWrite)
{
	(void)NULL;
	return b7318_ioctl_dir_pack(u32WantRead, u32WantWrite);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_pack_u_7318(uint32_t u32WantRead,
                                    uint32_t u32WantWrite)
    __attribute__((alias("gj_ioctl_dir_pack_u_7318")));
