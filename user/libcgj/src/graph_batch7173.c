/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7173: dlopen RTLD_GLOBAL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_global_id_7173(void);
 *     - Return soft RTLD_GLOBAL constant (0x100).
 *   uint32_t __gj_rtld_global_id_7173  (alias)
 *   __libcgj_batch7173_marker = "libcgj-batch7173"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_global_id_7173 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7173_marker[] = "libcgj-batch7173";

/* RTLD_GLOBAL: make symbols available for subsequent loads (0x100). */
#define B7173_RTLD_GLOBAL ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7173_rtld_global_id(void)
{
	return B7173_RTLD_GLOBAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_global_id_7173 - soft RTLD_GLOBAL flag constant.
 *
 * Always returns 0x100 (POSIX/glibc RTLD_GLOBAL). Catalog id only; does
 * not call dlopen. No parent wires.
 */
uint32_t
gj_rtld_global_id_7173(void)
{
	(void)NULL;
	return b7173_rtld_global_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_global_id_7173(void)
    __attribute__((alias("gj_rtld_global_id_7173")));
