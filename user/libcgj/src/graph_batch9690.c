/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9690: nftables soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9690(void);
 *     - Returns the compile-time graph batch number for this TU (9690).
 *   uint32_t __gj_batch_id_9690  (alias)
 *   __libcgj_batch9690_marker = "libcgj-batch9690"
 *
 * Exclusive continuum CREATE-ONLY (9681-9690: nftables soft id stubs —
 * nft_table_ok_u_9681, nft_chain_ok_u_9682, nft_rule_ok_u_9683,
 * nft_set_ok_u_9684, nft_verdict_ok_u_9685, nft_add_ok_u_9686,
 * nft_del_ok_u_9687, nft_flush_ok_u_9688, nftables_ready_u_9689,
 * batch_id_9690). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9690_marker[] = "libcgj-batch9690";

#define B9690_BATCH_ID  9690u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9690_id(void)
{
	return B9690_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9690 - report this TU's graph batch number.
 *
 * Always returns 9690.
 */
uint32_t
gj_batch_id_9690(void)
{
	(void)NULL;
	return b9690_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9690(void)
    __attribute__((alias("gj_batch_id_9690")));
