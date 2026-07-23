/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6621: epoll EPOLLIN event bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_in_u_6621(uint32_t events);
 *     - Return 1 if EPOLLIN (readable) is set in events, else 0.
 *   uint32_t __gj_epoll_in_u_6621  (alias)
 *   __libcgj_batch6621_marker = "libcgj-batch6621"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Unique gj_epoll_in_u_6621 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6621_marker[] = "libcgj-batch6621";

/* EPOLLIN: data available for read (bit 0). */
#define B6621_EPOLLIN ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6621_epoll_in(uint32_t u32Events)
{
	return ((u32Events & B6621_EPOLLIN) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_in_u_6621 - 1 if EPOLLIN is set in epoll events mask.
 *
 * events: raw epoll_event.events bitmask
 *
 * Returns 1 when the readable (EPOLLIN) bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_epoll_in_u_6621(uint32_t u32Events)
{
	(void)NULL;
	return b6621_epoll_in(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_in_u_6621(uint32_t u32Events)
    __attribute__((alias("gj_epoll_in_u_6621")));
