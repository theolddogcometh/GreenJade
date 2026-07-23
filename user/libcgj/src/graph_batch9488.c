/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9488: libxml2 namespace ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xml2_ns_ok_u_9488(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       libxml2 namespace / xmlNs validity probe.
 *   uint32_t __gj_xml2_ns_ok_u_9488  (alias)
 *   __libcgj_batch9488_marker = "libcgj-batch9488"
 *
 * Exclusive continuum CREATE-ONLY (9481-9490: libxml2 soft id stubs —
 * xml2_doc_ok_u_9481, xml2_node_ok_u_9482, xml2_xpath_ok_u_9483,
 * xml2_schema_ok_u_9484, xml2_dtd_ok_u_9485, xml2_parse_ok_u_9486,
 * xml2_serialize_ok_u_9487, xml2_ns_ok_u_9488, libxml2_ready_u_9489,
 * batch_id_9490). Unique surface only; no multi-def. No parent wires.
 * No __int128. No libxml2 implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9488_marker[] = "libcgj-batch9488";

/* Soft libxml2 ns-ok lamp: always off (not a real namespace probe). */
#define B9488_XML2_NS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9488_xml2_ns_ok(void)
{
	return B9488_XML2_NS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml2_ns_ok_u_9488 - libxml2 namespace ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not resolve namespaces
 * or call libc/libxml2. No parent wires.
 */
uint32_t
gj_xml2_ns_ok_u_9488(void)
{
	(void)NULL;
	return b9488_xml2_ns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xml2_ns_ok_u_9488(void)
    __attribute__((alias("gj_xml2_ns_ok_u_9488")));
