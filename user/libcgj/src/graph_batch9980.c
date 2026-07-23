/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9980: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9980(void);
 *     - Returns the compile-time graph batch number for this TU (9980).
 *   uint32_t __gj_batch_id_9980  (alias)
 *   __libcgj_batch9980_marker = "libcgj-batch9980"
 *
 * Exclusive continuum CREATE-ONLY (9971-9980: product score aggregation
 * soft stubs — product_shell_score_u, product_libcgj_score_u,
 * product_smoke_score_u, product_dyn_score_u, product_bar3_score_u,
 * product_steam_score_u, product_install_score_u, product_deck_score_u,
 * product_total_soft_u, batch_id_9980). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Soft pure-data catalog only; no product probes. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9980_marker[] = "libcgj-batch9980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9980_id(void)
{
	return 9980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9980 - report this TU's graph batch number.
 *
 * Always returns 9980. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9980(void)
{
	(void)NULL;
	return b9980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9980(void)
    __attribute__((alias("gj_batch_id_9980")));
