/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1921: ephemeral (dynamic) port-range hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_ephemeral_hint(unsigned port);
 *     — Return 1 if port is in the IANA dynamic/private (ephemeral)
 *       range 49152..65535 inclusive, else 0. Pure local range data;
 *       not a bind/connect policy oracle.
 *   int __gj_port_is_ephemeral_hint  (alias)
 *   __libcgj_batch1921_marker = "libcgj-batch1921"
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

const char __libcgj_batch1921_marker[] = "libcgj-batch1921";

/* IANA dynamic/private port range (ephemeral). */
#define B1921_EPH_LO  49152u
#define B1921_EPH_HI  65535u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is in [B1921_EPH_LO, B1921_EPH_HI]. */
static int
b1921_is_eph(unsigned uPort)
{
	return (uPort >= B1921_EPH_LO && uPort <= B1921_EPH_HI) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_ephemeral_hint — 1 if port is 49152..65535.
 *
 * port: TCP/UDP port number (0..65535 typical; values above still fail)
 * Returns 1 in ephemeral range, else 0.
 */
int
gj_port_is_ephemeral_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1921_is_eph(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_ephemeral_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_ephemeral_hint")));
