/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3590: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3590(void);
 *     - Returns the compile-time graph batch number for this TU (3590).
 *   uint32_t __gj_batch_id_3590  (alias)
 *   __libcgj_batch3590_marker = "libcgj-batch3590"
 *
 * Exclusive continuum CREATE-ONLY wave (3581-3590:
 * utf8_lead_bytes_u, utf8_is_valid_seq_u, utf16_is_surrogate_u,
 * utf16_is_high_surr_u, utf16_is_low_surr_u, ascii_fold_char_u,
 * ascii_eq_ci_char_u, ws_is_space_u, ws_skip_u, batch_id_3590).
 * Distinct from prior batch_id_* symbols — unique surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3590_marker[] = "libcgj-batch3590";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3590_id(void)
{
	return 3590u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3590 - report this TU's graph batch number.
 *
 * Always returns 3590. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3590(void)
{
	(void)NULL;
	return b3590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3590(void)
    __attribute__((alias("gj_batch_id_3590")));
