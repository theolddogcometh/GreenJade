/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8845: rtnetlink RTF_STATIC route flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtf_static_id_8845(void);
 *     - Returns 1024 (soft RTF_STATIC manually-added flag id, 0x0400).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtf_static_id_8845  (alias)
 *   __libcgj_batch8845_marker = "libcgj-batch8845"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rtf_static_id_8845 surface only; no
 * multi-def. Distinct from rtf_reject (batch8844) and rt_scope_link
 * (batch8846). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8845_marker[] = "libcgj-batch8845";

/* Soft RTF_STATIC (manually added). */
#define B8845_RTF_STATIC  1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8845_static_flag(void)
{
	return B8845_RTF_STATIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtf_static_id_8845 - soft RTF_STATIC route flag catalog id.
 *
 * Always returns 1024 (0x0400). Pure-data stub for product route-flag
 * catalogs; does not open netlink sockets or parse nlmsg. No parent
 * wires.
 */
uint32_t
gj_rtf_static_id_8845(void)
{
	(void)NULL;
	return b8845_static_flag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtf_static_id_8845(void)
    __attribute__((alias("gj_rtf_static_id_8845")));
