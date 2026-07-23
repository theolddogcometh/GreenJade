/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5660: first-boot wizard continuum id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5660(void);
 *     - Returns the compile-time graph batch number for this TU (5660).
 *   uint32_t gj_graph_milestone_5660(void);
 *     - Returns milestone revision 5660 (end of 5651-5660 wave).
 *   uint32_t gj_fbwiz_wave_5660(void);
 *     - Wave fingerprint: 0x1613161c (5651<<16 | 5660).
 *   uint32_t gj_fbwiz_wave_ready_5660(void);
 *     - Soft readiness constant 1 for first-boot wizard continuum.
 *   uint32_t __gj_batch_id_5660 / __gj_graph_milestone_5660
 *   uint32_t __gj_fbwiz_wave_5660 / __gj_fbwiz_wave_ready_5660
 *   __libcgj_batch5660_marker = "libcgj-batch5660"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags —
 * fbwiz_step_5651, fbwiz_phase_5652, locale_pack_5653, locale_tag_5654,
 * kbd_layout_5655, kbd_variant_5656, net_flags_5657, net_ready_5658,
 * fbwiz_score_5659, batch_id_5660). Unique *_5660 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5660_marker[] = "libcgj-batch5660";

#define B5660_BATCH_ID  5660u
#define B5660_WAVE_LO   5651u
#define B5660_WAVE_HI   5660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5660_id(void)
{
	return B5660_BATCH_ID;
}

static uint32_t
b5660_wave_fp(void)
{
	return (B5660_WAVE_LO << 16) | B5660_WAVE_HI;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_batch_id_5660(void)
{
	(void)NULL;
	return b5660_id();
}

uint32_t
gj_graph_milestone_5660(void)
{
	return b5660_id();
}

uint32_t
gj_fbwiz_wave_5660(void)
{
	return b5660_wave_fp();
}

uint32_t
gj_fbwiz_wave_ready_5660(void)
{
	return 1u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5660(void)
    __attribute__((alias("gj_batch_id_5660")));

uint32_t __gj_graph_milestone_5660(void)
    __attribute__((alias("gj_graph_milestone_5660")));

uint32_t __gj_fbwiz_wave_5660(void)
    __attribute__((alias("gj_fbwiz_wave_5660")));

uint32_t __gj_fbwiz_wave_ready_5660(void)
    __attribute__((alias("gj_fbwiz_wave_ready_5660")));
