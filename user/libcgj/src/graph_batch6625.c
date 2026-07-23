/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6625: epoll EPOLLRDHUP event bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_rdhup_u_6625(uint32_t events);
 *     - Return 1 if EPOLLRDHUP (peer shutdown read half) is set, else 0.
 *   uint32_t __gj_epoll_rdhup_u_6625  (alias)
 *   __libcgj_batch6625_marker = "libcgj-batch6625"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Unique gj_epoll_rdhup_u_6625 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6625_marker[] = "libcgj-batch6625";

/* EPOLLRDHUP: stream socket peer closed connection / shut down writing. */
#define B6625_EPOLLRDHUP ((uint32_t)0x2000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6625_epoll_rdhup(uint32_t u32Events)
{
	return ((u32Events & B6625_EPOLLRDHUP) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_rdhup_u_6625 - 1 if EPOLLRDHUP is set in epoll events mask.
 *
 * events: raw epoll_event.events bitmask
 *
 * Returns 1 when the peer half-close (EPOLLRDHUP) bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_epoll_rdhup_u_6625(uint32_t u32Events)
{
	(void)NULL;
	return b6625_epoll_rdhup(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_rdhup_u_6625(uint32_t u32Events)
    __attribute__((alias("gj_epoll_rdhup_u_6625")));
