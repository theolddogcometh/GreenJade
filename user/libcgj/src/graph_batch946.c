/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch946: camelCase to snake_case transform into out.
 *
 * Surface (unique symbols):
 *   void gj_str_camel_to_snake(const char *in, char *out, size_t out_cap);
 *     — Convert camelCase/PascalCase in → snake_case out (ASCII).
 *       Inserts '_' before an uppercase letter that follows a lowercase
 *       or digit, then lowercases A-Z. out_cap includes NUL.
 *   void __gj_str_camel_to_snake  (alias)
 *   __libcgj_batch946_marker = "libcgj-batch946"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch946_marker[] = "libcgj-batch946";

/* ---- helpers ----------------------------------------------------------- */

static int
b946_is_lower(unsigned char u8C)
{
	return (u8C >= 'a' && u8C <= 'z');
}

static int
b946_is_upper(unsigned char u8C)
{
	return (u8C >= 'A' && u8C <= 'Z');
}

static int
b946_is_digit(unsigned char u8C)
{
	return (u8C >= '0' && u8C <= '9');
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_camel_to_snake — simple camelCase → snake_case (ASCII).
 *
 * Before each uppercase letter that is not the first character and
 * follows a lowercase letter or a digit, emit '_'. All A-Z are then
 * written as a-z. Truncates safely within out_cap.
 */
void
gj_str_camel_to_snake(const char *pIn, char *pOut, size_t cOutCap)
{
	size_t iOut;
	const char *p;
	unsigned char u8Prev;

	if (pIn == NULL || pOut == NULL || cOutCap == 0u) {
		return;
	}

	iOut = 0u;
	u8Prev = 0u;
	p = pIn;
	while (*p != '\0' && (iOut + 1u) < cOutCap) {
		unsigned char u8C = (unsigned char)*p;

		if (b946_is_upper(u8C)) {
			if (iOut > 0u &&
			    (b946_is_lower(u8Prev) || b946_is_digit(u8Prev))) {
				if ((iOut + 2u) >= cOutCap) {
					break;
				}
				pOut[iOut] = '_';
				iOut++;
			}
			u8C = (unsigned char)(u8C - 'A' + 'a');
		}
		pOut[iOut] = (char)u8C;
		iOut++;
		u8Prev = (unsigned char)*p;
		p++;
	}
	pOut[iOut] = '\0';
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_str_camel_to_snake(const char *pIn, char *pOut, size_t cOutCap)
    __attribute__((alias("gj_str_camel_to_snake")));
