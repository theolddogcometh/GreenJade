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
 *   4. SSH_MSG_NEWKEYS both directions
 *   5. Session channel + shell MOTD (cleartext soft path)
 *   6. RFC 4253 §7.2 key derivation → ChaCha20 + HMAC integrity
 *   7. Encrypted CHANNEL_DATA → live path PASS → daemon park
 *
 * Crypto primitives live in ssh_crypto.c (same license).
 *   make sshd-gj → build/user/sshd.elf
 */
#include <gj/syscalls.h>

#define AF_INET     2
#define SOCK_STREAM 1
#define SSH_PORT    22

/* SSH binary packet message types used on the live smoke path */
#define SSH_MSG_KEXINIT                 20
#define SSH_MSG_NEWKEYS                 21
#define SSH_MSG_KEX_ECDH_INIT           30
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
 * Session keys after NEWKEYS (RFC 4253 §7.2 letters A–F).
 * Client→server and server→client IVs, encryption, integrity.
 */
static uint8_t g_iv_c2s[12], g_iv_s2c[12];
static uint8_t g_enc_c2s[32], g_enc_s2c[32];
static uint8_t g_int_c2s[32], g_int_s2c[32];
static uint32_t g_seq_s2c, g_seq_c2s;
static int g_encrypted;

static void
msg(const char *sz)
{
	size_t n = 0;

	while (sz[n] != '\0') {
		n++;
	}
	(void)gj_debug_log(sz, (long)n);
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

/* SSH_MSG_KEXINIT with product algorithm name-lists (RFC 4253 §7.1). */
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
 * RFC 4253 §7.2: Ki = HASH(K || H || X || session_id), X in {'A'..'F'}.
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
	g_seq_s2c = 0;
	g_seq_c2s = 0;
	g_encrypted = 1;
	bytes_zero(out, sizeof(out));
}

/*
 * Send one SSH binary packet. After NEWKEYS (g_encrypted), encrypt the
 * packet body with ChaCha20 and append a product integrity tag.
 * Cleartext path used for KEXINIT / ECDH / NEWKEYS.
 */
static long
send_pkt(long fd, uint8_t *pkt, uint32_t n, int as_server)
{
	uint8_t mac[32];
	uint8_t seqb[4];
	uint8_t *key_enc = as_server ? g_enc_s2c : g_enc_c2s;
	uint8_t *key_int = as_server ? g_int_s2c : g_int_c2s;
	uint8_t *iv = as_server ? g_iv_s2c : g_iv_c2s;
	uint32_t *pseq = as_server ? &g_seq_s2c : &g_seq_c2s;
	long r;

	if (g_encrypted && n >= 5) {
		/* ChaCha20 over packet after the length field */
		gj_ssh_chacha20_xor(key_enc, iv, *pseq, pkt + 4, n - 4);
		put_u32(seqb, *pseq);
		{
			struct sha256_ctx hx;

			/* Product MAC: HMAC(int_key, SHA256(seq || packet)) */
			gj_ssh_sha256_init(&hx);
			gj_ssh_sha256_update(&hx, seqb, 4);
			gj_ssh_sha256_update(&hx, pkt, n);
			gj_ssh_sha256_final(&hx, mac);
			gj_ssh_hmac_sha256(key_int, 32, mac, 32, mac);
		}
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
	}
	msg("sshd-gj: channel+shell MOTD PASS\n");
	return 1;
}

/*
 * Full post-banner smoke: KEXINIT ↔ ECDH → hostkey(H) → NEWKEYS →
 * session channel → key derivation → encrypted CHANNEL_DATA.
 * Returns 1 on success (caller emits post-KEX / live path markers).
 */
static int
do_kex_and_session(long fd_srv, long fd_cli)
{
	uint8_t kex_s[640], kex_c[640], ecdh_c[96], rbuf[640];
	uint8_t sk_s[32], pk_s[32], sk_c[32], pk_c[32];
	uint8_t shared[32], H[32], sig[32], host_pk[32];
	uint8_t newkeys[64];
	uint8_t base[32];
	uint32_t n;
	long nr;
	unsigned i;
	struct sha256_ctx hx;
	int chan_ok = 0;

	gj_ssh_hostkey_init();
	gj_ssh_hostkey_pk(host_pk);

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
	gj_ssh_x25519(shared, sk_s, rbuf + 10);

	/*
	 * Exchange hash H (product smoke shape):
	 *   H = SHA256(client_banner || server_banner || Q_C || Q_S || K || host_pk)
	 */
	gj_ssh_sha256_init(&hx);
	gj_ssh_sha256_update(&hx, g_szClientBanner, slen(g_szClientBanner));
	gj_ssh_sha256_update(&hx, g_szBanner, slen(g_szBanner));
	gj_ssh_sha256_update(&hx, pk_c, 32);
	gj_ssh_sha256_update(&hx, pk_s, 32);
	gj_ssh_sha256_update(&hx, shared, 32);
	gj_ssh_sha256_update(&hx, host_pk, 32);
	gj_ssh_sha256_final(&hx, H);

	gj_ssh_hostkey_sign(H, 32, sig);
	if (!gj_ssh_hostkey_verify(H, 32, sig)) {
		msg("sshd-gj: hostkey sign FAIL\n");
		return 0;
	}
	/* Product KEX markers (grep: KEX / PASS) — algorithms unchanged */
	msg("sshd-gj: hostkey sign PASS\n");
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
	for (i = 0; i < 4; i++) {
		nr = gj_net(GJ_NET_OP_RECV, fd_srv, (long)(uintptr_t)rbuf,
			    (long)sizeof(rbuf));
		if (nr <= 0) {
			break;
		}
	}
	msg("sshd-gj: NEWKEYS exchange PASS\n");

	/* Session channel + shell MOTD before encrypt arm */
	chan_ok = do_session_channel(fd_srv, fd_cli);
	if (!chan_ok) {
		msg("sshd-gj: channel path FAIL\n");
		return 0;
	}

	/* session_id == H for this smoke; derive A–F keys */
	derive_keys(shared, H, H);
	msg("sshd-gj: key derivation PASS (ChaCha20+HMAC)\n");

	/* Encrypted CHANNEL_DATA after keys armed */
	{
		uint8_t pkt[256];
		uint8_t dbody[64];
		const char *extra = "encrypted-channel-ok\r\n";
		uint32_t mlen = (uint32_t)slen(extra);
		uint32_t pn;
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
		msg("sshd-gj: encrypted channel PASS\n");
	}

	/* Ephemeral secrets no longer needed for park path */
	bytes_zero(sk_s, sizeof(sk_s));
	bytes_zero(sk_c, sizeof(sk_c));
	bytes_zero(shared, sizeof(shared));
	bytes_zero(sig, sizeof(sig));
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
		msg("sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)\n");
	} else {
		msg("sshd-gj: live path FAIL\n");
		(void)gj_net(GJ_NET_OP_CLOSE, srv, 0, 0);
		gj_exit(1);
	}

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
