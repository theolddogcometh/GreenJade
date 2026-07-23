/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8423: djb2 seed/init constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_djb2_init_8423(void);
 *     - Return the classic djb2 seed (5381). Use as the starting hash
 *       for incremental djb2 folds (see batch8424).
 *   uint32_t __gj_hash_djb2_init_8423  (alias)
 *   __libcgj_batch8423_marker = "libcgj-batch8423"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from djb2_hash
 * (batch42) and gj_hash_djb2_seed (batch961) — unique _8423 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8423_marker[] = "libcgj-batch8423";

/* Classic djb2 seed constant. */
#define B8423_DJB2_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8423_djb2_init(void)
{
	return B8423_DJB2_SEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_djb2_init_8423 - classic djb2 seed value.
 *
 * Always returns 5381u. Stateless pure function.
 */
uint32_t
gj_hash_djb2_init_8423(void)
{
	(void)NULL;
	return b8423_djb2_init();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_djb2_init_8423(void)
    __attribute__((alias("gj_hash_djb2_init_8423")));
