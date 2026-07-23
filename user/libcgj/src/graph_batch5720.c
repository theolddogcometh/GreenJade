/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5720: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5720(void);
 *     - Returns the compile-time graph batch number for this TU (5720).
 *   uint32_t __gj_batch_id_5720  (alias)
 *   __libcgj_batch5720_marker = "libcgj-batch5720"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits —
 * bar3_steam_client_bit_5711, steam_client_ready_p_5712,
 * bar3_steam_slot_ok_5713, steam_client_bin_ready_5714,
 * steam_client_path_ready_5715, bar3_client_ready_mask_5716,
 * bar3_steam_client_ready_5717, steam_client_ready_bits_5718,
 * bar3_steam_client_score_5719, batch_id_5720). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5720_marker[] = "libcgj-batch5720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5720_id(void)
{
	return 5720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5720 - report this TU's graph batch number.
 *
 * Always returns 5720. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5720(void)
{
	(void)NULL;
	return b5720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5720(void)
    __attribute__((alias("gj_batch_id_5720")));
