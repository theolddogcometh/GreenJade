/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5659: post-install first-boot wizard score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fbwiz_score_5659(uint32_t step_count, uint32_t locale_ok,
 *                                uint32_t kbd_ok, uint32_t net_score);
 *     - Weighted completion score 0..100:
 *         steps: min(step_count,8)*5  (0..40)
 *         locale_ok: 0/1 → +20
 *         kbd_ok:    0/1 → +20
 *         net_score: clamped 0..100, contributes /5 (0..20)
 *   uint32_t gj_fbwiz_complete_5659(uint32_t score);
 *     - 1 if score >= 80, else 0.
 *   uint32_t gj_fbwiz_skip_mask_5659(uint32_t want_locale, uint32_t want_kbd,
 *                                    uint32_t want_net);
 *     - Bit0 locale_skip, bit1 kbd_skip, bit2 net_skip when want_* is 0.
 *   uint32_t gj_batch_id_5659(void);
 *   __ aliases
 *   __libcgj_batch5659_marker = "libcgj-batch5659"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5659
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5659_marker[] = "libcgj-batch5659";

#define B5659_BATCH_ID   5659u
#define B5659_STEP_CAP   8u
#define B5659_STEP_W     5u
#define B5659_LOCALE_W   20u
#define B5659_KBD_W      20u
#define B5659_NET_DIV    5u
#define B5659_COMPLETE   80u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5659_clamp100(uint32_t u32V)
{
	if (u32V > 100u) {
		return 100u;
	}
	return u32V;
}

static uint32_t
b5659_score(uint32_t u32Steps, uint32_t u32Locale, uint32_t u32Kbd,
    uint32_t u32Net)
{
	uint32_t u32S;

	if (u32Steps > B5659_STEP_CAP) {
		u32Steps = B5659_STEP_CAP;
	}
	u32S = u32Steps * B5659_STEP_W;
	if (u32Locale != 0u) {
		u32S += B5659_LOCALE_W;
	}
	if (u32Kbd != 0u) {
		u32S += B5659_KBD_W;
	}
	u32S += b5659_clamp100(u32Net) / B5659_NET_DIV;
	return b5659_clamp100(u32S);
}

static uint32_t
b5659_skip(uint32_t u32WantLocale, uint32_t u32WantKbd, uint32_t u32WantNet)
{
	uint32_t u32M;

	u32M = 0u;
	if (u32WantLocale == 0u) {
		u32M |= 1u;
	}
	if (u32WantKbd == 0u) {
		u32M |= 2u;
	}
	if (u32WantNet == 0u) {
		u32M |= 4u;
	}
	return u32M;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_fbwiz_score_5659(uint32_t step_count, uint32_t locale_ok,
    uint32_t kbd_ok, uint32_t net_score)
{
	(void)NULL;
	return b5659_score(step_count, locale_ok, kbd_ok, net_score);
}

uint32_t
gj_fbwiz_complete_5659(uint32_t score)
{
	return (score >= B5659_COMPLETE) ? 1u : 0u;
}

uint32_t
gj_fbwiz_skip_mask_5659(uint32_t want_locale, uint32_t want_kbd,
    uint32_t want_net)
{
	return b5659_skip(want_locale, want_kbd, want_net);
}

uint32_t
gj_batch_id_5659(void)
{
	return B5659_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fbwiz_score_5659(uint32_t step_count, uint32_t locale_ok,
    uint32_t kbd_ok, uint32_t net_score)
    __attribute__((alias("gj_fbwiz_score_5659")));

uint32_t __gj_fbwiz_complete_5659(uint32_t score)
    __attribute__((alias("gj_fbwiz_complete_5659")));

uint32_t __gj_fbwiz_skip_mask_5659(uint32_t want_locale, uint32_t want_kbd,
    uint32_t want_net)
    __attribute__((alias("gj_fbwiz_skip_mask_5659")));

uint32_t __gj_batch_id_5659(void)
    __attribute__((alias("gj_batch_id_5659")));
