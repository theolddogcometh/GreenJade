/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6941: sendfile offset word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sendfile_offset_word_bits_6941(void);
 *     - Returns 64 (soft sendfile offset width in bits; off64_t / loff_t).
 *   uint32_t __gj_sendfile_offset_word_bits_6941  (alias)
 *   __libcgj_batch6941_marker = "libcgj-batch6941"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs —
 * word_bits_6941, null_means_cur_6942, remaining_6943, count_clamp_6944,
 * advance_6945, plan_count_6946, offset_ok_6947, add_sat_6948,
 * at_eof/past_eof_6949, continuum + batch_id_6950). Unique
 * gj_sendfile_offset_word_bits_6941 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6941_marker[] = "libcgj-batch6941";

/* Soft: sendfile offset is a 64-bit file position (off64_t / loff_t style). */
#define B6941_WORD_BITS  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6941_word_bits(void)
{
	return B6941_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_word_bits_6941 - sendfile offset word width.
 *
 * Always returns 64. Soft pure-data layout tag for a single 64-bit
 * file offset used by sendfile/sendfile64-style transfers. Does not
 * call sendfile. No parent wires.
 */
uint32_t
gj_sendfile_offset_word_bits_6941(void)
{
	(void)NULL;
	return b6941_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sendfile_offset_word_bits_6941(void)
    __attribute__((alias("gj_sendfile_offset_word_bits_6941")));
