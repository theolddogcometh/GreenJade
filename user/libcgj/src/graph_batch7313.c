/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7313: ioctl request dir READ id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioctl_dir_read_id_7313(void);
 *     - Return soft _IOC_READ direction class (2).
 *   uint32_t __gj_ioctl_dir_read_id_7313  (alias)
 *   __libcgj_batch7313_marker = "libcgj-batch7313"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Unique gj_ioctl_dir_read_id_7313 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7313_marker[] = "libcgj-batch7313";

/* _IOC_READ: kernel writes arg to userspace (Linux ioctl dir class). */
#define B7313_IOC_READ ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7313_ioctl_dir_read_id(void)
{
	return B7313_IOC_READ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioctl_dir_read_id_7313 - soft _IOC_READ direction class constant.
 *
 * Always returns 2. Catalog id only; does not issue ioctl. No parent wires.
 */
uint32_t
gj_ioctl_dir_read_id_7313(void)
{
	(void)NULL;
	return b7313_ioctl_dir_read_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioctl_dir_read_id_7313(void)
    __attribute__((alias("gj_ioctl_dir_read_id_7313")));
