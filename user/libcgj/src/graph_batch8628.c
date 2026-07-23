/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8628: soft dynlink R_*_JUMP_SLOT reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_jump_slot_id_8628(void);
 *     - Return soft reloc-type catalog id for JUMP_SLOT (7).
 *   uint32_t __gj_rel_jump_slot_id_8628  (alias)
 *   __libcgj_batch8628_marker = "libcgj-batch8628"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_jump_slot_id_8628 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8628_marker[] = "libcgj-batch8628";

/* Soft dynlink reloc catalog: JUMP_SLOT (PLT GOT entry). */
#define B8628_REL_JUMP_SLOT  ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8628_jump_slot_id(void)
{
	return B8628_REL_JUMP_SLOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_jump_slot_id_8628 - soft R_*_JUMP_SLOT reloc type catalog id.
 *
 * Always returns 7 (JUMP_SLOT). Pure data tag; does not bind a PLT/GOT
 * slot or perform lazy resolution. No parent wires.
 */
uint32_t
gj_rel_jump_slot_id_8628(void)
{
	(void)NULL;
	return b8628_jump_slot_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_jump_slot_id_8628(void)
    __attribute__((alias("gj_rel_jump_slot_id_8628")));
