/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6646: signalfd mask membership test stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_mask_has_bit_6646(uint64_t mask, uint32_t sig);
 *     - Returns 1 when sig is in [1, 64] and bit (sig-1) is set in mask,
 *       else 0. Soft pure-integer membership probe.
 *   uint32_t __gj_sfd_mask_has_bit_6646  (alias)
 *   __libcgj_batch6646_marker = "libcgj-batch6646"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_mask_has_bit_6646 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6646_marker[] = "libcgj-batch6646";

#define B6646_SIG_MIN  1u
#define B6646_SIG_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6646_has_bit(uint64_t uMask, uint32_t uSig)
{
	uint32_t uPos;

	if (uSig < B6646_SIG_MIN || uSig > B6646_SIG_MAX) {
		return 0u;
	}
	uPos = uSig - 1u;
	return ((uMask >> uPos) & 1u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_mask_has_bit_6646 - soft signalfd mask bit membership test.
 *
 * mask: current 64-bit soft mask word.
 * sig:  signal number; valid range [1, 64].
 * Returns 1 if the bit is set and sig is in range, else 0.
 * No parent wires.
 */
uint32_t
gj_sfd_mask_has_bit_6646(uint64_t mask, uint32_t sig)
{
	(void)NULL;
	return b6646_has_bit(mask, sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_mask_has_bit_6646(uint64_t mask, uint32_t sig)
    __attribute__((alias("gj_sfd_mask_has_bit_6646")));
