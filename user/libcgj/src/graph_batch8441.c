/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8441: base64 uppercase alphabet start id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_b64_alpha_A_id_8441(void);
 *     - Returns ASCII 'A' (0x41), the first RFC 4648 §4 uppercase
 *       alphabet code unit (indices 0..25 map A..Z).
 *   uint32_t __gj_b64_alpha_A_id_8441  (alias)
 *   __libcgj_batch8441_marker = "libcgj-batch8441"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_enc_sextet_u / gj_b64_is_char_u — pure alphabet-range id only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8441_marker[] = "libcgj-batch8441";

/* RFC 4648 §4 uppercase alphabet start. */
#define B8441_ALPHA_A  0x41u /* 'A' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8441_alpha_A(void)
{
	return B8441_ALPHA_A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_alpha_A_id_8441 - ASCII code of base64 uppercase range start.
 *
 * Always returns 0x41 ('A'). Soft pure-data alphabet id; does not
 * encode or call libc. No parent wires.
 */
uint32_t
gj_b64_alpha_A_id_8441(void)
{
	(void)NULL;
	return b8441_alpha_A();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_b64_alpha_A_id_8441(void)
    __attribute__((alias("gj_b64_alpha_A_id_8441")));
