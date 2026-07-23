/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7511: waitpid WNOHANG option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_wnohang_id_7511(void);
 *     - Return soft WNOHANG waitpid option class (1).
 *   uint32_t __gj_waitpid_wnohang_id_7511  (alias)
 *   __libcgj_batch7511_marker = "libcgj-batch7511"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_wnohang_id_7511 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7511_marker[] = "libcgj-batch7511";

/* Soft WNOHANG: return immediately if no child has exited. */
#define B7511_WNOHANG ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7511_waitpid_wnohang_id(void)
{
	return B7511_WNOHANG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_wnohang_id_7511 - soft WNOHANG waitpid option constant.
 *
 * Always returns 1. Catalog id only; does not call waitpid.
 * No parent wires.
 */
uint32_t
gj_waitpid_wnohang_id_7511(void)
{
	(void)NULL;
	return b7511_waitpid_wnohang_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_wnohang_id_7511(void)
    __attribute__((alias("gj_waitpid_wnohang_id_7511")));
