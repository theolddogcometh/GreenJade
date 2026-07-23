/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2660: milestone 2660 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2660(void);
 *     - Returns the compile-time graph batch number for this TU (2660).
 *   uint32_t gj_graph_milestone_2660(void);
 *     - Returns the current graph milestone revision (2660).
 *   uint32_t __gj_batch_id_2660  (alias)
 *   uint32_t __gj_graph_milestone_2660  (alias)
 *   __libcgj_batch2660_marker = "libcgj-batch2660"
 *
 * Milestone for the streaming checksum wave (batches 2651-2659:
 * crc32_update_u, crc32_final_u, adler32_update_u, adler32_final_u,
 * fletcher16_update_u, fletcher16_final_u, fletcher32_update_u,
 * fletcher32_final_u, checksum_xor8_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols — avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2660_marker[] = "libcgj-batch2660";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2660_id(void)
{
	return 2660u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2660 - report this TU's graph batch number.
 *
 * Always returns 2660.
 */
uint32_t
gj_batch_id_2660(void)
{
	(void)NULL;
	return b2660_id();
}

/*
 * gj_graph_milestone_2660 - report this TU's graph milestone revision.
 *
 * Always returns 2660 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2660(void)
{
	return b2660_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2660(void)
    __attribute__((alias("gj_batch_id_2660")));

uint32_t __gj_graph_milestone_2660(void)
    __attribute__((alias("gj_graph_milestone_2660")));
