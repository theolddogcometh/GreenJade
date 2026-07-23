/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6235: HDR HLG nonlinear signal clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_hlg_signal_clamp_6235(uint32_t signal_q16);
 *     - Clamp an HLG nonlinear signal E' expressed in Q16 fixed-point
 *       (0..65536 maps to 0..1) into the legal OETF/EOTF domain [0, 65536].
 *   uint32_t __gj_hdr_hlg_signal_clamp_6235  (alias)
 *   __libcgj_batch6235_marker = "libcgj-batch6235"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_hlg_signal_clamp_6235 surface only; no multi-def.
 * Distinct from gj_hdr_pq_code_clamp_6231 (PQ path). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6235_marker[] = "libcgj-batch6235";

/* Q16 unit: 65536 represents normalized HLG signal 1.0. */
#define B6235_HLG_SIGNAL_MAX_Q16  65536u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6235_clamp(uint32_t u32SignalQ16)
{
	if (u32SignalQ16 > B6235_HLG_SIGNAL_MAX_Q16) {
		return B6235_HLG_SIGNAL_MAX_Q16;
	}
	return u32SignalQ16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_hlg_signal_clamp_6235 - clamp HLG E' into [0, 1] Q16.
 *
 * signal_q16: HLG nonlinear sample in Q16 (0 = black, 65536 = peak 1.0).
 * Returns clamped Q16 signal. Soft pure-integer HLG transfer prep; not a
 * full ARIB STD-B67 / BT.2100 HLG OETF. No parent wires.
 */
uint32_t
gj_hdr_hlg_signal_clamp_6235(uint32_t u32SignalQ16)
{
	(void)NULL;
	return b6235_clamp(u32SignalQ16);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_hlg_signal_clamp_6235(uint32_t u32SignalQ16)
    __attribute__((alias("gj_hdr_hlg_signal_clamp_6235")));
