/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2940: milestone 2940 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2940(void);
 *     - Returns the compile-time graph batch number for this TU (2940).
 *   uint32_t __gj_batch_id_2940  (alias)
 *   __libcgj_batch2940_marker = "libcgj-batch2940"
 *
 * Milestone for the pack/mod exclusive wave (batches 2931-2939:
 * unpack_u32_le_u, pack_u64_be_u, pack_u64_le_u, unpack_u64_be_u,
 * unpack_u64_le_u, u32_add_mod_u, u32_sub_mod_u, u32_mul_mod_u,
 * u64_add_mod_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2940_marker[] = "libcgj-batch2940";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2940_id(void)
{
	return 2940u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2940 - report this TU's graph batch number.
 *
 * Always returns 2940. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2940(void)
{
	(void)NULL;
	return b2940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2940(void)
    __attribute__((alias("gj_batch_id_2940")));
