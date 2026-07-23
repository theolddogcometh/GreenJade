/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8623: soft dynlink R_*_PC32 reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_pc32_id_8623(void);
 *     - Return soft reloc-type catalog id for PC-relative 32-bit (2).
 *   uint32_t __gj_rel_pc32_id_8623  (alias)
 *   __libcgj_batch8623_marker = "libcgj-batch8623"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_pc32_id_8623 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8623_marker[] = "libcgj-batch8623";

/* Soft dynlink reloc catalog: PC-relative 32-bit. */
#define B8623_REL_PC32  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8623_pc32_id(void)
{
	return B8623_REL_PC32;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_pc32_id_8623 - soft R_*_PC32 reloc type catalog id.
 *
 * Always returns 2 (PC-relative 32-bit). Pure data tag; does not compute
 * S + A - P into a call/jump displacement. No parent wires.
 */
uint32_t
gj_rel_pc32_id_8623(void)
{
	(void)NULL;
	return b8623_pc32_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_pc32_id_8623(void)
    __attribute__((alias("gj_rel_pc32_id_8623")));
