/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7832: regex REG_ICASE cflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_icase_id_7832(void);
 *     - Return soft REG_ICASE constant (0x2).
 *   uint32_t __gj_reg_icase_id_7832  (alias)
 *   __libcgj_batch7832_marker = "libcgj-batch7832"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_icase_id_7832 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7832_marker[] = "libcgj-batch7832";

/* REG_ICASE: ignore case in match (1 << 1). */
#define B7832_REG_ICASE ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7832_reg_icase_id(void)
{
	return B7832_REG_ICASE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_icase_id_7832 - soft REG_ICASE cflag constant.
 *
 * Always returns 0x2 (POSIX REG_ICASE). Catalog id only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_icase_id_7832(void)
{
	(void)NULL;
	return b7832_reg_icase_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_icase_id_7832(void)
    __attribute__((alias("gj_reg_icase_id_7832")));
