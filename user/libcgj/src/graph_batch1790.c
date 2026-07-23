/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1790: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1790(void);
 *     — Returns the compile-time graph batch number for this TU (1790).
 *   uint32_t __gj_batch_id_1790  (alias)
 *   __libcgj_batch1790_marker = "libcgj-batch1790"
 *
 * Milestone for the string-builder exclusive continuum 1781..1789
 * (struct gj_sb_t: init / remaining / append_char / append_str /
 * append_u64_dec / append_hex_u64 / append_mem / terminate / clear).
 * Does NOT redefine prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1790_marker[] = "libcgj-batch1790";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1790 — report this TU's graph batch number.
 *
 * Always returns 1790.
 */
uint32_t
gj_batch_id_1790(void)
{
	return 1790u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1790(void)
    __attribute__((alias("gj_batch_id_1790")));
