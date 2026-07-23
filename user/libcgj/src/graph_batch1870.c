/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1870: milestone 1870 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1870(void);
 *     — Returns the compile-time graph batch number for this TU (1870).
 *   uint32_t gj_graph_milestone_1870(void);
 *     — Returns the current graph milestone revision (1870).
 *   uint32_t __gj_batch_id_1870  (alias)
 *   uint32_t __gj_graph_milestone_1870  (alias)
 *   __libcgj_batch1870_marker = "libcgj-batch1870"
 *
 * Milestone for the Steam/desktop product exclusive wave (batches
 * 1861–1869). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1870_marker[] = "libcgj-batch1870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1870_id(void)
{
	return 1870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1870 — report this TU's graph batch number.
 *
 * Always returns 1870.
 */
uint32_t
gj_batch_id_1870(void)
{
	(void)sizeof(NULL);
	return b1870_id();
}

/*
 * gj_graph_milestone_1870 — report this TU's graph milestone revision.
 *
 * Always returns 1870 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1870(void)
{
	return b1870_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1870(void)
    __attribute__((alias("gj_batch_id_1870")));

uint32_t __gj_graph_milestone_1870(void)
    __attribute__((alias("gj_graph_milestone_1870")));
