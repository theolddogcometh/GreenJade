/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7317: ioctl request dir validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_ok_u_7317(uint32_t dir);
 *     - Return 1 if dir uses only the 2-bit _IOC_DIRMASK (0..3), else 0.
 *   uint32_t __gj_ioctl_dir_ok_u_7317  (alias)
 *   __libcgj_batch7317_marker = "libcgj-batch7317"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_ok_u_7317 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7317_marker[] = "libcgj-batch7317";

/* Soft 2-bit direction class mask (NONE/WRITE/READ/RDWR). */
#define B7317_IOC_DIRMASK ((uint32_t)0x3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7317_ioctl_dir_ok(uint32_t u32Dir)
{
	if ((u32Dir & ~B7317_IOC_DIRMASK) != 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_ok_u_7317 - 1 if dir is a valid soft ioctl dir class.
 *
 * dir: soft _IOC direction field
 *
 * Returns 1 when only bits in {0x1, 0x2} may be set (values 0..3),
 * else 0. Soft catalog check; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_dir_ok_u_7317(uint32_t u32Dir)
{
	(void)NULL;
	return b7317_ioctl_dir_ok(u32Dir);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_ok_u_7317(uint32_t u32Dir)
    __attribute__((alias("gj_ioctl_dir_ok_u_7317")));
