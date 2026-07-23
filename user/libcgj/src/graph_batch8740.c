/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8740: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8740(void);
 *     - Returns the compile-time graph batch number for this TU (8740).
 *   uint32_t __gj_batch_id_8740  (alias)
 *   __libcgj_batch8740_marker = "libcgj-batch8740"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Soft pure-data catalog only; no mount/umount
 * syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8740_marker[] = "libcgj-batch8740";

#define B8740_BATCH_ID  8740u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8740_id(void)
{
	return B8740_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8740 - report this TU's graph batch number.
 *
 * Always returns 8740. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8740(void)
{
	(void)NULL;
	return b8740_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8740(void)
    __attribute__((alias("gj_batch_id_8740")));
