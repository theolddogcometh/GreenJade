/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2830: milestone 2830 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2830(void);
 *     - Returns the compile-time graph batch number for this TU (2830).
 *   uint32_t __gj_batch_id_2830  (alias)
 *   __libcgj_batch2830_marker = "libcgj-batch2830"
 *
 * Milestone for the ASCII/path exclusive wave (batches 2821-2829:
 * ascii_isalnum_u, ascii_isspace_u, ascii_isprint_u, path_is_abs_u,
 * path_is_dot_u, path_is_dotdot_u, path_sep_p_u, path_join_need_u,
 * url_encode_need_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2830_marker[] = "libcgj-batch2830";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2830_id(void)
{
	return 2830u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2830 - report this TU's graph batch number.
 *
 * Always returns 2830. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2830(void)
{
	(void)NULL;
	return b2830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2830(void)
    __attribute__((alias("gj_batch_id_2830")));
