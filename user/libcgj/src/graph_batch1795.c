/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1795: semver pre-release hyphen hint.
 *
 * Surface (unique symbols):
 *   int gj_semver_is_prerelease_hint(const char *s);
 *     — Return 1 if s looks like a core "x.y.z" triple immediately
 *       followed by '-' (semver pre-release introducer), else 0.
 *       Does not validate the pre-release identifier contents.
 *       NULL / incomplete core / no '-' after patch → 0.
 *   int __gj_semver_is_prerelease_hint  (alias)
 *   __libcgj_batch1795_marker = "libcgj-batch1795"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1795_marker[] = "libcgj-batch1795";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1795_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/* Skip a non-empty digit run; returns 0 ok, -1 if no digits. */
static int
b1795_skip_digits(const char **pp)
{
	const char *p;
	int fAny;

	p = *pp;
	fAny = 0;
	while (b1795_is_digit(*p)) {
		fAny = 1;
		p++;
	}
	if (!fAny) {
		return -1;
	}
	*pp = p;
	return 0;
}

/*
 * Advance past a leading "x.y.z" core (digits.digits.digits).
 * Returns 0 and leaves *pp at the first char after patch digits, or -1.
 */
static int
b1795_skip_core(const char **pp)
{
	const char *p;

	p = *pp;
	if (b1795_skip_digits(&p) != 0) {
		return -1;
	}
	if (*p != '.') {
		return -1;
	}
	p++;
	if (b1795_skip_digits(&p) != 0) {
		return -1;
	}
	if (*p != '.') {
		return -1;
	}
	p++;
	if (b1795_skip_digits(&p) != 0) {
		return -1;
	}
	*pp = p;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_semver_is_prerelease_hint — 1 iff core "x.y.z" is followed by '-'.
 */
int
gj_semver_is_prerelease_hint(const char *sz)
{
	const char *p;

	if (sz == NULL) {
		return 0;
	}
	p = sz;
	if (b1795_skip_core(&p) != 0) {
		return 0;
	}
	if (*p != '-') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_is_prerelease_hint(const char *sz)
    __attribute__((alias("gj_semver_is_prerelease_hint")));
