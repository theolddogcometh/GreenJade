/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7841: regexec REG_NOTBOL eflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_notbol_id_7841(void);
 *     - Return soft REG_NOTBOL constant (0x1).
 *   uint32_t __gj_regexec_notbol_id_7841  (alias)
 *   __libcgj_batch7841_marker = "libcgj-batch7841"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_notbol_id_7841 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7841_marker[] = "libcgj-batch7841";

/* REG_NOTBOL: string head is not beginning of line (1 << 0). */
#define B7841_REG_NOTBOL ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7841_regexec_notbol_id(void)
{
	return B7841_REG_NOTBOL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_notbol_id_7841 - soft REG_NOTBOL eflag constant.
 *
 * Always returns 0x1 (POSIX/glibc REG_NOTBOL). Catalog id only; does not
 * call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_notbol_id_7841(void)
{
	(void)NULL;
	return b7841_regexec_notbol_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_notbol_id_7841(void)
    __attribute__((alias("gj_regexec_notbol_id_7841")));
