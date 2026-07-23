/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2390: milestone 2390 batch identity (closes
 * post-2380 checksum exclusive wave 2381-2390).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2390(void);
 *     - Returns the compile-time graph batch number for this TU (2390).
 *   uint32_t gj_graph_milestone_2390(void);
 *     - Returns the current graph milestone revision (2390).
 *   uint32_t __gj_batch_id_2390  (alias)
 *   uint32_t __gj_graph_milestone_2390  (alias)
 *   __libcgj_batch2390_marker = "libcgj-batch2390"
 *
 * Milestone for the post-2380 checksum exclusive wave (batches
 * 2381-2389: crc8_smbus_u, crc8_rohc_u, crc16_ibm_u, crc16_dnp_u,
 * crc32_bzip2_u, crc32_posix_u, sum8_bytes, sum16_ones_comp,
 * luhn_check_digit). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2390_marker[] = "libcgj-batch2390";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2390_id(void)
{
	return 2390u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2390 - report this TU's graph batch number.
 *
 * Always returns 2390.
 */
uint32_t
gj_batch_id_2390(void)
{
	(void)NULL;
	return b2390_id();
}

/*
 * gj_graph_milestone_2390 - report this TU's graph milestone revision.
 *
 * Always returns 2390 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2390(void)
{
	return b2390_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2390(void)
    __attribute__((alias("gj_batch_id_2390")));

uint32_t __gj_graph_milestone_2390(void)
    __attribute__((alias("gj_graph_milestone_2390")));
