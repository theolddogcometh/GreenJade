/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7848: regexec eflags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_eflags_pack_u_7848(uint32_t want_notbol,
 *                                          uint32_t want_noteol,
 *                                          uint32_t want_startend);
 *     - Pack soft REG_* eflag wants into a mask (0x1|0x2|0x4).
 *   uint32_t __gj_regexec_eflags_pack_u_7848  (alias)
 *   __libcgj_batch7848_marker = "libcgj-batch7848"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_eflags_pack_u_7848 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7848_marker[] = "libcgj-batch7848";

/* Soft pack bits matching POSIX/glibc regexec eflags layout. */
#define B7848_REG_NOTBOL   ((uint32_t)0x1u)
#define B7848_REG_NOTEOL   ((uint32_t)0x2u)
#define B7848_REG_STARTEND ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7848_regexec_eflags_pack(uint32_t u32WantNotbol,
                          uint32_t u32WantNoteol,
                          uint32_t u32WantStartend)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantNotbol != 0u)
		u32Pack |= B7848_REG_NOTBOL;
	if (u32WantNoteol != 0u)
		u32Pack |= B7848_REG_NOTEOL;
	if (u32WantStartend != 0u)
		u32Pack |= B7848_REG_STARTEND;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_eflags_pack_u_7848 - soft-pack regexec eflag wants.
 *
 * want_notbol:   non-zero to set REG_NOTBOL (0x1)
 * want_noteol:   non-zero to set REG_NOTEOL (0x2)
 * want_startend: non-zero to set REG_STARTEND (0x4)
 *
 * Returns bitmask of requested REG_* eflag bits. Pure integer pack; does
 * not call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_eflags_pack_u_7848(uint32_t u32WantNotbol,
                              uint32_t u32WantNoteol,
                              uint32_t u32WantStartend)
{
	(void)NULL;
	return b7848_regexec_eflags_pack(u32WantNotbol, u32WantNoteol,
	                                 u32WantStartend);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_eflags_pack_u_7848(uint32_t u32WantNotbol,
                                         uint32_t u32WantNoteol,
                                         uint32_t u32WantStartend)
    __attribute__((alias("gj_regexec_eflags_pack_u_7848")));
