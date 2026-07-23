/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8735: MS_REMOUNT mount flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_remount_id_8735(void);
 *     - Return soft MS_REMOUNT mount flag bit (32).
 *   uint32_t __gj_ms_remount_id_8735  (alias)
 *   __libcgj_batch8735_marker = "libcgj-batch8735"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Unique gj_ms_remount_id_8735 surface only; no multi-def. Catalog id only;
 * no mount/umount syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8735_marker[] = "libcgj-batch8735";

/* Soft mount flag: MS_REMOUNT (alter flags of a mounted FS). */
#define B8735_MS_REMOUNT  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8735_remount_id(void)
{
	return B8735_MS_REMOUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_remount_id_8735 - soft MS_REMOUNT mount flag id.
 *
 * Always returns 32. Catalog id only; does not call mount. No parent
 * wires.
 */
uint32_t
gj_ms_remount_id_8735(void)
{
	(void)NULL;
	return b8735_remount_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_remount_id_8735(void)
    __attribute__((alias("gj_ms_remount_id_8735")));
