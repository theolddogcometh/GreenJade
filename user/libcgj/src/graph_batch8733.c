/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8733: MS_NODEV mount flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_nodev_id_8733(void);
 *     - Return soft MS_NODEV mount flag bit (4).
 *   uint32_t __gj_ms_nodev_id_8733  (alias)
 *   __libcgj_batch8733_marker = "libcgj-batch8733"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Unique gj_ms_nodev_id_8733 surface only; no multi-def. Catalog id only;
 * no mount/umount syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8733_marker[] = "libcgj-batch8733";

/* Soft mount flag: MS_NODEV (disallow device special files). */
#define B8733_MS_NODEV  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8733_nodev_id(void)
{
	return B8733_MS_NODEV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_nodev_id_8733 - soft MS_NODEV mount flag id.
 *
 * Always returns 4. Catalog id only; does not call mount. No parent
 * wires.
 */
uint32_t
gj_ms_nodev_id_8733(void)
{
	(void)NULL;
	return b8733_nodev_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_nodev_id_8733(void)
    __attribute__((alias("gj_ms_nodev_id_8733")));
