/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6831: pidfd_open flags word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_word_bits_6831(void);
 *     - Returns 32 (soft pidfd_open flags word width in bits).
 *   uint32_t __gj_pidfd_open_flags_word_bits_6831  (alias)
 *   __libcgj_batch6831_marker = "libcgj-batch6831"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs —
 * word_bits_6831, bit_test_6832, bit_set_6833, bit_clear_6834,
 * bit_toggle_6835, mask_pop_6836, mask_or_6837, mask_and/has_all_6838,
 * known_flags/known_flags_ok_6839, continuum + batch_id_6840). Unique
 * gj_pidfd_open_flags_word_bits_6831 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6831_marker[] = "libcgj-batch6831";

/* Soft: pidfd_open flags argument is a uint32_t-class bitfield. */
#define B6831_WORD_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6831_word_bits(void)
{
	return B6831_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_word_bits_6831 - pidfd_open flags word width.
 *
 * Always returns 32. Soft pure-data layout tag for a single flags word
 * (pidfd_open second argument style). Does not open a pidfd.
 * No parent wires.
 */
uint32_t
gj_pidfd_open_flags_word_bits_6831(void)
{
	(void)NULL;
	return b6831_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pidfd_open_flags_word_bits_6831(void)
    __attribute__((alias("gj_pidfd_open_flags_word_bits_6831")));
