/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8629: soft dynlink R_*_RELATIVE reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_relative_id_8629(void);
 *     - Return soft reloc-type catalog id for RELATIVE (8).
 *   uint32_t __gj_rel_relative_id_8629  (alias)
 *   __libcgj_batch8629_marker = "libcgj-batch8629"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_relative_id_8629 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8629_marker[] = "libcgj-batch8629";

/* Soft dynlink reloc catalog: RELATIVE (base + addend). */
#define B8629_REL_RELATIVE  ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8629_relative_id(void)
{
	return B8629_REL_RELATIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_relative_id_8629 - soft R_*_RELATIVE reloc type catalog id.
 *
 * Always returns 8 (RELATIVE). Pure data tag; does not add load base
 * to a reloc addend. No parent wires.
 */
uint32_t
gj_rel_relative_id_8629(void)
{
	(void)NULL;
	return b8629_relative_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_relative_id_8629(void)
    __attribute__((alias("gj_rel_relative_id_8629")));
