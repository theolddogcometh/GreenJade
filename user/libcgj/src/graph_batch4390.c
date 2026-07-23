/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4390: wave batch identity (UUID-ish / bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4390(void);
 *     - Returns the compile-time graph batch number for this TU (4390).
 *   uint32_t __gj_batch_id_4390  (alias)
 *   __libcgj_batch4390_marker = "libcgj-batch4390"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390: uuid_version_u,
 * uuid_variant_u, bytes_eq16_u, bytes_eq32_u, bytes_is_zero16_u,
 * bytes_is_zero32_u, u128_xor_lo64_u, u128_xor_hi64_u, fnv1a32_u8_u,
 * batch_id_4390). Does NOT redefine gj_batch_id / prior batch_id_* —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4390_marker[] = "libcgj-batch4390";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4390_id(void)
{
	return 4390u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4390 - report this TU's graph batch number.
 *
 * Always returns 4390.
 * Self-contained; no parent wires.
 */
uint32_t
gj_batch_id_4390(void)
{
	(void)NULL;
	return b4390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4390(void)
    __attribute__((alias("gj_batch_id_4390")));
