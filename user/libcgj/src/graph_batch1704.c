/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1704: sdbm 64-bit single-byte update.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sdbm64_update(uint64_t h, uint8_t b);
 *     — Fold one octet into a running 64-bit sdbm state:
 *       h = b + (h << 6) + (h << 16) - h.
 *   uint64_t __gj_sdbm64_update  (alias)
 *   __libcgj_batch1704_marker = "libcgj-batch1704"
 *
 * 64-bit streaming step; distinct from 32-bit gj_sdbm (batch423),
 * gj_sdbm_n (batch621), and sdbm_hash (batch42). Does NOT redefine
 * those symbols.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1704_marker[] = "libcgj-batch1704";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1704_sdbm_step(uint64_t u64H, uint8_t u8B)
{
	return (uint64_t)u8B + (u64H << 6) + (u64H << 16) - u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sdbm64_update — fold one byte into a running 64-bit sdbm hash.
 *
 * u64H: current hash state (classic seed is 0 on the first byte).
 * u8B:  next input octet.
 *
 * Returns the updated 64-bit sdbm state.
 */
uint64_t
gj_sdbm64_update(uint64_t u64H, uint8_t u8B)
{
	return b1704_sdbm_step(u64H, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sdbm64_update(uint64_t u64H, uint8_t u8B)
    __attribute__((alias("gj_sdbm64_update")));
