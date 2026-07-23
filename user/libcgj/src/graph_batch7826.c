/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7826: wordexp WRDE_NOCMD presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_has_nocmd_u_7826(uint32_t flags);
 *     - Return 1 if flags has WRDE_NOCMD (0x4) set, else 0.
 *   uint32_t __gj_wordexp_has_nocmd_u_7826  (alias)
 *   __libcgj_batch7826_marker = "libcgj-batch7826"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_has_nocmd_u_7826 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7826_marker[] = "libcgj-batch7826";

/* WRDE_NOCMD bit (0x4). */
#define B7826_WRDE_NOCMD ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7826_wordexp_has_nocmd(uint32_t u32Flags)
{
	return ((u32Flags & B7826_WRDE_NOCMD) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_has_nocmd_u_7826 - 1 if flags includes WRDE_NOCMD.
 *
 * flags: wordexp(3) flags argument
 *
 * Returns 1 when bit 0x4 is set, else 0. Soft bit test only; does not
 * call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_has_nocmd_u_7826(uint32_t u32Flags)
{
	(void)NULL;
	return b7826_wordexp_has_nocmd(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_has_nocmd_u_7826(uint32_t u32Flags)
    __attribute__((alias("gj_wordexp_has_nocmd_u_7826")));
