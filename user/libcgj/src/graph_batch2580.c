/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2580: milestone 2580 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2580(void);
 *     - Returns the compile-time graph batch number for this TU (2580).
 *   uint32_t __gj_batch_id_2580  (alias)
 *   __libcgj_batch2580_marker = "libcgj-batch2580"
 *
 * Milestone for the UUID exclusive wave (batches 2571-2579:
 * is_nil_u, version_u, variant_u, set_version_u, set_variant_rfc_u,
 * cmp_u, copy_u, from_bytes_u, to_bytes_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2580_marker[] = "libcgj-batch2580";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2580_id(void)
{
	return 2580u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2580 - report this TU's graph batch number.
 *
 * Always returns 2580. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2580(void)
{
	(void)NULL;
	return b2580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2580(void)
    __attribute__((alias("gj_batch_id_2580")));
