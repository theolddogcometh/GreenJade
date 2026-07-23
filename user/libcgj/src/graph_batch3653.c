/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3653: gj_xxh32_update_u - XXH32 stream update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xxh32_update_u(uint32_t state, const void *data, size_t n);
 *     - Fold n bytes into a running single-lane XXH32-style accumulator
 *       using the public residual stripe (4-byte PRIME32_3 / rotl17 /
 *       PRIME32_4 and byte PRIME32_5 / rotl11 / PRIME32_1). Start a new
 *       stream with (seed + PRIME32_5). data == NULL or n == 0 → state
 *       unchanged. Not a full 4-lane XXH32 streaming engine — unique
 *       single-lane _update_u surface.
 *   uint32_t __gj_xxh32_update_u  (alias)
 *   __libcgj_batch3653_marker = "libcgj-batch3653"
 *
 * Hash exclusive wave (3651-3660). Companion finalizer: gj_xxh32_final_u
 * (batch3654). Distinct from one-shot gj_xxh32 (batch458) /
 * gj_xxh32_seed (batch624) / gj_xxh32_avalanche (batch966).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3653_marker[] = "libcgj-batch3653";

/* XXH32 primes (public algorithm constants). */
#define B3653_PRIME32_1 0x9E3779B1u
#define B3653_PRIME32_3 0xC2B2AE3Du
#define B3653_PRIME32_4 0x27D4EB2Fu
#define B3653_PRIME32_5 0x165667B1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3653_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b3653_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t
b3653_update(uint32_t u32H, const unsigned char *pData, size_t cbN)
{
	const unsigned char *p;
	const unsigned char *pEnd;

	p = pData;
	pEnd = pData + cbN;
	while (p + 4u <= pEnd) {
		u32H += b3653_read32le(p) * B3653_PRIME32_3;
		u32H = b3653_rotl32(u32H, 17) * B3653_PRIME32_4;
		p += 4;
	}
	while (p < pEnd) {
		u32H += (uint32_t)(*p) * B3653_PRIME32_5;
		u32H = b3653_rotl32(u32H, 11) * B3653_PRIME32_1;
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xxh32_update_u - fold n bytes into a running XXH32-style state.
 *
 * state: accumulator (init seed + 0x165667B1 for a new stream)
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated accumulator. Call gj_xxh32_final_u for avalanche.
 * Length is not mixed here — callers that need length diffusion should
 * fold it into state before final (e.g. state += (uint32_t)total_len).
 */
uint32_t
gj_xxh32_update_u(uint32_t u32State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u32State;
	}
	p = (const unsigned char *)pData;
	return b3653_update(u32State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xxh32_update_u(uint32_t u32State, const void *pData, size_t cbN)
    __attribute__((alias("gj_xxh32_update_u")));
