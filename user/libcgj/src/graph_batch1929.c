/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1929: port-zero means-any predicate.
 *
 * Surface (unique symbols):
 *   int gj_port_zero_means_any_p(unsigned port);
 *     — Return 1 if port is 0 (the conventional "any / unspecified
 *       port" wildcard used by bind and many stack APIs), else 0.
 *       Pure local equality predicate.
 *   int __gj_port_zero_means_any_p  (alias)
 *   __libcgj_batch1929_marker = "libcgj-batch1929"
 *
 * Network port/service exclusive pure-data wave (1921–1930). Distinct
 * from range hints (batches 1921–1923) which exclude 0 — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1929_marker[] = "libcgj-batch1929";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is zero (any/unspecified). */
static int
b1929_is_any(unsigned uPort)
{
	return (uPort == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_zero_means_any_p — 1 if port is 0 (wildcard any).
 *
 * port: TCP/UDP port number
 * Returns 1 when port is 0, else 0.
 */
int
gj_port_zero_means_any_p(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1929_is_any(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_zero_means_any_p(unsigned uPort)
    __attribute__((alias("gj_port_zero_means_any_p")));
