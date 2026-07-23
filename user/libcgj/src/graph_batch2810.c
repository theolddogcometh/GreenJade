/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2810: milestone 2810 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2810(void);
 *     - Returns the compile-time graph batch number for this TU (2810).
 *   uint32_t gj_graph_milestone_2810(void);
 *     - Returns the current graph milestone revision (2810).
 *   uint32_t __gj_batch_id_2810  (alias)
 *   uint32_t __gj_graph_milestone_2810  (alias)
 *   __libcgj_batch2810_marker = "libcgj-batch2810"
 *
 * Milestone for the exclusive hash/checksum helpers wave (batches
 * 2801-2809: fnv1a32_u, fnv1a64_u, djb2_u, sdbm_u, crc16_ccitt_u,
 * crc16_modbus_n, xor_fold16_u, xor_fold32_u, luhn_check_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2810_marker[] = "libcgj-batch2810";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2810_id(void)
{
	return 2810u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2810 - report this TU's graph batch number.
 *
 * Always returns 2810.
 */
uint32_t
gj_batch_id_2810(void)
{
	(void)NULL;
	return b2810_id();
}

/*
 * gj_graph_milestone_2810 - report this TU's graph milestone revision.
 *
 * Always returns 2810 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2810(void)
{
	return b2810_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2810(void)
    __attribute__((alias("gj_batch_id_2810")));

uint32_t __gj_graph_milestone_2810(void)
    __attribute__((alias("gj_graph_milestone_2810")));
