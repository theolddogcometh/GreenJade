/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4680: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4680(void);
 *     - Returns the compile-time graph batch number for this TU (4680).
 *   uint32_t __gj_batch_id_4680  (alias)
 *   __libcgj_batch4680_marker = "libcgj-batch4680"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680: nlen_u2, has_prefix_u2,
 * has_suffix_u2, contains_char_u2, count_chars_u, is_ws_u,
 * first_nonws_u, last_nonws_u, hash32_u, batch_id_4680). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4680_marker[] = "libcgj-batch4680";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4680_id(void)
{
	return 4680u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4680 - report this TU's graph batch number.
 *
 * Always returns 4680. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4680(void)
{
	(void)NULL;
	return b4680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4680(void)
    __attribute__((alias("gj_batch_id_4680")));
