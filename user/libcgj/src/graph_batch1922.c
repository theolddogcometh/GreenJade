/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1922: system (well-known) port-range hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_system_hint(unsigned port);
 *     — Return 1 if port is in the classic system/well-known range
 *       1..1023 inclusive, else 0. Port 0 is excluded (see batch1929
 *       for "any" semantics). Pure local range data only.
 *   int __gj_port_is_system_hint  (alias)
 *   __libcgj_batch1922_marker = "libcgj-batch1922"
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

const char __libcgj_batch1922_marker[] = "libcgj-batch1922";

/* Classic system / well-known port range (excludes 0). */
#define B1922_SYS_LO  1u
#define B1922_SYS_HI  1023u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is in [B1922_SYS_LO, B1922_SYS_HI]. */
static int
b1922_is_sys(unsigned uPort)
{
	return (uPort >= B1922_SYS_LO && uPort <= B1922_SYS_HI) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_system_hint — 1 if port is 1..1023.
 *
 * port: TCP/UDP port number
 * Returns 1 in system/well-known range, else 0.
 */
int
gj_port_is_system_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1922_is_sys(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_system_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_system_hint")));
