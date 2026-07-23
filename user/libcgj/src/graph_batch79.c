/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch79: name-based UUID convenience (RFC 4122).
 * Clean-room freestanding pure C — integer/pointer only (no SSE).
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   uuid_generate          → graph_batch36.c  (v4 random via uuid_create)
 *   uuid_generate_random /
 *   uuid_generate_time /
 *   uuid_generate_time_safe → graph_batch36.c
 *   uuid_generate_md5      → graph_batch38.c  (name-based v3 / MD5)
 *   uuid_generate_sha1     → graph_batch38.c  (name-based v5 / SHA-1)
 *   uuid_create family     → graph_batch35.c  (BSD RFC 4122 surface)
 *   uuid_clear/copy/parse/unparse → graph_batch36.c
 *   uuid_type/variant/time → graph_batch38.c
 *
 * This TU adds only symbols that were missing:
 *   uuid_v5 / __uuid_v5
 *     void uuid_v5(const uuid_t ns, const char *name, uuid_t out)
 *     RFC 4122 version-5 (SHA-1) name-based UUID; ns + name → out.
 *     Parameter order differs from libuuid uuid_generate_sha1
 *     (which is out, ns, name).
 *   uuid_v3 / __uuid_v3
 *     void uuid_v3(const uuid_t ns, const char *name, uuid_t out)
 *     RFC 4122 version-3 (MD5) twin of uuid_v5; same arg order.
 *   __libcgj_batch79_marker = "libcgj-batch79"
 *
 * Implements digests via batch38 uuid_generate_md5 / uuid_generate_sha1
 * (extern only). Freestanding-safe local helpers; no string.h.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch79_marker[] = "libcgj-batch79";

/* libuuid uuid_t shape: 16 raw octets. */
typedef unsigned char uuid_t[16];

/* batch38 — name-based generators (do not redefine) */
void uuid_generate_md5(unsigned char out[16], const unsigned char ns[16],
                       const char *szName);
void uuid_generate_sha1(unsigned char out[16], const unsigned char ns[16],
                        const char *szName);

/* --------------------------------------------------------------------------
 * Tiny freestanding helpers (-fno-builtin safe)
 * -------------------------------------------------------------------------- */

static void
b79_bzero(void *pDst, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0; i < cb; i++) {
		p[i] = 0;
	}
}

/*
 * uuid_v5 — RFC 4122 version-5 (SHA-1) name-based UUID.
 *
 * @ns    namespace UUID (16 bytes); NULL → zero out
 * @name  name string (NUL-terminated); NULL treated as empty
 * @out   result UUID (16 bytes)
 *
 * Delegates digest + version/variant bits to batch38 uuid_generate_sha1.
 */
void
uuid_v5(const uuid_t ns, const char *name, uuid_t out)
{
	if (out == NULL) {
		return;
	}
	if (ns == NULL) {
		b79_bzero(out, 16u);
		return;
	}
	/* uuid_generate_sha1 accepts NULL name as empty. */
	uuid_generate_sha1(out, (const unsigned char *)ns, name);
}

/*
 * uuid_v3 — RFC 4122 version-3 (MD5) name-based UUID.
 * Same argument order as uuid_v5; twin of libuuid uuid_generate_md5.
 */
void
uuid_v3(const uuid_t ns, const char *name, uuid_t out)
{
	if (out == NULL) {
		return;
	}
	if (ns == NULL) {
		b79_bzero(out, 16u);
		return;
	}
	uuid_generate_md5(out, (const unsigned char *)ns, name);
}

void __uuid_v5(const uuid_t ns, const char *name, uuid_t out)
    __attribute__((alias("uuid_v5")));
void __uuid_v3(const uuid_t ns, const char *name, uuid_t out)
    __attribute__((alias("uuid_v3")));
