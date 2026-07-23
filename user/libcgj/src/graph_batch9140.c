/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9140: archive/zip soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9140(void);
 *     - Returns the compile-time graph batch number for this TU (9140).
 *   uint32_t __gj_batch_id_9140  (alias)
 *   __libcgj_batch9140_marker = "libcgj-batch9140"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9140_marker[] = "libcgj-batch9140";

#define B9140_BATCH_ID  9140u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9140_id(void)
{
	return B9140_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9140 - report this TU's graph batch number.
 *
 * Always returns 9140. Link-time presence tags the exclusive
 * archive/zip soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9140(void)
{
	(void)NULL;
	return b9140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9140(void)
    __attribute__((alias("gj_batch_id_9140")));
