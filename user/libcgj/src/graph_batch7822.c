/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7822: wordexp WRDE_APPEND flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_append_id_7822(void);
 *     - Return soft WRDE_APPEND constant (0x2).
 *   uint32_t __gj_wordexp_append_id_7822  (alias)
 *   __libcgj_batch7822_marker = "libcgj-batch7822"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_append_id_7822 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7822_marker[] = "libcgj-batch7822";

/* WRDE_APPEND: append to results of a previous call (1 << 1). */
#define B7822_WRDE_APPEND ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7822_wordexp_append_id(void)
{
	return B7822_WRDE_APPEND;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_append_id_7822 - soft WRDE_APPEND flag constant.
 *
 * Always returns 0x2 (POSIX/glibc WRDE_APPEND). Catalog id only; does
 * not call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_append_id_7822(void)
{
	(void)NULL;
	return b7822_wordexp_append_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_append_id_7822(void)
    __attribute__((alias("gj_wordexp_append_id_7822")));
