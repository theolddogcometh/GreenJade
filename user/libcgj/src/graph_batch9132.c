/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9132: soft ZIP central-directory signature id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_central_sig_id_9132(void);
 *     - Returns 0x02014B50 (PK\\x01\\x02 central file header soft catalog
 *       value). Pure-data signature id stub; does not walk CD or extract.
 *   uint32_t __gj_zip_central_sig_id_9132  (alias)
 *   __libcgj_batch9132_marker = "libcgj-batch9132"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. Distinct from
 * gj_zip_local_sig_id_9131 (batch9131). No parent wires. No __int128.
 * No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9132_marker[] = "libcgj-batch9132";

/* Soft ZIP central file header signature (PK\\x01\\x02, LE). */
#define B9132_ZIP_CENTRAL_SIG  0x02014B50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9132_central_sig(void)
{
	return B9132_ZIP_CENTRAL_SIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_central_sig_id_9132 - soft ZIP central-directory signature id.
 *
 * Always returns 0x02014B50. Soft continuum constant; does not walk
 * central directory or extract entries. No parent wires.
 */
uint32_t
gj_zip_central_sig_id_9132(void)
{
	(void)NULL;
	return b9132_central_sig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_central_sig_id_9132(void)
    __attribute__((alias("gj_zip_central_sig_id_9132")));
