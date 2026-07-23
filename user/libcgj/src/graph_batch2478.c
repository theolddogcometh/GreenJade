/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2478: freestanding UNC path shape predicate.
 *
 * Surface (unique symbols):
 *   unsigned gj_path_is_unc_u(const char *path);
 *     — Return 1u if path has UNC (Universal Naming Convention) shape:
 *       two leading separators ('/' or '\\', may be mixed) followed by
 *       a non-separator server name character. Examples: "\\\\srv\\sh",
 *       "//srv/sh", "\\/srv". Bare "\\\\" or "//" alone is not UNC.
 *       NULL → 0u.
 *   unsigned __gj_path_is_unc_u  (alias)
 *   __libcgj_batch2478_marker = "libcgj-batch2478"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_is_abs /
 * gj_path_is_abs_win_drive / gj_path_has_drive_u — unique UNC surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2478_marker[] = "libcgj-batch2478";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2478_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static unsigned
b2478_is_unc(const char *szPath)
{
	if (szPath == NULL) {
		return 0u;
	}

	/* Need at least "\\x" (three bytes). */
	if (!b2478_is_sep(szPath[0])) {
		return 0u;
	}
	if (!b2478_is_sep(szPath[1])) {
		return 0u;
	}
	if (szPath[2] == '\0') {
		return 0u;
	}
	/* Third byte must begin the server name (not another sep). */
	if (b2478_is_sep(szPath[2])) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_unc_u — 1u if path is UNC-shaped (\\\\server or //server).
 *
 * Examples:
 *   "\\\\server\\share"  → 1u
 *   "//server/share"     → 1u
 *   "\\/server"          → 1u
 *   "\\\\"               → 0u
 *   "\\server"           → 0u  (only one leading sep)
 *   "C:\\foo"            → 0u
 *   "/unix"              → 0u
 *   NULL                 → 0u
 */
unsigned
gj_path_is_unc_u(const char *szPath)
{
	(void)NULL;
	return b2478_is_unc(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_path_is_unc_u(const char *szPath)
    __attribute__((alias("gj_path_is_unc_u")));
