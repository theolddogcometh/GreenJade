/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7316: ioctl request dir has-WRITE stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_has_write_u_7316(uint32_t dir);
 *     - Return 1 if dir has _IOC_WRITE (bit 0 / value 1) set, else 0.
 *   uint32_t __gj_ioctl_has_write_u_7316  (alias)
 *   __libcgj_batch7316_marker = "libcgj-batch7316"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_has_write_u_7316 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7316_marker[] = "libcgj-batch7316";

/* _IOC_WRITE bit in the 2-bit direction class field. */
#define B7316_IOC_WRITE ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7316_ioctl_has_write(uint32_t u32Dir)
{
	return ((u32Dir & B7316_IOC_WRITE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_has_write_u_7316 - 1 if ioctl dir class includes WRITE.
 *
 * dir: soft _IOC direction field (0..3 typically)
 *
 * Returns 1 when bit value 1 is set (WRITE or READ|WRITE), else 0.
 * Soft bit test only; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_has_write_u_7316(uint32_t u32Dir)
{
	(void)NULL;
	return b7316_ioctl_has_write(u32Dir);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_has_write_u_7316(uint32_t u32Dir)
    __attribute__((alias("gj_ioctl_has_write_u_7316")));
