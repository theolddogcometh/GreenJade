/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9470: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9470(void);
 *     - Returns the compile-time graph batch number for this TU (9470).
 *   uint32_t __gj_batch_id_9470  (alias)
 *   __libcgj_batch9470_marker = "libcgj-batch9470"
 *
 * Exclusive continuum CREATE-ONLY (9461-9470: gpgme soft id stubs —
 * key_ok, sign_ok→0, verify_ok→0, encrypt_ok→0, decrypt_ok→0,
 * import_ok→0, export_ok→0, ctx_ok, gpgme_ready, batch_id_9470).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9470_marker[] = "libcgj-batch9470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9470_id(void)
{
	return 9470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9470 - report this TU's graph batch number.
 *
 * Always returns 9470.
 */
uint32_t
gj_batch_id_9470(void)
{
	(void)NULL;
	return b9470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9470(void)
    __attribute__((alias("gj_batch_id_9470")));
