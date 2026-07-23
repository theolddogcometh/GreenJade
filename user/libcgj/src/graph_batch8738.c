/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8738: MNT_FORCE umount flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umount_force_id_8738(void);
 *     - Return soft MNT_FORCE umount flag bit (1).
 *   uint32_t __gj_umount_force_id_8738  (alias)
 *   __libcgj_batch8738_marker = "libcgj-batch8738"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Unique gj_umount_force_id_8738 surface only; no multi-def. Catalog id only;
 * no mount/umount syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8738_marker[] = "libcgj-batch8738";

/* Soft umount flag: MNT_FORCE (attempt to forcibly umount). */
#define B8738_MNT_FORCE  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8738_force_id(void)
{
	return B8738_MNT_FORCE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umount_force_id_8738 - soft MNT_FORCE umount flag id.
 *
 * Always returns 1. Catalog id only; does not call umount/umount2. No
 * parent wires.
 */
uint32_t
gj_umount_force_id_8738(void)
{
	(void)NULL;
	return b8738_force_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umount_force_id_8738(void)
    __attribute__((alias("gj_umount_force_id_8738")));
