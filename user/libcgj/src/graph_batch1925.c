/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1925: HTTPS well-known port hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_https_hint(unsigned port);
 *     — Return 1 if port is the conventional HTTPS/TLS port 443,
 *       else 0. Pure local equality data only.
 *   int __gj_port_is_https_hint  (alias)
 *   __libcgj_batch1925_marker = "libcgj-batch1925"
 *
 * Network port/service exclusive pure-data wave (1921–1930). Distinct
 * from gj_port_is_http_hint (batch1924) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1925_marker[] = "libcgj-batch1925";

/* Conventional HTTPS / TLS port. */
#define B1925_HTTPS  443u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port equals 443. */
static int
b1925_is_https(unsigned uPort)
{
	return (uPort == B1925_HTTPS) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_https_hint — 1 if port is 443.
 *
 * port: TCP/UDP port number
 * Returns 1 for HTTPS-conventional port, else 0.
 */
int
gj_port_is_https_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1925_is_https(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_https_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_https_hint")));
