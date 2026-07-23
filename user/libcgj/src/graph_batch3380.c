/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3380: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3380(void);
 *     - Returns the compile-time graph batch number for this TU (3380).
 *   uint32_t __gj_batch_id_3380  (alias)
 *   __libcgj_batch3380_marker = "libcgj-batch3380"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3380_marker[] = "libcgj-batch3380";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3380_id(void)
{
	return 3380u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3380 - report this TU's graph batch number.
 *
 * Always returns 3380.
 */
uint32_t
gj_batch_id_3380(void)
{
	(void)NULL;
	return b3380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3380(void)
    __attribute__((alias("gj_batch_id_3380")));
