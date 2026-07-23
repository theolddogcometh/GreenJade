/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7314: ioctl request dir mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_mask_id_7314(void);
 *     - Return soft _IOC_DIRMASK (0x3) for 2-bit direction field.
 *   uint32_t __gj_ioctl_dir_mask_id_7314  (alias)
 *   __libcgj_batch7314_marker = "libcgj-batch7314"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_mask_id_7314 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7314_marker[] = "libcgj-batch7314";

/*
 * Soft 2-bit ioctl direction mask:
 *   _IOC_NONE(0) | _IOC_WRITE(1) | _IOC_READ(2) => dir field 0..3
 *   _IOC_DIRMASK = (1 << 2) - 1 = 0x3
 */
#define B7314_IOC_DIRMASK ((uint32_t)0x3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7314_ioctl_dir_mask_id(void)
{
	return B7314_IOC_DIRMASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_mask_id_7314 - soft _IOC_DIRMASK for ioctl dir class.
 *
 * Always returns 0x3. Catalog mask only; does not issue ioctl.
 * No parent wires.
 */
uint32_t
gj_ioctl_dir_mask_id_7314(void)
{
	(void)NULL;
	return b7314_ioctl_dir_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_mask_id_7314(void)
    __attribute__((alias("gj_ioctl_dir_mask_id_7314")));
