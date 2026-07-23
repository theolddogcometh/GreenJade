/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6629: epoll error-ish compound bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_errorish_u_6629(uint32_t events);
 *     - Return 1 if any of EPOLLERR, EPOLLHUP, or EPOLLRDHUP is set,
 *       else 0.
 *   uint32_t __gj_epoll_errorish_u_6629  (alias)
 *   __libcgj_batch6629_marker = "libcgj-batch6629"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Unique gj_epoll_errorish_u_6629 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6629_marker[] = "libcgj-batch6629";

/* Error-ish: EPOLLERR | EPOLLHUP | EPOLLRDHUP. */
#define B6629_EPOLLERR   ((uint32_t)0x8u)
#define B6629_EPOLLHUP   ((uint32_t)0x10u)
#define B6629_EPOLLRDHUP ((uint32_t)0x2000u)
#define B6629_ERRORISH_MASK \
	(B6629_EPOLLERR | B6629_EPOLLHUP | B6629_EPOLLRDHUP)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6629_epoll_errorish(uint32_t u32Events)
{
	return ((u32Events & B6629_ERRORISH_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_errorish_u_6629 - 1 if ERR, HUP, or RDHUP is set.
 *
 * events: raw epoll_event.events bitmask
 *
 * Returns 1 when any error/hangup-class bit is present. Self-contained;
 * does not call gj_epoll_err_u_6623 / hup_u_6624 / rdhup_u_6625.
 * No parent wires.
 */
uint32_t
gj_epoll_errorish_u_6629(uint32_t u32Events)
{
	(void)NULL;
	return b6629_epoll_errorish(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_errorish_u_6629(uint32_t u32Events)
    __attribute__((alias("gj_epoll_errorish_u_6629")));
