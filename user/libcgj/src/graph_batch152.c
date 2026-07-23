/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch152: SSH protocol identification banner parse.
 * Clean-room freestanding pure C from the public SSH transport banner
 * layout (RFC 4253 §4.2). Builds under userspace -msse2. No third-party
 * SSH / OpenSSH source was copied.
 *
 * Surface (unique symbols):
 *   int gj_ssh_parse_banner(const char *line, char *proto, size_t pcap,
 *                           char *soft, size_t scap);
 *     Parse an SSH identification string of the form:
 *       SSH-protoversion-softwareversion [SP comments] [CR] [LF]
 *     Example: "SSH-2.0-OpenSSH_8.9p1"
 *       proto ← "2.0"
 *       soft  ← "OpenSSH_8.9p1"
 *     Caps include the trailing NUL. Optional comments after SP are
 *     accepted and ignored. Bare CR, bare LF, or CRLF trailers are ok.
 *     Returns 0 on success, -1 on error (errno set).
 *
 *   __gj_ssh_parse_banner  (alias)
 *   __libcgj_batch152_marker = "libcgj-batch152"
 *
 * Grammar (RFC 4253 subset):
 *   "SSH-" 1*(printable non-space non-'-') "-" 1*(printable non-space
 *   non-'-') [ SP *VCHAR ] [ CR ] [ LF ]
 *   protoversion / softwareversion: printable US-ASCII without whitespace
 *   and without the separator minus (per RFC 4253 §4.2).
 *
 * Errors:
 *   EINVAL  bad args / missing prefix / empty fields / illegal octets
 *   ENOSPC  output buffer too small for field + NUL
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch152_marker[] = "libcgj-batch152";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b152_is_sp(unsigned char ch)
{
	return ch == (unsigned char)' ';
}

static int
b152_is_cr(unsigned char ch)
{
	return ch == (unsigned char)'\r';
}

static int
b152_is_lf(unsigned char ch)
{
	return ch == (unsigned char)'\n';
}

/*
 * Field octets for protoversion / softwareversion (RFC 4253):
 * printable US-ASCII, excluding space and '-'. Reject DEL and controls.
 */
static int
b152_is_field_char(unsigned char ch)
{
	if (ch <= (unsigned char)0x20u || ch >= (unsigned char)0x7fu) {
		return 0;
	}
	if (ch == (unsigned char)'-') {
		return 0;
	}
	return 1;
}

/*
 * Copy n bytes from pSrc into szDst (cap includes NUL). ENOSPC if n+1 > cap.
 */
static int
b152_copy_n(char *szDst, size_t cbCap, const char *pSrc, size_t n)
{
	size_t i;

	if (szDst == NULL || cbCap == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (n >= cbCap) {
		errno = ENOSPC;
		return -1;
	}
	for (i = 0u; i < n; i++) {
		szDst[i] = pSrc[i];
	}
	szDst[n] = '\0';
	return 0;
}

/* Match fixed ASCII prefix; returns advanced pointer or NULL. */
static const char *
b152_match_prefix(const char *sz, const char *szPrefix)
{
	const char *p = sz;
	const char *q = szPrefix;

	if (p == NULL || q == NULL) {
		return NULL;
	}
	while (*q != '\0') {
		if (*p == '\0' || *p != *q) {
			return NULL;
		}
		p++;
		q++;
	}
	return p;
}

/* ---- public: gj_ssh_parse_banner --------------------------------------- */

/*
 * Parse SSH identification banner into protocol version and software
 * version strings. Comments after the first SP are skipped.
 */
int
gj_ssh_parse_banner(const char *szLine, char *szProto, size_t cbProto,
                    char *szSoft, size_t cbSoft)
{
	const char *p;
	const char *pProto;
	const char *pSoft;
	size_t nProto;
	size_t nSoft;

	if (szLine == NULL || szProto == NULL || cbProto == 0u ||
	    szSoft == NULL || cbSoft == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* Required "SSH-" prefix (case-sensitive). */
	p = b152_match_prefix(szLine, "SSH-");
	if (p == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* protoversion: 1+ field chars until separator '-'. */
	pProto = p;
	nProto = 0u;
	while (b152_is_field_char((unsigned char)*p)) {
		nProto++;
		p++;
	}
	if (nProto == 0u || *p != '-') {
		errno = EINVAL;
		return -1;
	}
	p++; /* skip separator '-' */

	/* softwareversion: 1+ field chars until SP / CR / LF / NUL. */
	pSoft = p;
	nSoft = 0u;
	while (b152_is_field_char((unsigned char)*p)) {
		nSoft++;
		p++;
	}
	if (nSoft == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* Optional comments: SP then anything until CR/LF/NUL (not stored). */
	if (b152_is_sp((unsigned char)*p)) {
		p++;
		while (*p != '\0' && !b152_is_cr((unsigned char)*p) &&
		       !b152_is_lf((unsigned char)*p)) {
			/* Reject NULs already handled; ban other C0 except TAB. */
			unsigned char ch = (unsigned char)*p;

			if (ch < (unsigned char)0x20u && ch != (unsigned char)'\t') {
				errno = EINVAL;
				return -1;
			}
			if (ch == (unsigned char)0x7fu) {
				errno = EINVAL;
				return -1;
			}
			p++;
		}
	}

	/* Optional CR / LF / CRLF trailer, then must be end of string. */
	if (b152_is_cr((unsigned char)*p)) {
		p++;
	}
	if (b152_is_lf((unsigned char)*p)) {
		p++;
	}
	if (*p != '\0') {
		errno = EINVAL;
		return -1;
	}

	if (b152_copy_n(szProto, cbProto, pProto, nProto) != 0) {
		return -1;
	}
	if (b152_copy_n(szSoft, cbSoft, pSoft, nSoft) != 0) {
		return -1;
	}
	return 0;
}

int __gj_ssh_parse_banner(const char *line, char *proto, size_t pcap,
                          char *soft, size_t scap)
    __attribute__((alias("gj_ssh_parse_banner")));
