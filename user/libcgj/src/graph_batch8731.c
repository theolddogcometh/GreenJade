/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8731: MS_RDONLY mount flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_rdonly_id_8731(void);
 *     - Return soft MS_RDONLY mount flag bit (1).
 *   uint32_t __gj_ms_rdonly_id_8731  (alias)
 *   __libcgj_batch8731_marker = "libcgj-batch8731"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Unique gj_ms_rdonly_id_8731 surface only; no multi-def. Catalog id only;
 * no mount/umount syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8731_marker[] = "libcgj-batch8731";

/* Soft mount flag: MS_RDONLY (mount read-only). */
#define B8731_MS_RDONLY  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8731_rdonly_id(void)
{
	return B8731_MS_RDONLY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_rdonly_id_8731 - soft MS_RDONLY mount flag id.
 *
 * Always returns 1. Catalog id only; does not call mount. No parent
 * wires.
 */
uint32_t
gj_ms_rdonly_id_8731(void)
{
	(void)NULL;
	return b8731_rdonly_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_rdonly_id_8731(void)
    __attribute__((alias("gj_ms_rdonly_id_8731")));
