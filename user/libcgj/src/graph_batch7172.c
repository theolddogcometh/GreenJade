/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7172: dlopen RTLD_NOW flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_now_id_7172(void);
 *     - Return soft RTLD_NOW constant (0x2).
 *   uint32_t __gj_rtld_now_id_7172  (alias)
 *   __libcgj_batch7172_marker = "libcgj-batch7172"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_now_id_7172 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7172_marker[] = "libcgj-batch7172";

/* RTLD_NOW: resolve all symbols at load time (0x2). */
#define B7172_RTLD_NOW ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7172_rtld_now_id(void)
{
	return B7172_RTLD_NOW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_now_id_7172 - soft RTLD_NOW flag constant.
 *
 * Always returns 0x2 (POSIX/glibc RTLD_NOW). Catalog id only; does not
 * call dlopen. No parent wires.
 */
uint32_t
gj_rtld_now_id_7172(void)
{
	(void)NULL;
	return b7172_rtld_now_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_now_id_7172(void)
    __attribute__((alias("gj_rtld_now_id_7172")));
