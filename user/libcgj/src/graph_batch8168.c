/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8168: soft strerror catalog table size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strerror_table_n_8168(void);
 *     - Return soft named strerror row count (18).
 *   uint32_t __gj_strerror_table_n_8168  (alias)
 *   __libcgj_batch8168_marker = "libcgj-batch8168"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_strerror_table_n_8168 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8168_marker[] = "libcgj-batch8168";

/*
 * Soft catalog size: Success + 17 common errno rows matching
 * gj_strerror_known_ok_i_8167 (unknown default is not counted).
 */
#define B8168_STRERROR_TABLE_N  ((uint32_t)18u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8168_table_n(void)
{
	return B8168_STRERROR_TABLE_N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strerror_table_n_8168 - soft named strerror catalog row count.
 *
 * Always returns 18. Does not inspect or allocate real strerror tables.
 * No parent wires.
 */
uint32_t
gj_strerror_table_n_8168(void)
{
	(void)NULL;
	return b8168_table_n();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strerror_table_n_8168(void)
    __attribute__((alias("gj_strerror_table_n_8168")));
