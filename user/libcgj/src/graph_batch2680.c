/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2680: milestone 2680 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2680(void);
 *     - Returns the compile-time graph batch number for this TU (2680).
 *   uint32_t __gj_batch_id_2680  (alias)
 *   __libcgj_batch2680_marker = "libcgj-batch2680"
 *
 * Milestone for the exclusive product bar3 bitmask helpers wave
 * (batches 2671-2679: install_bit, steam/deck/top50 ready_p,
 * all_ready_p, set_install/steam/deck/top50). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols -
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2680_marker[] = "libcgj-batch2680";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2680_id(void)
{
	return 2680u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2680 - report this TU's graph batch number.
 *
 * Always returns 2680.
 */
uint32_t
gj_batch_id_2680(void)
{
	(void)NULL;
	return b2680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2680(void)
    __attribute__((alias("gj_batch_id_2680")));
