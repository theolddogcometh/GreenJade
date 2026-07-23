/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3196: shell product ok extract (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_ok_u(uint32_t flags);
 *     - Return 1 if flags has the product shell ready/ok bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime probe of
 *       greenjade-shell. Unsigned form.
 *   uint32_t __gj_shell_ok_u  (alias)
 *   __libcgj_batch3196_marker = "libcgj-batch3196"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_shell_ok_u surface only; no multi-def. Distinct from
 * gj_shell_green_u (batch3097), gj_shell_ready_u (batch2995),
 * gj_product_shell_ok_u (batch2896), and gj_libcgj_ok_u (batch3197).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3196_marker[] = "libcgj-batch3196";

/* Product shell ready/ok lamp bit (bit0). */
#define B3196_SHELL_OK_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell ok bit is set in flags. */
static uint32_t
b3196_ok_u(uint32_t uFlags)
{
if ((uFlags & B3196_SHELL_OK_BIT) != 0u) {
return 1u;
}
return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ok_u - 1u iff product shell ready/ok bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell ok)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_ok_u(uint32_t uFlags)
{
(void)NULL;
return b3196_ok_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_ok_u(uint32_t uFlags)
    __attribute__((alias("gj_shell_ok_u")));
