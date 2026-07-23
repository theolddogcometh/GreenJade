/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4809: event-loop registration id successor.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_id_next_u(uint32_t id);
 *     - Return id + 1 (next registration / generation id). Unsigned
 *       wrap is intentional at UINT32_MAX.
 *   uint32_t __gj_ev_id_next_u  (alias)
 *   __libcgj_batch4809_marker = "libcgj-batch4809"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_id_next_u surface only; no multi-def. Distinct from
 * gj_batch_id_4810 (batch4810). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4809_marker[] = "libcgj-batch4809";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4809_id_next(uint32_t u32Id)
{
	return u32Id + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_id_next_u - compute the next event-loop registration id.
 *
 * id: current registration / generation id
 *
 * Returns id + 1, wrapping at UINT32_MAX. No parent wires.
 */
uint32_t
gj_ev_id_next_u(uint32_t u32Id)
{
	(void)NULL;
	return b4809_id_next(u32Id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_id_next_u(uint32_t u32Id)
    __attribute__((alias("gj_ev_id_next_u")));
