/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6628: epoll EPOLLPRI event bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_pri_u_6628(uint32_t events);
 *     - Return 1 if EPOLLPRI (urgent/out-of-band) is set, else 0.
 *   uint32_t __gj_epoll_pri_u_6628  (alias)
 *   __libcgj_batch6628_marker = "libcgj-batch6628"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Unique gj_epoll_pri_u_6628 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6628_marker[] = "libcgj-batch6628";

/* EPOLLPRI: exceptional/urgent condition (bit 1). */
#define B6628_EPOLLPRI ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6628_epoll_pri(uint32_t u32Events)
{
	return ((u32Events & B6628_EPOLLPRI) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_pri_u_6628 - 1 if EPOLLPRI is set in epoll events mask.
 *
 * events: raw epoll_event.events bitmask
 *
 * Returns 1 when the priority/urgent (EPOLLPRI) bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_epoll_pri_u_6628(uint32_t u32Events)
{
	(void)NULL;
	return b6628_epoll_pri(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_pri_u_6628(uint32_t u32Events)
    __attribute__((alias("gj_epoll_pri_u_6628")));
