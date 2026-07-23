/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8848: rtnetlink RT_TABLE_MAIN table stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rt_table_main_id_8848(void);
 *     - Returns 254 (soft RT_TABLE_MAIN routing table id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rt_table_main_id_8848  (alias)
 *   __libcgj_batch8848_marker = "libcgj-batch8848"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rt_table_main_id_8848 surface only; no
 * multi-def. Distinct from rt_scope_universe (batch8847) and
 * rt_ready_u (batch8849). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8848_marker[] = "libcgj-batch8848";

/* Soft RT_TABLE_MAIN (main routing table). */
#define B8848_RT_TABLE_MAIN  254u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8848_table_main(void)
{
	return B8848_RT_TABLE_MAIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rt_table_main_id_8848 - soft RT_TABLE_MAIN routing table catalog id.
 *
 * Always returns 254. Pure-data stub for product route-table catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rt_table_main_id_8848(void)
{
	(void)NULL;
	return b8848_table_main();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rt_table_main_id_8848(void)
    __attribute__((alias("gj_rt_table_main_id_8848")));
