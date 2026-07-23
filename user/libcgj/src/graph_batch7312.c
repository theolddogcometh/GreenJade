/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7312: ioctl request dir WRITE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_write_id_7312(void);
 *     - Return soft _IOC_WRITE direction class (1).
 *   uint32_t __gj_ioctl_dir_write_id_7312  (alias)
 *   __libcgj_batch7312_marker = "libcgj-batch7312"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_write_id_7312 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7312_marker[] = "libcgj-batch7312";

/* _IOC_WRITE: userspace writes arg to kernel (Linux ioctl dir class). */
#define B7312_IOC_WRITE ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7312_ioctl_dir_write_id(void)
{
	return B7312_IOC_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_write_id_7312 - soft _IOC_WRITE direction class constant.
 *
 * Always returns 1. Catalog id only; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_dir_write_id_7312(void)
{
	(void)NULL;
	return b7312_ioctl_dir_write_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_write_id_7312(void)
    __attribute__((alias("gj_ioctl_dir_write_id_7312")));
