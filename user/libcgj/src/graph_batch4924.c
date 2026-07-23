/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4924: HTTP status 4xx client-error class.
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_is_4xx_u(uint32_t code);
 *     - Return 1 if code is in [400, 499] (HTTP client error);
 *       else 0. Pure integer range test; no string tables.
 *   uint32_t __gj_http_is_4xx_u  (alias)
 *   __libcgj_batch4924_marker = "libcgj-batch4924"
 *
 * Exclusive continuum CREATE-ONLY (4921-4930: http_is_1xx_u, http_is_2xx_u,
 * http_is_3xx_u, http_is_4xx_u, http_is_5xx_u, http_is_success_u,
 * http_is_error_u, http_is_redirect_u, http_class_u, batch_id_4930).
 * Distinct from gj_http_is_error_u (batch4927) and gj_http_is_5xx_u
 * (batch4925) — unique gj_http_is_4xx_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4924_marker[] = "libcgj-batch4924";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4924_is_4xx(uint32_t u32Code)
{
	if (u32Code >= 400u && u32Code <= 499u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_is_4xx_u - true if HTTP status code is client error (4xx).
 *
 * code: unsigned HTTP-ish status code
 *
 * Returns 1 when code is in [400, 499], else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_http_is_4xx_u(uint32_t u32Code)
{
	(void)NULL;
	return b4924_is_4xx(u32Code);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_is_4xx_u(uint32_t u32Code)
    __attribute__((alias("gj_http_is_4xx_u")));
