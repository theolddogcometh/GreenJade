/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7821: wordexp WRDE_DOOFFS flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_dooffs_id_7821(void);
 *     - Return soft WRDE_DOOFFS constant (0x1).
 *   uint32_t __gj_wordexp_dooffs_id_7821  (alias)
 *   __libcgj_batch7821_marker = "libcgj-batch7821"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_dooffs_id_7821 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7821_marker[] = "libcgj-batch7821";

/* WRDE_DOOFFS: insert we_offs NULLs at start of we_wordv (1 << 0). */
#define B7821_WRDE_DOOFFS ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7821_wordexp_dooffs_id(void)
{
	return B7821_WRDE_DOOFFS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_dooffs_id_7821 - soft WRDE_DOOFFS flag constant.
 *
 * Always returns 0x1 (POSIX/glibc WRDE_DOOFFS). Catalog id only; does
 * not call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_dooffs_id_7821(void)
{
	(void)NULL;
	return b7821_wordexp_dooffs_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_dooffs_id_7821(void)
    __attribute__((alias("gj_wordexp_dooffs_id_7821")));
