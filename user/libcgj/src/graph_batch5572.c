/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5572: GreenJade update channel beta tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_upd_chan_beta_u(void);
 *     - Returns 2 (beta update channel id). Soft compile-time product
 *       tag for GreenJade OS image update continuum.
 *   uint32_t __gj_upd_chan_beta_u  (alias)
 *   __libcgj_batch5572_marker = "libcgj-batch5572"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique gj_upd_chan_beta_u
 * surface only; no multi-def. Distinct from gj_upd_chan_stable_u
 * (batch5571) and gj_upd_chan_preview_u (batch5573). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5572_marker[] = "libcgj-batch5572";

/* Beta update channel id. */
#define B5572_CHAN_BETA  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5572_chan_beta(void)
{
	return B5572_CHAN_BETA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_upd_chan_beta_u - report the beta update channel id.
 *
 * Always returns 2 (beta). Soft pure-data product tag only; does not
 * probe update servers or config. Does not call libc. No parent wires.
 */
uint32_t
gj_upd_chan_beta_u(void)
{
	(void)NULL;
	return b5572_chan_beta();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_upd_chan_beta_u(void)
    __attribute__((alias("gj_upd_chan_beta_u")));
