/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4990: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4990(void);
 *     - Returns the compile-time graph batch number for this TU (4990).
 *   uint32_t __gj_batch_id_4990  (alias)
 *   __libcgj_batch4990_marker = "libcgj-batch4990"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990: pte_present_u,
 * pte_writable_u, pte_user_u, pte_nx_bit_u, pte_is_present_u,
 * pte_is_writable_u, pte_addr_u, pte_flags_u, pte_make_u,
 * batch_id_4990). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4990_marker[] = "libcgj-batch4990";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4990_id(void)
{
	return 4990u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4990 - report this TU's graph batch number.
 *
 * Always returns 4990. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4990(void)
{
	(void)NULL;
	return b4990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4990(void)
    __attribute__((alias("gj_batch_id_4990")));
