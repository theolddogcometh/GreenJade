/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch158: PCI BDF address parse.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * No third-party PCI util source was copied. Builds with -msse2.
 *
 * Surface (unique symbols):
 *   int gj_pci_parse_bdf(const char *s, unsigned *domain, unsigned *bus,
 *                        unsigned *dev, unsigned *func);
 *     Parse a PCI location string in Linux sysfs style:
 *       DDDD:BB:DD.F   e.g.  "0000:00:1f.0"
 *     Also accepts abbreviated form without domain (domain defaults to 0):
 *       BB:DD.F        e.g.  "00:1f.0"
 *     Fields are hexadecimal. Leading zeros optional within each field.
 *     Range checks (PCI geometry):
 *       domain  0..0xffff
 *       bus     0..0xff
 *       device  0..0x1f   (5-bit slot)
 *       function 0..0x7   (3-bit function)
 *     On success writes domain, bus, dev, and func outs and returns 0.
 *     On failure returns -1 and sets errno = EINVAL (NULL args / malformed
 *     string / out-of-range field / trailing garbage).
 *   __gj_pci_parse_bdf  (alias)
 *   __libcgj_batch158_marker = "libcgj-batch158"
 */

#include <errno.h>
#include <stddef.h>

const char __libcgj_batch158_marker[] = "libcgj-batch158";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b158_hex_val(unsigned char ch)
{
	if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') {
		return (int)(ch - (unsigned char)'0');
	}
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'F') {
		return (int)(ch - (unsigned char)'A') + 10;
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'f') {
		return (int)(ch - (unsigned char)'a') + 10;
	}
	return -1;
}

/*
 * Parse 1..nMaxDigits hex digits from *pp into *pVal.
 * Requires at least one digit. Rejects overflow beyond nMaxDigits.
 * Advances *pp past the digits. Returns 0 on success, -1 on failure.
 */
static int
b158_parse_hex(const char **pp, unsigned *pVal, unsigned nMaxDigits)
{
	const char *p = *pp;
	unsigned u = 0u;
	unsigned n = 0u;
	int d;

	d = b158_hex_val((unsigned char)*p);
	if (d < 0) {
		return -1;
	}
	while ((d = b158_hex_val((unsigned char)*p)) >= 0) {
		n++;
		if (n > nMaxDigits) {
			return -1;
		}
		u = (u << 4) | (unsigned)d;
		p++;
	}
	*pVal = u;
	*pp = p;
	return 0;
}

static int
b158_fail(void)
{
	errno = EINVAL;
	return -1;
}

/*
 * gj_pci_parse_bdf — parse "0000:00:1f.0" (or "00:1f.0") into components.
 */
int
gj_pci_parse_bdf(const char *s, unsigned *domain, unsigned *bus,
                 unsigned *dev, unsigned *func)
{
	const char *p;
	unsigned uDom = 0u;
	unsigned uBus = 0u;
	unsigned uDev = 0u;
	unsigned uFun = 0u;
	unsigned uA = 0u;
	unsigned uB = 0u;

	if (s == NULL || domain == NULL || bus == NULL || dev == NULL ||
	    func == NULL) {
		return b158_fail();
	}

	p = s;

	/* First hex field (domain or bus). */
	if (b158_parse_hex(&p, &uA, 4u) != 0) {
		return b158_fail();
	}
	if (*p != ':') {
		return b158_fail();
	}
	p++;

	/* Second hex field (bus or device). */
	if (b158_parse_hex(&p, &uB, 4u) != 0) {
		return b158_fail();
	}

	if (*p == ':') {
		/* Full form: domain:bus:dev.func */
		uDom = uA;
		uBus = uB;
		p++;
		if (b158_parse_hex(&p, &uDev, 2u) != 0) {
			return b158_fail();
		}
	} else if (*p == '.') {
		/* Abbreviated: bus:dev.func  (domain = 0) */
		uDom = 0u;
		uBus = uA;
		uDev = uB;
	} else {
		return b158_fail();
	}

	if (*p != '.') {
		return b158_fail();
	}
	p++;

	if (b158_parse_hex(&p, &uFun, 1u) != 0) {
		return b158_fail();
	}

	/* No trailing garbage (allow nothing after the function field). */
	if (*p != '\0') {
		return b158_fail();
	}

	/* PCI geometry bounds. */
	if (uDom > 0xffffu || uBus > 0xffu || uDev > 0x1fu || uFun > 0x7u) {
		return b158_fail();
	}

	/* For abbreviated bus:dev.func, first field was capped at 4 hex digits
	 * above; re-check bus fits in 8 bits (already covered by uBus > 0xff).
	 * Device in abbreviated path used 4-digit cap on uB before assign —
	 * tighten: uDev must already be <= 0x1f from the range check. */

	*domain = uDom;
	*bus = uBus;
	*dev = uDev;
	*func = uFun;
	return 0;
}

int __gj_pci_parse_bdf(const char *s, unsigned *domain, unsigned *bus,
                       unsigned *dev, unsigned *func)
    __attribute__((alias("gj_pci_parse_bdf")));
