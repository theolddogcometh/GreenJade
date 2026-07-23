/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8847: rtnetlink RT_SCOPE_UNIVERSE scope stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rt_scope_universe_id_8847(void);
 *     - Returns 0 (soft RT_SCOPE_UNIVERSE everywhere scope id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rt_scope_universe_id_8847  (alias)
 *   __libcgj_batch8847_marker = "libcgj-batch8847"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rt_scope_universe_id_8847 surface only; no
 * multi-def. Distinct from rt_scope_link (batch8846) and
 * rt_table_main (batch8848). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8847_marker[] = "libcgj-batch8847";

/* Soft RT_SCOPE_UNIVERSE (everywhere). */
#define B8847_RT_SCOPE_UNIVERSE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8847_scope_universe(void)
{
	return B8847_RT_SCOPE_UNIVERSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rt_scope_universe_id_8847 - soft RT_SCOPE_UNIVERSE catalog id.
 *
 * Always returns 0. Pure-data stub for product route-scope catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rt_scope_universe_id_8847(void)
{
	(void)NULL;
	return b8847_scope_universe();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rt_scope_universe_id_8847(void)
    __attribute__((alias("gj_rt_scope_universe_id_8847")));
