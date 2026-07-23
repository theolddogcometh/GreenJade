/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8444: base64 plus ('+') alphabet id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_b64_plus_id_8444(void);
 *     - Returns ASCII '+' (0x2B), RFC 4648 §4 alphabet index 62.
 *   uint32_t __gj_b64_plus_id_8444  (alias)
 *   __libcgj_batch8444_marker = "libcgj-batch8444"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from base64url
 * '-' (index 62) — standard plus only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8444_marker[] = "libcgj-batch8444";

/* RFC 4648 §4 alphabet index 62. */
#define B8444_PLUS  0x2Bu /* '+' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8444_plus(void)
{
	return B8444_PLUS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_plus_id_8444 - ASCII code of standard base64 plus.
 *
 * Always returns 0x2B ('+'). Soft pure-data alphabet id; does not
 * encode or call libc. No parent wires.
 */
uint32_t
gj_b64_plus_id_8444(void)
{
	(void)NULL;
	return b8444_plus();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_b64_plus_id_8444(void)
    __attribute__((alias("gj_b64_plus_id_8444")));
