/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6968: mlockall flags known-mask soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlockall_mask_ok_6968(uint32_t flags);
 *     - Return 1 when flags has only known soft bits
 *       MCL_CURRENT|MCL_FUTURE|MCL_ONFAULT (low 3 bits); 0 if any
 *       higher bit is set.
 *   uint32_t __gj_mlockall_mask_ok_6968  (alias)
 *   __libcgj_batch6968_marker = "libcgj-batch6968"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlockall_mask_ok_6968 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6968_marker[] = "libcgj-batch6968";

/* Soft known mlockall flags (CURRENT|FUTURE|ONFAULT). */
#define B6968_MCL_KNOWN  ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6968_mcl_mask_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B6968_MCL_KNOWN) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlockall_mask_ok_6968 - soft known-mlockall-bits validity.
 *
 * flags: raw mlockall flags word
 *
 * Returns 1 when only bits 0..2 may be set (MCL_CURRENT|FUTURE|ONFAULT,
 * including zero), else 0. Pure integer. No parent wires.
 */
uint32_t
gj_mlockall_mask_ok_6968(uint32_t flags)
{
	(void)NULL;
	return b6968_mcl_mask_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlockall_mask_ok_6968(uint32_t flags)
    __attribute__((alias("gj_mlockall_mask_ok_6968")));
