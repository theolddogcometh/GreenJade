/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5573: GreenJade update channel preview tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_upd_chan_preview_u(void);
 *     - Returns 3 (preview update channel id). Soft compile-time product
 *       tag for GreenJade OS image update continuum.
 *   uint32_t __gj_upd_chan_preview_u  (alias)
 *   __libcgj_batch5573_marker = "libcgj-batch5573"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique gj_upd_chan_preview_u
 * surface only; no multi-def. Distinct from gj_upd_chan_stable_u
 * (batch5571) and gj_upd_chan_beta_u (batch5572). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5573_marker[] = "libcgj-batch5573";

/* Preview update channel id. */
#define B5573_CHAN_PREVIEW  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5573_chan_preview(void)
{
	return B5573_CHAN_PREVIEW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_upd_chan_preview_u - report the preview update channel id.
 *
 * Always returns 3 (preview). Soft pure-data product tag only; does not
 * probe update servers or config. Does not call libc. No parent wires.
 */
uint32_t
gj_upd_chan_preview_u(void)
{
	(void)NULL;
	return b5573_chan_preview();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_upd_chan_preview_u(void)
    __attribute__((alias("gj_upd_chan_preview_u")));
