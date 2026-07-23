/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3940: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3940(void);
 *     - Returns the compile-time graph batch number for this TU (3940).
 *   uint32_t __gj_batch_id_3940  (alias)
 *   __libcgj_batch3940_marker = "libcgj-batch3940"
 *
 * Exclusive continuum CREATE-ONLY (3931-3940): deque_u64_pop_front_u,
 * deque_u64_pop_back_u, deque_u64_len_u, slist_u32_push_u, slist_u32_pop_u,
 * slist_u32_find_u, dlist_u32_insert_u, dlist_u32_remove_u, dlist_u32_len_u,
 * batch_id_3940. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3940_marker[] = "libcgj-batch3940";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3940_id(void)
{
	return 3940u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3940 - report this TU's graph batch number.
 *
 * Always returns 3940.
 */
uint32_t
gj_batch_id_3940(void)
{
	(void)NULL;
	return b3940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3940(void)
    __attribute__((alias("gj_batch_id_3940")));
