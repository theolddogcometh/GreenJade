/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5571: GreenJade update channel stable tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_upd_chan_stable_u(void);
 *     - Returns 1 (stable update channel id). Soft compile-time product
 *       tag for GreenJade OS image update continuum.
 *   uint32_t __gj_upd_chan_stable_u  (alias)
 *   __libcgj_batch5571_marker = "libcgj-batch5571"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique — upd_chan_stable_u,
 * upd_chan_beta_u, upd_chan_preview_u, upd_chan_is_known_u,
 * os_image_ver_major_u, os_image_ver_minor_u, ab_slot_a_u, ab_slot_b_u,
 * ab_slot_active_default_u, batch_id_5580). Unique gj_upd_chan_stable_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5571_marker[] = "libcgj-batch5571";

/* Stable update channel id. */
#define B5571_CHAN_STABLE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5571_chan_stable(void)
{
	return B5571_CHAN_STABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_upd_chan_stable_u - report the stable update channel id.
 *
 * Always returns 1 (stable). Soft pure-data product tag only; does not
 * probe update servers or config. Does not call libc. No parent wires.
 */
uint32_t
gj_upd_chan_stable_u(void)
{
	(void)NULL;
	return b5571_chan_stable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_upd_chan_stable_u(void)
    __attribute__((alias("gj_upd_chan_stable_u")));
