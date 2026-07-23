/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8625: soft dynlink R_*_PLT32 reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_plt_id_8625(void);
 *     - Return soft reloc-type catalog id for PLT32 (4).
 *   uint32_t __gj_rel_plt_id_8625  (alias)
 *   __libcgj_batch8625_marker = "libcgj-batch8625"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_plt_id_8625 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8625_marker[] = "libcgj-batch8625";

/* Soft dynlink reloc catalog: PLT32 (PC-relative to PLT entry). */
#define B8625_REL_PLT  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8625_plt_id(void)
{
	return B8625_REL_PLT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_plt_id_8625 - soft R_*_PLT32 reloc type catalog id.
 *
 * Always returns 4 (PLT32). Pure data tag; does not patch a call site
 * or allocate a PLT slot. No parent wires.
 */
uint32_t
gj_rel_plt_id_8625(void)
{
	(void)NULL;
	return b8625_plt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_plt_id_8625(void)
    __attribute__((alias("gj_rel_plt_id_8625")));
