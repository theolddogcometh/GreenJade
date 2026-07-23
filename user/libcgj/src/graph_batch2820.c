/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2820: milestone 2820 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2820(void);
 *     - Returns the compile-time graph batch number for this TU (2820).
 *   uint32_t __gj_batch_id_2820  (alias)
 *   __libcgj_batch2820_marker = "libcgj-batch2820"
 *
 * Milestone for the exclusive encoding/ASCII helpers wave (batches
 * 2811-2819: base64_enc_len_u, base64_dec_len_u, hex_enc_len_u,
 * hex_dec_len_u, ascii_case_lower_u, ascii_case_upper_u,
 * ascii_isxdigit_u, ascii_isdigit_u, ascii_isalpha_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2820_marker[] = "libcgj-batch2820";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2820_id(void)
{
	return 2820u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2820 - report this TU's graph batch number.
 *
 * Always returns 2820.
 */
uint32_t
gj_batch_id_2820(void)
{
	(void)NULL;
	return b2820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2820(void)
    __attribute__((alias("gj_batch_id_2820")));
