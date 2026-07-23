/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8580: iconv charset id continuum batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8580(void);
 *     - Returns the compile-time graph batch number for this TU (8580).
 *   uint32_t __gj_batch_id_8580  (alias)
 *   __libcgj_batch8580_marker = "libcgj-batch8580"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8580_marker[] = "libcgj-batch8580";

#define B8580_BATCH_ID  8580u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8580_id(void)
{
	return B8580_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8580 - report this TU's graph batch number.
 *
 * Always returns 8580.
 */
uint32_t
gj_batch_id_8580(void)
{
	(void)NULL;
	return b8580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8580(void)
    __attribute__((alias("gj_batch_id_8580")));
