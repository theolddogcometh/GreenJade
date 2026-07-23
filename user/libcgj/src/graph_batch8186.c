/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8186: jmp_buf soft word-count id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_jmp_buf_words_id_8186(void);
 *     - Return soft jmp_buf word count hint (64).
 *   uint32_t __gj_jmp_buf_words_id_8186  (alias)
 *   __libcgj_batch8186_marker = "libcgj-batch8186"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_jmp_buf_words_id_8186 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8186_marker[] = "libcgj-batch8186";

/* Soft opaque slot count; not the real ABI sizeof(jmp_buf)/sizeof(long). */
#define B8186_JMP_BUF_WORDS  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8186_words(void)
{
	return B8186_JMP_BUF_WORDS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jmp_buf_words_id_8186 - soft jmp_buf storage word-count id.
 *
 * Always returns 64. Compile-time product tag for opaque env buffer
 * sizing discussions; does not measure real jmp_buf. No parent wires.
 */
uint32_t
gj_jmp_buf_words_id_8186(void)
{
	(void)NULL;
	return b8186_words();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jmp_buf_words_id_8186(void)
    __attribute__((alias("gj_jmp_buf_words_id_8186")));
