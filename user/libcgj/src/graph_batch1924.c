/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1924: HTTP well-known port hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_http_hint(unsigned port);
 *     — Return 1 if port is the conventional HTTP cleartext port 80
 *       or the common alternate 8080, else 0. Pure local equality
 *       data; not a protocol sniffer.
 *   int __gj_port_is_http_hint  (alias)
 *   __libcgj_batch1924_marker = "libcgj-batch1924"
 *
 * Network port/service exclusive pure-data wave (1921–1930). Distinct
 * names — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1924_marker[] = "libcgj-batch1924";

/* Conventional HTTP ports. */
#define B1924_HTTP     80u
#define B1924_HTTP_ALT 8080u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is 80 or 8080. */
static int
b1924_is_http(unsigned uPort)
{
	return (uPort == B1924_HTTP || uPort == B1924_HTTP_ALT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_http_hint — 1 if port is 80 or 8080.
 *
 * port: TCP/UDP port number
 * Returns 1 for HTTP-conventional ports, else 0.
 */
int
gj_port_is_http_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1924_is_http(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_http_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_http_hint")));
