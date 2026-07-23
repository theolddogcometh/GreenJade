/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4440: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4440(void);
 *     - Returns the compile-time graph batch number for this TU (4440).
 *   uint32_t __gj_batch_id_4440  (alias)
 *   __libcgj_batch4440_marker = "libcgj-batch4440"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4440_marker[] = "libcgj-batch4440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4440_id(void)
{
	return 4440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4440 - report this TU's graph batch number.
 *
 * Always returns 4440. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4440(void)
{
	(void)NULL;
	return b4440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4440(void)
    __attribute__((alias("gj_batch_id_4440")));
