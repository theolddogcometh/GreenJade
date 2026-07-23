/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7515: waitpid options mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_opts_mask_id_7515(void);
 *     - Return soft classic waitpid options mask (0xb).
 *   uint32_t __gj_waitpid_opts_mask_id_7515  (alias)
 *   __libcgj_batch7515_marker = "libcgj-batch7515"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_opts_mask_id_7515 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7515_marker[] = "libcgj-batch7515";

/*
 * Soft classic waitpid options mask:
 *   WNOHANG(1) | WUNTRACED(2) | WCONTINUED(8) => 0xb
 */
#define B7515_WAITPID_OPTS_MASK ((uint32_t)0xbu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7515_waitpid_opts_mask_id(void)
{
	return B7515_WAITPID_OPTS_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_opts_mask_id_7515 - soft classic waitpid options mask.
 *
 * Always returns 0xb (WNOHANG|WUNTRACED|WCONTINUED). Catalog mask only;
 * does not call waitpid. No parent wires.
 */
uint32_t
gj_waitpid_opts_mask_id_7515(void)
{
	(void)NULL;
	return b7515_waitpid_opts_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_opts_mask_id_7515(void)
    __attribute__((alias("gj_waitpid_opts_mask_id_7515")));
