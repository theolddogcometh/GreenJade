/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5930: MILESTONE 5930 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5930(void);
 *     - Returns the compile-time graph batch number for this TU (5930).
 *   uint32_t gj_graph_milestone_5930(void);
 *     - Returns the current graph milestone revision (5930).
 *   uint32_t __gj_batch_id_5930  (alias)
 *   uint32_t __gj_graph_milestone_5930  (alias)
 *   __libcgj_batch5930_marker = "libcgj-batch5930"
 *
 * MILESTONE 5930 for the exclusive continuum CREATE-ONLY wave
 * (batches 5921-5929: shell_cmd_reg_cap_5921, shell_cmd_reg_stride_5922,
 * shell_cmd_surf_name_max_5923, shell_cmd_reg_slot_ok_5924,
 * shell_cmd_reg_lookup_stub_5925, shell_cmd_reg_drop_stub_5926,
 * shell_cmd_surf_flag_mask_5927, shell_cmd_reg_ready_5928,
 * shell_cmd_surf_abi_ver_5929). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5930_marker[] = "libcgj-batch5930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5930_id(void)
{
	return 5930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5930 - report this TU's graph batch number.
 *
 * Always returns 5930.
 */
uint32_t
gj_batch_id_5930(void)
{
	(void)NULL;
	return b5930_id();
}

/*
 * gj_graph_milestone_5930 - report this TU's graph milestone revision.
 *
 * Always returns 5930 (MILESTONE 5930). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5930(void)
{
	return b5930_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5930(void)
    __attribute__((alias("gj_batch_id_5930")));

uint32_t __gj_graph_milestone_5930(void)
    __attribute__((alias("gj_graph_milestone_5930")));
