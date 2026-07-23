/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8626: soft dynlink R_*_COPY reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_copy_id_8626(void);
 *     - Return soft reloc-type catalog id for COPY (5).
 *   uint32_t __gj_rel_copy_id_8626  (alias)
 *   __libcgj_batch8626_marker = "libcgj-batch8626"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_copy_id_8626 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8626_marker[] = "libcgj-batch8626";

/* Soft dynlink reloc catalog: COPY (object copy into executable BSS). */
#define B8626_REL_COPY  ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8626_copy_id(void)
{
	return B8626_REL_COPY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_copy_id_8626 - soft R_*_COPY reloc type catalog id.
 *
 * Always returns 5 (COPY). Pure data tag; does not copy object bytes
 * from a shared library into the main image. No parent wires.
 */
uint32_t
gj_rel_copy_id_8626(void)
{
	(void)NULL;
	return b8626_copy_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_copy_id_8626(void)
    __attribute__((alias("gj_rel_copy_id_8626")));
