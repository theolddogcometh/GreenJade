/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7311: ioctl request dir NONE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_none_id_7311(void);
 *     - Return soft _IOC_NONE direction class (0).
 *   uint32_t __gj_ioctl_dir_none_id_7311  (alias)
 *   __libcgj_batch7311_marker = "libcgj-batch7311"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_none_id_7311 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7311_marker[] = "libcgj-batch7311";

/* _IOC_NONE: no userspace transfer (Linux ioctl dir class). */
#define B7311_IOC_NONE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7311_ioctl_dir_none_id(void)
{
	return B7311_IOC_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_none_id_7311 - soft _IOC_NONE direction class constant.
 *
 * Always returns 0. Catalog id only; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_dir_none_id_7311(void)
{
	(void)NULL;
	return b7311_ioctl_dir_none_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_none_id_7311(void)
    __attribute__((alias("gj_ioctl_dir_none_id_7311")));
