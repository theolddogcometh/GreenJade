/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8442: base64 lowercase alphabet start id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_b64_alpha_a_id_8442(void);
 *     - Returns ASCII 'a' (0x61), the first RFC 4648 §4 lowercase
 *       alphabet code unit (indices 26..51 map a..z).
 *   uint32_t __gj_b64_alpha_a_id_8442  (alias)
 *   __libcgj_batch8442_marker = "libcgj-batch8442"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_alpha_A_id_8441 and gj_b64_enc_sextet_u — lowercase range id
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8442_marker[] = "libcgj-batch8442";

/* RFC 4648 §4 lowercase alphabet start. */
#define B8442_ALPHA_a  0x61u /* 'a' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8442_alpha_a(void)
{
	return B8442_ALPHA_a;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_alpha_a_id_8442 - ASCII code of base64 lowercase range start.
 *
 * Always returns 0x61 ('a'). Soft pure-data alphabet id; does not
 * encode or call libc. No parent wires.
 */
uint32_t
gj_b64_alpha_a_id_8442(void)
{
	(void)NULL;
	return b8442_alpha_a();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_b64_alpha_a_id_8442(void)
    __attribute__((alias("gj_b64_alpha_a_id_8442")));
