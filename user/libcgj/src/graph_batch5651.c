/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5651: post-install first-boot wizard step flags.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fbwiz_step_set_5651(uint32_t flags, uint32_t step);
 *     - Set bit for step (0..31) in flags; step>31 is a no-op.
 *   uint32_t gj_fbwiz_step_clear_5651(uint32_t flags, uint32_t step);
 *   uint32_t gj_fbwiz_step_test_5651(uint32_t flags, uint32_t step);
 *     - 1 if step bit set, else 0 (step>31 → 0).
 *   uint32_t gj_fbwiz_step_count_5651(uint32_t flags);
 *     - Population count of completed step bits.
 *   uint32_t gj_batch_id_5651(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5651_marker = "libcgj-batch5651"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags —
 * fbwiz_step_5651, fbwiz_phase_5652, locale_pack_5653, locale_tag_5654,
 * kbd_layout_5655, kbd_variant_5656, net_flags_5657, net_ready_5658,
 * fbwiz_score_5659, batch_id_5660). Unique *_5651 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5651_marker[] = "libcgj-batch5651";

#define B5651_BATCH_ID 5651u
#define B5651_STEP_MAX 32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5651_set(uint32_t u32Flags, uint32_t u32Step)
{
	if (u32Step >= B5651_STEP_MAX) {
		return u32Flags;
	}
	return u32Flags | (1u << u32Step);
}

static uint32_t
b5651_clear(uint32_t u32Flags, uint32_t u32Step)
{
	if (u32Step >= B5651_STEP_MAX) {
		return u32Flags;
	}
	return u32Flags & ~(1u << u32Step);
}

static uint32_t
b5651_test(uint32_t u32Flags, uint32_t u32Step)
{
	if (u32Step >= B5651_STEP_MAX) {
		return 0u;
	}
	return ((u32Flags >> u32Step) & 1u);
}

static uint32_t
b5651_popcnt(uint32_t u32Flags)
{
	uint32_t u32C;

	u32C = 0u;
	while (u32Flags != 0u) {
		u32C += u32Flags & 1u;
		u32Flags >>= 1;
	}
	return u32C;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_fbwiz_step_set_5651(uint32_t flags, uint32_t step)
{
	(void)NULL;
	return b5651_set(flags, step);
}

uint32_t
gj_fbwiz_step_clear_5651(uint32_t flags, uint32_t step)
{
	return b5651_clear(flags, step);
}

uint32_t
gj_fbwiz_step_test_5651(uint32_t flags, uint32_t step)
{
	return b5651_test(flags, step);
}

uint32_t
gj_fbwiz_step_count_5651(uint32_t flags)
{
	return b5651_popcnt(flags);
}

uint32_t
gj_batch_id_5651(void)
{
	return B5651_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fbwiz_step_set_5651(uint32_t flags, uint32_t step)
    __attribute__((alias("gj_fbwiz_step_set_5651")));

uint32_t __gj_fbwiz_step_clear_5651(uint32_t flags, uint32_t step)
    __attribute__((alias("gj_fbwiz_step_clear_5651")));

uint32_t __gj_fbwiz_step_test_5651(uint32_t flags, uint32_t step)
    __attribute__((alias("gj_fbwiz_step_test_5651")));

uint32_t __gj_fbwiz_step_count_5651(uint32_t flags)
    __attribute__((alias("gj_fbwiz_step_count_5651")));

uint32_t __gj_batch_id_5651(void)
    __attribute__((alias("gj_batch_id_5651")));
