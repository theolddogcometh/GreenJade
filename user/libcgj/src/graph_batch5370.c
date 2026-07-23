/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5370: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5370(void);
 *     - Returns the compile-time graph batch number for this TU (5370).
 *   uint32_t __gj_batch_id_5370  (alias)
 *   __libcgj_batch5370_marker = "libcgj-batch5370"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5370_marker[] = "libcgj-batch5370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5370_id(void)
{
	return 5370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5370 - report this TU's graph batch number.
 *
 * Always returns 5370. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5370(void)
{
	(void)NULL;
	return b5370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5370(void)
    __attribute__((alias("gj_batch_id_5370")));
