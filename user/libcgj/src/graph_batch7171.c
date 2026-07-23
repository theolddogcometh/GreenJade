/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7171: dlopen RTLD_LAZY flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_lazy_id_7171(void);
 *     - Return soft RTLD_LAZY constant (0x1).
 *   uint32_t __gj_rtld_lazy_id_7171  (alias)
 *   __libcgj_batch7171_marker = "libcgj-batch7171"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_lazy_id_7171 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7171_marker[] = "libcgj-batch7171";

/* RTLD_LAZY: resolve symbols as needed (0x1). */
#define B7171_RTLD_LAZY ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7171_rtld_lazy_id(void)
{
	return B7171_RTLD_LAZY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_lazy_id_7171 - soft RTLD_LAZY flag constant.
 *
 * Always returns 0x1 (POSIX/glibc RTLD_LAZY). Catalog id only; does not
 * call dlopen. No parent wires.
 */
uint32_t
gj_rtld_lazy_id_7171(void)
{
	(void)NULL;
	return b7171_rtld_lazy_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_lazy_id_7171(void)
    __attribute__((alias("gj_rtld_lazy_id_7171")));
