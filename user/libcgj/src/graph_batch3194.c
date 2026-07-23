/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3194: paired static smoke ok (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_pair_ok_u(int code_a, int code_b);
 *     - Return 1 if both static smoke codes are strict green PASS
 *       (exactly 0), else 0. Soft pure-data gate for paired static
 *       smoke lamps (e.g. host + guest static smoke). Unsigned form.
 *   uint32_t __gj_smoke_pair_ok_u  (alias)
 *   __libcgj_batch3194_marker = "libcgj-batch3194"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_smoke_pair_ok_u surface only; no multi-def. Distinct from
 * gj_smoke_gate_pair_u (batch2993), gj_smoke_green_u (batch3095),
 * gj_smoke_green_pair_p (batch2693), and gj_dyn_pair_ok_u
 * (batch3195). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3194_marker[] = "libcgj-batch3194";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if both codes are strict green PASS (exactly 0). */
static uint32_t
b3194_pair_ok(int nCodeA, int nCodeB)
{
if (nCodeA != 0) {
return 0u;
}
if (nCodeB != 0) {
return 0u;
}
return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_pair_ok_u - 1u iff both static smoke codes are green PASS.
 *
 * code_a: first static smoke result
 * code_b: second static smoke result
 *
 * Green PASS is exactly 0 for both. Pair with gj_dyn_pair_ok_u for the
 * dyn soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_pair_ok_u(int nCodeA, int nCodeB)
{
(void)NULL;
return b3194_pair_ok(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_pair_ok_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_smoke_pair_ok_u")));
