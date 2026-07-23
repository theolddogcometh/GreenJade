/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2090: milestone 2090 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2090(void);
 *     - Returns the compile-time graph batch number for this TU (2090).
 *   uint32_t gj_graph_milestone_2090(void);
 *     - Returns the current graph milestone revision (2090).
 *   uint32_t __gj_batch_id_2090  (alias)
 *   uint32_t __gj_graph_milestone_2090  (alias)
 *   __libcgj_batch2090_marker = "libcgj-batch2090"
 *
 * Milestone for the endian store exclusive wave (batches 2081-2089:
 * store_be16/32/64_u, store_le16/32/64_u, load_be16_u, load_le16_u,
 * bswap16_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2090_marker[] = "libcgj-batch2090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2090_id(void)
{
	return 2090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2090 - report this TU's graph batch number.
 *
 * Always returns 2090.
 */
uint32_t
gj_batch_id_2090(void)
{
	(void)NULL;
	return b2090_id();
}

/*
 * gj_graph_milestone_2090 - report this TU's graph milestone revision.
 *
 * Always returns 2090 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2090(void)
{
	return b2090_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2090(void)
    __attribute__((alias("gj_batch_id_2090")));

uint32_t __gj_graph_milestone_2090(void)
    __attribute__((alias("gj_graph_milestone_2090")));
