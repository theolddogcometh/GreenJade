/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6626: epoll EPOLLET (edge-triggered) bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_et_u_6626(uint32_t events);
 *     - Return 1 if EPOLLET (edge-triggered) is set in events, else 0.
 *   uint32_t __gj_epoll_et_u_6626  (alias)
 *   __libcgj_batch6626_marker = "libcgj-batch6626"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Unique gj_epoll_et_u_6626 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6626_marker[] = "libcgj-batch6626";

/* EPOLLET: edge-triggered mode (bit 31). */
#define B6626_EPOLLET ((uint32_t)(1u << 31))

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6626_epoll_et(uint32_t u32Events)
{
	return ((u32Events & B6626_EPOLLET) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_et_u_6626 - 1 if EPOLLET is set in epoll events mask.
 *
 * events: raw epoll_event.events bitmask
 *
 * Returns 1 when the edge-triggered (EPOLLET) bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_epoll_et_u_6626(uint32_t u32Events)
{
	(void)NULL;
	return b6626_epoll_et(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_et_u_6626(uint32_t u32Events)
    __attribute__((alias("gj_epoll_et_u_6626")));
