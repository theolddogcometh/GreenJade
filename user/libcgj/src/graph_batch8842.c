/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8842: rtnetlink RTF_GATEWAY route flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtf_gateway_id_8842(void);
 *     - Returns 2 (soft RTF_GATEWAY destination-is-gateway flag id,
 *       0x0002). Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtf_gateway_id_8842  (alias)
 *   __libcgj_batch8842_marker = "libcgj-batch8842"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rtf_gateway_id_8842 surface only; no
 * multi-def. Distinct from rtf_up (batch8841) and rtf_host
 * (batch8843). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8842_marker[] = "libcgj-batch8842";

/* Soft RTF_GATEWAY (destination is a gateway). */
#define B8842_RTF_GATEWAY  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8842_gateway(void)
{
	return B8842_RTF_GATEWAY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtf_gateway_id_8842 - soft RTF_GATEWAY route flag catalog id.
 *
 * Always returns 2 (0x0002). Pure-data stub for product route-flag
 * catalogs; does not open netlink sockets or parse nlmsg. No parent
 * wires.
 */
uint32_t
gj_rtf_gateway_id_8842(void)
{
	(void)NULL;
	return b8842_gateway();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtf_gateway_id_8842(void)
    __attribute__((alias("gj_rtf_gateway_id_8842")));
