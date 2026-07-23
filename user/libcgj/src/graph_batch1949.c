/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1949: span-id soft acceptability hint.
 *
 * Surface (unique symbols):
 *   int gj_span_id_ok_hint(uint64_t span_id);
 *     — Return 1 if span_id is a soft-valid 64-bit trace span
 *       identifier (non-zero and not all-ones), else 0. Catalog-free
 *       policy hint only; does not allocate or consult a tracer.
 *   int __gj_span_id_ok_hint  (alias)
 *   __libcgj_batch1949_marker = "libcgj-batch1949"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1949_marker[] = "libcgj-batch1949";

/* Reserved invalid span ids used as soft policy sentinels. */
#define B1949_SPAN_ZERO  0ull
#define B1949_SPAN_ALL1  0xffffffffffffffffull

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uSpan is neither zero nor all-ones. */
static int
b1949_span_ok(uint64_t uSpan)
{
	if (uSpan == B1949_SPAN_ZERO) {
		return 0;
	}
	if (uSpan == B1949_SPAN_ALL1) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_span_id_ok_hint — 1 if span_id looks soft-valid.
 *
 * span_id: 64-bit distributed-trace span identifier
 * Returns 1 when non-zero and not all-ones, else 0.
 */
int
gj_span_id_ok_hint(uint64_t uSpan)
{
	(void)NULL;
	return b1949_span_ok(uSpan);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_span_id_ok_hint(uint64_t uSpan)
    __attribute__((alias("gj_span_id_ok_hint")));
