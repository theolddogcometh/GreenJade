/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1927: DNS well-known port hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_dns_hint(unsigned port);
 *     — Return 1 if port is the conventional DNS port 53, else 0.
 *       Pure local equality data only (covers TCP and UDP DNS by
 *       port number alone).
 *   int __gj_port_is_dns_hint  (alias)
 *   __libcgj_batch1927_marker = "libcgj-batch1927"
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

const char __libcgj_batch1927_marker[] = "libcgj-batch1927";

/* Conventional DNS port. */
#define B1927_DNS  53u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port equals 53. */
static int
b1927_is_dns(unsigned uPort)
{
	return (uPort == B1927_DNS) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_dns_hint — 1 if port is 53.
 *
 * port: TCP/UDP port number
 * Returns 1 for DNS-conventional port, else 0.
 */
int
gj_port_is_dns_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1927_is_dns(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_dns_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_dns_hint")));
