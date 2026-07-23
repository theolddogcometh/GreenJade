/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4139: reverse octet order of a uint16_t (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_bswap_u(uint16_t x);
 *     - Swap the two octets of x: (x<<8)|(x>>8) with 16-bit wrap.
 *       Pure integer; no builtins, no host endian assumptions.
 *   uint16_t __gj_u16_bswap_u  (alias)
 *   __libcgj_batch4139_marker = "libcgj-batch4139"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Distinct from gj_bswap32/64 (batch416/417) and gj_u16_bswap_buf
 * (batch639) — unique gj_u16_bswap_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4139_marker[] = "libcgj-batch4139";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4139_bswap(uint16_t x)
{
	return (uint16_t)((x << 8) | (x >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_bswap_u - reverse the two octets of a 16-bit word.
 *
 * x: value to byte-swap
 *
 * Returns ((x & 0xFF) << 8) | (x >> 8). Example:
 * gj_u16_bswap_u(0x1234u) -> 0x3412u.
 * Self-contained; no parent wires.
 */
uint16_t
gj_u16_bswap_u(uint16_t x)
{
	(void)NULL;
	return b4139_bswap(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_bswap_u(uint16_t x)
    __attribute__((alias("gj_u16_bswap_u")));
