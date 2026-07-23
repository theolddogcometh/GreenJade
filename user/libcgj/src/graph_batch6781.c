/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6781: inotify event-mask word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_word_bits_6781(void);
 *     - Returns 32 (soft inotify event mask width in bits).
 *   uint32_t __gj_inotify_mask_word_bits_6781  (alias)
 *   __libcgj_batch6781_marker = "libcgj-batch6781"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs —
 * word_bits_6781, bit_test_6782, bit_set_6783, bit_clear_6784,
 * bit_toggle_6785, mask_pop_6786, mask_or_6787, mask_and/has_all_6788,
 * all_events/all_events_ok_6789, continuum + batch_id_6790). Unique
 * gj_inotify_mask_word_bits_6781 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6781_marker[] = "libcgj-batch6781";

/* Soft: inotify_event.mask is a uint32_t bitfield. */
#define B6781_WORD_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6781_word_bits(void)
{
	return B6781_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_word_bits_6781 - inotify event-mask word width.
 *
 * Always returns 32. Soft pure-data layout tag for a single uint32_t
 * inotify mask (struct inotify_event.mask style). Does not open an
 * inotify fd. No parent wires.
 */
uint32_t
gj_inotify_mask_word_bits_6781(void)
{
	(void)NULL;
	return b6781_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_mask_word_bits_6781(void)
    __attribute__((alias("gj_inotify_mask_word_bits_6781")));
