/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8844: rtnetlink RTF_REJECT route flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtf_reject_id_8844(void);
 *     - Returns 512 (soft RTF_REJECT reject-route flag id, 0x0200).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtf_reject_id_8844  (alias)
 *   __libcgj_batch8844_marker = "libcgj-batch8844"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rtf_reject_id_8844 surface only; no
 * multi-def. Distinct from rtf_host (batch8843) and rtf_static
 * (batch8845). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8844_marker[] = "libcgj-batch8844";

/* Soft RTF_REJECT (reject route). */
#define B8844_RTF_REJECT  512u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8844_reject(void)
{
	return B8844_RTF_REJECT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtf_reject_id_8844 - soft RTF_REJECT route flag catalog id.
 *
 * Always returns 512 (0x0200). Pure-data stub for product route-flag
 * catalogs; does not open netlink sockets or parse nlmsg. No parent
 * wires.
 */
uint32_t
gj_rtf_reject_id_8844(void)
{
	(void)NULL;
	return b8844_reject();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtf_reject_id_8844(void)
    __attribute__((alias("gj_rtf_reject_id_8844")));
