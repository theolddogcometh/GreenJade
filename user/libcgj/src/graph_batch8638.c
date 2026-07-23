/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8638: auxv AT_UID key id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_at_uid_id_8638(void);
 *     - Return soft auxv key id for AT_UID (11).
 *   uint32_t __gj_at_uid_id_8638  (alias)
 *   __libcgj_batch8638_marker = "libcgj-batch8638"
 *
 * Exclusive continuum CREATE-ONLY (8631-8640: auxv key id stubs —
 * at_null_id, at_phdr_id, at_phent_id, at_phnum_id, at_pagesz_id,
 * at_base_id, at_entry_id, at_uid_id, at_random_id, batch_id_8640).
 * Unique gj_at_uid_id_8638 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8638_marker[] = "libcgj-batch8638";

/* Soft auxv key id matching Linux AT_UID. */
#define B8638_AT_UID  ((uint32_t)11u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8638_at_uid_id(void)
{
	return B8638_AT_UID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_at_uid_id_8638 - soft AT_UID auxv key id.
 *
 * Always returns 11. Pure constant tag; no parent wires. Does not call
 * getauxval or walk the auxiliary vector.
 */
uint32_t
gj_at_uid_id_8638(void)
{
	(void)NULL;
	return b8638_at_uid_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_at_uid_id_8638(void)
    __attribute__((alias("gj_at_uid_id_8638")));
