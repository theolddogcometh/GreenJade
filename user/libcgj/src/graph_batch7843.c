/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7843: regexec REG_STARTEND eflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_startend_id_7843(void);
 *     - Return soft REG_STARTEND constant (0x4).
 *   uint32_t __gj_regexec_startend_id_7843  (alias)
 *   __libcgj_batch7843_marker = "libcgj-batch7843"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_startend_id_7843 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7843_marker[] = "libcgj-batch7843";

/* REG_STARTEND: use pmatch[0] to bound the match string (1 << 2). */
#define B7843_REG_STARTEND ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7843_regexec_startend_id(void)
{
	return B7843_REG_STARTEND;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_startend_id_7843 - soft REG_STARTEND eflag constant.
 *
 * Always returns 0x4 (GNU/glibc REG_STARTEND). Catalog id only; does not
 * call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_startend_id_7843(void)
{
	(void)NULL;
	return b7843_regexec_startend_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_startend_id_7843(void)
    __attribute__((alias("gj_regexec_startend_id_7843")));
