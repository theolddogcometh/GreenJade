/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5470: DNS stub QTYPE validity + batch identity.
 *
 * Surface (unique symbols):
 *   int gj_dns_stub_qtype_ok_5470(uint16_t qtype);
 *     — Return 1 if qtype is a common stub-resolver type (A=1, NS=2,
 *       CNAME=5, SOA=6, PTR=12, MX=15, TXT=16, AAAA=28, SRV=33, ANY=255),
 *       else 0. Soft DNS-stub network hint; no I/O.
 *   uint32_t gj_batch_id_5470(void);
 *     — Returns the compile-time graph batch number for this TU (5470).
 *   int __gj_dns_stub_qtype_ok_5470  (alias)
 *   uint32_t __gj_batch_id_5470  (alias)
 *   __libcgj_batch5470_marker = "libcgj-batch5470"
 *
 * Milestone for the Steam network/filesystem exclusive wave (5461–5469).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5470_marker[] = "libcgj-batch5470";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5470_qtype_ok(uint16_t uType)
{
	switch (uType) {
	case 1u:   /* A */
	case 2u:   /* NS */
	case 5u:   /* CNAME */
	case 6u:   /* SOA */
	case 12u:  /* PTR */
	case 15u:  /* MX */
	case 16u:  /* TXT */
	case 28u:  /* AAAA */
	case 33u:  /* SRV */
	case 255u: /* ANY */
		return 1;
	default:
		return 0;
	}
}

static uint32_t
b5470_id(void)
{
	return 5470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_stub_qtype_ok_5470 — 1 iff qtype is a common DNS stub type.
 *
 * qtype: DNS question type field (host byte order value).
 * Returns 1 for A/NS/CNAME/SOA/PTR/MX/TXT/AAAA/SRV/ANY, else 0.
 */
int
gj_dns_stub_qtype_ok_5470(uint16_t uType)
{
	(void)NULL;
	return b5470_qtype_ok(uType);
}

/*
 * gj_batch_id_5470 — report this TU's graph batch number.
 *
 * Always returns 5470.
 */
uint32_t
gj_batch_id_5470(void)
{
	return b5470_id();
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_dns_stub_qtype_ok_5470(uint16_t uType)
    __attribute__((alias("gj_dns_stub_qtype_ok_5470")));

uint32_t __gj_batch_id_5470(void)
    __attribute__((alias("gj_batch_id_5470")));
