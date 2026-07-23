/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3197: libcgj product ok extract (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_ok_u(uint32_t flags);
 *     - Return 1 if flags has the product libcgj ready/ok bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime dlopen
 *       or soname probe. Unsigned form.
 *   uint32_t __gj_libcgj_ok_u  (alias)
 *   __libcgj_batch3197_marker = "libcgj-batch3197"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_libcgj_ok_u surface only; no multi-def. Distinct from
 * gj_libcgj_green_u (batch3098), gj_libcgj_ready_u (batch2996),
 * gj_product_libcgj_ok_u (batch2897), and gj_shell_ok_u (batch3196).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3197_marker[] = "libcgj-batch3197";

/* Product libcgj ready/ok lamp bit (bit0). */
#define B3197_LIBCGJ_OK_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if libcgj ok bit is set in flags. */
static uint32_t
b3197_ok_u(uint32_t uFlags)
{
if ((uFlags & B3197_LIBCGJ_OK_BIT) != 0u) {
return 1u;
}
return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_ok_u - 1u iff product libcgj ready/ok bit is set.
 *
 * flags: product readiness bitmask (bit0 = libcgj ok)
 *
 * Returns 1 when bit0 is set, else 0. Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_ok_u(uint32_t uFlags)
{
(void)NULL;
return b3197_ok_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_ok_u(uint32_t uFlags)
    __attribute__((alias("gj_libcgj_ok_u")));
