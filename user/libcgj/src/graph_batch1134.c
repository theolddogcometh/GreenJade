/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1134: MessagePack array-format predicate.
 *
 * Surface (unique symbols):
 *   int gj_msgpack_is_array(uint8_t b);
 *     — Return 1 if b is a MessagePack array format byte:
 *       fixarray 0x90..0x9f
 *       array16  0xdc
 *       array32  0xdd
 *       else 0.
 *   int __gj_msgpack_is_array  (alias)
 *   __libcgj_batch1134_marker = "libcgj-batch1134"
 *
 * Distinct from gj_msgpack_is_map (batch1133) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1134_marker[] = "libcgj-batch1134";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msgpack_is_array — 1 if u8B is a MessagePack array type byte.
 */
int
gj_msgpack_is_array(uint8_t u8B)
{
	if ((u8B & 0xf0u) == 0x90u) {
		return 1; /* fixarray */
	}
	if (u8B == 0xdcu || u8B == 0xddu) {
		return 1; /* array16 / array32 */
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_msgpack_is_array(uint8_t u8B)
    __attribute__((alias("gj_msgpack_is_array")));
