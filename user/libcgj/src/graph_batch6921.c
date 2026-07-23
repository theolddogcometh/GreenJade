/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6921: copy_file_range flags word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_flags_word_bits_6921(void);
 *     - Returns 32 (soft copy_file_range flags word width in bits).
 *   uint32_t __gj_cfr_flags_word_bits_6921  (alias)
 *   __libcgj_batch6921_marker = "libcgj-batch6921"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs —
 * flags_word_bits_6921, flags_must_zero_6922, flags_ok_6923,
 * off_null_means_cur_6924, fd_ok_6925, len_zero_ok_6926, same_fd_u_6927,
 * nr_x86_64_6928, arg_count/known_flags_6929, continuum + batch_id_6930).
 * Unique gj_cfr_flags_word_bits_6921 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6921_marker[] = "libcgj-batch6921";

/* Soft: copy_file_range flags argument is a uint32_t-class bitfield. */
#define B6921_WORD_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6921_word_bits(void)
{
	return B6921_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_flags_word_bits_6921 - copy_file_range flags word width.
 *
 * Always returns 32. Soft pure-data layout tag for a single flags word
 * (copy_file_range sixth-argument style). Does not call copy_file_range.
 * No parent wires.
 */
uint32_t
gj_cfr_flags_word_bits_6921(void)
{
	(void)NULL;
	return b6921_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_flags_word_bits_6921(void)
    __attribute__((alias("gj_cfr_flags_word_bits_6921")));
