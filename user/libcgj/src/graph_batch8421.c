/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8421: FNV-1a 32-bit offset-basis init.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_fnv1a_init_8421(void);
 *     - Return the FNV-1a 32-bit offset basis (2166136261). Use as the
 *       starting hash for incremental FNV-1a folds (see batch8422).
 *   uint32_t __gj_hash_fnv1a_init_8421  (alias)
 *   __libcgj_batch8421_marker = "libcgj-batch8421"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from fnv1a_32
 * (batch41) and gj_hash_fnv1_32 (batch963) — unique _8421 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8421_marker[] = "libcgj-batch8421";

/* FNV-1a 32-bit offset basis (public domain constant). */
#define B8421_FNV1A32_OFFSET 2166136261u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8421_fnv1a_init(void)
{
	return B8421_FNV1A32_OFFSET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fnv1a_init_8421 - FNV-1a 32-bit offset basis.
 *
 * Always returns 2166136261u. Stateless pure function.
 */
uint32_t
gj_hash_fnv1a_init_8421(void)
{
	(void)NULL;
	return b8421_fnv1a_init();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_fnv1a_init_8421(void)
    __attribute__((alias("gj_hash_fnv1a_init_8421")));
