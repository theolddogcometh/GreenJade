/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7833: regex REG_NEWLINE cflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_newline_id_7833(void);
 *     - Return soft REG_NEWLINE constant (0x4).
 *   uint32_t __gj_reg_newline_id_7833  (alias)
 *   __libcgj_batch7833_marker = "libcgj-batch7833"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_newline_id_7833 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7833_marker[] = "libcgj-batch7833";

/* REG_NEWLINE: change newline handling (1 << 2). */
#define B7833_REG_NEWLINE ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7833_reg_newline_id(void)
{
	return B7833_REG_NEWLINE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_newline_id_7833 - soft REG_NEWLINE cflag constant.
 *
 * Always returns 0x4 (POSIX REG_NEWLINE). Catalog id only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_newline_id_7833(void)
{
	(void)NULL;
	return b7833_reg_newline_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_newline_id_7833(void)
    __attribute__((alias("gj_reg_newline_id_7833")));
