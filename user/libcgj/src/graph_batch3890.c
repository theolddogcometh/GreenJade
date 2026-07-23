/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3890: MILESTONE / wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3890(void);
 *     - Returns the compile-time graph batch number for this TU (3890).
 *   uint32_t __gj_batch_id_3890  (alias)
 *   __libcgj_batch3890_marker = "libcgj-batch3890"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave (batches
 * 3881-3889: json_is_ws_u2, json_skip_string_u, json_skip_number_u,
 * json_depth_delta_u, toml_is_key_char_u, ini_is_key_char_u,
 * yaml_is_indent_u, xml_is_name_start_u, xml_is_name_char_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3890_marker[] = "libcgj-batch3890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3890_id(void)
{
	return 3890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3890 — report this TU's graph batch number.
 *
 * Always returns 3890. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_3890(void)
{
	(void)NULL;
	return b3890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3890(void)
    __attribute__((alias("gj_batch_id_3890")));
