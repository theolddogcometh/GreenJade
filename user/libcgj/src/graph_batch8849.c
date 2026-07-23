/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8849: rtnetlink route flags ready tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rt_ready_u_8849(void);
 *     - Returns 1 (soft rtnetlink route-flags catalog readiness
 *       complete). Pure-data product tag; not a live netlink query.
 *   uint32_t __gj_rt_ready_u_8849  (alias)
 *   __libcgj_batch8849_marker = "libcgj-batch8849"
 *
 * Exclusive continuum CREATE-ONLY (8841-8850: rtnetlink route flags
 * stubs — rtf_up, rtf_gateway, rtf_host, rtf_reject, rtf_static,
 * rt_scope_link, rt_scope_universe, rt_table_main, rt_ready_u,
 * batch_id_8850). Unique gj_rt_ready_u_8849 surface only; no
 * multi-def. Soft compile-time readiness tag only. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8849_marker[] = "libcgj-batch8849";

/* Soft rtnetlink route-flags catalog readiness lamp. */
#define B8849_RT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8849_rt_ready(void)
{
	return B8849_RT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rt_ready_u_8849 - report soft rtnetlink route-flags readiness.
 *
 * Always returns 1 (ready). Link-time presence of this symbol tags the
 * route-flags stub wave. Does not call libc. No parent wires.
 */
uint32_t
gj_rt_ready_u_8849(void)
{
	(void)NULL;
	return b8849_rt_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rt_ready_u_8849(void)
    __attribute__((alias("gj_rt_ready_u_8849")));
