/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4929: HTTP status class digit (code/100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_class_u(uint32_t code);
 *     - Return the integer class of an HTTP-ish status code as
 *       code / 100 (e.g. 404 -> 4, 200 -> 2, 99 -> 0). Pure unsigned
 *       division; no range clamping.
 *   uint32_t __gj_http_class_u  (alias)
 *   __libcgj_batch4929_marker = "libcgj-batch4929"
 *
 * Exclusive continuum CREATE-ONLY (4921-4930: http_is_1xx_u, http_is_2xx_u,
 * http_is_3xx_u, http_is_4xx_u, http_is_5xx_u, http_is_success_u,
 * http_is_error_u, http_is_redirect_u, http_class_u, batch_id_4930).
 * Distinct from gj_http_is_*_u class predicates (batch4921-4928) —
 * unique gj_http_class_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4929_marker[] = "libcgj-batch4929";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4929_class(uint32_t u32Code)
{
	return u32Code / 100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_class_u - HTTP status class as the hundreds digit (code/100).
 *
 * code: unsigned HTTP-ish status code
 *
 * Returns code / 100 (unsigned integer division). Examples:
 * gj_http_class_u(100u) -> 1; gj_http_class_u(404u) -> 4;
 * gj_http_class_u(99u) -> 0. Self-contained; no parent wires.
 */
uint32_t
gj_http_class_u(uint32_t u32Code)
{
	(void)NULL;
	return b4929_class(u32Code);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_class_u(uint32_t u32Code)
    __attribute__((alias("gj_http_class_u")));
