/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8185: setjmp plain-vs-sig surface id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_sig_vs_plain_id_8185(uint32_t use_sig);
 *     - Return 2 if use_sig nonzero (sigsetjmp/siglongjmp), else 1
 *       (setjmp/longjmp plain).
 *   uint32_t __gj_setjmp_sig_vs_plain_id_8185  (alias)
 *   __libcgj_batch8185_marker = "libcgj-batch8185"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_setjmp_sig_vs_plain_id_8185 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8185_marker[] = "libcgj-batch8185";

/* Soft family ids: 1 = plain setjmp/longjmp, 2 = sigsetjmp/siglongjmp. */
#define B8185_PLAIN_ID  1u
#define B8185_SIG_ID    2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8185_family_id(uint32_t u32UseSig)
{
	if (u32UseSig != 0u)
		return B8185_SIG_ID;
	return B8185_PLAIN_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_sig_vs_plain_id_8185 - family id for plain vs signal setjmp.
 *
 * use_sig: nonzero selects sigsetjmp/siglongjmp family, else plain
 *
 * Returns 2 for signal family, 1 for plain. Soft pure tag only; does
 * not call libc. No parent wires.
 */
uint32_t
gj_setjmp_sig_vs_plain_id_8185(uint32_t u32UseSig)
{
	(void)NULL;
	return b8185_family_id(u32UseSig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setjmp_sig_vs_plain_id_8185(uint32_t u32UseSig)
    __attribute__((alias("gj_setjmp_sig_vs_plain_id_8185")));
