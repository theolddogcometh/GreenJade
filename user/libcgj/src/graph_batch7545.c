/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7545: sigaction flag ok/gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigaction_flag_ok_7545(uint32_t sa_flags);
 *     - Return 1 if sa_flags has no bits outside the known SA_* mask
 *       (including sa_flags == 0); else 0. Soft pure-data gate; does
 *       not install or query signal dispositions.
 *   uint32_t __gj_sigaction_flag_ok_7545  (alias)
 *   __libcgj_batch7545_marker = "libcgj-batch7545"
 *
 * Exclusive continuum CREATE-ONLY (7541-7550: sigaction flag stubs).
 * Unique gj_sigaction_flag_ok_7545 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7545_marker[] = "libcgj-batch7545";

/* Known sa_flags OR (local freestanding SA_* copies). */
#define B7545_SA_KNOWN_MASK  (0x00000001u | 0x00000002u | 0x00000004u | \
			      0x08000000u | 0x10000000u | 0x40000000u | \
			      0x80000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7545_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B7545_SA_KNOWN_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigaction_flag_ok_7545 - soft known-bits gate for sa_flags.
 *
 * Returns 1 when only known SA_* bits (or none) are set; else 0.
 * Soft pure-data only. No parent wires.
 */
uint32_t
gj_sigaction_flag_ok_7545(uint32_t sa_flags)
{
	(void)NULL;
	return b7545_ok(sa_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigaction_flag_ok_7545(uint32_t sa_flags)
    __attribute__((alias("gj_sigaction_flag_ok_7545")));
