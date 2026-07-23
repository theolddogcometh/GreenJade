/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6645: signalfd mask clear-bit stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sfd_mask_clr_bit_6645(uint64_t mask, uint32_t sig);
 *     - Returns mask with bit (sig-1) cleared when sig is in [1, 64];
 *       otherwise returns mask unchanged. Soft pure-integer AND-not stub.
 *   uint64_t __gj_sfd_mask_clr_bit_6645  (alias)
 *   __libcgj_batch6645_marker = "libcgj-batch6645"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_mask_clr_bit_6645 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6645_marker[] = "libcgj-batch6645";

#define B6645_SIG_MIN  1u
#define B6645_SIG_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6645_clr_bit(uint64_t uMask, uint32_t uSig)
{
	uint32_t uPos;

	if (uSig < B6645_SIG_MIN || uSig > B6645_SIG_MAX) {
		return uMask;
	}
	uPos = uSig - 1u;
	return uMask & ~((uint64_t)1u << uPos);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_mask_clr_bit_6645 - soft signalfd mask bit clear (AND-not).
 *
 * mask: current 64-bit soft mask word.
 * sig:  signal number; valid range [1, 64].
 * Returns mask with the corresponding bit cleared, or mask if sig invalid.
 * No parent wires.
 */
uint64_t
gj_sfd_mask_clr_bit_6645(uint64_t mask, uint32_t sig)
{
	(void)NULL;
	return b6645_clr_bit(mask, sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sfd_mask_clr_bit_6645(uint64_t mask, uint32_t sig)
    __attribute__((alias("gj_sfd_mask_clr_bit_6645")));
