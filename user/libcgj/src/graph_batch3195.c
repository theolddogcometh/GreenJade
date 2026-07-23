/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3195: paired dyn smoke soft-ok (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_pair_ok_u(int code_a, int code_b);
 *     - Return 1 if both dyn-smoke codes are soft-green: each code is
 *       in [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for paired cgj_dyn_smoke results.
 *       Unsigned form.
 *   uint32_t __gj_dyn_pair_ok_u  (alias)
 *   __libcgj_batch3195_marker = "libcgj-batch3195"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_dyn_pair_ok_u surface only; no multi-def. Distinct from
 * gj_dyn_gate_pair_u (batch2994), gj_dyn_green_u (batch3096),
 * gj_dyn_green_pair_p (batch2694), and gj_smoke_pair_ok_u
 * (batch3194). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3195_marker[] = "libcgj-batch3195";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B3195_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if one dyn code is in soft-green band [0, B3195_DYN_GREEN_MAX]. */
static uint32_t
b3195_one_ok(int nCode)
{
if (nCode < 0) {
return 0u;
}
if (nCode > B3195_DYN_GREEN_MAX) {
return 0u;
}
return 1u;
}

/* 1u if both dyn codes are soft-green. */
static uint32_t
b3195_pair_ok(int nCodeA, int nCodeB)
{
if (b3195_one_ok(nCodeA) == 0u) {
return 0u;
}
if (b3195_one_ok(nCodeB) == 0u) {
return 0u;
}
return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_pair_ok_u - 1u iff both dyn-smoke codes are soft-green.
 *
 * code_a: first dyn-smoke result
 * code_b: second dyn-smoke result
 *
 * Soft-green band is [0, 2] for each code. Distinct from static smoke
 * pair ok (exact 0) and single-code dyn green. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_pair_ok_u(int nCodeA, int nCodeB)
{
(void)NULL;
return b3195_pair_ok(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_pair_ok_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_dyn_pair_ok_u")));
