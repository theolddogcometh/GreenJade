/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7831: regex REG_EXTENDED cflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_extended_id_7831(void);
 *     - Return soft REG_EXTENDED constant (0x1).
 *   uint32_t __gj_reg_extended_id_7831  (alias)
 *   __libcgj_batch7831_marker = "libcgj-batch7831"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_extended_id_7831 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7831_marker[] = "libcgj-batch7831";

/* REG_EXTENDED: use extended regular expressions (1 << 0). */
#define B7831_REG_EXTENDED ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7831_reg_extended_id(void)
{
	return B7831_REG_EXTENDED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_extended_id_7831 - soft REG_EXTENDED cflag constant.
 *
 * Always returns 0x1 (POSIX REG_EXTENDED). Catalog id only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_extended_id_7831(void)
{
	(void)NULL;
	return b7831_reg_extended_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_extended_id_7831(void)
    __attribute__((alias("gj_reg_extended_id_7831")));
