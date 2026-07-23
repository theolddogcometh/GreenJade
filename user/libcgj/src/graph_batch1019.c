/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1019: Thomas Wang 32-bit integer hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wang_hash32(uint32_t key);
 *     — Avalanche mix of a 32-bit integer key (Thomas Wang's hash32).
 *       Pure function of key; no global state. Suitable for hash tables
 *       and non-crypto fingerprinting — not a cryptographic hash.
 *   uint32_t __gj_wang_hash32  (alias)
 *   __libcgj_batch1019_marker = "libcgj-batch1019"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1019_marker[] = "libcgj-batch1019";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wang_hash32 — Thomas Wang integer hash of a 32-bit key.
 *
 * key: input word
 *
 * Classic mix sequence:
 *   key = ~key + (key << 15);
 *   key = key ^ (key >> 12);
 *   key = key + (key << 2);
 *   key = key ^ (key >> 4);
 *   key = key * 2057;          // == key + (key<<3) + (key<<11)
 *   key = key ^ (key >> 16);
 *
 * Returns the mixed 32-bit value.
 */
uint32_t
gj_wang_hash32(uint32_t key)
{
	key = ~key + (key << 15);
	key = key ^ (key >> 12);
	key = key + (key << 2);
	key = key ^ (key >> 4);
	key = key * 2057u;
	key = key ^ (key >> 16);
	return key;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wang_hash32(uint32_t key)
    __attribute__((alias("gj_wang_hash32")));
