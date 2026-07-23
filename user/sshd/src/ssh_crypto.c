/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding product crypto for GreenJade sshd (sshd_gj).
 *
 * Primitives (clean-room pure C; dual MIT OR Apache-2.0):
 *   SHA-256          — FIPS 180-4
 *   X25519           — RFC 7748 Montgomery ladder
 *   HMAC-SHA256      — RFC 2104 over SHA-256
 *   ChaCha20         — RFC 8439 quarter-round stream cipher
 *   Host identity    — seeded product key + HMAC-SHA256 of exchange hash H
 *
 * Used by freestanding KEX: curve25519-sha256@libssh.org, NEWKEYS key
 * derivation (RFC 4253 §7.2), and post-NEWKEYS channel encrypt/MAC.
 *
 * Not an OpenSSH / Dropbear source paste. No GPL/LGPL code. Algorithms
 * follow published RFCs and well-known public-domain ladder structure.
 */
#include <stddef.h>
#include <stdint.h>

/* ---- SHA-256 (FIPS 180-4) ----------------------------------------------- */

struct sha256_ctx {
	uint32_t state[8];
	uint64_t bitlen;
	uint8_t data[64];
	uint32_t datalen;
};

static uint32_t
rotr(uint32_t x, uint32_t n)
{
	return (x >> n) | (x << (32 - n));
}

static void
sha256_transform(struct sha256_ctx *ctx, const uint8_t data[64])
{
	static const uint32_t k[64] = {
	    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
	    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
	    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
	    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
	    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
	    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
	    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
	    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
	    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
	    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
	    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
	    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
	    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
	};
	uint32_t m[64];
	uint32_t a, b, c, d, e, f, g, h;
	uint32_t i, t1, t2;

	for (i = 0; i < 16; i++) {
		m[i] = ((uint32_t)data[i * 4] << 24) |
		       ((uint32_t)data[i * 4 + 1] << 16) |
		       ((uint32_t)data[i * 4 + 2] << 8) |
		       ((uint32_t)data[i * 4 + 3]);
	}
	for (; i < 64; i++) {
		uint32_t s0 = rotr(m[i - 15], 7) ^ rotr(m[i - 15], 18) ^
			      (m[i - 15] >> 3);
		uint32_t s1 = rotr(m[i - 2], 17) ^ rotr(m[i - 2], 19) ^
			      (m[i - 2] >> 10);

		m[i] = m[i - 16] + s0 + m[i - 7] + s1;
	}
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];
	for (i = 0; i < 64; i++) {
		uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
		uint32_t ch = (e & f) ^ ((~e) & g);
		uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
		uint32_t maj = (a & b) ^ (a & c) ^ (b & c);

		t1 = h + S1 + ch + k[i] + m[i];
		t2 = S0 + maj;
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}
	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void
gj_ssh_sha256_init(struct sha256_ctx *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667u;
	ctx->state[1] = 0xbb67ae85u;
	ctx->state[2] = 0x3c6ef372u;
	ctx->state[3] = 0xa54ff53au;
	ctx->state[4] = 0x510e527fu;
	ctx->state[5] = 0x9b05688cu;
	ctx->state[6] = 0x1f83d9abu;
	ctx->state[7] = 0x5be0cd19u;
}

void
gj_ssh_sha256_update(struct sha256_ctx *ctx, const void *data, size_t len)
{
	const uint8_t *p = (const uint8_t *)data;
	size_t i;

	for (i = 0; i < len; i++) {
		ctx->data[ctx->datalen++] = p[i];
		if (ctx->datalen == 64) {
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void
gj_ssh_sha256_final(struct sha256_ctx *ctx, uint8_t hash[32])
{
	uint32_t i = ctx->datalen;

	ctx->data[i++] = 0x80;
	if (i > 56) {
		while (i < 64) {
			ctx->data[i++] = 0;
		}
		sha256_transform(ctx, ctx->data);
		i = 0;
	}
	while (i < 56) {
		ctx->data[i++] = 0;
	}
	ctx->bitlen += (uint64_t)ctx->datalen * 8u;
	ctx->data[63] = (uint8_t)ctx->bitlen;
	ctx->data[62] = (uint8_t)(ctx->bitlen >> 8);
	ctx->data[61] = (uint8_t)(ctx->bitlen >> 16);
	ctx->data[60] = (uint8_t)(ctx->bitlen >> 24);
	ctx->data[59] = (uint8_t)(ctx->bitlen >> 32);
	ctx->data[58] = (uint8_t)(ctx->bitlen >> 40);
	ctx->data[57] = (uint8_t)(ctx->bitlen >> 48);
	ctx->data[56] = (uint8_t)(ctx->bitlen >> 56);
	sha256_transform(ctx, ctx->data);
	for (i = 0; i < 4; i++) {
		hash[i] = (uint8_t)((ctx->state[0] >> (24 - i * 8)) & 0xff);
		hash[i + 4] =
		    (uint8_t)((ctx->state[1] >> (24 - i * 8)) & 0xff);
		hash[i + 8] =
		    (uint8_t)((ctx->state[2] >> (24 - i * 8)) & 0xff);
		hash[i + 12] =
		    (uint8_t)((ctx->state[3] >> (24 - i * 8)) & 0xff);
		hash[i + 16] =
		    (uint8_t)((ctx->state[4] >> (24 - i * 8)) & 0xff);
		hash[i + 20] =
		    (uint8_t)((ctx->state[5] >> (24 - i * 8)) & 0xff);
		hash[i + 24] =
		    (uint8_t)((ctx->state[6] >> (24 - i * 8)) & 0xff);
		hash[i + 28] =
		    (uint8_t)((ctx->state[7] >> (24 - i * 8)) & 0xff);
	}
}

void
gj_ssh_sha256(const void *data, size_t len, uint8_t out[32])
{
	struct sha256_ctx ctx;

	gj_ssh_sha256_init(&ctx);
	gj_ssh_sha256_update(&ctx, data, len);
	gj_ssh_sha256_final(&ctx, out);
}


/*
 * ---- X25519 (RFC 7748 Montgomery ladder) --------------------------------
 * Compact field arithmetic + constant-time conditional swap. Structure
 * matches the classic public-domain Montgomery ladder form (TweetNaCl-
 * style), rewritten as GreenJade product C. Not OpenSSH source.
 */
typedef int64_t gf[16];

static void
car25519(gf o)
{
	int i;
	int64_t c;

	for (i = 0; i < 16; i++) {
		o[i] += (1LL << 16);
		c = o[i] >> 16;
		o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
		o[i] -= c << 16;
	}
}

static void
sel25519(gf p, gf q, int b)
{
	int64_t t, i, c = ~(int64_t)(b - 1);

	for (i = 0; i < 16; i++) {
		t = c & (p[i] ^ q[i]);
		p[i] ^= t;
		q[i] ^= t;
	}
}

static void
pack25519(uint8_t *o, const gf n)
{
	int i, j, b;
	gf m, t;

	for (i = 0; i < 16; i++) {
		t[i] = n[i];
	}
	car25519(t);
	car25519(t);
	car25519(t);
	for (j = 0; j < 2; j++) {
		m[0] = t[0] - 0xffed;
		for (i = 1; i < 15; i++) {
			m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
			m[i - 1] &= 0xffff;
		}
		m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
		b = (m[15] >> 16) & 1;
		m[14] &= 0xffff;
		sel25519(t, m, 1 - b);
	}
	for (i = 0; i < 16; i++) {
		o[2 * i] = (uint8_t)t[i];
		o[2 * i + 1] = (uint8_t)(t[i] >> 8);
	}
}

static void
unpack25519(gf o, const uint8_t *n)
{
	int i;

	for (i = 0; i < 16; i++) {
		o[i] = n[2 * i] + ((int64_t)n[2 * i + 1] << 8);
	}
	o[15] &= 0x7fff;
}

static void
A(gf o, const gf a, const gf b)
{
	int i;

	for (i = 0; i < 16; i++) {
		o[i] = a[i] + b[i];
	}
}

static void
Z(gf o, const gf a, const gf b)
{
	int i;

	for (i = 0; i < 16; i++) {
		o[i] = a[i] - b[i];
	}
}

static void
M(gf o, const gf a, const gf b)
{
	int64_t i, j, t[31];

	for (i = 0; i < 31; i++) {
		t[i] = 0;
	}
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			t[i + j] += a[i] * b[j];
		}
	}
	for (i = 0; i < 15; i++) {
		t[i] += 38 * t[i + 16];
	}
	for (i = 0; i < 16; i++) {
		o[i] = t[i];
	}
	car25519(o);
	car25519(o);
}

static void
S(gf o, const gf a)
{
	M(o, a, a);
}

static void
inv25519(gf o, const gf i)
{
	gf c;
	int a;

	for (a = 0; a < 16; a++) {
		c[a] = i[a];
	}
	for (a = 253; a >= 0; a--) {
		S(c, c);
		if (a != 2 && a != 4) {
			M(c, c, i);
		}
	}
	for (a = 0; a < 16; a++) {
		o[a] = c[a];
	}
}

static const gf _121665 = { 0xDB41, 1 };

void
gj_ssh_x25519(uint8_t *q, const uint8_t *n, const uint8_t *p)
{
	uint8_t z[32];
	int64_t r;
	int i;
	gf a, b, c, d, e, f;
	gf x;

	for (i = 0; i < 31; i++) {
		z[i] = n[i];
	}
	z[31] = (n[31] & 127) | 64;
	z[0] &= 248;
	unpack25519(x, p);
	for (i = 0; i < 16; i++) {
		b[i] = x[i];
		d[i] = a[i] = c[i] = 0;
	}
	a[0] = d[0] = 1;
	for (i = 254; i >= 0; --i) {
		r = (z[i >> 3] >> (i & 7)) & 1;
		sel25519(a, b, (int)r);
		sel25519(c, d, (int)r);
		A(e, a, c);
		Z(a, a, c);
		A(c, b, d);
		Z(b, b, d);
		S(d, e);
		S(f, a);
		M(a, c, a);
		M(c, b, e);
		A(e, a, c);
		Z(a, a, c);
		S(b, a);
		Z(c, d, f);
		M(a, c, _121665);
		A(a, a, d);
		M(c, c, a);
		M(a, d, f);
		M(d, b, x);
		S(b, e);
		sel25519(a, b, (int)r);
		sel25519(c, d, (int)r);
	}
	inv25519(c, c);
	M(a, a, c);
	pack25519(q, a);
}

/* ---- HMAC-SHA256 (RFC 2104) --------------------------------------------- */

/* Best-effort wipe of stack secrets (no libc; pure C). */
static void
bytes_wipe(uint8_t *p, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		p[i] = 0;
	}
}

void
gj_ssh_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data,
		   size_t data_len, uint8_t out[32])
{
	struct sha256_ctx ctx;
	uint8_t k[64];
	uint8_t kopad[64];
	uint8_t kipad[64];
	uint8_t tk[32];
	uint8_t ih[32];
	size_t i;

	if (key_len > 64) {
		gj_ssh_sha256(key, key_len, tk);
		key = tk;
		key_len = 32;
	}
	for (i = 0; i < 64; i++) {
		k[i] = 0;
	}
	for (i = 0; i < key_len; i++) {
		k[i] = key[i];
	}
	for (i = 0; i < 64; i++) {
		kipad[i] = (uint8_t)(k[i] ^ 0x36);
		kopad[i] = (uint8_t)(k[i] ^ 0x5c);
	}
	gj_ssh_sha256_init(&ctx);
	gj_ssh_sha256_update(&ctx, kipad, 64);
	gj_ssh_sha256_update(&ctx, data, data_len);
	gj_ssh_sha256_final(&ctx, ih);
	gj_ssh_sha256_init(&ctx);
	gj_ssh_sha256_update(&ctx, kopad, 64);
	gj_ssh_sha256_update(&ctx, ih, 32);
	gj_ssh_sha256_final(&ctx, out);
	bytes_wipe(k, sizeof(k));
	bytes_wipe(kipad, sizeof(kipad));
	bytes_wipe(kopad, sizeof(kopad));
	bytes_wipe(tk, sizeof(tk));
	bytes_wipe(ih, sizeof(ih));
}

/* ---- ChaCha20 (RFC 8439, 256-bit key) ----------------------------------- */

static uint32_t
rr(uint32_t v, int n)
{
	return (v >> n) | (v << (32 - n));
}

/* RFC 8439 quarter-round (column/diagonal rounds use this). */
static void
chacha_qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	*a += *b;
	*d ^= *a;
	*d = rr(*d, 16);
	*c += *d;
	*b ^= *c;
	*b = rr(*b, 12);
	*a += *b;
	*d ^= *a;
	*d = rr(*d, 8);
	*c += *d;
	*b ^= *c;
	*b = rr(*b, 7);
}

static void
chacha_block(uint32_t out[16], const uint32_t in[16])
{
	uint32_t x[16];
	int i;

	for (i = 0; i < 16; i++) {
		x[i] = in[i];
	}
	for (i = 0; i < 10; i++) {
		chacha_qr(&x[0], &x[4], &x[8], &x[12]);
		chacha_qr(&x[1], &x[5], &x[9], &x[13]);
		chacha_qr(&x[2], &x[6], &x[10], &x[14]);
		chacha_qr(&x[3], &x[7], &x[11], &x[15]);
		chacha_qr(&x[0], &x[5], &x[10], &x[15]);
		chacha_qr(&x[1], &x[6], &x[11], &x[12]);
		chacha_qr(&x[2], &x[7], &x[8], &x[13]);
		chacha_qr(&x[3], &x[4], &x[9], &x[14]);
	}
	for (i = 0; i < 16; i++) {
		out[i] = x[i] + in[i];
	}
}

void
gj_ssh_chacha20_xor(const uint8_t key[32], const uint8_t nonce[12],
		    uint32_t counter, uint8_t *data, size_t len)
{
	uint32_t st[16];
	uint32_t blk[16];
	size_t off = 0;
	int i;
	static const char sigma[16] = "expand 32-byte k";

	for (i = 0; i < 4; i++) {
		st[i] = (uint32_t)(uint8_t)sigma[4 * i] |
			((uint32_t)(uint8_t)sigma[4 * i + 1] << 8) |
			((uint32_t)(uint8_t)sigma[4 * i + 2] << 16) |
			((uint32_t)(uint8_t)sigma[4 * i + 3] << 24);
	}
	for (i = 0; i < 8; i++) {
		st[4 + i] = (uint32_t)key[4 * i] | ((uint32_t)key[4 * i + 1] << 8) |
			    ((uint32_t)key[4 * i + 2] << 16) |
			    ((uint32_t)key[4 * i + 3] << 24);
	}
	st[12] = counter;
	st[13] = (uint32_t)nonce[0] | ((uint32_t)nonce[1] << 8) |
		 ((uint32_t)nonce[2] << 16) | ((uint32_t)nonce[3] << 24);
	st[14] = (uint32_t)nonce[4] | ((uint32_t)nonce[5] << 8) |
		 ((uint32_t)nonce[6] << 16) | ((uint32_t)nonce[7] << 24);
	st[15] = (uint32_t)nonce[8] | ((uint32_t)nonce[9] << 8) |
		 ((uint32_t)nonce[10] << 16) | ((uint32_t)nonce[11] << 24);
	while (off < len) {
		uint8_t *kb = (uint8_t *)blk;
		size_t n = len - off;
		size_t j;

		chacha_block(blk, st);
		if (n > 64) {
			n = 64;
		}
		for (j = 0; j < n; j++) {
			data[off + j] ^= kb[j];
		}
		off += n;
		st[12]++;
	}
}

/*
 * ---- Production host identity -------------------------------------------
 * Seeded from a fixed product label (stable across boots until entropy
 * injection lands). Public key is X25519(sk, basepoint); signatures of
 * exchange hash H are HMAC-SHA256(sk, H) for the live KEX smoke path.
 * Not OpenSSH host key blobs; product-owned dual MIT/Apache code.
 */
static uint8_t g_host_sk[32];
static uint8_t g_host_pk[32];
static int g_host_ready;

void
gj_ssh_hostkey_init(void)
{
	static const char seed_label[] = "GreenJade-sshd-hostkey-v1";
	uint8_t base[32];
	struct sha256_ctx hx;
	unsigned i;

	if (g_host_ready) {
		return;
	}
	for (i = 0; i < 32; i++) {
		base[i] = 0;
	}
	base[0] = 9; /* RFC 7748 base point u=9 */

	/* Permanent product identity: SHA-256(label || "prod") → clamped sk */
	gj_ssh_sha256_init(&hx);
	gj_ssh_sha256_update(&hx, seed_label, sizeof(seed_label) - 1);
	gj_ssh_sha256_update(&hx, "prod", 4);
	gj_ssh_sha256_final(&hx, g_host_sk);
	/* X25519 scalar clamp (RFC 7748) */
	g_host_sk[0] &= 248;
	g_host_sk[31] &= 127;
	g_host_sk[31] |= 64;
	gj_ssh_x25519(g_host_pk, g_host_sk, base);
	g_host_ready = 1;
}

void
gj_ssh_hostkey_pk(uint8_t pk[32])
{
	unsigned i;

	gj_ssh_hostkey_init();
	for (i = 0; i < 32; i++) {
		pk[i] = g_host_pk[i];
	}
}

void
gj_ssh_hostkey_sign(const uint8_t *msg, size_t msg_len, uint8_t sig[32])
{
	gj_ssh_hostkey_init();
	gj_ssh_hmac_sha256(g_host_sk, 32, msg, msg_len, sig);
}

/* Constant-time compare of MAC tags (no early exit on mismatch). */
int
gj_ssh_hostkey_verify(const uint8_t *msg, size_t msg_len, const uint8_t sig[32])
{
	uint8_t t[32];
	unsigned i;
	int d = 0;

	gj_ssh_hostkey_sign(msg, msg_len, t);
	for (i = 0; i < 32; i++) {
		d |= t[i] ^ sig[i];
	}
	bytes_wipe(t, sizeof(t));
	return d == 0;
}
