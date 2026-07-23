/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8846: rtnetlink RT_SCOPE_LINK scope stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rt_scope_link_id_8846(void);
 *     - Returns 253 (soft RT_SCOPE_LINK route scope id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rt_scope_link_id_8846  (alias)
 *   __libcgj_batch8846_marker = "libcgj-batch8846"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rt_scope_link_id_8846 surface only; no
 * multi-def. Distinct from rtf_static (batch8845) and
 * rt_scope_universe (batch8847). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8846_marker[] = "libcgj-batch8846";

/* Soft RT_SCOPE_LINK (destination on link). */
#define B8846_RT_SCOPE_LINK  253u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8846_scope_link(void)
{
	return B8846_RT_SCOPE_LINK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rt_scope_link_id_8846 - soft RT_SCOPE_LINK route scope catalog id.
 *
 * Always returns 253. Pure-data stub for product route-scope catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rt_scope_link_id_8846(void)
{
	(void)NULL;
	return b8846_scope_link();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rt_scope_link_id_8846(void)
    __attribute__((alias("gj_rt_scope_link_id_8846")));
