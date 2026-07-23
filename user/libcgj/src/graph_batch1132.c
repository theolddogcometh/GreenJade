/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1132: CBOR major-type extract.
 *
 * Surface (unique symbols):
 *   unsigned gj_cbor_major_type(uint8_t initial);
 *     — Return the CBOR major type (top 3 bits of the initial byte),
 *       range 0..7 (RFC 8949 §3).
 *   unsigned __gj_cbor_major_type  (alias)
 *   __libcgj_batch1132_marker = "libcgj-batch1132"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1132_marker[] = "libcgj-batch1132";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cbor_major_type — major type from CBOR initial byte (bits 7..5).
 */
unsigned
gj_cbor_major_type(uint8_t u8Initial)
{
	return (unsigned)((u8Initial >> 5) & 0x7u);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_cbor_major_type(uint8_t u8Initial)
    __attribute__((alias("gj_cbor_major_type")));
