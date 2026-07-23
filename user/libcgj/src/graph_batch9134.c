/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9134: soft ZIP store compression method id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_store_id_9134(void);
 *     - Returns 0 (ZIP method STORE soft catalog value). Pure-data
 *       method id stub; does not extract or copy stored payloads.
 *   uint32_t __gj_zip_store_id_9134  (alias)
 *   __libcgj_batch9134_marker = "libcgj-batch9134"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9134_marker[] = "libcgj-batch9134";

/* Soft ZIP compression method STORE (0). */
#define B9134_ZIP_STORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9134_store_id(void)
{
	return B9134_ZIP_STORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_store_id_9134 - soft ZIP STORE method catalog id.
 *
 * Always returns 0. Soft continuum constant; does not extract stored
 * entries or touch archive bytes. No parent wires.
 */
uint32_t
gj_zip_store_id_9134(void)
{
	(void)NULL;
	return b9134_store_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_store_id_9134(void)
    __attribute__((alias("gj_zip_store_id_9134")));
