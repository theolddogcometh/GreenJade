/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1184: pack boolean flags into a uint32 feature mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_feature_pack_u32(const int *flags, unsigned count);
 *     — Pack up to 32 non-zero flags into bit positions [0..count-1].
 *       NULL flags or count==0 → 0. count > 32 is truncated to 32.
 *       Non-zero *flags[i] sets bit i; zero clears it.
 *   uint32_t __gj_feature_pack_u32  (alias)
 *   __libcgj_batch1184_marker = "libcgj-batch1184"
 *
 * Distinct from gj_pack_u32be (batch513) — feature-bit packing, not endian store; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1184_marker[] = "libcgj-batch1184";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_feature_pack_u32 — pack bool-ish ints into a bit mask.
 *
 * pfFlags: array of 0/non-zero flags (may be NULL only if cFlags==0).
 * cFlags:  number of flags; only first 32 are used.
 */
uint32_t
gj_feature_pack_u32(const int *pfFlags, unsigned cFlags)
{
	uint32_t u32Pack;
	unsigned i;
	unsigned cUse;

	if (pfFlags == NULL || cFlags == 0u) {
		return 0u;
	}
	cUse = (cFlags > 32u) ? 32u : cFlags;
	u32Pack = 0u;
	for (i = 0u; i < cUse; i++) {
		if (pfFlags[i] != 0) {
			u32Pack |= (1u << i);
		}
	}
	return u32Pack;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_feature_pack_u32(const int *pfFlags, unsigned cFlags)
    __attribute__((alias("gj_feature_pack_u32")));
