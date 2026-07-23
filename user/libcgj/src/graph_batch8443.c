/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8443: base64 digit alphabet start id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_b64_digit_id_8443(void);
 *     - Returns ASCII '0' (0x30), the first RFC 4648 §4 digit
 *       alphabet code unit (indices 52..61 map 0..9).
 *   uint32_t __gj_b64_digit_id_8443  (alias)
 *   __libcgj_batch8443_marker = "libcgj-batch8443"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_alpha_A_id_8441 / ctype isdigit — digit-range id only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8443_marker[] = "libcgj-batch8443";

/* RFC 4648 §4 digit alphabet start. */
#define B8443_DIGIT  0x30u /* '0' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8443_digit(void)
{
	return B8443_DIGIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_digit_id_8443 - ASCII code of base64 digit range start.
 *
 * Always returns 0x30 ('0'). Soft pure-data alphabet id; does not
 * encode or call libc. No parent wires.
 */
uint32_t
gj_b64_digit_id_8443(void)
{
	(void)NULL;
	return b8443_digit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_b64_digit_id_8443(void)
    __attribute__((alias("gj_b64_digit_id_8443")));
