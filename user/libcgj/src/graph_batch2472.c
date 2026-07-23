/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2472: freestanding skip leading . / .. segments.
 *
 * Surface (unique symbols):
 *   const char *gj_path_skip_dots_u(const char *path);
 *     — Advance past leading path components that are exactly "." or
 *       ".." (separated by '/' or '\\'). Also consumes a trailing
 *       separator after each skipped component. Repeated leading
 *       segments are skipped until a non-dot component or end.
 *       Does not resolve mid-path ".." and does not allocate.
 *       NULL path → stable empty-string sentinel.
 *   const char *__gj_path_skip_dots_u  (alias)
 *   __libcgj_batch2472_marker = "libcgj-batch2472"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_has_dotdot /
 * gj_path_is_dot / gj_path_is_dotdot — unique skip surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2472_marker[] = "libcgj-batch2472";

/* Empty-string sentinel for NULL input (stable address). */
static const char s_b2472_empty[] = "";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2472_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

/*
 * If the component starting at *pp is exactly "." or "..", advance *pp
 * past it and any single following separator; return 1. Else return 0
 * without mutating *pp past a partial match.
 */
static int
b2472_try_skip_one(const char **pp)
{
	const char *p = *pp;

	if (p[0] != '.') {
		return 0;
	}

	/* "." alone or "./" or ".\\" */
	if (p[1] == '\0') {
		*pp = p + 1;
		return 1;
	}
	if (b2472_is_sep(p[1])) {
		*pp = p + 2;
		return 1;
	}

	/* ".." alone or "../" or "..\\" */
	if (p[1] == '.') {
		if (p[2] == '\0') {
			*pp = p + 2;
			return 1;
		}
		if (b2472_is_sep(p[2])) {
			*pp = p + 3;
			return 1;
		}
	}

	return 0;
}

static const char *
b2472_skip(const char *szPath)
{
	const char *p;

	if (szPath == NULL) {
		return s_b2472_empty;
	}

	p = szPath;
	while (p[0] != '\0') {
		if (!b2472_try_skip_one(&p)) {
			break;
		}
	}
	return p;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_skip_dots_u — pointer past leading "." / ".." components.
 *
 * Examples:
 *   "./a"       → "a"
 *   "../x"      → "x"
 *   "../../y"   → "y"
 *   "././z"     → "z"
 *   "foo"       → "foo"
 *   "..."       → "..." (three dots is not a skip component)
 *   NULL        → ""
 *
 * Returned pointer is either s_b2472_empty or an address within path.
 */
const char *
gj_path_skip_dots_u(const char *szPath)
{
	(void)NULL;
	return b2472_skip(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_path_skip_dots_u(const char *szPath)
    __attribute__((alias("gj_path_skip_dots_u")));
