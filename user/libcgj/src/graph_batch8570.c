/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8570: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8570(void);
 *     - Returns the compile-time graph batch number for this TU (8570).
 *   uint32_t __gj_batch_id_8570  (alias)
 *   __libcgj_batch8570_marker = "libcgj-batch8570"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8570_marker[] = "libcgj-batch8570";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8570_id(void)
{
	return 8570u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8570 - report this TU's graph batch number.
 *
 * Always returns 8570. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8570(void)
{
	(void)NULL;
	return b8570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8570(void)
    __attribute__((alias("gj_batch_id_8570")));
