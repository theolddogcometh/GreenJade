/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8634: auxv AT_PHNUM key id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_at_phnum_id_8634(void);
 *     - Return soft auxv key id for AT_PHNUM (5).
 *   uint32_t __gj_at_phnum_id_8634  (alias)
 *   __libcgj_batch8634_marker = "libcgj-batch8634"
 *
 * Exclusive continuum CREATE-ONLY (8631-8640: auxv key id stubs —
 * at_null_id, at_phdr_id, at_phent_id, at_phnum_id, at_pagesz_id,
 * at_base_id, at_entry_id, at_uid_id, at_random_id, batch_id_8640).
 * Unique gj_at_phnum_id_8634 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8634_marker[] = "libcgj-batch8634";

/* Soft auxv key id matching Linux AT_PHNUM. */
#define B8634_AT_PHNUM  ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8634_at_phnum_id(void)
{
	return B8634_AT_PHNUM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_at_phnum_id_8634 - soft AT_PHNUM auxv key id.
 *
 * Always returns 5. Pure constant tag; no parent wires. Does not call
 * getauxval or walk the auxiliary vector.
 */
uint32_t
gj_at_phnum_id_8634(void)
{
	(void)NULL;
	return b8634_at_phnum_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_at_phnum_id_8634(void)
    __attribute__((alias("gj_at_phnum_id_8634")));
