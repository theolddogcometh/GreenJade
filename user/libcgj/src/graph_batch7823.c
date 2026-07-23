/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7823: wordexp WRDE_NOCMD flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_nocmd_id_7823(void);
 *     - Return soft WRDE_NOCMD constant (0x4).
 *   uint32_t __gj_wordexp_nocmd_id_7823  (alias)
 *   __libcgj_batch7823_marker = "libcgj-batch7823"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_nocmd_id_7823 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7823_marker[] = "libcgj-batch7823";

/* WRDE_NOCMD: don't do command substitution (1 << 2). */
#define B7823_WRDE_NOCMD ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7823_wordexp_nocmd_id(void)
{
	return B7823_WRDE_NOCMD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_nocmd_id_7823 - soft WRDE_NOCMD flag constant.
 *
 * Always returns 0x4 (POSIX/glibc WRDE_NOCMD). Catalog id only; does
 * not call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_nocmd_id_7823(void)
{
	(void)NULL;
	return b7823_wordexp_nocmd_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_nocmd_id_7823(void)
    __attribute__((alias("gj_wordexp_nocmd_id_7823")));
