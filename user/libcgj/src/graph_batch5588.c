/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5588: product legal-accepted flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_legal_ok_5588(void);
 *     - Returns 1 (product legal surface tagged ready / accepted for
 *       this continuum). Soft compile-time product legal flag.
 *   uint32_t __gj_product_legal_ok_5588  (alias)
 *   __libcgj_batch5588_marker = "libcgj-batch5588"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_product_legal_ok_5588 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5588_marker[] = "libcgj-batch5588";

/* Product legal OK lamp for wave 5581-5590. */
#define B5588_PRODUCT_LEGAL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5588_legal_ok(void)
{
	return B5588_PRODUCT_LEGAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_legal_ok_5588 - report product legal-accepted flag.
 *
 * Always returns 1 (legal surface tagged ok). Soft pure-data only;
 * does not present UI or parse licenses. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_product_legal_ok_5588(void)
{
	(void)NULL;
	return b5588_legal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_legal_ok_5588(void)
    __attribute__((alias("gj_product_legal_ok_5588")));
