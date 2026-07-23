/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5850: MILESTONE 5850 batch identity for the
 * clean-room license dual-assert continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5850(void);
 *     - Returns the compile-time graph batch number for this TU (5850).
 *   uint32_t gj_graph_milestone_5850(void);
 *     - Returns the current graph milestone revision (5850).
 *   uint32_t __gj_batch_id_5850  (alias)
 *   uint32_t __gj_graph_milestone_5850  (alias)
 *   __libcgj_batch5850_marker = "libcgj-batch5850"
 *
 * MILESTONE 5850 for the exclusive continuum CREATE-ONLY wave
 * (batches 5841-5849: cleanroom_present_5841, mit_license_assert_5842,
 * apache_license_assert_5843, dual_license_assert_5844,
 * cleanroom_source_assert_5845, no_third_party_copy_5846,
 * spdx_dual_ok_5847, dual_assert_green_5848,
 * cleanroom_license_ready_5849). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5850_marker[] = "libcgj-batch5850";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5850_id(void)
{
	return 5850u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5850 - report this TU's graph batch number.
 *
 * Always returns 5850.
 */
uint32_t
gj_batch_id_5850(void)
{
	(void)NULL;
	return b5850_id();
}

/*
 * gj_graph_milestone_5850 - report this TU's graph milestone revision.
 *
 * Always returns 5850 (MILESTONE 5850). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5850(void)
{
	return b5850_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5850(void)
    __attribute__((alias("gj_batch_id_5850")));

uint32_t __gj_graph_milestone_5850(void)
    __attribute__((alias("gj_graph_milestone_5850")));
