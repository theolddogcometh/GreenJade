/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5656: keyboard variant / option flags.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kbd_variant_set_5656(uint32_t flags, uint32_t bit);
 *     - Set variant option bit (0..31); bit>31 no-op.
 *   uint32_t gj_kbd_variant_clear_5656(uint32_t flags, uint32_t bit);
 *   uint32_t gj_kbd_variant_test_5656(uint32_t flags, uint32_t bit);
 *   uint32_t gj_kbd_variant_merge_5656(uint32_t base, uint32_t add,
 *                                      uint32_t mask);
 *     - (base & ~mask) | (add & mask)
 *   uint32_t gj_batch_id_5656(void);
 *   __ aliases
 *   __libcgj_batch5656_marker = "libcgj-batch5656"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5656
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5656_marker[] = "libcgj-batch5656";

#define B5656_BATCH_ID 5656u
#define B5656_BIT_MAX  32u

/* Known soft option bit indices (documentation only; values free):
 * 0 compose, 1 caps_as_ctrl, 2 swap_alt_win, 3 numlock_on,
 * 4 terminate_ctrl_alt_bksp, 5 lv3_ralt, 6 ...
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5656_set(uint32_t u32Flags, uint32_t u32Bit)
{
	if (u32Bit >= B5656_BIT_MAX) {
		return u32Flags;
	}
	return u32Flags | (1u << u32Bit);
}

static uint32_t
b5656_clear(uint32_t u32Flags, uint32_t u32Bit)
{
	if (u32Bit >= B5656_BIT_MAX) {
		return u32Flags;
	}
	return u32Flags & ~(1u << u32Bit);
}

static uint32_t
b5656_test(uint32_t u32Flags, uint32_t u32Bit)
{
	if (u32Bit >= B5656_BIT_MAX) {
		return 0u;
	}
	return (u32Flags >> u32Bit) & 1u;
}

static uint32_t
b5656_merge(uint32_t u32Base, uint32_t u32Add, uint32_t u32Mask)
{
	return (u32Base & ~u32Mask) | (u32Add & u32Mask);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_kbd_variant_set_5656(uint32_t flags, uint32_t bit)
{
	(void)NULL;
	return b5656_set(flags, bit);
}

uint32_t
gj_kbd_variant_clear_5656(uint32_t flags, uint32_t bit)
{
	return b5656_clear(flags, bit);
}

uint32_t
gj_kbd_variant_test_5656(uint32_t flags, uint32_t bit)
{
	return b5656_test(flags, bit);
}

uint32_t
gj_kbd_variant_merge_5656(uint32_t base, uint32_t add, uint32_t mask)
{
	return b5656_merge(base, add, mask);
}

uint32_t
gj_batch_id_5656(void)
{
	return B5656_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_kbd_variant_set_5656(uint32_t flags, uint32_t bit)
    __attribute__((alias("gj_kbd_variant_set_5656")));

uint32_t __gj_kbd_variant_clear_5656(uint32_t flags, uint32_t bit)
    __attribute__((alias("gj_kbd_variant_clear_5656")));

uint32_t __gj_kbd_variant_test_5656(uint32_t flags, uint32_t bit)
    __attribute__((alias("gj_kbd_variant_test_5656")));

uint32_t __gj_kbd_variant_merge_5656(uint32_t base, uint32_t add,
    uint32_t mask)
    __attribute__((alias("gj_kbd_variant_merge_5656")));

uint32_t __gj_batch_id_5656(void)
    __attribute__((alias("gj_batch_id_5656")));
