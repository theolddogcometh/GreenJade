/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1703: djb2 64-bit single-byte update.
 *
 * Surface (unique symbols):
 *   uint64_t gj_djb2_64_update(uint64_t h, uint8_t b);
 *     — Fold one octet into a running 64-bit djb2 state:
 *       h = h * 33 + b  (equivalently (h << 5) + h + b).
 *   uint64_t __gj_djb2_64_update  (alias)
 *   __libcgj_batch1703_marker = "libcgj-batch1703"
 *
 * 64-bit streaming step; distinct from 32-bit gj_djb2 (batch422) and
 * djb2_hash (batch42). Does NOT redefine those symbols.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1703_marker[] = "libcgj-batch1703";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1703_djb2_step(uint64_t u64H, uint8_t u8B)
{
	/* hash * 33 + c */
	return ((u64H << 5) + u64H) + (uint64_t)u8B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_djb2_64_update — fold one byte into a running 64-bit djb2 hash.
 *
 * u64H: current hash state (classic seed is 5381 on the first byte).
 * u8B:  next input octet.
 *
 * Returns the updated 64-bit djb2 state.
 */
uint64_t
gj_djb2_64_update(uint64_t u64H, uint8_t u8B)
{
	return b1703_djb2_step(u64H, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_djb2_64_update(uint64_t u64H, uint8_t u8B)
    __attribute__((alias("gj_djb2_64_update")));
