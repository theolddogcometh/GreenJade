/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6114: nested output refresh Hz soft pick (_6114).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_refresh_hz_6114(uint32_t want_hz, uint32_t cap_hz);
 *     - Pick nested-output refresh: if want_hz == 0 or want_hz > cap_hz
 *       (and cap_hz != 0), return cap_hz; if cap_hz == 0 return want_hz;
 *       else return want_hz (accepted under cap).
 *   uint32_t __gj_nested_out_refresh_hz_6114  (alias)
 *   __libcgj_batch6114_marker = "libcgj-batch6114"
 *
 * Exclusive continuum CREATE-ONLY (6111-6120: gamescope nested output —
 * nested_out_mode_pack_6111, nested_out_scale_6112, nested_out_xform_6113,
 * nested_out_refresh_hz_6114, nested_out_phys_mm_6115, nested_out_tag_6116,
 * nested_out_pos_pack_6117, nested_out_bpp_6118, nested_out_subpixel_6119,
 * batch_id_6120). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6114_marker[] = "libcgj-batch6114";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6114_pick(uint32_t u32Want, uint32_t u32Cap)
{
	if (u32Cap == 0u) {
		return u32Want;
	}
	if (u32Want == 0u || u32Want > u32Cap) {
		return u32Cap;
	}
	return u32Want;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_refresh_hz_6114 - soft nested output refresh under cap.
 *
 * want_hz: preferred nested refresh (0 → use cap)
 * cap_hz:  outer/panel cap (0 → passthrough want)
 *
 * Returns want when 1..cap inclusive, else cap (or want if cap is 0).
 * Pure integer; no mode-set ioctl. No parent wires.
 */
uint32_t
gj_nested_out_refresh_hz_6114(uint32_t u32WantHz, uint32_t u32CapHz)
{
	(void)NULL;
	return b6114_pick(u32WantHz, u32CapHz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_refresh_hz_6114(uint32_t u32WantHz, uint32_t u32CapHz)
    __attribute__((alias("gj_nested_out_refresh_hz_6114")));
