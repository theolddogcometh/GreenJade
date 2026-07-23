/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8720: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8720(void);
 *     - Returns the compile-time graph batch number for this TU (8720).
 *   uint32_t __gj_batch_id_8720  (alias)
 *   __libcgj_batch8720_marker = "libcgj-batch8720"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8720_marker[] = "libcgj-batch8720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8720_id(void)
{
	return 8720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8720 - report this TU's graph batch number.
 *
 * Always returns 8720. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8720(void)
{
	(void)NULL;
	return b8720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8720(void)
    __attribute__((alias("gj_batch_id_8720")));
