/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8446: base64 pad ('=') alphabet id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_b64_pad_id_8446(void);
 *     - Returns ASCII '=' (0x3D), the RFC 4648 §4 pad character.
 *   uint32_t __gj_b64_pad_id_8446  (alias)
 *   __libcgj_batch8446_marker = "libcgj-batch8446"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_is_pad_u / gj_b64_pad_count_u — pad char id only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8446_marker[] = "libcgj-batch8446";

/* RFC 4648 §4 pad character. */
#define B8446_PAD  0x3Du /* '=' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8446_pad(void)
{
	return B8446_PAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_pad_id_8446 - ASCII code of base64 pad character.
 *
 * Always returns 0x3D ('='). Soft pure-data alphabet id; does not
 * encode or call libc. No parent wires.
 */
uint32_t
gj_b64_pad_id_8446(void)
{
	(void)NULL;
	return b8446_pad();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_b64_pad_id_8446(void)
    __attribute__((alias("gj_b64_pad_id_8446")));
