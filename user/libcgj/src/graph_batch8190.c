/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8190: setjmp/longjmp continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_longjmp_continuum_ok_8190(void);
 *     - Returns 1 (soft lamp: 8181-8190 setjmp/longjmp stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_8190(void);
 *     - Returns the compile-time graph batch number for this TU (8190).
 *   uint32_t __gj_setjmp_longjmp_continuum_ok_8190  (alias)
 *   uint32_t __gj_batch_id_8190  (alias)
 *   __libcgj_batch8190_marker = "libcgj-batch8190"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique surfaces only; no
 * multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8190_marker[] = "libcgj-batch8190";

#define B8190_CONTINUUM_OK  1u
#define B8190_BATCH_ID      8190u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8190_continuum(void)
{
	return B8190_CONTINUUM_OK;
}

static uint32_t
b8190_id(void)
{
	return B8190_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_longjmp_continuum_ok_8190 - continuum-ready tag for 8181-8190.
 *
 * Always returns 1. Soft pure-data product tag that the setjmp/longjmp
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_setjmp_longjmp_continuum_ok_8190(void)
{
	(void)NULL;
	return b8190_continuum();
}

/*
 * gj_batch_id_8190 - report this TU's graph batch number.
 *
 * Always returns 8190.
 */
uint32_t
gj_batch_id_8190(void)
{
	return b8190_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_setjmp_longjmp_continuum_ok_8190(void)
    __attribute__((alias("gj_setjmp_longjmp_continuum_ok_8190")));

uint32_t __gj_batch_id_8190(void)
    __attribute__((alias("gj_batch_id_8190")));
