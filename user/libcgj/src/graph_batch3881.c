/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3881: JSON whitespace predicate (_u2).
 *
 * Surface (unique symbols):
 *   int gj_json_is_ws_u2(int c);
 *     - Return 1 if c is JSON insignificant whitespace (RFC 8259 §2):
 *       space (0x20), tab (0x09), LF (0x0A), CR (0x0D). Else 0.
 *       Renamed from gj_json_is_ws_u (collision with batch1591).
 *   int __gj_json_is_ws_u2  (alias)
 *   __libcgj_batch3881_marker = "libcgj-batch3881"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890: json_is_ws_u2,
 * json_skip_string_u, json_skip_number_u, json_depth_delta_u,
 * toml_is_key_char_u, ini_is_key_char_u, yaml_is_indent_u,
 * xml_is_name_start_u, xml_is_name_char_u, batch_id_3890).
 * Distinct from gj_json_is_ws_u (batch1591) and gj_json_is_ws
 * (batch741) — unique gj_json_is_ws_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3881_marker[] = "libcgj-batch3881";

/* ---- freestanding helpers ---------------------------------------------- */

/* JSON ws code points only (no form-feed / vertical-tab). */
static int
b3881_is_ws(int nC)
{
	return (nC == 0x20 || nC == 0x09 || nC == 0x0a || nC == 0x0d) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_ws_u2 — true for JSON insignificant whitespace.
 *
 * Other values (including EOF-style negatives) return 0.
 * No parent wires.
 */
int
gj_json_is_ws_u2(int nC)
{
	(void)NULL;
	return b3881_is_ws(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_ws_u2(int nC)
    __attribute__((alias("gj_json_is_ws_u2")));
