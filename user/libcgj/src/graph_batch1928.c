/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1928: NTP well-known port hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_ntp_hint(unsigned port);
 *     — Return 1 if port is the conventional NTP port 123, else 0.
 *       Pure local equality data only.
 *   int __gj_port_is_ntp_hint  (alias)
 *   __libcgj_batch1928_marker = "libcgj-batch1928"
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

const char __libcgj_batch1928_marker[] = "libcgj-batch1928";

/* Conventional NTP port. */
#define B1928_NTP  123u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port equals 123. */
static int
b1928_is_ntp(unsigned uPort)
{
	return (uPort == B1928_NTP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_ntp_hint — 1 if port is 123.
 *
 * port: TCP/UDP port number
 * Returns 1 for NTP-conventional port, else 0.
 */
int
gj_port_is_ntp_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1928_is_ntp(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_ntp_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_ntp_hint")));
