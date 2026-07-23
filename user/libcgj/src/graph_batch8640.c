/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8640: auxv key id continuum batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8640(void);
 *     - Returns the compile-time graph batch number for this TU (8640).
 *   uint32_t __gj_batch_id_8640  (alias)
 *   __libcgj_batch8640_marker = "libcgj-batch8640"
 *
 * Exclusive continuum CREATE-ONLY (8631-8640: auxv key id stubs —
 * at_null_id, at_phdr_id, at_phent_id, at_phnum_id, at_pagesz_id,
 * at_base_id, at_entry_id, at_uid_id, at_random_id, batch_id_8640).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8640_marker[] = "libcgj-batch8640";

#define B8640_BATCH_ID  8640u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8640_id(void)
{
	return B8640_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8640 - report this TU's graph batch number.
 *
 * Always returns 8640.
 */
uint32_t
gj_batch_id_8640(void)
{
	(void)NULL;
	return b8640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8640(void)
    __attribute__((alias("gj_batch_id_8640")));
