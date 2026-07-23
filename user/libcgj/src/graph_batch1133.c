/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1133: MessagePack map-format predicate.
 *
 * Surface (unique symbols):
 *   int gj_msgpack_is_map(uint8_t b);
 *     — Return 1 if b is a MessagePack map format byte:
 *       fixmap  0x80..0x8f
 *       map16   0xde
 *       map32   0xdf
 *       else 0.
 *   int __gj_msgpack_is_map  (alias)
 *   __libcgj_batch1133_marker = "libcgj-batch1133"
 *
 * Distinct from array format checks — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1133_marker[] = "libcgj-batch1133";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msgpack_is_map — 1 if u8B is a MessagePack map type byte.
 */
int
gj_msgpack_is_map(uint8_t u8B)
{
	if ((u8B & 0xf0u) == 0x80u) {
		return 1; /* fixmap */
	}
	if (u8B == 0xdeu || u8B == 0xdfu) {
		return 1; /* map16 / map32 */
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_msgpack_is_map(uint8_t u8B)
    __attribute__((alias("gj_msgpack_is_map")));
