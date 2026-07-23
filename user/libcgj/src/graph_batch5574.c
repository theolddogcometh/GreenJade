/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5574: known update channel predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_upd_chan_is_known_u(uint32_t chan);
 *     - Return 1 when chan is a known update channel id
 *       (1=stable, 2=beta, 3=preview), else 0.
 *   uint32_t __gj_upd_chan_is_known_u  (alias)
 *   __libcgj_batch5574_marker = "libcgj-batch5574"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique gj_upd_chan_is_known_u
 * surface only; no multi-def. Distinct from gj_upd_chan_stable_u /
 * beta_u / preview_u (batch5571-5573). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5574_marker[] = "libcgj-batch5574";

/* Known channel id range: stable=1, beta=2, preview=3. */
#define B5574_CHAN_MIN  1u
#define B5574_CHAN_MAX  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5574_is_known(uint32_t u32Chan)
{
	return (u32Chan >= B5574_CHAN_MIN && u32Chan <= B5574_CHAN_MAX)
	    ? 1u
	    : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_upd_chan_is_known_u - true when chan is a known update channel id.
 *
 * chan: update channel id candidate
 *
 * Returns 1 if chan is 1 (stable), 2 (beta), or 3 (preview), else 0.
 * Soft pure-data only; does not read policy config. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_upd_chan_is_known_u(uint32_t u32Chan)
{
	(void)NULL;
	return b5574_is_known(u32Chan);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_upd_chan_is_known_u(uint32_t u32Chan)
    __attribute__((alias("gj_upd_chan_is_known_u")));
