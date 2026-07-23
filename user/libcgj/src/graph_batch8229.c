/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8229: string bound underflow predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_underflow_u_8229(uint32_t need);
 *     - Return 1 if need is 0 (no room/write planned — catalog underflow).
 *   uint32_t __gj_str_underflow_u_8229  (alias)
 *   __libcgj_batch8229_marker = "libcgj-batch8229"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_str_underflow_u_8229 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8229_marker[] = "libcgj-batch8229";

/* Soft minimum need: empty source still writes one NUL. */
#define B8229_MIN_NEED  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8229_str_underflow(uint32_t u32Need)
{
	/* Catalog underflow: zero need cannot hold a terminating NUL. */
	return (u32Need < B8229_MIN_NEED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_underflow_u_8229 - 1 if need is catalog-underflow (zero).
 *
 * need: octets required (payload + NUL)
 *
 * Returns 1 when need is 0 (insufficient to store even a NUL), else 0.
 * Soft catalog test. No parent wires.
 */
uint32_t
gj_str_underflow_u_8229(uint32_t u32Need)
{
	(void)NULL;
	return b8229_str_underflow(u32Need);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_underflow_u_8229(uint32_t u32Need)
    __attribute__((alias("gj_str_underflow_u_8229")));
