/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3887: YAML indentation character predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_yaml_is_indent_u(int c);
 *     - Return 1 if c is a YAML block-structure indentation octet:
 *       space (0x20) or tab (0x09). Else 0.
 *   int __gj_yaml_is_indent_u  (alias)
 *   __libcgj_batch3887_marker = "libcgj-batch3887"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_yaml_indent_spaces (batch1138 leading-space count) — single-octet
 * predicate; unique name; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3887_marker[] = "libcgj-batch3887";

/* ---- freestanding helpers ---------------------------------------------- */

/* YAML indent octets: space or tab (not CR/LF). */
static int
b3887_is_indent(int nC)
{
	return (nC == 0x20 || nC == 0x09) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_yaml_is_indent_u — true for YAML indentation octets (SP / HT).
 *
 * Other values (including newline / EOF-style negatives) return 0.
 * No parent wires.
 */
int
gj_yaml_is_indent_u(int nC)
{
	(void)NULL;
	return b3887_is_indent(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_yaml_is_indent_u(int nC)
    __attribute__((alias("gj_yaml_is_indent_u")));
