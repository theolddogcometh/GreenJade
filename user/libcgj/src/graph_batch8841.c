/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8841: rtnetlink RTF_UP route flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtf_up_id_8841(void);
 *     - Returns 1 (soft RTF_UP route usable flag id, 0x0001).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtf_up_id_8841  (alias)
 *   __libcgj_batch8841_marker = "libcgj-batch8841"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rtf_up_id_8841 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8841_marker[] = "libcgj-batch8841";

/* Soft RTF_UP (route usable). */
#define B8841_RTF_UP  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8841_up(void)
{
	return B8841_RTF_UP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtf_up_id_8841 - soft RTF_UP route flag catalog id.
 *
 * Always returns 1 (0x0001). Pure-data stub for product route-flag
 * catalogs; does not open netlink sockets or parse nlmsg. No parent
 * wires.
 */
uint32_t
gj_rtf_up_id_8841(void)
{
	(void)NULL;
	return b8841_up();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtf_up_id_8841(void)
    __attribute__((alias("gj_rtf_up_id_8841")));
