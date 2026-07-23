/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4862: unsigned u16 PCM to signed s16 (unbias).
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_u16_to_s16_u(uint16_t u);
 *     - Inverse of gj_pcm_s16_to_u16_u: subtract mid-scale bias 32768
 *       to recover signed 16-bit PCM. Full range: 0 → INT16_MIN,
 *       32768 → 0, 65535 → INT16_MAX.
 *   int16_t __gj_pcm_u16_to_s16_u  (alias)
 *   __libcgj_batch4862_marker = "libcgj-batch4862"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_u8_bias_to_s16 (batch1673) and gj_pcm_s16_to_u16_u
 * (batch4861) — unique gj_pcm_u16_to_s16_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4862_marker[] = "libcgj-batch4862";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unbias u16 into s16: out = (int32_t)u - 32768. */
static int16_t
b4862_u16_to_s16(uint16_t u16U)
{
	return (int16_t)((int32_t)u16U - 32768);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_u16_to_s16_u - convert unsigned 16-bit PCM to signed 16-bit.
 *
 * u: unsigned PCM sample in [0, 65535]
 *
 * Returns s16 with zero at mid-scale. Self-contained; no parent wires.
 */
int16_t
gj_pcm_u16_to_s16_u(uint16_t u16U)
{
	(void)NULL;
	return b4862_u16_to_s16(u16U);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_u16_to_s16_u(uint16_t u16U)
    __attribute__((alias("gj_pcm_u16_to_s16_u")));
