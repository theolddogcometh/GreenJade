/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2010: milestone 2010 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2010(void);
 *     — Returns the compile-time graph batch number for this TU (2010).
 *   uint32_t gj_graph_milestone_2010(void);
 *     — Returns the current graph milestone revision (2010).
 *   uint32_t __gj_batch_id_2010  (alias)
 *   uint32_t __gj_graph_milestone_2010  (alias)
 *   __libcgj_batch2010_marker = "libcgj-batch2010"
 *
 * Milestone for the post-2000 continuum str helpers exclusive wave
 * (batches 2001–2009: empty_u / nonempty_p / eq_literal_p /
 * starts_with_lit / ends_with_lit / count_char_n / index_char_n /
 * rindex_char_n / span_class_n). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2010_marker[] = "libcgj-batch2010";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2010_id(void)
{
	return 2010u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2010 — report this TU's graph batch number.
 *
 * Always returns 2010.
 */
uint32_t
gj_batch_id_2010(void)
{
	(void)NULL;
	return b2010_id();
}

/*
 * gj_graph_milestone_2010 — report this TU's graph milestone revision.
 *
 * Always returns 2010 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2010(void)
{
	return b2010_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2010(void)
    __attribute__((alias("gj_batch_id_2010")));

uint32_t __gj_graph_milestone_2010(void)
    __attribute__((alias("gj_graph_milestone_2010")));
