/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7315: ioctl request dir has-READ stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_has_read_u_7315(uint32_t dir);
 *     - Return 1 if dir has _IOC_READ (bit 1 / value 2) set, else 0.
 *   uint32_t __gj_ioctl_has_read_u_7315  (alias)
 *   __libcgj_batch7315_marker = "libcgj-batch7315"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_has_read_u_7315 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7315_marker[] = "libcgj-batch7315";

/* _IOC_READ bit in the 2-bit direction class field. */
#define B7315_IOC_READ ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7315_ioctl_has_read(uint32_t u32Dir)
{
	return ((u32Dir & B7315_IOC_READ) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_has_read_u_7315 - 1 if ioctl dir class includes READ.
 *
 * dir: soft _IOC direction field (0..3 typically)
 *
 * Returns 1 when bit value 2 is set (READ or READ|WRITE), else 0.
 * Soft bit test only; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_has_read_u_7315(uint32_t u32Dir)
{
	(void)NULL;
	return b7315_ioctl_has_read(u32Dir);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_has_read_u_7315(uint32_t u32Dir)
    __attribute__((alias("gj_ioctl_has_read_u_7315")));
