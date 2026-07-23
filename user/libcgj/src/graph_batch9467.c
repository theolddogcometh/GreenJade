/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9467: GPGME soft export-ok id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpg_export_ok_u_9467(void);
 *     - Always returns 0 (soft: export path not live).
 *   uint32_t __gj_gpg_export_ok_u_9467  (alias)
 *   __libcgj_batch9467_marker = "libcgj-batch9467"
 *
 * Exclusive continuum CREATE-ONLY (9461-9470: gpgme soft id stubs —
 * key_ok, sign_ok→0, verify_ok→0, encrypt_ok→0, decrypt_ok→0,
 * import_ok→0, export_ok→0, ctx_ok, gpgme_ready, batch_id_9470).
 * Unique gj_gpg_export_ok_u_9467 surface only; no multi-def.
 * Soft pure-data catalog only; no real GPGME, no gpgme_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9467_marker[] = "libcgj-batch9467";

/* Soft GPGME export path not-ok (freestanding: no live crypto). */
#define B9467_GPG_EXPORT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9467_gpg_export_ok(void)
{
	return B9467_GPG_EXPORT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpg_export_ok_u_9467 - soft GPGME export-ok tag.
 *
 * Always returns 0. Soft catalog only; does not export keys or call
 * gpgme_*. No parent wires.
 */
uint32_t
gj_gpg_export_ok_u_9467(void)
{
	(void)NULL;
	return b9467_gpg_export_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpg_export_ok_u_9467(void)
    __attribute__((alias("gj_gpg_export_ok_u_9467")));
