/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7516: waitpid has-WNOHANG option stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_has_wnohang_u_7516(uint32_t opts);
 *     - Return 1 if opts has WNOHANG (bit value 1) set, else 0.
 *   uint32_t __gj_waitpid_has_wnohang_u_7516  (alias)
 *   __libcgj_batch7516_marker = "libcgj-batch7516"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_has_wnohang_u_7516 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7516_marker[] = "libcgj-batch7516";

/* Soft WNOHANG bit in waitpid options. */
#define B7516_WNOHANG ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7516_waitpid_has_wnohang(uint32_t u32Opts)
{
	return ((u32Opts & B7516_WNOHANG) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_has_wnohang_u_7516 - 1 if waitpid opts includes WNOHANG.
 *
 * opts: soft waitpid options bitmask
 *
 * Returns 1 when bit value 1 is set, else 0.
 * Soft bit test only; does not call waitpid. No parent wires.
 */
uint32_t
gj_waitpid_has_wnohang_u_7516(uint32_t u32Opts)
{
	(void)NULL;
	return b7516_waitpid_has_wnohang(u32Opts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_has_wnohang_u_7516(uint32_t u32Opts)
    __attribute__((alias("gj_waitpid_has_wnohang_u_7516")));
