/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14730: continuum core mark identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14730(void);
 *     - Returns the compile-time graph batch number for this TU (14730).
 *   uint32_t gj_graph_milestone_14730(void);
 *     - Returns the current graph milestone revision (14730).
 *   uint32_t __gj_batch_id_14730  (alias)
 *   uint32_t __gj_graph_milestone_14730  (alias)
 *   __libcgj_batch14730_marker = "libcgj-batch14730"
 *
 * Exclusive continuum CREATE-ONLY wave (14726-14750). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14730_marker[] = "libcgj-batch14730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14730_mark(void)
{
	return 14730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14730 - report this TU's graph batch number.
 *
 * Always returns 14730.
 */
uint32_t
gj_batch_id_14730(void)
{
	(void)NULL;
	return b14730_mark();
}

/*
 * gj_graph_milestone_14730 - report this TU's graph milestone revision.
 *
 * Always returns 14730. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14730(void)
{
	return b14730_mark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14730(void)
    __attribute__((alias("gj_batch_id_14730")));

uint32_t __gj_graph_milestone_14730(void)
    __attribute__((alias("gj_graph_milestone_14730")));
