/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3410: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3410(void);
 *     - Returns the compile-time graph batch number for this TU (3410).
 *   uint32_t __gj_batch_id_3410  (alias)
 *   __libcgj_batch3410_marker = "libcgj-batch3410"
 *
 * Exclusive continuum CREATE-ONLY wave (3401-3410: u32_cmovz_u,
 * u64_cmovz_u, u32_cmovnz_u, u64_cmovnz_u, u32_blsi_u, u64_blsi_u,
 * u32_blsr_u, u64_blsr_u, u32_blsmsk_u, batch_id_3410). Distinct from
 * gj_batch_id_3400 (batch3400) and other batch_id_* symbols — unique
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3410_marker[] = "libcgj-batch3410";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3410_id(void)
{
	return 3410u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3410 - report this TU's graph batch number.
 *
 * Always returns 3410. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3410(void)
{
	(void)NULL;
	return b3410_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3410(void)
    __attribute__((alias("gj_batch_id_3410")));
