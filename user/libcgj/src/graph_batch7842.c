/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7842: regexec REG_NOTEOL eflag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_noteol_id_7842(void);
 *     - Return soft REG_NOTEOL constant (0x2).
 *   uint32_t __gj_regexec_noteol_id_7842  (alias)
 *   __libcgj_batch7842_marker = "libcgj-batch7842"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_noteol_id_7842 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7842_marker[] = "libcgj-batch7842";

/* REG_NOTEOL: string tail is not end of line (1 << 1). */
#define B7842_REG_NOTEOL ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7842_regexec_noteol_id(void)
{
	return B7842_REG_NOTEOL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_noteol_id_7842 - soft REG_NOTEOL eflag constant.
 *
 * Always returns 0x2 (POSIX/glibc REG_NOTEOL). Catalog id only; does not
 * call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_noteol_id_7842(void)
{
	(void)NULL;
	return b7842_regexec_noteol_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_noteol_id_7842(void)
    __attribute__((alias("gj_regexec_noteol_id_7842")));
