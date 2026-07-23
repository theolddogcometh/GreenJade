/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4560: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4560(void);
 *     - Returns the compile-time graph batch number for this TU (4560).
 *   uint32_t __gj_batch_id_4560  (alias)
 *   __libcgj_batch4560_marker = "libcgj-batch4560"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4560_marker[] = "libcgj-batch4560";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4560_id(void)
{
	return 4560u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4560 - report this TU's graph batch number.
 *
 * Always returns 4560. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4560(void)
{
	(void)NULL;
	return b4560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4560(void)
    __attribute__((alias("gj_batch_id_4560")));
