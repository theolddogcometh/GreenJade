/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5360: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5360(void);
 *     - Returns the compile-time graph batch number for this TU (5360).
 *   uint32_t __gj_batch_id_5360  (alias)
 *   __libcgj_batch5360_marker = "libcgj-batch5360"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: steam_appid_placeholder_u,
 * deck_res_w_u, deck_res_h_u, deck_is_native_u, proton_abi_ok_u,
 * bar3_install_ready_u, bar3_steam_ready_u, bar3_top50_ready_u,
 * product_path_score_u, batch_id_5360). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5360_marker[] = "libcgj-batch5360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5360_id(void)
{
	return 5360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5360 - report this TU's graph batch number.
 *
 * Always returns 5360. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5360(void)
{
	(void)NULL;
	return b5360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5360(void)
    __attribute__((alias("gj_batch_id_5360")));
