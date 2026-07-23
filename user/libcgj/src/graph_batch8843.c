/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8843: rtnetlink RTF_HOST route flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtf_host_id_8843(void);
 *     - Returns 4 (soft RTF_HOST host-entry flag id, 0x0004).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtf_host_id_8843  (alias)
 *   __libcgj_batch8843_marker = "libcgj-batch8843"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rtf_host_id_8843 surface only; no
 * multi-def. Distinct from rtf_gateway (batch8842) and rtf_reject
 * (batch8844). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8843_marker[] = "libcgj-batch8843";

/* Soft RTF_HOST (host entry; net otherwise). */
#define B8843_RTF_HOST  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8843_host(void)
{
	return B8843_RTF_HOST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtf_host_id_8843 - soft RTF_HOST route flag catalog id.
 *
 * Always returns 4 (0x0004). Pure-data stub for product route-flag
 * catalogs; does not open netlink sockets or parse nlmsg. No parent
 * wires.
 */
uint32_t
gj_rtf_host_id_8843(void)
{
	(void)NULL;
	return b8843_host();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtf_host_id_8843(void)
    __attribute__((alias("gj_rtf_host_id_8843")));
