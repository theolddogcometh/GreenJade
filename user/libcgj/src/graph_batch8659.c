/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8659: poll/epoll events soft-ready gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_ready_u_8659(uint32_t events);
 *     - Return 1 if events mask is non-zero (any readiness bit present),
 *       else 0. Soft poll/epoll ready gate.
 *   uint32_t __gj_poll_ready_u_8659  (alias)
 *   __libcgj_batch8659_marker = "libcgj-batch8659"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_poll_ready_u_8659 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8659_marker[] = "libcgj-batch8659";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8659_poll_ready(uint32_t u32Events)
{
	return (u32Events != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_ready_u_8659 - 1 if events mask reports any readiness.
 *
 * events: raw poll/epoll events bitmask
 *
 * Returns 1 when events is non-zero, else 0. Soft readiness gate; no
 * parent wires. Does not call poll or epoll_wait.
 */
uint32_t
gj_poll_ready_u_8659(uint32_t events)
{
	(void)NULL;
	return b8659_poll_ready(events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_ready_u_8659(uint32_t events)
    __attribute__((alias("gj_poll_ready_u_8659")));
