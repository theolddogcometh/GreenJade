/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7834: regex REG_NOSUB cflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_nosub_id_7834(void);
 *     - Return soft REG_NOSUB constant (0x8).
 *   uint32_t __gj_reg_nosub_id_7834  (alias)
 *   __libcgj_batch7834_marker = "libcgj-batch7834"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_nosub_id_7834 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7834_marker[] = "libcgj-batch7834";

/* REG_NOSUB: report only success/fail (1 << 3). */
#define B7834_REG_NOSUB ((uint32_t)0x8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7834_reg_nosub_id(void)
{
	return B7834_REG_NOSUB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_nosub_id_7834 - soft REG_NOSUB cflag constant.
 *
 * Always returns 0x8 (POSIX REG_NOSUB). Catalog id only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_nosub_id_7834(void)
{
	(void)NULL;
	return b7834_reg_nosub_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_nosub_id_7834(void)
    __attribute__((alias("gj_reg_nosub_id_7834")));
