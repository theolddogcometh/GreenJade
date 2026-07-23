/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7519: waitpid options errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_opts_errorish_u_7519(uint32_t opts);
 *     - Return 1 if opts has bits outside classic waitpid mask (0xb),
 *       else 0.
 *   uint32_t __gj_waitpid_opts_errorish_u_7519  (alias)
 *   __libcgj_batch7519_marker = "libcgj-batch7519"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_opts_errorish_u_7519 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7519_marker[] = "libcgj-batch7519";

/* Soft classic waitpid options mask: WNOHANG|WUNTRACED|WCONTINUED. */
#define B7519_WAITPID_OPTS_MASK ((uint32_t)0xbu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7519_waitpid_opts_errorish(uint32_t u32Opts)
{
	if ((u32Opts & ~B7519_WAITPID_OPTS_MASK) != 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_opts_errorish_u_7519 - 1 if opts looks invalid for waitpid.
 *
 * opts: soft waitpid options bitmask
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x8} is set; else 0.
 * Soft inverse of opts_ok; does not call waitpid. No parent wires.
 */
uint32_t
gj_waitpid_opts_errorish_u_7519(uint32_t u32Opts)
{
	(void)NULL;
	return b7519_waitpid_opts_errorish(u32Opts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_opts_errorish_u_7519(uint32_t u32Opts)
    __attribute__((alias("gj_waitpid_opts_errorish_u_7519")));
