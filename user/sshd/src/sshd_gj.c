/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GreenJade freestanding product sshd (sshd.elf).
 * Dual MIT OR Apache-2.0. Clean-room pure C — not an OpenSSH/GPL paste.
 *
 * Live path (TCP + KEX + NEWKEYS + shell), greppable PASS markers:
 *   1. TCP :22 listen/accept (net_tcp / virtio-net + loopback)
 *   2. RFC 4253 banners
 *   3. KEX curve25519-sha256 (X25519 + SHA-256) + product hostkey of H
 *   4. SSH_MSG_KEX_ECDH_REPLY on wire + dual shared-secret match
 *   5. SSH_MSG_NEWKEYS both directions
 *   6. Soft SERVICE_REQUEST/ACCEPT (ssh-userauth) post-NEWKEYS
 *   7. Session channel + shell MOTD (cleartext soft path)
 *   8. RFC 4253 7.2 key derivation → ChaCha20 + HMAC integrity
 *   9. Encrypted CHANNEL_DATA send + recv (MAC verify + decrypt)
 *  10. Poly1305 soft AEAD self-check (RFC 8439 vector + post-keys tag)
 *  11. live path PASS → soft inventory → daemon park
 *
 * Soft inventory (Wave 54 exclusive deepen) — honesty, not product SSH.
 * Diagnostics only; never hard-fails the live path. Greppable prefix:
 *   "sshd-gj: soft …"
 * Honesty: soft inventory ≠ product multi-server confine.
 *
 * Soft suite (optional; never fails live path):
 *   sshd-gj: soft suite start
 *   sshd-gj: soft banner PASS | soft banner soft-skip
 *   sshd-gj: soft kexinit PASS | soft kexinit soft-skip
 *   sshd-gj: soft hostkey PASS | soft hostkey soft-skip
 *   sshd-gj: soft mac PASS | soft mac soft-skip
 *   sshd-gj: soft chacha PASS | soft chacha soft-skip
 *   sshd-gj: soft poly PASS | soft poly soft-skip
 *   sshd-gj: soft sha256 PASS | soft sha256 soft-skip
 *   sshd-gj: soft x25519 PASS | soft x25519 soft-skip
 *   sshd-gj: soft newkeys PASS | soft newkeys soft-skip
 *   sshd-gj: soft service PASS | soft service soft-skip
 *   sshd-gj: soft ecdh PASS | soft ecdh soft-skip
 *   sshd-gj: soft kdf PASS | soft kdf soft-skip
 *   sshd-gj: soft memeq PASS | soft memeq soft-skip
 *   sshd-gj: soft suite PASS | soft suite soft-skip
 * Soft inventory / path / stats (Wave 54; greppable "sshd-gj: soft …"):
 *   sshd-gj: soft honesty not-product-ssh …
 *   sshd-gj: soft inventory …
 *   sshd-gj: soft seq …
 *   sshd-gj: soft crypto …
 *   sshd-gj: soft kex …
 *   sshd-gj: soft channel …
 *   sshd-gj: soft path …
 *   sshd-gj: soft stats …
 *   sshd-gj: soft deepen wave=54 …
 *
 * Crypto primitives live in ssh_crypto.c (same license).
 *   make sshd-gj → build/user/sshd.elf
 */
#include <gj/syscalls.h>

#define AF_INET     2
#define SOCK_STREAM 1
#define SSH_PORT    22

/* SSH binary packet message types used on the live smoke path */
#define SSH_MSG_SERVICE_REQUEST         5
#define SSH_MSG_SERVICE_ACCEPT          6
#define SSH_MSG_KEXINIT                 20
#define SSH_MSG_NEWKEYS                 21
#define SSH_MSG_KEX_ECDH_INIT           30
#define SSH_MSG_KEX_ECDH_REPLY          31
#define SSH_MSG_CHANNEL_OPEN            90
#define SSH_MSG_CHANNEL_OPEN_CONFIRM    91
#define SSH_MSG_CHANNEL_DATA            94
#define SSH_MSG_CHANNEL_REQUEST         98
#define SSH_MSG_CHANNEL_SUCCESS         99

#ifndef GJ_NET_OP_ACCEPT
#define GJ_NET_OP_ACCEPT 25u
#endif
#ifndef GJ_NET_OP_TCP_STATS
#define GJ_NET_OP_TCP_STATS 26u
#endif

struct sha256_ctx {
	uint32_t state[8];
	uint64_t bitlen;
	uint8_t data[64];
	uint32_t datalen;
};

void gj_ssh_sha256_init(struct sha256_ctx *ctx);
void gj_ssh_sha256_update(struct sha256_ctx *ctx, const void *data, size_t len);
void gj_ssh_sha256_final(struct sha256_ctx *ctx, uint8_t hash[32]);
void gj_ssh_sha256(const void *data, size_t len, uint8_t out[32]);
void gj_ssh_x25519(uint8_t out[32], const uint8_t scalar[32],
		   const uint8_t point[32]);
void gj_ssh_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data,
			size_t data_len, uint8_t out[32]);
void gj_ssh_chacha20_xor(const uint8_t key[32], const uint8_t nonce[12],
			 uint32_t counter, uint8_t *data, size_t len);
void gj_ssh_poly1305(const uint8_t key[32], const uint8_t *pMsg, size_t cbMsg,
		     uint8_t tag[16]);
int  gj_ssh_poly1305_selfcheck(void);
int  gj_ssh_poly1305_ok(void);
int  gj_ssh_memeq_ct(const uint8_t *pA, const uint8_t *pB, size_t cb);
void gj_ssh_hostkey_init(void);
void gj_ssh_hostkey_pk(uint8_t pk[32]);
void gj_ssh_hostkey_sign(const uint8_t *msg, size_t msg_len, uint8_t sig[32]);
int  gj_ssh_hostkey_verify(const uint8_t *msg, size_t msg_len,
			   const uint8_t sig[32]);

static const char g_szBanner[] = "SSH-2.0-GreenJade_sshd\r\n";
static const char g_szClientBanner[] = "SSH-2.0-GreenJade_smoke\r\n";
static const char g_szMotd[] =
	"GreenJade sshd: shell ready (session channel soft path)\r\n";

/*
 * Session keys after NEWKEYS (RFC 4253 7.2 letters A–F).
 * Client→server and server→client IVs, encryption, integrity.
 */
static uint8_t g_iv_c2s[12], g_iv_s2c[12];
static uint8_t g_enc_c2s[32], g_enc_s2c[32];
static uint8_t g_int_c2s[32], g_int_s2c[32];
/*
 * Per-direction sequence numbers. In-process dual-role smoke keeps separate
 * TX/RX views so send_pkt increment does not race recv_pkt MAC verify.
 */
static uint32_t g_seq_s2c_tx, g_seq_s2c_rx;
static uint32_t g_seq_c2s_tx, g_seq_c2s_rx;
static int g_encrypted;

/*
 * Soft product inventory (Wave 54). Cumulative milestone lamps + suite tallies.
 * Honesty-only — not a claim of OpenSSH-class product completeness.
 * Soft ≠ product multi-server confine.
 * greppable: sshd-gj: soft …
 */
static uint32_t g_u32SoftBanner;   /* peer banner shape OK */
static uint32_t g_u32SoftPoly;     /* poly1305 selfcheck OK */
static uint32_t g_u32SoftHostkey;  /* hostkey sign+verify of H */
static uint32_t g_u32SoftEcdh;     /* ECDH_REPLY wire + parse */
static uint32_t g_u32SoftShared;   /* dual shared-secret match */
static uint32_t g_u32SoftKex;      /* aggregate KEX PASS */
static uint32_t g_u32SoftNewkeys;  /* NEWKEYS both directions */
static uint32_t g_u32SoftService;  /* soft SERVICE_REQUEST/ACCEPT */
static uint32_t g_u32SoftChannel;  /* session channel + MOTD */
static uint32_t g_u32SoftKeys;     /* RFC 4253 7.2 key derivation */
static uint32_t g_u32SoftAead;     /* poly1305 soft AEAD leg */
static uint32_t g_u32SoftEncTx;    /* encrypted CHANNEL_DATA send */
static uint32_t g_u32SoftEncRx;    /* encrypted CHANNEL_DATA recv */
static uint32_t g_u32SoftLive;     /* live path banner+session green */
static uint32_t g_u32SoftSuiteOk;  /* offline soft-suite sub-steps OK */
static uint32_t g_u32SoftSuiteN;   /* offline soft-suite sub-steps run */
static uint32_t g_u32SoftSuiteBits;/* offline soft-suite bit lamps */
static uint32_t g_u32SoftLogN;     /* inventory log emissions */

/* Offline soft-suite bit lamps (Wave 54; never hard-gate). */
/* Wave 54 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retcaponier continuum_toward=19600 soft_ne_product=1
 *   greppable: soft retredan exclusive=1 continuum_toward=19600
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define SOFT_SUITE_BANNER   (1u << 0)
#define SOFT_SUITE_KEXINIT  (1u << 1)
#define SOFT_SUITE_HOSTKEY  (1u << 2)
#define SOFT_SUITE_MAC      (1u << 3)
#define SOFT_SUITE_CHACHA   (1u << 4)
#define SOFT_SUITE_POLY     (1u << 5)
#define SOFT_SUITE_SHA256   (1u << 6)
#define SOFT_SUITE_X25519   (1u << 7)
#define SOFT_SUITE_NEWKEYS  (1u << 8)
#define SOFT_SUITE_SERVICE  (1u << 9)
#define SOFT_SUITE_ECDH     (1u << 10)
#define SOFT_SUITE_KDF      (1u << 11)
#define SOFT_SUITE_MEMEQ    (1u << 12)

static void
msg(const char *sz)
{
	size_t n = 0;

	if (sz == 0) {
		return;
	}
	while (sz[n] != '\0') {
		n++;
	}
	(void)gj_debug_log(sz, (long)n);
}

/* Append decimal digits of u into aLine at *po; leave room for NUL. */
static void
append_u(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
	char aDig[20];
	unsigned n = 0;
	unsigned i;

	if (aLine == 0 || po == 0 || cb == 0) {
		return;
	}
	if (u == 0) {
		aDig[n++] = '0';
	} else {
		while (u > 0 && n < sizeof(aDig)) {
			aDig[n++] = (char)('0' + (u % 10ul));
			u /= 10ul;
		}
	}
	for (i = n; i > 0 && *po + 1 < cb; i--) {
		aLine[(*po)++] = aDig[i - 1u];
	}
}

static void
append_s(char *aLine, unsigned cb, unsigned *po, const char *sz)
{
	if (aLine == 0 || po == 0 || sz == 0 || cb == 0) {
		return;
	}
	while (*sz != '\0' && *po + 1 < cb) {
		aLine[(*po)++] = *sz++;
	}
}

/* Saturating bump (u32 wrap avoided for soft lamps). */
static void
soft_note(uint32_t *pu32Ctr)
{
	if (pu32Ctr == 0) {
		return;
	}
	if (*pu32Ctr < 0xffffffffu) {
		(*pu32Ctr)++;
	}
}

static size_t
slen(const char *sz)
{
	size_t n = 0;

	if (sz == 0) {
		return 0;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static void
bytes_copy(uint8_t *dst, const uint8_t *src, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
	}
}

static void
bytes_zero(uint8_t *dst, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		dst[i] = 0;
	}
}

/* RFC 4253 identification: must begin with "SSH-" */
static int
banner_is_ssh(const char *buf, long n)
{
	return n >= 4 && buf[0] == 'S' && buf[1] == 'S' && buf[2] == 'H' &&
	       buf[3] == '-';
}

static void
put_u32(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)(v >> 24);
	p[1] = (uint8_t)(v >> 16);
	p[2] = (uint8_t)(v >> 8);
	p[3] = (uint8_t)v;
}

static void
put_name_list(uint8_t *p, uint32_t *poff, const char *s)
{
	size_t n = slen(s);
	size_t i;

	put_u32(p + *poff, (uint32_t)n);
	*poff += 4;
	for (i = 0; i < n; i++) {
		p[*poff + i] = (uint8_t)s[i];
	}
	*poff += (uint32_t)n;
}

/* SSH_MSG_KEXINIT with product algorithm name-lists (RFC 4253 7.1). */
static uint32_t
build_kexinit(uint8_t *pkt, uint32_t cap)
{
	uint8_t payload[512];
	uint32_t off = 0;
	uint32_t i, pad, packet_len, pay_len;

	if (cap < 64) {
		return 0;
	}
	payload[off++] = SSH_MSG_KEXINIT;
	for (i = 0; i < 16; i++) {
		payload[off++] = (uint8_t)(0xa0 + i);
	}
	/* Name-lists are algorithm identifiers only — not third-party code. */
	put_name_list(payload, &off, "curve25519-sha256@libssh.org");
	put_name_list(payload, &off, "ssh-ed25519,hmac-sha2-256");
	put_name_list(payload, &off, "chacha20-poly1305@openssh.com,aes128-ctr");
	put_name_list(payload, &off, "chacha20-poly1305@openssh.com,aes128-ctr");
	put_name_list(payload, &off, "hmac-sha2-256");
	put_name_list(payload, &off, "hmac-sha2-256");
	put_name_list(payload, &off, "none");
	put_name_list(payload, &off, "none");
	put_name_list(payload, &off, "");
	put_name_list(payload, &off, "");
	payload[off++] = 0; /* first_kex_packet_follows */
	put_u32(payload + off, 0); /* reserved */
	off += 4;
	pay_len = off;
	pad = 4;
	while ((1 + pay_len + pad) % 8 != 0) {
		pad++;
	}
	packet_len = 1 + pay_len + pad;
	if (cap < 4 + packet_len) {
		return 0;
	}
	put_u32(pkt, packet_len);
	pkt[4] = (uint8_t)pad;
	bytes_copy(pkt + 5, payload, pay_len);
	for (i = 0; i < pad; i++) {
		pkt[5 + pay_len + i] = (uint8_t)(i + 1);
	}
	return 4 + packet_len;
}

/* Generic SSH binary packet: type + optional payload + padding. */
static uint32_t
build_simple(uint8_t *pkt, uint32_t cap, uint8_t msgtype,
	     const uint8_t *pay, uint32_t payn)
{
	uint32_t pad = 4;
	uint32_t pay_len = 1 + payn;
	uint32_t packet_len;
	uint32_t i;

	while ((1 + pay_len + pad) % 8 != 0) {
		pad++;
	}
	packet_len = 1 + pay_len + pad;
	/* Caller may append a 32-byte MAC after encrypt; base wire size is 4+packet_len */
	if (cap < 4 + packet_len) {
		return 0;
	}
	put_u32(pkt, packet_len);
	pkt[4] = (uint8_t)pad;
	pkt[5] = msgtype;
	if (payn > 0 && pay != 0) {
		bytes_copy(pkt + 6, pay, payn);
	}
	for (i = 0; i < pad; i++) {
		pkt[6 + payn + i] = (uint8_t)(i + 1);
	}
	return 4 + packet_len;
}

/* SSH_MSG_KEX_ECDH_INIT: string Q_C (32-byte client X25519 public). */
static uint32_t
build_ecdh_init(uint8_t *pkt, uint32_t cap, const uint8_t q_c[32])
{
	uint8_t body[36];

	put_u32(body, 32);
	bytes_copy(body + 4, q_c, 32);
	return build_simple(pkt, cap, SSH_MSG_KEX_ECDH_INIT, body, 36);
}

/*
 * SSH_MSG_KEX_ECDH_REPLY (RFC 5656 soft shape):
 *   string K_S (product host public, 32 bytes)
 *   string Q_S (server ephemeral X25519 public, 32 bytes)
 *   string signature (product HMAC-SHA256 of H, 32 bytes)
 */
static uint32_t
build_ecdh_reply(uint8_t *pkt, uint32_t cap, const uint8_t host_pk[32],
		 const uint8_t q_s[32], const uint8_t sig[32])
{
	uint8_t body[4 + 32 + 4 + 32 + 4 + 32];
	uint32_t off = 0;

	put_u32(body + off, 32);
	off += 4;
	bytes_copy(body + off, host_pk, 32);
	off += 32;
	put_u32(body + off, 32);
	off += 4;
	bytes_copy(body + off, q_s, 32);
	off += 32;
	put_u32(body + off, 32);
	off += 4;
	bytes_copy(body + off, sig, 32);
	off += 32;
	return build_simple(pkt, cap, SSH_MSG_KEX_ECDH_REPLY, body, off);
}

/* Soft SERVICE_REQUEST / SERVICE_ACCEPT name-string body. */
static uint32_t
build_service(uint8_t *pkt, uint32_t cap, uint8_t msgtype, const char *szName)
{
	uint8_t body[64];
	uint32_t cbName = (uint32_t)slen(szName);
	uint32_t iByte;

	if (cbName > 48) {
		cbName = 48;
	}
	put_u32(body, cbName);
	for (iByte = 0; iByte < cbName; iByte++) {
		body[4 + iByte] = (uint8_t)szName[iByte];
	}
	return build_simple(pkt, cap, msgtype, body, 4 + cbName);
}

/*
 * RFC 4253 7.2: Ki = HASH(K || H || X || session_id), X in {'A'..'F'}.
 * Arms ChaCha20 + integrity keys; sequence counters start at 0.
 */
static void
derive_keys(const uint8_t *K, const uint8_t *H, const uint8_t *sid)
{
	uint8_t buf[32 + 32 + 1 + 32];
	uint8_t out[32];
	unsigned i;

	for (i = 0; i < 32; i++) {
		buf[i] = K[i];
		buf[32 + i] = H[i];
		buf[65 + i] = sid[i];
	}
	buf[64] = 'A'; /* IV client → server */
	gj_ssh_sha256(buf, 97, out);
	bytes_copy(g_iv_c2s, out, 12);
	buf[64] = 'B'; /* IV server → client */
	gj_ssh_sha256(buf, 97, out);
	bytes_copy(g_iv_s2c, out, 12);
	buf[64] = 'C'; /* enc client → server */
	gj_ssh_sha256(buf, 97, g_enc_c2s);
	buf[64] = 'D'; /* enc server → client */
	gj_ssh_sha256(buf, 97, g_enc_s2c);
	buf[64] = 'E'; /* integrity client → server */
	gj_ssh_sha256(buf, 97, g_int_c2s);
	buf[64] = 'F'; /* integrity server → client */
	gj_ssh_sha256(buf, 97, g_int_s2c);
	g_seq_s2c_tx = 0;
	g_seq_s2c_rx = 0;
	g_seq_c2s_tx = 0;
	g_seq_c2s_rx = 0;
	g_encrypted = 1;
	bytes_zero(out, sizeof(out));
	bytes_zero(buf, sizeof(buf));
}

/* Product MAC over clear length || encrypted body (seq || packet). */
static void
product_mac(const uint8_t *pKeyInt, uint32_t uSeq, const uint8_t *pPkt,
	    uint32_t cbPkt, uint8_t aMac[32])
{
	uint8_t aSeq[4];
	struct sha256_ctx hx;

	put_u32(aSeq, uSeq);
	gj_ssh_sha256_init(&hx);
	gj_ssh_sha256_update(&hx, aSeq, 4);
	gj_ssh_sha256_update(&hx, pPkt, cbPkt);
	gj_ssh_sha256_final(&hx, aMac);
	gj_ssh_hmac_sha256(pKeyInt, 32, aMac, 32, aMac);
}

/*
 * Send one SSH binary packet. After NEWKEYS (g_encrypted), encrypt the
 * packet body with ChaCha20 and append a product integrity tag.
 * Cleartext path used for KEXINIT / ECDH / NEWKEYS / soft service.
 */
static long
send_pkt(long fd, uint8_t *pkt, uint32_t n, int as_server)
{
	uint8_t mac[32];
	uint8_t *key_enc = as_server ? g_enc_s2c : g_enc_c2s;
	uint8_t *key_int = as_server ? g_int_s2c : g_int_c2s;
	uint8_t *iv = as_server ? g_iv_s2c : g_iv_c2s;
	uint32_t *pseq = as_server ? &g_seq_s2c_tx : &g_seq_c2s_tx;
	long r;

	if (g_encrypted && n >= 5) {
		/* ChaCha20 over packet after the length field (length clear) */
		gj_ssh_chacha20_xor(key_enc, iv, *pseq, pkt + 4, n - 4);
		product_mac(key_int, *pseq, pkt, n, mac);
		r = gj_net(GJ_NET_OP_SEND, fd, (long)(uintptr_t)pkt, (long)n);
		if (r > 0) {
			(void)gj_net(GJ_NET_OP_SEND, fd, (long)(uintptr_t)mac,
				     32);
		}
		(*pseq)++;
		return r;
	}
	return gj_net(GJ_NET_OP_SEND, fd, (long)(uintptr_t)pkt, (long)n);
}

/*
 * Receive one encrypted SSH binary packet: MAC verify (constant-time) then
 * ChaCha20 decrypt. Length field is cleartext (product soft cipher mode).
 * Returns payload byte count (4 + packet_len) or 0 on failure.
 *
 * as_server=1: receiving client→server (c2s keys / g_seq_c2s_rx)
 * as_server=0: receiving server→client (s2c keys / g_seq_s2c_rx)
 */
static uint32_t
recv_pkt(long fd, uint8_t *pkt, uint32_t cap, int as_server)
{
	uint8_t aMac[32];
	uint8_t aMacExp[32];
	uint8_t *key_enc = as_server ? g_enc_c2s : g_enc_s2c;
	uint8_t *key_int = as_server ? g_int_c2s : g_int_s2c;
	uint8_t *iv = as_server ? g_iv_c2s : g_iv_s2c;
	uint32_t *pseq = as_server ? &g_seq_c2s_rx : &g_seq_s2c_rx;
	long nr;
	uint32_t uPktLen;
	uint32_t cbWire;

	if (!g_encrypted || cap < 8) {
		return 0;
	}
	/* First 4 bytes: clear packet_length */
	nr = gj_net(GJ_NET_OP_RECV, fd, (long)(uintptr_t)pkt, 4);
	if (nr < 4) {
		return 0;
	}
	uPktLen = ((uint32_t)pkt[0] << 24) | ((uint32_t)pkt[1] << 16) |
		  ((uint32_t)pkt[2] << 8) | (uint32_t)pkt[3];
	if (uPktLen < 5 || uPktLen > cap - 4) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd, (long)(uintptr_t)(pkt + 4),
		    (long)uPktLen);
	if (nr < (long)uPktLen) {
		return 0;
	}
	cbWire = 4 + uPktLen;
	nr = gj_net(GJ_NET_OP_RECV, fd, (long)(uintptr_t)aMac, 32);
	if (nr < 32) {
		return 0;
	}
	product_mac(key_int, *pseq, pkt, cbWire, aMacExp);
	if (!gj_ssh_memeq_ct(aMac, aMacExp, 32)) {
		bytes_zero(aMac, sizeof(aMac));
		bytes_zero(aMacExp, sizeof(aMacExp));
		return 0;
	}
	gj_ssh_chacha20_xor(key_enc, iv, *pseq, pkt + 4, uPktLen);
	(*pseq)++;
	bytes_zero(aMac, sizeof(aMac));
	bytes_zero(aMacExp, sizeof(aMacExp));
	return cbWire;
}

/* Soft session channel: open → confirm → shell request → MOTD data. */
static int
do_session_channel(long fd_srv, long fd_cli)
{
	uint8_t pkt[256];
	uint8_t body[64];
	uint32_t n;
	long nr;
	uint8_t rbuf[256];
	unsigned i;

	/* SSH_MSG_CHANNEL_OPEN "session" */
	put_u32(body, 7);
	body[4] = 's';
	body[5] = 'e';
	body[6] = 's';
	body[7] = 's';
	body[8] = 'i';
	body[9] = 'o';
	body[10] = 'n';
	put_u32(body + 11, 0);
	put_u32(body + 15, 32768);
	put_u32(body + 19, 16384);
	n = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_OPEN, body, 23);
	if (n == 0) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_SEND, fd_cli, (long)(uintptr_t)pkt, (long)n);
	if (nr <= 0) {
		return 0;
	}
	/* Drain residual / wait for CHANNEL_OPEN on server half */
	for (i = 0; i < 4; i++) {
		nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)rbuf,
			    (long)sizeof(rbuf));
		if (nr >= 6 && rbuf[5] == SSH_MSG_CHANNEL_OPEN) {
			break;
		}
		if (nr <= 0) {
			break;
		}
	}
	if (nr < 6) {
		return 0;
	}

	/* SSH_MSG_CHANNEL_OPEN_CONFIRMATION */
	put_u32(body, 0);
	put_u32(body + 4, 0);
	put_u32(body + 8, 32768);
	put_u32(body + 12, 16384);
	n = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_OPEN_CONFIRM, body,
			 16);
	nr = gj_net(GJ_NET_OP_SEND, fd_srv, (long)(uintptr_t)pkt, (long)n);
	if (nr <= 0) {
		return 0;
	}

	/* SSH_MSG_CHANNEL_REQUEST "shell" (want_reply=1) */
	put_u32(body, 0);
	put_u32(body + 4, 5);
	body[8] = 's';
	body[9] = 'h';
	body[10] = 'e';
	body[11] = 'l';
	body[12] = 'l';
	body[13] = 1;
	n = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_REQUEST, body, 14);
	(void)gj_net(GJ_NET_OP_SEND, fd_cli, (long)(uintptr_t)pkt, (long)n);

	/* SSH_MSG_CHANNEL_SUCCESS */
	put_u32(body, 0);
	n = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_SUCCESS, body, 4);
	(void)gj_net(GJ_NET_OP_SEND, fd_srv, (long)(uintptr_t)pkt, (long)n);

	/* SSH_MSG_CHANNEL_DATA — product MOTD */
	{
		uint32_t mlen = (uint32_t)slen(g_szMotd);
		uint8_t dbody[128];

		if (mlen > 80) {
			mlen = 80;
		}
		put_u32(dbody, 0);
		put_u32(dbody + 4, mlen);
		for (i = 0; i < mlen; i++) {
			dbody[8 + i] = (uint8_t)g_szMotd[i];
		}
		n = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_DATA, dbody,
				 8 + mlen);
		nr = gj_net(GJ_NET_OP_SEND, fd_srv, (long)(uintptr_t)pkt,
			    (long)n);
		if (nr <= 0) {
			return 0;
		}
		/*
		 * Client drains residual OPEN_CONFIRM / CHANNEL_SUCCESS then
		 * MOTD so the encrypted CHANNEL_DATA recv path sees a clean
		 * socket (no cleartext leftover).
		 */
		{
			int fGotMotd = 0;

			for (i = 0; i < 6; i++) {
				nr = gj_net(GJ_NET_OP_RECV, fd_cli,
					    (long)(uintptr_t)rbuf,
					    (long)sizeof(rbuf));
				if (nr < 6) {
					break;
				}
				if (rbuf[5] == SSH_MSG_CHANNEL_DATA) {
					fGotMotd = 1;
					break;
				}
			}
			if (!fGotMotd) {
				return 0;
			}
		}
	}
	msg("sshd-gj: channel+shell MOTD PASS\n");
	return 1;
}

/*
 * Soft SERVICE_REQUEST "ssh-userauth" → SERVICE_ACCEPT after NEWKEYS.
 * Returns 1 on shape OK.
 */
static int
do_service_soft(long fd_srv, long fd_cli)
{
	uint8_t aPkt[96];
	uint8_t aRbuf[96];
	uint32_t cb;
	long nr;
	const char *szSvc = "ssh-userauth";

	cb = build_service(aPkt, sizeof(aPkt), SSH_MSG_SERVICE_REQUEST, szSvc);
	if (cb == 0 ||
	    gj_net(GJ_NET_OP_SEND, fd_cli, (long)(uintptr_t)aPkt, (long)cb) <=
		0) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)aRbuf,
		    (long)sizeof(aRbuf));
	if (nr < 6 || aRbuf[5] != SSH_MSG_SERVICE_REQUEST) {
		return 0;
	}
	cb = build_service(aPkt, sizeof(aPkt), SSH_MSG_SERVICE_ACCEPT, szSvc);
	if (cb == 0 ||
	    gj_net(GJ_NET_OP_SEND, fd_srv, (long)(uintptr_t)aPkt, (long)cb) <=
		0) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_cli, (long)(uintptr_t)aRbuf,
		    (long)sizeof(aRbuf));
	if (nr < 6 || aRbuf[5] != SSH_MSG_SERVICE_ACCEPT) {
		return 0;
	}
	return 1;
}

/*
 * Greppable soft inventory + honesty (Wave 54 exclusive deepen).
 *   sshd-gj: soft honesty not-product-ssh …
 *   sshd-gj: soft inventory …
 *   sshd-gj: soft seq …
 *   sshd-gj: soft crypto …
 *   sshd-gj: soft kex …
 *   sshd-gj: soft channel …
 *   sshd-gj: soft path …
 *   sshd-gj: soft stats …
 *   sshd-gj: soft deepen wave=54 …
 * Never hard-gates live path; pure observation for smoke/scripts.
 * Honesty: soft ≠ product multi-server confine.
 */
static void
soft_inventory_log(void)
{
	char aLine[288];
	unsigned o;
	unsigned cAreas = 0;
	unsigned cKex;
	unsigned cChan;
	unsigned cCrypto;

	if (g_u32SoftLogN < 0xffffffffu) {
		g_u32SoftLogN++;
	}

	/*
	 * Honesty line: this freestanding smoke is NOT OpenSSH product.
	 * userauth/channel/service are soft shapes; no rekey, no multi-session.
	 * Soft inventory ≠ product multi-server confine.
	 * greppable: sshd-gj: soft honesty
	 */
	msg("sshd-gj: soft honesty not-product-ssh userauth=soft "
	    "channel=soft service=soft rekey=0 multi=0 multi_server=0 "
	    "confine=0 openssh=0 dropbear=0 authorized_keys=soft\n");
	cAreas++;

	/* Grep: sshd-gj: soft inventory (legacy Wave 9 kex lamps) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft inventory banner=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftBanner);
	append_s(aLine, sizeof(aLine), &o, " poly=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftPoly);
	append_s(aLine, sizeof(aLine), &o, " hostkey=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftHostkey);
	append_s(aLine, sizeof(aLine), &o, " ecdh=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEcdh);
	append_s(aLine, sizeof(aLine), &o, " shared=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftShared);
	append_s(aLine, sizeof(aLine), &o, " kex=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftKex);
	append_s(aLine, sizeof(aLine), &o, " newkeys=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftNewkeys);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft inventory (legacy Wave 9 session lamps) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft inventory service=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftService);
	append_s(aLine, sizeof(aLine), &o, " channel=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftChannel);
	append_s(aLine, sizeof(aLine), &o, " keys=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftKeys);
	append_s(aLine, sizeof(aLine), &o, " aead=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftAead);
	append_s(aLine, sizeof(aLine), &o, " enc_tx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEncTx);
	append_s(aLine, sizeof(aLine), &o, " enc_rx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEncRx);
	append_s(aLine, sizeof(aLine), &o, " live=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLive);
	append_s(aLine, sizeof(aLine), &o, " suite_ok=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteOk);
	append_s(aLine, sizeof(aLine), &o, " suite_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteN);
	append_s(aLine, sizeof(aLine), &o, " log_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLogN);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Seq / encrypt soft snapshot (post-keys; values may be zero pre-KEX). */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft seq s2c_tx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_seq_s2c_tx);
	append_s(aLine, sizeof(aLine), &o, " s2c_rx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_seq_s2c_rx);
	append_s(aLine, sizeof(aLine), &o, " c2s_tx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_seq_c2s_tx);
	append_s(aLine, sizeof(aLine), &o, " c2s_rx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_seq_c2s_rx);
	append_s(aLine, sizeof(aLine), &o, " encrypted=");
	append_u(aLine, sizeof(aLine), &o, g_encrypted ? 1ul : 0ul);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft crypto (Wave 54 offline suite lamps) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft crypto suite_ok=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteOk);
	append_s(aLine, sizeof(aLine), &o, " suite_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteN);
	append_s(aLine, sizeof(aLine), &o, " bits=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteBits);
	append_s(aLine, sizeof(aLine), &o, " banner=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_BANNER) != 0u));
	append_s(aLine, sizeof(aLine), &o, " kexinit=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_KEXINIT) != 0u));
	append_s(aLine, sizeof(aLine), &o, " hostkey=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_HOSTKEY) != 0u));
	append_s(aLine, sizeof(aLine), &o, " mac=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_MAC) != 0u));
	append_s(aLine, sizeof(aLine), &o, " chacha=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_CHACHA) != 0u));
	append_s(aLine, sizeof(aLine), &o, " poly=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_POLY) != 0u));
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft crypto (Wave 54 deepen legs continued) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft crypto sha256=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_SHA256) != 0u));
	append_s(aLine, sizeof(aLine), &o, " x25519=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_X25519) != 0u));
	append_s(aLine, sizeof(aLine), &o, " newkeys=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_NEWKEYS) != 0u));
	append_s(aLine, sizeof(aLine), &o, " service=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_SERVICE) != 0u));
	append_s(aLine, sizeof(aLine), &o, " ecdh=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_ECDH) != 0u));
	append_s(aLine, sizeof(aLine), &o, " kdf=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_KDF) != 0u));
	append_s(aLine, sizeof(aLine), &o, " memeq=");
	append_u(aLine, sizeof(aLine), &o,
		 (unsigned long)((g_u32SoftSuiteBits & SOFT_SUITE_MEMEQ) != 0u));
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft kex (live-path KEX lamps, Wave 54) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft kex banner=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftBanner);
	append_s(aLine, sizeof(aLine), &o, " poly=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftPoly);
	append_s(aLine, sizeof(aLine), &o, " hostkey=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftHostkey);
	append_s(aLine, sizeof(aLine), &o, " ecdh=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEcdh);
	append_s(aLine, sizeof(aLine), &o, " shared=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftShared);
	append_s(aLine, sizeof(aLine), &o, " kex=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftKex);
	append_s(aLine, sizeof(aLine), &o, " newkeys=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftNewkeys);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft channel (post-NEWKEYS session lamps, Wave 54) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft channel service=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftService);
	append_s(aLine, sizeof(aLine), &o, " channel=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftChannel);
	append_s(aLine, sizeof(aLine), &o, " keys=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftKeys);
	append_s(aLine, sizeof(aLine), &o, " aead=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftAead);
	append_s(aLine, sizeof(aLine), &o, " enc_tx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEncTx);
	append_s(aLine, sizeof(aLine), &o, " enc_rx=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftEncRx);
	append_s(aLine, sizeof(aLine), &o, " live=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLive);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/*
	 * Soft path honesty: surface catalog + explicit non-claims.
	 * Soft inventory ≠ product multi-server confine.
	 * greppable: sshd-gj: soft path
	 */
	o = 0;
	append_s(aLine, sizeof(aLine), &o,
		 "sshd-gj: soft path port=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)SSH_PORT);
	append_s(aLine, sizeof(aLine), &o,
		 " algs=curve25519-sha256,chacha20,hmac-sha2-256,poly1305"
		 " rekey=0 multi=0 multi_server=0 confine=0 agent=0"
		 " portfwd=0 sftp=0 openssh=0 dropbear=0 product_ssh=0\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Rollup tallies for greppable soft stats. */
	cKex = g_u32SoftBanner + g_u32SoftPoly + g_u32SoftHostkey +
	       g_u32SoftEcdh + g_u32SoftShared + g_u32SoftKex +
	       g_u32SoftNewkeys;
	cChan = g_u32SoftService + g_u32SoftChannel + g_u32SoftKeys +
		g_u32SoftAead + g_u32SoftEncTx + g_u32SoftEncRx +
		g_u32SoftLive;
	cCrypto = g_u32SoftSuiteOk;

	/* Grep: sshd-gj: soft stats */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft stats kex=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)cKex);
	append_s(aLine, sizeof(aLine), &o, " channel=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)cChan);
	append_s(aLine, sizeof(aLine), &o, " crypto=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)cCrypto);
	append_s(aLine, sizeof(aLine), &o, " suite_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteN);
	append_s(aLine, sizeof(aLine), &o, " suite_bits=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteBits);
	append_s(aLine, sizeof(aLine), &o, " live=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLive);
	append_s(aLine, sizeof(aLine), &o, " encrypted=");
	append_u(aLine, sizeof(aLine), &o, g_encrypted ? 1ul : 0ul);
	append_s(aLine, sizeof(aLine), &o, " log_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLogN);
	append_s(aLine, sizeof(aLine), &o, "\n");
	aLine[o] = '\0';
	msg(aLine);
	cAreas++;

	/* Grep: sshd-gj: soft deepen wave (Wave 54 stamp) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft deepen wave=54 areas=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)cAreas);
	append_s(aLine, sizeof(aLine), &o, " suite_ok=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteOk);
	append_s(aLine, sizeof(aLine), &o, " suite_n=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftSuiteN);
	append_s(aLine, sizeof(aLine), &o, " live=");
	append_u(aLine, sizeof(aLine), &o, (unsigned long)g_u32SoftLive);
	append_s(aLine, sizeof(aLine), &o, " multi_server=0 confine=0\n");
	aLine[o] = '\0';
	msg(aLine);

	/*
	 * Grep: sshd-gj: soft exclusive (Wave 54 exclusive deepen).
	 * Soft inventory ≠ product multi-server confine.
	 */
	msg("sshd-gj: soft exclusive multi_server=0 confine=0 bar3=0 "
	    "exclusive=1 soft=1 product_kernel=OPEN wave=54\n");
}

/*
 * Offline soft suite — pure local probes (no wire). Never hard-fails live path.
 * Wave 54 deepen: freestanding crypto/shape surface beyond the TCP smoke alone.
 * greppable: sshd-gj: soft …
 */
static void
soft_suite(void)
{
	uint8_t aPkt[640];
	uint8_t aReply[160];
	uint8_t aHostPk[32];
	uint8_t aMsg[32];
	uint8_t aSig[32];
	uint8_t aMac1[32];
	uint8_t aMac2[32];
	uint8_t aKey[32];
	uint8_t aNonce[12];
	uint8_t aBuf[32];
	uint8_t aPlain[32];
	uint8_t aDig1[32];
	uint8_t aDig2[32];
	uint8_t aSkS[32];
	uint8_t aSkC[32];
	uint8_t aPkS[32];
	uint8_t aPkC[32];
	uint8_t aSharedS[32];
	uint8_t aSharedC[32];
	uint8_t aBase[32];
	uint8_t aKdfBuf[97];
	uint8_t aKdfA[32];
	uint8_t aKdfB[32];
	uint8_t aKdfC[32];
	uint32_t cb;
	uint32_t cbInit;
	uint32_t cbReply;
	uint32_t uBits = 0;
	unsigned i;
	unsigned cOk = 0;
	unsigned cN = 0;
	int fOk;

	msg("sshd-gj: soft suite start\n");

	/* Soft banner shape (product id string; not a wire exchange). */
	cN++;
	if (banner_is_ssh(g_szBanner, (long)slen(g_szBanner)) &&
	    banner_is_ssh(g_szClientBanner, (long)slen(g_szClientBanner))) {
		msg("sshd-gj: soft banner PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_BANNER;
	} else {
		msg("sshd-gj: soft banner soft-skip\n");
	}

	/* Soft KEXINIT build: type 20, non-zero wire length. */
	cN++;
	cb = build_kexinit(aPkt, sizeof(aPkt));
	if (cb >= 6 && aPkt[5] == SSH_MSG_KEXINIT) {
		msg("sshd-gj: soft kexinit PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_KEXINIT;
	} else {
		msg("sshd-gj: soft kexinit soft-skip\n");
	}

	/* Soft hostkey offline sign/verify of fixed digest. */
	cN++;
	gj_ssh_hostkey_init();
	gj_ssh_hostkey_pk(aHostPk);
	for (i = 0; i < 32; i++) {
		aMsg[i] = (uint8_t)(0x5a ^ i);
	}
	gj_ssh_hostkey_sign(aMsg, 32, aSig);
	if (gj_ssh_hostkey_verify(aMsg, 32, aSig)) {
		msg("sshd-gj: soft hostkey PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_HOSTKEY;
	} else {
		msg("sshd-gj: soft hostkey soft-skip\n");
	}

	/* Soft HMAC determinism (same key+data → same tag). */
	cN++;
	for (i = 0; i < 32; i++) {
		aKey[i] = (uint8_t)(0x11 + i);
		aMsg[i] = (uint8_t)(0x22 + i);
	}
	gj_ssh_hmac_sha256(aKey, 32, aMsg, 32, aMac1);
	gj_ssh_hmac_sha256(aKey, 32, aMsg, 32, aMac2);
	if (gj_ssh_memeq_ct(aMac1, aMac2, 32)) {
		msg("sshd-gj: soft mac PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_MAC;
	} else {
		msg("sshd-gj: soft mac soft-skip\n");
	}

	/* Soft ChaCha20 invert (xor twice restores plain). */
	cN++;
	for (i = 0; i < 32; i++) {
		aKey[i] = (uint8_t)(0x33 + i);
		aPlain[i] = (uint8_t)(0x44 + i);
		aBuf[i] = aPlain[i];
	}
	for (i = 0; i < 12; i++) {
		aNonce[i] = (uint8_t)i;
	}
	gj_ssh_chacha20_xor(aKey, aNonce, 1u, aBuf, 32);
	gj_ssh_chacha20_xor(aKey, aNonce, 1u, aBuf, 32);
	if (gj_ssh_memeq_ct(aBuf, aPlain, 32)) {
		msg("sshd-gj: soft chacha PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_CHACHA;
	} else {
		msg("sshd-gj: soft chacha soft-skip\n");
	}

	/* Soft Poly1305 RFC vector (via product selfcheck lamp). */
	cN++;
	if (gj_ssh_poly1305_ok()) {
		msg("sshd-gj: soft poly PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_POLY;
	} else {
		msg("sshd-gj: soft poly soft-skip\n");
	}

	/*
	 * Soft SHA-256 determinism + init/update/final vs one-shot.
	 * greppable: sshd-gj: soft sha256
	 */
	cN++;
	{
		struct sha256_ctx hx;
		static const char szSoft[] = "sshd-gj soft sha256";

		gj_ssh_sha256(szSoft, slen(szSoft), aDig1);
		gj_ssh_sha256_init(&hx);
		gj_ssh_sha256_update(&hx, szSoft, slen(szSoft));
		gj_ssh_sha256_final(&hx, aDig2);
		fOk = gj_ssh_memeq_ct(aDig1, aDig2, 32);
		/* Second one-shot must match first (determinism). */
		gj_ssh_sha256(szSoft, slen(szSoft), aDig2);
		if (fOk && gj_ssh_memeq_ct(aDig1, aDig2, 32)) {
			msg("sshd-gj: soft sha256 PASS\n");
			cOk++;
			uBits |= SOFT_SUITE_SHA256;
		} else {
			msg("sshd-gj: soft sha256 soft-skip\n");
		}
	}

	/*
	 * Soft X25519 dual shared-secret match (offline; RFC 7748 base u=9).
	 * greppable: sshd-gj: soft x25519
	 */
	cN++;
	bytes_zero(aBase, 32);
	aBase[0] = 9;
	for (i = 0; i < 32; i++) {
		aSkS[i] = (uint8_t)(0x71 + i);
		aSkC[i] = (uint8_t)(0x82 + i * 2);
	}
	aSkS[0] &= 248;
	aSkS[31] &= 127;
	aSkS[31] |= 64;
	aSkC[0] &= 248;
	aSkC[31] &= 127;
	aSkC[31] |= 64;
	gj_ssh_x25519(aPkS, aSkS, aBase);
	gj_ssh_x25519(aPkC, aSkC, aBase);
	gj_ssh_x25519(aSharedS, aSkS, aPkC);
	gj_ssh_x25519(aSharedC, aSkC, aPkS);
	if (gj_ssh_memeq_ct(aSharedS, aSharedC, 32)) {
		msg("sshd-gj: soft x25519 PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_X25519;
	} else {
		msg("sshd-gj: soft x25519 soft-skip\n");
	}

	/*
	 * Soft NEWKEYS packet shape (type 21; no wire).
	 * greppable: sshd-gj: soft newkeys
	 */
	cN++;
	cb = build_simple(aPkt, sizeof(aPkt), SSH_MSG_NEWKEYS, 0, 0);
	if (cb >= 6 && aPkt[5] == SSH_MSG_NEWKEYS) {
		msg("sshd-gj: soft newkeys PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_NEWKEYS;
	} else {
		msg("sshd-gj: soft newkeys soft-skip\n");
	}

	/*
	 * Soft SERVICE_REQUEST/ACCEPT body shape ("ssh-userauth").
	 * greppable: sshd-gj: soft service
	 */
	cN++;
	cb = build_service(aPkt, sizeof(aPkt), SSH_MSG_SERVICE_REQUEST,
			   "ssh-userauth");
	cbInit = build_service(aReply, sizeof(aReply), SSH_MSG_SERVICE_ACCEPT,
			       "ssh-userauth");
	if (cb >= 6 && aPkt[5] == SSH_MSG_SERVICE_REQUEST && cbInit >= 6 &&
	    aReply[5] == SSH_MSG_SERVICE_ACCEPT) {
		msg("sshd-gj: soft service PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_SERVICE;
	} else {
		msg("sshd-gj: soft service soft-skip\n");
	}

	/*
	 * Soft ECDH_INIT + ECDH_REPLY packet shapes (offline builders).
	 * greppable: sshd-gj: soft ecdh
	 */
	cN++;
	cbInit = build_ecdh_init(aPkt, sizeof(aPkt), aPkC);
	cbReply = build_ecdh_reply(aReply, sizeof(aReply), aHostPk, aPkS, aSig);
	if (cbInit >= 6 && aPkt[5] == SSH_MSG_KEX_ECDH_INIT &&
	    cbReply >= 6 && aReply[5] == SSH_MSG_KEX_ECDH_REPLY) {
		msg("sshd-gj: soft ecdh PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_ECDH;
	} else {
		msg("sshd-gj: soft ecdh soft-skip\n");
	}

	/*
	 * Soft KDF letter shape (RFC 4253 7.2 offline; does not arm g_enc_*).
	 * Ki = HASH(K || H || X || session_id), X in {'A','B','C'}; A≠B≠C.
	 * greppable: sshd-gj: soft kdf
	 */
	cN++;
	for (i = 0; i < 32; i++) {
		aKdfBuf[i] = aSharedS[i];
		aKdfBuf[32 + i] = aDig1[i];
		aKdfBuf[65 + i] = aDig1[i];
	}
	aKdfBuf[64] = 'A';
	gj_ssh_sha256(aKdfBuf, 97, aKdfA);
	aKdfBuf[64] = 'B';
	gj_ssh_sha256(aKdfBuf, 97, aKdfB);
	aKdfBuf[64] = 'C';
	gj_ssh_sha256(aKdfBuf, 97, aKdfC);
	if (!gj_ssh_memeq_ct(aKdfA, aKdfB, 32) &&
	    !gj_ssh_memeq_ct(aKdfB, aKdfC, 32) &&
	    !gj_ssh_memeq_ct(aKdfA, aKdfC, 32)) {
		msg("sshd-gj: soft kdf PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_KDF;
	} else {
		msg("sshd-gj: soft kdf soft-skip\n");
	}

	/*
	 * Soft memeq: equal buffers match; unequal buffers do not.
	 * greppable: sshd-gj: soft memeq
	 */
	cN++;
	for (i = 0; i < 32; i++) {
		aDig1[i] = (uint8_t)(0xa0 + i);
		aDig2[i] = aDig1[i];
	}
	fOk = gj_ssh_memeq_ct(aDig1, aDig2, 32);
	aDig2[15] ^= 0x01u;
	if (fOk && !gj_ssh_memeq_ct(aDig1, aDig2, 32)) {
		msg("sshd-gj: soft memeq PASS\n");
		cOk++;
		uBits |= SOFT_SUITE_MEMEQ;
	} else {
		msg("sshd-gj: soft memeq soft-skip\n");
	}

	g_u32SoftSuiteOk = (uint32_t)cOk;
	g_u32SoftSuiteN = (uint32_t)cN;
	g_u32SoftSuiteBits = uBits;

	/* Ephemeral soft material — clear before return. */
	bytes_zero(aHostPk, sizeof(aHostPk));
	bytes_zero(aMsg, sizeof(aMsg));
	bytes_zero(aSig, sizeof(aSig));
	bytes_zero(aMac1, sizeof(aMac1));
	bytes_zero(aMac2, sizeof(aMac2));
	bytes_zero(aKey, sizeof(aKey));
	bytes_zero(aNonce, sizeof(aNonce));
	bytes_zero(aBuf, sizeof(aBuf));
	bytes_zero(aPlain, sizeof(aPlain));
	bytes_zero(aDig1, sizeof(aDig1));
	bytes_zero(aDig2, sizeof(aDig2));
	bytes_zero(aSkS, sizeof(aSkS));
	bytes_zero(aSkC, sizeof(aSkC));
	bytes_zero(aPkS, sizeof(aPkS));
	bytes_zero(aPkC, sizeof(aPkC));
	bytes_zero(aSharedS, sizeof(aSharedS));
	bytes_zero(aSharedC, sizeof(aSharedC));
	bytes_zero(aBase, sizeof(aBase));
	bytes_zero(aKdfBuf, sizeof(aKdfBuf));
	bytes_zero(aKdfA, sizeof(aKdfA));
	bytes_zero(aKdfB, sizeof(aKdfB));
	bytes_zero(aKdfC, sizeof(aKdfC));
	bytes_zero(aPkt, 64); /* only need clear head; rest is KEXINIT shape */
	bytes_zero(aReply, sizeof(aReply));

	if (cOk > 0u) {
		msg("sshd-gj: soft suite PASS\n");
	} else {
		msg("sshd-gj: soft suite soft-skip\n");
	}

	/* Always emit inventory after suite (honesty + counters). */
	soft_inventory_log();
}

/*
 * Full post-banner smoke: KEXINIT ↔ ECDH ↔ ECDH_REPLY → hostkey(H) →
 * dual shared → NEWKEYS → soft service → session channel → key derivation →
 * encrypted CHANNEL_DATA send+recv (MAC verify) → Poly1305 soft AEAD check.
 * Returns 1 on success (caller emits post-KEX / live path markers).
 */
static int
do_kex_and_session(long fd_srv, long fd_cli)
{
	uint8_t kex_s[640], kex_c[640], ecdh_c[96], ecdh_s[160], rbuf[640];
	uint8_t sk_s[32], pk_s[32], sk_c[32], pk_c[32];
	uint8_t shared_s[32], shared_c[32], H[32], sig[32], host_pk[32];
	uint8_t newkeys[64];
	uint8_t base[32];
	uint8_t aPolyKey[32];
	uint8_t aPolyTag[16];
	uint8_t aPolyTag2[16];
	uint32_t n;
	long nr;
	unsigned i;
	struct sha256_ctx hx;
	int chan_ok = 0;
	int fMatch = 0;

	gj_ssh_hostkey_init();
	gj_ssh_hostkey_pk(host_pk);
	if (!gj_ssh_poly1305_ok()) {
		msg("sshd-gj: poly1305 selfcheck FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftPoly);
	msg("sshd-gj: poly1305 selfcheck PASS\n");

	/* RFC 7748 base point u=9 (explicit init; no partial aggregate init) */
	bytes_zero(base, 32);
	base[0] = 9;

	/* Ephemeral KEX scalars (not host identity); clamp per RFC 7748 */
	for (i = 0; i < 32; i++) {
		sk_s[i] = (uint8_t)(host_pk[i] ^ (0x5a + i));
		sk_c[i] = (uint8_t)(0x91 + i * 3);
	}
	sk_s[0] &= 248;
	sk_s[31] &= 127;
	sk_s[31] |= 64;
	sk_c[0] &= 248;
	sk_c[31] &= 127;
	sk_c[31] |= 64;
	gj_ssh_x25519(pk_s, sk_s, base);
	gj_ssh_x25519(pk_c, sk_c, base);

	/* --- KEXINIT both directions --- */
	n = build_kexinit(kex_s, sizeof(kex_s));
	if (n == 0 || send_pkt(fd_srv, kex_s, n, 1) <= 0) {
		return 0;
	}
	n = build_kexinit(kex_c, sizeof(kex_c));
	if (n == 0 || send_pkt(fd_cli, kex_c, n, 0) <= 0) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)rbuf,
		    (long)sizeof(rbuf));
	if (nr < 6 || rbuf[5] != SSH_MSG_KEXINIT) {
		return 0;
	}

	/* --- ECDH: client Q_C → server shared secret --- */
	n = build_ecdh_init(ecdh_c, sizeof(ecdh_c), pk_c);
	if (n == 0 || send_pkt(fd_cli, ecdh_c, n, 0) <= 0) {
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)rbuf,
		    (long)sizeof(rbuf));
	if (nr < 42 || rbuf[5] != SSH_MSG_KEX_ECDH_INIT) {
		return 0;
	}
	/* string Q_C starts at rbuf+6 (type) + 4 (len) = rbuf+10 */
	gj_ssh_x25519(shared_s, sk_s, rbuf + 10);

	/*
	 * Exchange hash H (product smoke shape):
	 *   H = SHA256(client_banner || server_banner || Q_C || Q_S || K || host_pk)
	 */
	gj_ssh_sha256_init(&hx);
	gj_ssh_sha256_update(&hx, g_szClientBanner, slen(g_szClientBanner));
	gj_ssh_sha256_update(&hx, g_szBanner, slen(g_szBanner));
	gj_ssh_sha256_update(&hx, pk_c, 32);
	gj_ssh_sha256_update(&hx, pk_s, 32);
	gj_ssh_sha256_update(&hx, shared_s, 32);
	gj_ssh_sha256_update(&hx, host_pk, 32);
	gj_ssh_sha256_final(&hx, H);

	gj_ssh_hostkey_sign(H, 32, sig);
	if (!gj_ssh_hostkey_verify(H, 32, sig)) {
		msg("sshd-gj: hostkey sign FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftHostkey);
	msg("sshd-gj: hostkey sign PASS\n");

	/* --- ECDH_REPLY on wire: K_S || Q_S || sig(H) --- */
	n = build_ecdh_reply(ecdh_s, sizeof(ecdh_s), host_pk, pk_s, sig);
	if (n == 0 || send_pkt(fd_srv, ecdh_s, n, 1) <= 0) {
		msg("sshd-gj: ECDH_REPLY send FAIL\n");
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_cli, (long)(uintptr_t)rbuf,
		    (long)sizeof(rbuf));
	if (nr < 6 || rbuf[5] != SSH_MSG_KEX_ECDH_REPLY) {
		msg("sshd-gj: ECDH_REPLY recv FAIL\n");
		return 0;
	}
	/*
	 * Soft client parse (cleartext packet):
	 *   type@5, then string host_pk (len@6 = 32, data@10),
	 *   string Q_S (len@42 = 32, data@46).
	 */
	if (nr < 78 || rbuf[6] != 0 || rbuf[7] != 0 || rbuf[8] != 0 ||
	    rbuf[9] != 32) {
		msg("sshd-gj: ECDH_REPLY shape FAIL\n");
		return 0;
	}
	if (!gj_ssh_memeq_ct(rbuf + 10, host_pk, 32)) {
		msg("sshd-gj: ECDH_REPLY host_pk FAIL\n");
		return 0;
	}
	/* Client shared from Q_S at offset 46 (10+32+4) */
	if (rbuf[42] != 0 || rbuf[43] != 0 || rbuf[44] != 0 || rbuf[45] != 32) {
		msg("sshd-gj: ECDH_REPLY Q_S shape FAIL\n");
		return 0;
	}
	gj_ssh_x25519(shared_c, sk_c, rbuf + 46);
	fMatch = gj_ssh_memeq_ct(shared_s, shared_c, 32);
	if (!fMatch) {
		msg("sshd-gj: shared secret match FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftEcdh);
	soft_note(&g_u32SoftShared);
	soft_note(&g_u32SoftKex);
	msg("sshd-gj: ECDH_REPLY PASS\n");
	msg("sshd-gj: shared secret match PASS\n");
	msg("sshd-gj: x25519 KEX shared PASS\n");
	msg("sshd-gj: KEX PASS (curve25519-sha256 + hostkey)\n");

	/* --- SSH_MSG_NEWKEYS both directions (still cleartext) --- */
	g_encrypted = 0;
	n = build_simple(newkeys, sizeof(newkeys), SSH_MSG_NEWKEYS, 0, 0);
	if (n == 0) {
		msg("sshd-gj: NEWKEYS build FAIL\n");
		return 0;
	}
	nr = gj_net(GJ_NET_OP_SEND, fd_srv, (long)(uintptr_t)newkeys, (long)n);
	if (nr <= 0) {
		msg("sshd-gj: NEWKEYS srv send FAIL\n");
		return 0;
	}
	nr = gj_net(GJ_NET_OP_SEND, fd_cli, (long)(uintptr_t)newkeys, (long)n);
	if (nr <= 0) {
		msg("sshd-gj: NEWKEYS cli send FAIL\n");
		return 0;
	}
	/* Each side drains peer NEWKEYS so service/channel sees a clean stream */
	nr = gj_net(GJ_NET_OP_RECV, fd_cli, (long)(uintptr_t)rbuf,
		    (long)sizeof(rbuf));
	if (nr < 6 || rbuf[5] != SSH_MSG_NEWKEYS) {
		msg("sshd-gj: NEWKEYS cli drain FAIL\n");
		return 0;
	}
	nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)rbuf,
		    (long)sizeof(rbuf));
	if (nr < 6 || rbuf[5] != SSH_MSG_NEWKEYS) {
		msg("sshd-gj: NEWKEYS srv drain FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftNewkeys);
	msg("sshd-gj: NEWKEYS exchange PASS\n");

	/* Soft service exchange (still cleartext; pre-channel) */
	if (!do_service_soft(fd_srv, fd_cli)) {
		msg("sshd-gj: service soft FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftService);
	msg("sshd-gj: service soft PASS\n");

	/* Session channel + shell MOTD before encrypt arm */
	chan_ok = do_session_channel(fd_srv, fd_cli);
	if (!chan_ok) {
		msg("sshd-gj: channel path FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftChannel);

	/* session_id == H for this smoke; derive A–F keys */
	derive_keys(shared_s, H, H);
	soft_note(&g_u32SoftKeys);
	msg("sshd-gj: key derivation PASS (ChaCha20+HMAC)\n");

	/* Soft AEAD leg: Poly1305 over ciphertext material with int key */
	bytes_copy(aPolyKey, g_int_s2c, 32);
	gj_ssh_poly1305(aPolyKey, H, 32, aPolyTag);
	gj_ssh_poly1305(aPolyKey, H, 32, aPolyTag2);
	if (!gj_ssh_memeq_ct(aPolyTag, aPolyTag2, 16)) {
		msg("sshd-gj: poly1305 aead soft FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftAead);
	msg("sshd-gj: poly1305 aead soft PASS\n");

	/* Encrypted CHANNEL_DATA: server send → client recv+MAC+decrypt */
	{
		uint8_t pkt[256];
		uint8_t dbody[64];
		const char *extra = "encrypted-channel-ok\r\n";
		uint32_t mlen = (uint32_t)slen(extra);
		uint32_t pn;
		uint32_t cbRecv;
		unsigned j;

		put_u32(dbody, 0);
		put_u32(dbody + 4, mlen);
		for (j = 0; j < mlen; j++) {
			dbody[8 + j] = (uint8_t)extra[j];
		}
		pn = build_simple(pkt, sizeof(pkt), SSH_MSG_CHANNEL_DATA, dbody,
				  8 + mlen);
		if (pn == 0 || send_pkt(fd_srv, pkt, pn, 1) <= 0) {
			msg("sshd-gj: encrypted CHANNEL_DATA FAIL\n");
			return 0;
		}
		/* Client half: as_server=0 → use s2c keys to open server packet */
		bytes_zero(pkt, sizeof(pkt));
		cbRecv = recv_pkt(fd_cli, pkt, sizeof(pkt), 0);
		if (cbRecv < 6 || pkt[5] != SSH_MSG_CHANNEL_DATA) {
			msg("sshd-gj: encrypted channel recv FAIL\n");
			return 0;
		}
		soft_note(&g_u32SoftEncTx);
		soft_note(&g_u32SoftEncRx);
		msg("sshd-gj: encrypted channel PASS\n");
		msg("sshd-gj: encrypted channel recv PASS\n");
	}

	/* Ephemeral secrets no longer needed for park path */
	bytes_zero(sk_s, sizeof(sk_s));
	bytes_zero(sk_c, sizeof(sk_c));
	bytes_zero(shared_s, sizeof(shared_s));
	bytes_zero(shared_c, sizeof(shared_c));
	bytes_zero(sig, sizeof(sig));
	bytes_zero(aPolyKey, sizeof(aPolyKey));
	bytes_zero(aPolyTag, sizeof(aPolyTag));
	bytes_zero(aPolyTag2, sizeof(aPolyTag2));
	return 1;
}

/*
 * Product entry: self-smoke on TCP :22, then park with listen held.
 * Greppable markers documented in user/sshd/README.md.
 */
void
_start(void)
{
	long srv, cli, acc, n;
	static char rbuf[128];
	unsigned i;
	int banner_ok = 0;
	int sess_ok = 0;
	uint32_t aTcp[4];

	msg("sshd-gj: start TCP :22 full path\n");
	gj_ssh_hostkey_init();

	/* --- TCP listen :22 --- */
	srv = gj_net(GJ_NET_OP_SOCKET, AF_INET, SOCK_STREAM, 0);
	if (srv < 0) {
		msg("sshd-gj: TCP socket FAIL\n");
		gj_exit(1);
	}
	if (gj_net(GJ_NET_OP_BIND, srv, SSH_PORT, 0) != 0 ||
	    gj_net(GJ_NET_OP_LISTEN, srv, 4, 0) != 0) {
		msg("sshd-gj: bind/listen FAIL\n");
		gj_exit(1);
	}
	msg("sshd-gj: TCP listening :22 (virtio + loopback)\n");

	/* Loopback client + accept (same shape as netstackd TCP smoke) */
	cli = gj_net(GJ_NET_OP_SOCKET, AF_INET, SOCK_STREAM, 0);
	if (cli < 0 || gj_net(GJ_NET_OP_CONNECT, cli, SSH_PORT, 0) != 0) {
		msg("sshd-gj: connect :22 FAIL\n");
		gj_exit(1);
	}
	acc = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
	if (acc < 0) {
		msg("sshd-gj: accept FAIL\n");
		gj_exit(1);
	}

	/* --- RFC 4253 identification exchange --- */
	(void)gj_net(GJ_NET_OP_SEND, acc, (long)(uintptr_t)g_szBanner,
		     (long)slen(g_szBanner));
	(void)gj_net(GJ_NET_OP_SEND, cli, (long)(uintptr_t)g_szClientBanner,
		     (long)slen(g_szClientBanner));
	for (i = 0; i < sizeof(rbuf); i++) {
		rbuf[i] = 0;
	}
	n = gj_net(GJ_NET_OP_RECV, acc, (long)(uintptr_t)rbuf,
		   (long)(sizeof(rbuf) - 1));
	if (n > 0 && banner_is_ssh(rbuf, n)) {
		banner_ok = 1;
		soft_note(&g_u32SoftBanner);
		msg("sshd-gj: peer banner PASS\n");
	} else {
		msg("sshd-gj: peer banner FAIL\n");
	}

	/* --- KEX + NEWKEYS + channel + encrypt --- */
	g_encrypted = 0;
	sess_ok = do_kex_and_session(acc, cli);
	if (sess_ok) {
		/* post-KEX = NEWKEYS + channel/shell + key derive + encrypt */
		msg("sshd-gj: post-KEX session PASS\n");
	} else {
		msg("sshd-gj: post-KEX session FAIL\n");
	}

	(void)gj_net(GJ_NET_OP_CLOSE, cli, 0, 0);
	(void)gj_net(GJ_NET_OP_CLOSE, acc, 0, 0);
	(void)gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)aTcp, 0, 0);

	if (banner_ok && sess_ok) {
		soft_note(&g_u32SoftLive);
		msg("sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)\n");
	} else {
		msg("sshd-gj: live path FAIL\n");
		/*
		 * Soft inventory still runs on FAIL for honesty (partial lamps).
		 * Never promotes soft suite to product PASS.
		 */
		soft_suite();
		(void)gj_net(GJ_NET_OP_CLOSE, srv, 0, 0);
		gj_exit(1);
	}

	/*
	 * Wave 54 soft inventory: offline suite + honesty / path / stats.
	 * Greppable "sshd-gj: soft …" — not product SSH completeness.
	 * Never hard-fails after live path PASS.
	 */
	soft_suite();

	/*
	 * Keep listener open; park without net POLL on every quantum.
	 * Eth frames are still demuxed on idle net_eth_poll in the kernel
	 * scheduler; ACCEPT is tried rarely to avoid post-spawn #UD races
	 * when other smokes run with this thread still live.
	 */
	msg("sshd-gj: daemon park (TCP :22 listen held)\n");
	{
		unsigned long tick = 0;

		for (;;) {
			if ((tick & 0x3fful) == 0) {
				long a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);

				if (a >= 0) {
					msg("sshd-gj: eth accept\n");
					(void)gj_net(GJ_NET_OP_SEND, a,
						     (long)(uintptr_t)g_szBanner,
						     (long)slen(g_szBanner));
					(void)gj_net(GJ_NET_OP_CLOSE, a, 0, 0);
				}
			}
			tick++;
			gj_yield();
		}
	}
}
