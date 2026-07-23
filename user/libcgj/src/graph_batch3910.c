/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3910: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3910(void);
 *     - Returns the compile-time graph batch number for this TU (3910).
 *   uint32_t __gj_batch_id_3910  (alias)
 *   __libcgj_batch3910_marker = "libcgj-batch3910"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910: u32_cdiv_u, u64_cdiv_u,
 * u32_fdiv_u, u64_fdiv_u, u32_rem_nz_u, u64_rem_nz_u, u32_quot_nz_u,
 * u64_quot_nz_u, u32_divisible_u, batch_id_3910). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3910_marker[] = "libcgj-batch3910";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3910_id(void)
{
	return 3910u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3910 - report this TU's graph batch number.
 *
 * Always returns 3910. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_3910(void)
{
	(void)NULL;
	return b3910_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3910(void)
    __attribute__((alias("gj_batch_id_3910")));
