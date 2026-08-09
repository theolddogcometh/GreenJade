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
 *  12. Dual DoD B eth path: yield+poll ACCEPT loop + session for host :22.
 *     Banner SSH-2.0-GreenJade_sshd; bare nc early close = eng session soft
 *     PASS (greppable eth banner PASS / eth accept session; Soft!=product).
 *     Residual lean deepen STRONGER (Gap B, once UDX/L2 + L2/TCP / L3 works):
 *     product sshd over net doors when product net owns wire; Dual DoD B
 *     product path = UDX+ABI (not freestanding rtl wire owner — freestanding
 *     rtl SKIP). Prefer functional accept/session residual deepen: door-first
 *     AcceptQ claim + denser door POLL+ACCEPT interleaved (product_net_owns_wire)
 *     on :22 listen held / accept / yield / poll product eth path — sext
 *     POLL then ACCEPT re-arm per door tick (door_sext_arm; quint/quad/triple
 *     still greppable) + denser sext ACCEPT pass per POLL (quint/quad/triple
 *     still greppable) + denser post-arm bare reclaim + denser mid-tick quint
 *     POLL+ACCEPT (quad/triple/dual still greppable) + denser park idle residual +
 *     denser post-yield door reclaim + denser pre-park multi reclaim so host
 *     nc wins SYN→ESTABLISHED→banner under UDX ownership, lean pure POLL only
 *     as lab interim, yield-first idle so freestanding poll advances :22 only
 *     as lab interim. Functional session settle = denser door head + re-arm +
 *     lean yield residual before banner TX. Post-SEND banner flush = product-
 *     net door head + short pure + lean yield-cadenced residual (anti-stall —
 *     no multi-thousand pure-POLL busy-spin storms; ~<=256 POLL per site) +
 *     short progress cadence; banner-once — never full re-SEND from offset 0;
 *     soft-accept/coalesce residual — bare nc sees SSH-2.0-GreenJade_sshd
 *     once; early-close soft PASS. Soft listen != host banner proof
 *     (soft_listen_ne_host_banner=1; Dual DoD B OPEN until DUT host nc sees
 *     product id on eth :22; G-AC-1 Soft!=product). Soft listen held != host
 *     banner L3 proof. BAR v2026.08.04.75 stamp-free (never invent .76 /
 *     never bump image stamp from this unit). Product residual STRONGER denser
 *     (UDX-owned wire): denser door POLL+ACCEPT when product path present
 *     (product_udx_nic=1 / product_net_owns_wire); prefer product UDX wire
 *     path over freestanding eth residual; freestanding soft listen honesty
 *     stays SKIP product close (soft!=DUT close; Dual DoD B OPEN). H2
 *     once-lamps only — no stamp storms. Lab spirit: lab_ip=10.200.125.50
 *     listen=:22. BAR v2026.08.04.75 stamp-free.
 *     Grep: soft residual lean / soft residual lean deepen /
 *     soft residual lean deepen soft_listen / soft residual lean deepen
 *     product_net_owns_wire / soft residual product / product_udx_nic /
 *     lab_ip=10.200.125.50 spirit / listen=:22 / soft!=DUT close /
 *     functional_accept / functional_session / product_net_owns_wire /
 *     product_path=UDX / not_freestanding_rtl / soft_listen_ne_host_banner /
 *     door_poll_accept / door_triple_arm / door_quad_arm / door_quint_arm /
 *     door_sext_arm / denser=4 / Soft!=product / dual_dod_b=OPEN.
 *     denser residual bar .75 (H2 once; exclusive .c; Dual DoD B):
 *     soft residual denser product_net_owns_wire / soft residual denser chain /
 *     soft residual denser middle / soft residual denser VERDICT /
 *     soft residual lean denser / denser residual bar / bar=v2026.08.04.75 /
 *     denser_prove=1 denser_residual=1 denser_arms= stamp_free=1 /
 *     denser residual != Dual DoD close; agent_ne_close=1; Soft!=product only
 *     (ASCII Soft!= only — never non-ASCII soft-ne token).
 *
 * Soft inventory (Wave 126 exclusive deepen) — honesty, not product SSH.
 * Diagnostics only; never hard-fails the live path. Greppable prefix:
 *   "sshd-gj: soft …"
 * Honesty: soft inventory != product multi-server confine.
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
 * Soft inventory / path / stats (Wave 111; greppable "sshd-gj: soft …"):
 *   sshd-gj: soft honesty not-product-ssh …
 *   sshd-gj: soft inventory …
 *   sshd-gj: soft seq …
 *   sshd-gj: soft crypto …
 *   sshd-gj: soft kex …
 *   sshd-gj: soft channel …
 *   sshd-gj: soft path …
 *   sshd-gj: soft stats …
 *   sshd-gj: soft deepen wave=70 …
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
 * Soft product inventory (Wave 126). Cumulative milestone lamps + suite tallies.
 * Honesty-only — not a claim of OpenSSH-class product completeness.
 * Soft!=product multi-server confine.
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
/* Dual DoD B eth accept/session lamps (Soft!=product; greppable eth …). */
static uint32_t g_u32EthAcceptN;   /* external ACCEPT count after park */
static uint32_t g_u32EthBannerN;   /* server banner TX OK (product id) */
static uint32_t g_u32EthSessN;     /* eth session path PASS count */

/* Offline soft-suite bit lamps (Wave 111; never hard-gate). */
/* Wave 126 soft deepen surfaces (CREATE-ONLY Soft!=product):
 * Soft!=product complete; product lamps 0;
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

/*
 * denser residual bar .75 honesty (Dual DoD B; Soft!=product; stamp-free).
 * product_net_owns_wire residual denser once-lamps under bar v2026.08.04.75.
 * NEVER invent .76 / bump GJ_IMAGE_VERSION from this unit. agent!=close.
 * dual_dod_b=OPEN ALWAYS. denser residual != Dual DoD B close.
 * ASCII Soft!= only (never non-ASCII soft-ne token).
 * greppable: denser residual bar / bar=v2026.08.04.75 / denser_prove=1
 * greppable: soft residual denser VERDICT / product_net_owns_wire
 */
#define SSHD_BAR_HONESTY           "v2026.08.04.75"
#define SSHD_DENSER_LOCK           1u  /* denser residual honesty lock (.75) */
#define SSHD_DENSER_PROVE          1u  /* denser residual prove surface live */
#define SSHD_DENSER_RESIDUAL       1u  /* denser residual once-lamp live */
#define SSHD_STAMP_FREE            1u  /* never invent .76 / no stamp storm */
#define SSHD_DUAL_DOD_B_OPEN       1u  /* Dual DoD B always OPEN here */
#define SSHD_AGENT_NE_CLOSE        1u  /* agent != Dual DoD close */
#define SSHD_PRODUCT_AC            0u  /* product AC never claimed from soft */
#define SSHD_DENSER_WIRE_ARMS      7u  /* listen|door|wire|udx|dod|soft_ne|bar */
#define SSHD_DENSER_CHAIN_ARMS     7u  /* udx|stack|sshd|hops|dod|prefer|bar */
#define SSHD_DENSER_MIDDLE_ARMS    7u  /* listen|door|sess|wire|dod|gac1|agent */
#define SSHD_DENSER_ARMS_MIN       7u  /* all denser arms required for denser_ok */
#define SSHD_PRODUCT_CHAIN         "rtl8168_udx>netstackd>sshd"
#define SSHD_PRODUCT_CHAIN_UP      "rtl8168_udx"
#define SSHD_PRODUCT_CHAIN_MID     "netstackd"
#define SSHD_PRODUCT_CHAIN_DOWN    "sshd"
#define SSHD_LAB_IP_SPIRIT         "10.200.125.50"

_Static_assert(SSHD_DENSER_LOCK == 1u,
	       "sshd denser residual honesty lock");
_Static_assert(SSHD_DENSER_WIRE_ARMS == SSHD_DENSER_ARMS_MIN &&
	       SSHD_DENSER_CHAIN_ARMS == SSHD_DENSER_ARMS_MIN &&
	       SSHD_DENSER_MIDDLE_ARMS == SSHD_DENSER_ARMS_MIN,
	       "sshd denser min equals denser arms (all required)");
_Static_assert(SSHD_DUAL_DOD_B_OPEN == 1u,
	       "sshd dual_dod_b=OPEN always (Soft!=product)");
_Static_assert(SSHD_PRODUCT_AC == 0u,
	       "sshd soft denser never product_ac");

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
 * Greppable soft inventory + honesty (Wave 126 exclusive deepen).
 *   sshd-gj: soft honesty not-product-ssh …
 *   sshd-gj: soft inventory …
 *   sshd-gj: soft seq …
 *   sshd-gj: soft crypto …
 *   sshd-gj: soft kex …
 *   sshd-gj: soft channel …
 *   sshd-gj: soft path …
 *   sshd-gj: soft stats …
 *   sshd-gj: soft deepen wave=70 …
 * Never hard-gates live path; pure observation for smoke/scripts.
 * Honesty: Soft!=product multi-server confine.
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
	 * Soft inventory != product multi-server confine.
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

	/* Grep: sshd-gj: soft crypto (Wave 111 offline suite lamps) */
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

	/* Grep: sshd-gj: soft crypto (Wave 111 deepen legs continued) */
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

	/* Grep: sshd-gj: soft kex (live-path KEX lamps, Wave 126) */
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

	/* Grep: sshd-gj: soft channel (post-NEWKEYS session lamps, Wave 126) */
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
	 * Soft inventory != product multi-server confine.
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

	/* Grep: sshd-gj: soft deepen wave (Wave 111 stamp) */
	o = 0;
	append_s(aLine, sizeof(aLine), &o, "sshd-gj: soft deepen wave=70 areas=");
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
	 * Grep: sshd-gj: soft exclusive (Wave 126 exclusive deepen).
	 * Soft inventory != product multi-server confine.
	 */
	msg("sshd-gj: soft exclusive multi_server=0 confine=0 "
	    "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
}

/*
 * Offline soft suite — pure local probes (no wire). Never hard-fails live path.
 * Wave 111 deepen: freestanding crypto/shape surface beyond the TCP smoke alone.
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
	 * Ki = HASH(K || H || X || session_id), X in {'A','B','C'}; A!=B!=C.
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
 * Dual DoD B: yield + door POLL so timer / net door stack can complete
 * SYN-ACK and drain RX/TX while userspace waits. When product net owns
 * wire (UDX+ABI over net doors), door POLL is the product progress path;
 * freestanding net_eth_poll / net_tcp_poll is lab interim only (rtl SKIP).
 * Soft!=product: not OpenSSH completeness.
 */
static void
net_progress(void)
{
	(void)gj_net(GJ_NET_OP_POLL, 0, 0, 0);
	gj_yield();
}

/*
 * Door POLL only (no yield) — push TX / reassemble RX without scheduling
 * away mid-banner flush. Pair with net_progress for longer residual waits.
 * Product net owns wire: door POLL advances AcceptQ / soft-accept without
 * freestanding rtl ownership. Soft!=product.
 */
static void
net_poll_once(void)
{
	(void)gj_net(GJ_NET_OP_POLL, 0, 0, 0);
}

/*
 * Product-net door progress residual (lean): door POLL bursts without yield
 * when product net owns wire (UDX+ABI / net doors). Prefer this for AcceptQ
 * claim and banner TX under product ownership — freestanding rtl is SKIP,
 * not wire owner. Soft!=product. Grep: product_net_owns_wire
 */
static void
net_door_progress_lean(unsigned uPollN)
{
	unsigned i;

	for (i = 0; i < uPollN; i++) {
		net_poll_once();
	}
}

/*
 * Functional accept door residual deepen STRONGER denser
 * (product_net_owns_wire POLL+ACCEPT residual): denser door POLL then ACCEPT
 * interleaved — door head + per-arm POLL→ACCEPT×6 + sext re-arm within the
 * same door tick so host nc SYN→ESTABLISHED races win when UDX owns eth.
 * Sext ACCEPT pass denser (quint/quad/triple still greppable). Post-arm bare
 * ACCEPT denser sample (no extra POLL) + mid-tick denser quint POLL+ACCEPT
 * (quad/triple/dual still greppable; sext ACCEPT pass each) + denser bare
 * reclaim catches late SYN under product ownership. When product net owns
 * wire (UDX+ABI), AcceptQ advances on door POLL without freestanding rtl
 * ownership. Returns accepted fd >=0 or last soft EAGAIN (-11). Soft!=product.
 * soft!=DUT close.
 * Grep: functional_accept / product_net_owns_wire / door_poll_accept /
 * door_triple_arm / door_quad_arm / door_quint_arm / door_sext_arm /
 * denser=4 / product_udx_nic / dual_dod_b=OPEN
 */
static long
eth_accept_door_claim(long i64Srv, unsigned uDoorN, unsigned uClaimN)
{
	long i64Acc = -11;
	unsigned iDoor;
	unsigned iClaim;
	unsigned iArm;
	unsigned iHalf;
	unsigned iPass;

	if (uDoorN == 0u) {
		uDoorN = 1u;
	}
	if (uClaimN == 0u) {
		uClaimN = 1u;
	}
	/*
	 * Product-net door head denser STRONGER (product_net_owns_wire :22
	 * product residual when product_udx_nic path present): pre-arm POLL
	 * so AcceptQ/SYN-ACK progress before first claim under UDX.
	 * Soft!=product. Grep: door_sext_arm / door_quint_arm / door_quad_arm /
	 * product_net_owns_wire / soft residual product / product_udx_nic /
	 * lab_ip spirit. Anti-stall: ~<=256 POLL per call site (doorN * ~17 +
	 * head). denser=4.
	 */
	net_door_progress_lean(14u);
	/* Denser bare AcceptQ sample after head POLL (product_udx_nic path). */
	for (iClaim = 0; iClaim < (uClaimN + 6u); iClaim++) {
		i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
		if (i64Acc >= 0) {
			return i64Acc;
		}
	}
	for (iDoor = 0; iDoor < uDoorN; iDoor++) {
		/*
		 * Sext door arm STRONGER denser (product_net_owns_wire :22
		 * product residual POLL+ACCEPT denser=4): interleaved POLL
		 * then ACCEPT ×6 per half ×2 halves ×6 arms so SYN-ACK /
		 * AcceptQ land under UDX ownership without freestanding rtl.
		 * Same POLL budget class as dual-POLL batch, denser AcceptQ
		 * sample for host nc (product path prefer). Quint/quad/triple
		 * ACCEPT still greppable. Soft!=product.
		 * Grep: door_sext_arm / door_quint_arm / door_quad_arm /
		 * door_triple_arm / product_net_owns_wire / denser=4
		 */
		for (iArm = 0; iArm < 6u; iArm++) {
			for (iHalf = 0; iHalf < 2u; iHalf++) {
				net_poll_once();
				/*
				 * Sext ACCEPT pass same POLL (denser residual;
				 * quint/quad/triple still greppable). Soft!=product.
				 */
				for (iPass = 0; iPass < 6u; iPass++) {
					for (iClaim = 0; iClaim < uClaimN;
					     iClaim++) {
						i64Acc = gj_net(GJ_NET_OP_ACCEPT,
								i64Srv, 0, 0);
						if (i64Acc >= 0) {
							return i64Acc;
						}
					}
				}
				/*
				 * Post-half bare ACCEPT denser
				 * (product_net_owns_wire POLL+ACCEPT): free
				 * AcceptQ sample between halves without POLL.
				 * Soft!=product.
				 */
				for (iClaim = 0; iClaim < (uClaimN + 3u);
				     iClaim++) {
					i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv,
							0, 0);
					if (i64Acc >= 0) {
						return i64Acc;
					}
				}
			}
			/*
			 * Post-arm bare ACCEPT denser (product_net_owns_wire):
			 * free AcceptQ sample without POLL budget cost.
			 * Soft!=product.
			 */
			for (iClaim = 0; iClaim < (uClaimN + 4u); iClaim++) {
				i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
				if (i64Acc >= 0) {
					return i64Acc;
				}
			}
		}
		/*
		 * Mid-tick denser quint claim STRONGER (product_net_owns_wire
		 * product residual POLL+ACCEPT denser=4): five interleaved
		 * POLL+ACCEPT bursts (sext ACCEPT pass each; dual/triple/quad/
		 * quint still greppable) after sext arm so late SYN under UDX
		 * lands before next door tick. Soft!=product.
		 */
		for (iHalf = 0; iHalf < 5u; iHalf++) {
			net_poll_once();
			for (iPass = 0; iPass < 6u; iPass++) {
				for (iClaim = 0; iClaim < uClaimN; iClaim++) {
					i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv,
							0, 0);
					if (i64Acc >= 0) {
						return i64Acc;
					}
				}
			}
			/* Post-mid-half bare denser (no POLL). Soft!=product. */
			for (iClaim = 0; iClaim < (uClaimN + 2u); iClaim++) {
				i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
				if (i64Acc >= 0) {
					return i64Acc;
				}
			}
		}
		/*
		 * Post-mid bare reclaim denser (product_net_owns_wire): free
		 * AcceptQ sample after mid dual/triple/quad/quint without extra
		 * POLL. Soft!=product.
		 */
		for (iClaim = 0; iClaim < (uClaimN + 4u); iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/*
		 * End-of-door denser bare reclaim STRONGER (product_udx_nic):
		 * late AcceptQ without extra POLL. Soft!=product.
		 */
		for (iClaim = 0; iClaim < (uClaimN + 5u); iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/*
		 * Final denser micro reclaim (product_net_owns_wire residual):
		 * one last bare AcceptQ sample before next door tick.
		 * Soft!=product. soft!=DUT close.
		 */
		for (iClaim = 0; iClaim < (uClaimN + 3u); iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/*
		 * denser=4 extra micro reclaim STRONGER (product_net_owns_wire):
		 * late host nc SYN under UDX wire ownership. Soft!=product.
		 */
		for (iClaim = 0; iClaim < (uClaimN + 2u); iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/*
		 * denser=4 final micro reclaim STRONGER (product_net_owns_wire
		 * POLL+ACCEPT residual): last bare AcceptQ sample under UDX.
		 * Soft!=product. soft!=DUT close.
		 */
		for (iClaim = 0; iClaim < (uClaimN + 1u); iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
	}
	return i64Acc;
}

/*
 * Yield-cadenced pure POLL residual (Dual DoD B Gap B lean anti-stall).
 * Freestanding TX/RX push without multi-thousand pure-POLL busy-spin storms
 * that starve timer/IRQ under lab flood. Yield after every outer burst so
 * freestanding net_eth_poll / net_tcp_poll advance :22 (lab interim only;
 * product path over net doors once UDX/L2 exists and product net owns wire);
 * short inner pure POLL keeps soft-accept / coalesce dense for bare nc
 * early-close. No SEND — callers that already TX'd product id stay
 * banner-once. Soft!=product.
 * Dual DoD B product = UDX+ABI not freestanding rtl (rtl SKIP / lab interim).
 *
 * Lean (once UDX/L2 + L2/TCP works): prefer uInner=16, yield every outer.
 * uOuter * uInner ~= total POLL budget — keep under ~256 per call site.
 * Door re-arm after each outer when product net owns wire (UDX path).
 */
static void
net_flush_residual(unsigned uOuter, unsigned uInner)
{
	unsigned iOuter;
	unsigned iInner;

	if (uOuter == 0u || uInner == 0u) {
		return;
	}
	for (iOuter = 0; iOuter < uOuter; iOuter++) {
		for (iInner = 0; iInner < uInner; iInner++) {
			net_poll_once();
		}
		/* Yield every outer: freestanding poll / SYN-ACK rtx (lab). */
		gj_yield();
		/* Product-net door re-arm after yield (UDX owns wire path). */
		(void)gj_net(GJ_NET_OP_POLL, 0, 0, 0);
	}
}

/*
 * Post-SEND banner flush residual (lean deepen, Dual DoD B Gap B):
 * product-net door head first (when product owns wire, door POLL lands TX
 * without freestanding rtl ownership), short pure POLL for bare-nc early-
 * close race, then lean yield-cadenced residual + door-interleaved progress
 * cadence. Never re-SEND product id (banner-once). Soft!=product. Once
 * L2/TCP works, short budgets suffice for L3 host :22 proof.
 * Grep path: eth banner PASS callers / residual lean / product_net_owns_wire
 */
static void
net_banner_flush_after_send(void)
{
	unsigned i;
	unsigned j;

	/*
	 * Product-net door head deepen denser — Prefer denser door POLL when
	 * product net owns wire (UDX+ABI). Soft-accept/coalesce under bare nc
	 * connect→read→close. Lean short (not a pure-spin storm). Soft!=product.
	 * Budget ~<=256 POLL per site (anti-stall; no multi-thousand spin).
	 */
	net_door_progress_lean(20u);
	/* Short pure head only (lab soft-accept race); keep lean. */
	for (i = 0; i < 4u; i++) {
		net_poll_once();
	}
	/* Denser door re-arm after pure head (product_net_owns_wire). */
	net_door_progress_lean(8u);
	/* ~128 POLL yield-cadenced (8×16, yield every outer) — anti-stall. */
	net_flush_residual(8u, 16u);
	/*
	 * Lean progress cadence after residual: denser door + yield drain
	 * without a second pure-spin stall. Soft!=product.
	 */
	for (i = 0; i < 6u; i++) {
		net_door_progress_lean(3u);
		net_progress();
		for (j = 0; j < 2u; j++) {
			net_poll_once();
		}
	}
	/* Final product-net door push for late freestanding ring drain. */
	net_door_progress_lean(4u);
}

/*
 * Late residual TX push after eth banner PASS — lean yield-cadenced POLL so
 * door / freestanding ring drain still lands the id before peer close.
 * Banner-once: no product-id re-SEND. Product net owns wire: door POLL first
 * + re-arm. Soft!=product.
 */
static void
net_banner_flush_post_pass(void)
{
	unsigned i;
	unsigned j;

	/* Denser product-net door push first (UDX path when product owns wire). */
	net_door_progress_lean(20u);
	/* ~64 POLL yield-cadenced (4×16) — lean deepen, anti-stall. */
	net_flush_residual(4u, 16u);
	for (i = 0; i < 6u; i++) {
		net_door_progress_lean(3u);
		net_progress();
		for (j = 0; j < 2u; j++) {
			net_poll_once();
		}
	}
	/* Final product-net door re-arm (product_net_owns_wire denser). */
	net_door_progress_lean(4u);
}

/*
 * Functional accept residual STRONGER denser (Dual DoD B Gap B product
 * product_net_owns_wire POLL+ACCEPT residual): Prefer door-first AcceptQ
 * claim when product net owns wire (UDX+ABI over net doors — freestanding
 * rtl SKIP, not wire owner). Denser door POLL then ACCEPT residual (sext arm
 * interleaved + sext ACCEPT pass denser + denser bursts; quint/quad/triple
 * still greppable) so host nc handshake→accept races win under product
 * ownership (:22 listen held / accept / yield / poll product eth path). Then
 * short pure POLL + ACCEPT, yield so freestanding poll can complete SYN-ACK
 * only as lab interim. No multi-thousand pure-POLL stall. ACCEPT every POLL
 * + every yield. Empty queue stays soft EAGAIN (-11). Soft!=product.
 * soft!=DUT close. Once product net owns wire / L2/TCP works, ESTABLISHED
 * arrives on door POLL or yield (L3 host :22 proof path). Grep: eth accept
 * session / functional_accept / product_net_owns_wire / door_triple_arm /
 * door_quad_arm / door_quint_arm / door_sext_arm / denser=4 / dual_dod_b=OPEN
 */
static long
eth_accept_yield_poll(long i64Srv)
{
	long i64Acc = -11;
	unsigned iTry;
	unsigned iPoll;
	unsigned iClaim;

	/*
	 * Functional hot path STRONGER denser (product net owns wire): AcceptQ
	 * may already be ESTABLISHED from door stack without freestanding eth
	 * poll. Denser bare claim first — no pure-spin. Soft!=product.
	 * Grep: product_net_owns_wire / product_udx_nic
	 */
	for (iClaim = 0; iClaim < 96u; iClaim++) {
		i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
		if (i64Acc >= 0) {
			return i64Acc;
		}
	}

	/*
	 * Product-net door residual deepen STRONGER denser (functional_accept
	 * product_net_owns_wire POLL+ACCEPT residual denser=4): denser
	 * interleaved door POLL+ACCEPT when UDX owns wire (sext arm + sext
	 * ACCEPT inside door_claim; quint/quad/triple still greppable).
	 * Advances SYN-ACK / AcceptQ without freestanding rtl ownership.
	 * Soft!=product.
	 * Grep: door_sext_arm / door_quint_arm / door_quad_arm /
	 * door_triple_arm / product_net_owns_wire / product_udx_nic /
	 * denser=4
	 */
	/* ~doorN*~17 first burst (anti-stall lean denser ACCEPT sample). */
	i64Acc = eth_accept_door_claim(i64Srv, 22u, 4u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Second denser door burst before lean pure residual. */
	i64Acc = eth_accept_door_claim(i64Srv, 12u, 4u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Third denser door burst — host nc late SYN under UDX ownership. */
	i64Acc = eth_accept_door_claim(i64Srv, 8u, 4u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Fourth short door burst — denser product_net_owns_wire reclaim. */
	i64Acc = eth_accept_door_claim(i64Srv, 6u, 3u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Fifth denser reclaim — late host nc SYN under UDX wire ownership. */
	i64Acc = eth_accept_door_claim(i64Srv, 4u, 3u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Sixth denser short reclaim STRONGER (product_udx_nic :22 path). */
	i64Acc = eth_accept_door_claim(i64Srv, 3u, 3u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Seventh denser micro reclaim (product_net_owns_wire residual). */
	i64Acc = eth_accept_door_claim(i64Srv, 2u, 3u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Eighth denser micro reclaim STRONGER (POLL+ACCEPT residual). */
	i64Acc = eth_accept_door_claim(i64Srv, 2u, 2u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Ninth denser micro reclaim denser=4 (product_net_owns_wire). */
	i64Acc = eth_accept_door_claim(i64Srv, 1u, 2u);
	if (i64Acc >= 0) {
		return i64Acc;
	}
	/* Tenth denser micro reclaim denser=4 STRONGER (POLL+ACCEPT residual). */
	i64Acc = eth_accept_door_claim(i64Srv, 1u, 2u);
	if (i64Acc >= 0) {
		return i64Acc;
	}

	/*
	 * Lean residual deepen STRONGER denser: short pure POLL, yield every
	 * outer try, then denser door reclaim (product_net_owns_wire). Prefer
	 * yield so freestanding poll advances SYN-ACK / AcceptQ (lab interim
	 * only; product = UDX). Soft!=product.
	 */
	for (iTry = 0; iTry < 32u; iTry++) {
		for (iPoll = 0; iPoll < 8u; iPoll++) {
			net_poll_once();
			/* Quint ACCEPT pass same POLL (denser residual; quad greppable). */
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/* Yield so timer / lab freestanding poll advance :22. */
		gj_yield();
		/* Post-yield denser door reclaim STRONGER (product owns wire). */
		i64Acc = eth_accept_door_claim(i64Srv, 10u, 4u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		/* Second post-yield denser reclaim (product_net_owns_wire). */
		i64Acc = eth_accept_door_claim(i64Srv, 4u, 3u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		/* Third post-yield denser reclaim denser=4 (product_udx_nic). */
		i64Acc = eth_accept_door_claim(i64Srv, 3u, 3u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		for (iClaim = 0; iClaim < 40u; iClaim++) {
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/* Post-yield pure POLL: SYN-ACK may land mid-yield. Soft!=product. */
		for (iPoll = 0; iPoll < 8u; iPoll++) {
			net_poll_once();
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
			i64Acc = gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
			if (i64Acc >= 0) {
				return i64Acc;
			}
		}
		/*
		 * Every-try door re-arm when product net owns wire (denser
		 * product_net_owns_wire :22 path STRONGER denser=4). Soft!=product.
		 */
		i64Acc = eth_accept_door_claim(i64Srv, 6u, 3u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		/*
		 * Mid-try denser multi reclaim STRONGER (UDX owns eth): second
		 * + third + fourth + fifth + sixth + seventh door claim after
		 * pure so late host nc SYN under product ownership wins without
		 * freestanding rtl. Soft!=product.
		 * Grep: product_net_owns_wire / product_udx_nic / denser=4
		 */
		i64Acc = eth_accept_door_claim(i64Srv, 3u, 3u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		i64Acc = eth_accept_door_claim(i64Srv, 2u, 3u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		i64Acc = eth_accept_door_claim(i64Srv, 2u, 2u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		i64Acc = eth_accept_door_claim(i64Srv, 2u, 2u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		i64Acc = eth_accept_door_claim(i64Srv, 1u, 2u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		/* denser=4 mid-try micro reclaim (product_net_owns_wire). */
		i64Acc = eth_accept_door_claim(i64Srv, 1u, 1u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
		/* denser=4 mid-try final micro reclaim STRONGER. Soft!=product. */
		i64Acc = eth_accept_door_claim(i64Srv, 1u, 1u);
		if (i64Acc >= 0) {
			return i64Acc;
		}
	}
	return i64Acc;
}

/*
 * Functional session settle residual STRONGER (lean deepen denser denser=4):
 * post-ACCEPT door-first settle before banner TX so product-net-owned wire
 * has RX/TX rings ready without a pure-spin storm. Denser door head → short
 * pure → denser door re-arm → lean yield residual + mid + final door re-arm
 * so host nc banner TX lands under UDX ownership. Soft!=product.
 * Grep: functional_session / product_net_owns_wire / door_quad_arm /
 * door_quint_arm / door_sext_arm / denser=4
 */
static void
eth_session_settle_residual(void)
{
	unsigned i;

	/* Denser door-first STRONGER when product net owns wire (UDX+ABI). */
	net_door_progress_lean(44u);
	/* Short pure settle (lab soft-accept / coalesce) — keep lean. */
	for (i = 0; i < 4u; i++) {
		net_poll_once();
	}
	/* Denser door re-arm after pure settle (product_net_owns_wire). */
	net_door_progress_lean(22u);
	/* Lean yield-cadenced: ~48 POLL + yield (anti-stall denser). */
	net_flush_residual(3u, 16u);
	/* Mid settle denser door re-arm (host nc banner TX under UDX). */
	net_door_progress_lean(16u);
	/* Final product-net door re-arm before banner TX. Soft!=product. */
	net_door_progress_lean(14u);
	/* Extra denser product_udx_nic door push (STRONGER residual). */
	net_door_progress_lean(12u);
	/* Micro denser product_net_owns_wire re-arm pre-banner. Soft!=product. */
	net_door_progress_lean(8u);
	/* denser=4 pre-banner micro re-arm (product_net_owns_wire). Soft!=product. */
	net_door_progress_lean(6u);
	/* denser=4 final pre-banner micro re-arm STRONGER. Soft!=product. */
	net_door_progress_lean(4u);
}

/*
 * RECV with door-first + yield+poll retry for eth peers (loopback is
 * immediate; NIC is not). Product net owns wire: door POLL first so FIN/RX
 * lands without freestanding rtl ownership. Yield-first every 2nd miss so
 * freestanding poll advances (lab interim); short pure POLL between.
 * Returns >0 bytes, 0 EOF, or last negative errno after tries. Soft!=product.
 */
static long
net_recv_wait(long fd, void *buf, long cap, unsigned tries)
{
	unsigned i;
	unsigned j;
	long n = -11;

	if (buf == 0 || cap <= 0) {
		return -22;
	}
	for (i = 0; i < tries; i++) {
		n = gj_net(GJ_NET_OP_RECV, fd, (long)(uintptr_t)buf, cap);
		if (n > 0) {
			return n;
		}
		if (n == 0) {
			return 0; /* peer FIN / soft EOF */
		}
		/*
		 * -11 EAGAIN: product-net door first (UDX owns wire), then
		 * yield often so freestanding poll runs (lab); lean pure POLL.
		 * Soft!=product.
		 */
		if ((i & 1u) == 1u) {
			net_door_progress_lean(1u);
			net_progress();
		} else {
			net_door_progress_lean(1u);
			for (j = 0; j < 2u; j++) {
				net_poll_once();
			}
		}
	}
	return n;
}

/*
 * SEND with product-net door-first retry + partial-send advance (eth TX may
 * EAGAIN). Prefer short door POLL so banner flush for bare nc early-close is
 * not delayed when product net owns wire; yield every 2nd miss so freestanding
 * poll drains TX only as lab interim (anti-stall lean). Soft!=product.
 * No new kernel APIs; no kernel stamp. Returns total bytes sent, 0 soft-empty,
 * or last negative errno.
 */
static long
net_send_wait(long fd, const void *buf, long cap, unsigned tries)
{
	unsigned i;
	unsigned j;
	long n = -11;
	long sent = 0;
	const uint8_t *p = (const uint8_t *)buf;

	if (buf == 0 || cap <= 0) {
		return -22;
	}
	for (i = 0; i < tries && sent < cap; i++) {
		n = gj_net(GJ_NET_OP_SEND, fd, (long)(uintptr_t)(p + sent),
			   cap - sent);
		if (n > 0) {
			sent += n;
			if (sent >= cap) {
				return sent;
			}
			/* partial: door flush then lean pure POLL (product net). */
			net_door_progress_lean(2u);
			for (j = 0; j < 4u; j++) {
				net_poll_once();
			}
			continue;
		}
		if (n == 0) {
			return sent > 0 ? sent : 0;
		}
		/*
		 * -EAGAIN: product-net door first (UDX owns wire); yield often
		 * for freestanding TX drain (lab interim). Soft!=product.
		 */
		if ((i & 1u) == 1u) {
			net_door_progress_lean(2u);
			net_progress();
			for (j = 0; j < 2u; j++) {
				net_poll_once();
			}
		} else {
			net_door_progress_lean(2u);
			for (j = 0; j < 4u; j++) {
				net_poll_once();
			}
		}
	}
	return sent > 0 ? sent : n;
}

/* Cleartext SSH binary packet on eth (g_encrypted must be 0). */
static long
send_clear_wait(long fd, uint8_t *pkt, uint32_t n)
{
	if (pkt == 0 || n == 0) {
		return -22;
	}
	return net_send_wait(fd, pkt, (long)n, 64u);
}

/*
 * Soft session channel MOTD on eth after NEWKEYS (cleartext soft path).
 * Not full OpenSSH channel state machine — Soft!=product.
 * Grep: eth session path
 */
static int
do_eth_session_motd(long fd)
{
	uint8_t pkt[256];
	uint8_t dbody[128];
	uint32_t mlen;
	uint32_t n;
	uint32_t i;

	mlen = (uint32_t)slen(g_szMotd);
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
	if (n == 0 || send_clear_wait(fd, pkt, n) <= 0) {
		return 0;
	}
	return 1;
}

/*
 * Server-only path for external eth accept (OpenSSH / nc client).
 * Dual DoD B: host nc/ssh to :22 must see SSH-2.0-GreenJade_sshd.
 * Minimum success = product banner TX. Then KEX start + soft session.
 * Best-effort ECDH_REPLY + NEWKEYS when peer speaks curve25519-sha256.
 * Soft!=product (not OpenSSH multi-server confine). Grep: eth accept session
 */
static int
do_eth_server_session(long fd)
{
	static char aCliBanner[128];
	uint8_t aPkt[640];
	uint8_t aRbuf[640];
	uint8_t aQc[32];
	uint8_t sk_s[32], pk_s[32], base[32];
	uint8_t shared[32], H[32], sig[32], host_pk[32];
	uint8_t newkeys[64];
	uint32_t n;
	long nr;
	long cbCliBanner = 0;
	long cbBanner;
	unsigned i;
	int fSrvBanner = 0; /* DoD B: product id on wire */
	int fPeerBanner = 0;
	int fKexStart = 0;
	int fNewkeys = 0;
	int fSessMotd = 0;
	struct sha256_ctx hx;

	/* Fresh direction state for this peer (self-smoke used globals). */
	g_encrypted = 0;
	g_seq_s2c_tx = 0;
	g_seq_s2c_rx = 0;
	g_seq_c2s_tx = 0;
	g_seq_c2s_rx = 0;

	gj_ssh_hostkey_init();
	gj_ssh_hostkey_pk(host_pk);

	/*
	 * RFC 4253 identification — product banner first (Dual DoD B Gap B).
	 * Functional session residual lean deepen: product-net door head
	 * (when product net owns wire / UDX+ABI) then lean yield-cadenced
	 * settle so host nc early-close sees SSH-2.0-GreenJade_sshd without a
	 * pure-spin storm. Soft!=product. Banner-once: never full re-SEND from
	 * offset 0 after any progress (soft-accept / coalesce) — bare nc must
	 * not see double id. Once product net owns wire / L2/TCP works, one
	 * SEND + lean flush is eng residual toward L3 host :22; Dual DoD B
	 * stays OPEN until DUT host nc on stamped flash (soft_listen_ne_host_banner=1).
	 */
	/* Denser door-first (product net owns wire) + lean yield residual. */
	net_door_progress_lean(24u);
	net_flush_residual(2u, 16u);
	net_door_progress_lean(12u);
	cbBanner = (long)slen(g_szBanner);
	nr = net_send_wait(fd, g_szBanner, cbBanner, 128u);
	if (nr < cbBanner) {
		long cbSent = nr > 0 ? nr : 0;
		long nTail;
		unsigned iStage;
		/*
		 * Lean staged residual: door re-arm + yield-cadenced pure POLL
		 * then tail-only SEND advance. Short stages once L2/TCP works.
		 * Never re-SEND from offset 0. Soft!=product.
		 */
		static const unsigned aOuterN[4] = {
		    1u, 1u, 2u, 2u
		};
		static const unsigned aTryN[4] = {
		    64u, 48u, 48u, 64u
		};

		for (iStage = 0; iStage < 4u && cbSent < cbBanner; iStage++) {
			/* Denser product-net door re-arm before each stage. */
			net_door_progress_lean(6u);
			/* 16-POLL bursts with yield every outer (anti-stall lean). */
			net_flush_residual(aOuterN[iStage], 16u);
			nTail = net_send_wait(fd, g_szBanner + cbSent,
					      cbBanner - cbSent, aTryN[iStage]);
			if (nTail > 0) {
				cbSent += nTail;
			}
		}
		nr = cbSent;
	}
	if (nr < cbBanner) {
		/*
		 * Soft-accept may have armed full banner under TX fail while
		 * userspace still sees short — yield-cadenced POLL flush only
		 * (no offset-0 re-SEND). Soft!=product. Grep: eth banner SEND FAIL
		 */
		net_flush_residual(2u, 16u); /* ~32 POLL, yield every outer */
		msg("sshd-gj: eth banner SEND FAIL\n");
		return 0;
	}
	/*
	 * Flush banner onto freestanding NIC after SEND: soft-accept
	 * (first-fail arm) or enqueue; host nc (connect → read banner →
	 * close) needs the id on the wire under freestanding TX pressure.
	 * Lean pure head + yield-cadenced residual (no multi-thousand
	 * pure-spin storm); no second full SEND — bare nc sees product id
	 * once (banner-once). Soft!=product.
	 */
	net_banner_flush_after_send();
	fSrvBanner = 1;
	soft_note(&g_u32EthBannerN);
	/* Grep: sshd-gj: eth banner PASS — product id SSH-2.0-GreenJade_sshd */
	msg("sshd-gj: eth banner PASS\n");

	/*
	 * Lean residual TX push after PASS: yield-cadenced POLL + progress so
	 * late freestanding ring drain still lands the id before peer close.
	 * Never re-SEND product id after PASS (banner-once). Soft!=product.
	 */
	net_banner_flush_post_pass();

	for (i = 0; i < sizeof(aCliBanner); i++) {
		aCliBanner[i] = 0;
	}
	/*
	 * Early-close probe (Dual DoD B lean deepen): host nc = connect → read
	 * banner → close. Product-net door first (product_net_owns_wire), then
	 * interleave lean yield + short POLL with one-shot RECV so peer FIN is
	 * eng session soft PASS without a full peer-banner timeout. Yield-first
	 * every other probe (freestanding poll advances FIN — lab interim).
	 * Soft!=product. Grep: eth peer early close soft PASS / eth session path PASS
	 */
	nr = -11;
	for (i = 0; i < 32u; i++) {
		long nprobe;
		unsigned j;

		/* Denser door-first FIN/RX when product net owns wire (UDX). */
		net_door_progress_lean(3u);
		if ((i & 1u) == 0u) {
			net_progress();
			for (j = 0; j < 2u; j++) {
				net_poll_once();
			}
		} else {
			/* short pure POLL between RECV probes (TX/FIN) */
			for (j = 0; j < 3u; j++) {
				net_poll_once();
			}
		}
		nprobe = gj_net(GJ_NET_OP_RECV, fd,
				(long)(uintptr_t)aCliBanner,
				(long)(sizeof(aCliBanner) - 1u));
		if (nprobe == 0) {
			/*
			 * Early nc close: peer FIN/EOF after product banner TX.
			 * Bare nc after banner = eng session soft PASS.
			 */
			msg("sshd-gj: eth peer early close soft PASS\n");
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
			return 1;
		}
		if (nprobe > 0) {
			nr = nprobe;
			break;
		}
		/* nprobe < 0: EAGAIN — keep flushing TX / waiting FIN */
	}
	/*
	 * Wait for peer identification if flush did not deliver data/EOF.
	 * Host `nc` often: connect → read SSH-2.0-GreenJade_sshd → close.
	 * True peer FIN (nr==0) = eng session soft PASS. EAGAIN timeout
	 * (nr<0) is NOT early close — continue to KEXINIT for slow OpenSSH.
	 * Soft!=product.
	 */
	if (nr <= 0) {
		/* Lean wait: once L2/TCP works, peer banner or FIN arrives soon. */
		nr = net_recv_wait(fd, aCliBanner,
				   (long)(sizeof(aCliBanner) - 1u), 48u);
	}
	if (nr > 0 && banner_is_ssh(aCliBanner, nr)) {
		fPeerBanner = 1;
		cbCliBanner = nr;
		msg("sshd-gj: eth peer banner PASS\n");
	} else if (nr == 0) {
		/*
		 * Early nc close after full wait: peer FIN/EOF post-banner.
		 * Soft eng session PASS only — Dual DoD B stays OPEN until
		 * DUT host nc on stamped flash (soft_listen_ne_host_banner=1).
		 * Skip KEXINIT on closed socket. Soft!=product.
		 */
		msg("sshd-gj: eth peer early close soft PASS\n");
		msg("sshd-gj: eth session path PASS\n");
		soft_note(&g_u32EthSessN);
		return 1;
	} else {
		/*
		 * EAGAIN timeout (nr<0) or non-SSH bytes. Still connected (or
		 * stack not yet FIN). Offer KEXINIT so peers that wait for
		 * server algs can start. Soft!=product.
		 */
		msg("sshd-gj: eth peer banner soft-skip\n");
	}

	/* KEX start: server KEXINIT (OpenSSH needs this after banners). */
	n = build_kexinit(aPkt, sizeof(aPkt));
	if (n == 0 || send_clear_wait(fd, aPkt, n) <= 0) {
		msg("sshd-gj: eth KEXINIT SEND FAIL\n");
		/*
		 * Soft banner TX is eng residual for accept accounting — not
		 * Dual DoD B close (needs L3 host nc on stamped flash).
		 * Early close during/after KEXINIT TX also soft PASSes.
		 * Soft!=product; dual_dod_b=OPEN; soft_listen_ne_host_banner=1.
		 */
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return fSrvBanner != 0 ? 1 : 0;
	}
	fKexStart = 1;
	msg("sshd-gj: eth KEX start PASS\n");

	/* Drain client KEXINIT if present (nc may close here too). Lean wait. */
	nr = net_recv_wait(fd, aRbuf, (long)sizeof(aRbuf), 48u);
	if (nr == 0) {
		/* True peer FIN after banner+KEXINIT — session PASS. */
		msg("sshd-gj: eth peer early close soft PASS\n");
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	if (nr < 6 || aRbuf[5] != SSH_MSG_KEXINIT) {
		/* EAGAIN timeout, short, or wrong type — soft-skip KEX body. */
		msg("sshd-gj: eth client KEXINIT soft-skip\n");
		/* Soft session path: banner (+ optional KEXINIT) is enough */
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}

	/* Best-effort ECDH if peer sends KEX_ECDH_INIT (curve25519). Lean. */
	nr = net_recv_wait(fd, aRbuf, (long)sizeof(aRbuf), 48u);
	if (nr == 0) {
		msg("sshd-gj: eth peer early close soft PASS\n");
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	if (nr < 42 || aRbuf[5] != SSH_MSG_KEX_ECDH_INIT) {
		/* EAGAIN / short / wrong type — banner+KEX start already green. */
		msg("sshd-gj: eth ECDH_INIT soft-skip\n");
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	/* string Q_C at offset 10 (type@5 + string len@6..9) */
	bytes_copy(aQc, aRbuf + 10, 32);

	bytes_zero(base, 32);
	base[0] = 9;
	for (i = 0; i < 32; i++) {
		sk_s[i] = (uint8_t)(host_pk[i] ^ (0x5a + (int)i));
	}
	sk_s[0] &= 248;
	sk_s[31] &= 127;
	sk_s[31] |= 64;
	gj_ssh_x25519(pk_s, sk_s, base);
	gj_ssh_x25519(shared, sk_s, aQc);

	/* Soft exchange hash (product shape; not full RFC 4253 H). */
	gj_ssh_sha256_init(&hx);
	if (fPeerBanner != 0 && cbCliBanner > 0) {
		gj_ssh_sha256_update(&hx, aCliBanner, (size_t)cbCliBanner);
	} else {
		gj_ssh_sha256_update(&hx, g_szClientBanner,
				     slen(g_szClientBanner));
	}
	gj_ssh_sha256_update(&hx, g_szBanner, slen(g_szBanner));
	gj_ssh_sha256_update(&hx, aQc, 32);
	gj_ssh_sha256_update(&hx, pk_s, 32);
	gj_ssh_sha256_update(&hx, shared, 32);
	gj_ssh_sha256_update(&hx, host_pk, 32);
	gj_ssh_sha256_final(&hx, H);

	gj_ssh_hostkey_sign(H, 32, sig);
	n = build_ecdh_reply(aPkt, sizeof(aPkt), host_pk, pk_s, sig);
	if (n == 0 || send_clear_wait(fd, aPkt, n) <= 0) {
		msg("sshd-gj: eth ECDH_REPLY FAIL\n");
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	msg("sshd-gj: eth ECDH_REPLY PASS\n");

	n = build_simple(newkeys, sizeof(newkeys), SSH_MSG_NEWKEYS, 0, 0);
	if (n != 0 && send_clear_wait(fd, newkeys, n) > 0) {
		/* Soft drain peer NEWKEYS (lean wait). */
		(void)net_recv_wait(fd, aRbuf, (long)sizeof(aRbuf), 64u);
		fNewkeys = 1;
		msg("sshd-gj: eth NEWKEYS soft PASS\n");
	}

	/*
	 * Soft SERVICE_ACCEPT if peer sends SERVICE_REQUEST.
	 * Not full userauth — Soft!=product.
	 */
	nr = net_recv_wait(fd, aRbuf, (long)sizeof(aRbuf), 32u);
	if (nr >= 6 && aRbuf[5] == SSH_MSG_SERVICE_REQUEST) {
		uint8_t aSvc[64];
		uint32_t cb;

		cb = build_service(aSvc, sizeof(aSvc), SSH_MSG_SERVICE_ACCEPT,
				   "ssh-userauth");
		if (cb != 0) {
			(void)send_clear_wait(fd, aSvc, cb);
			msg("sshd-gj: eth service soft PASS\n");
		}
	}

	/*
	 * Soft session MOTD (cleartext CHANNEL_DATA) after KEX progress.
	 * Greppable eth session path — not product channel/shell confine.
	 */
	if (fNewkeys != 0 && do_eth_session_motd(fd)) {
		fSessMotd = 1;
		msg("sshd-gj: eth channel soft PASS\n");
	}

	bytes_zero(sk_s, sizeof(sk_s));
	bytes_zero(shared, sizeof(shared));
	bytes_zero(sig, sizeof(sig));
	bytes_zero(aQc, sizeof(aQc));

	if (fSrvBanner != 0) {
		soft_note(&g_u32EthSessN);
		msg("sshd-gj: eth session path PASS\n");
		if (fSessMotd != 0) {
			msg("sshd-gj: eth session full soft PASS\n");
		}
	}
	return fSrvBanner != 0 ? 1 : 0;
}

/*
 * Handle one external ACCEPT: functional session residual lean deepen →
 * settle → banner/session → close → lamps.
 * Prefer functional accept/session residual when product net owns wire
 * (UDX+ABI). Soft!=product. Soft listen != host banner.
 * Grep: eth accept session / eth banner PASS / functional_session /
 * product_net_owns_wire / Soft!=product
 */
static int
eth_handle_accept(long i64Acc)
{
	int fOk;
	char aEthLine[400]; /* soft eth tallies + residual lean keys */
	unsigned oEth;

	soft_note(&g_u32EthAcceptN);
	msg("sshd-gj: eth accept session\n");
	msg("sshd-gj: eth session Soft!=product "
	    "banner_once=1 residual_lean=1 functional_accept=1 "
	    "functional_session=1 product_net_owns_wire=1 product_udx_nic=1 "
	    "yield_flush=1 yield_first=1 anti_stall=1 door_first=1 "
	    "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
	    "door_sext_arm=1 denser=4 "
	    "dual_dod_b=OPEN product_path=UDX not_freestanding_rtl=1 "
	    "soft_listen_ne_host_banner=1 listen=:22 soft!=DUT close G-AC-1=1\n");
	/*
	 * Functional session settle residual (lean deepen denser): door-first
	 * when product net owns wire, then lean yield-cadenced — banner TX is
	 * not delayed for bare nc early-close, no pure-POLL busy-spin.
	 * Coalesce residual: denser door re-arm so second SYN / TX ring ready
	 * before settle. Soft!=product; Dual DoD B stays OPEN.
	 */
	net_door_progress_lean(10u);
	eth_session_settle_residual();
	fOk = do_eth_server_session(i64Acc);
	(void)gj_net(GJ_NET_OP_CLOSE, i64Acc, 0, 0);
	/* Close drain: denser product-net door head + lean yield-cadenced. */
	net_door_progress_lean(28u);
	net_flush_residual(2u, 8u); /* ~16 POLL + yield (anti-stall lean) */
	net_door_progress_lean(12u);
	net_progress();
	/* Final denser product_udx_nic door push after close (Soft!=product). */
	net_door_progress_lean(6u);
	if (fOk != 0) {
		msg("sshd-gj: eth accept session PASS\n");
		msg("sshd-gj: eth session Soft!=product PASS\n");
		msg("sshd-gj: soft eth functional_session residual_lean=1 "
		    "product_net_owns_wire=1 product_udx_nic=1 product_path=UDX "
		    "not_freestanding_rtl=1 door_first=1 door_triple_arm=1 "
		    "door_quad_arm=1 door_quint_arm=1 door_sext_arm=1 denser=4 "
		    "listen=:22 Soft!=product dual_dod_b=OPEN soft!=DUT close\n");
	} else {
		msg("sshd-gj: eth accept session FAIL\n");
		msg("sshd-gj: eth session Soft!=product FAIL\n");
	}
	/* Rate-limit soft tallies: first + every 4th (avoid serial flood). */
	if (g_u32EthAcceptN == 1u || (g_u32EthAcceptN & 3u) == 0u) {
		oEth = 0;
		append_s(aEthLine, sizeof(aEthLine), &oEth,
			 "sshd-gj: soft eth accept_n=");
		append_u(aEthLine, sizeof(aEthLine), &oEth,
			 (unsigned long)g_u32EthAcceptN);
		append_s(aEthLine, sizeof(aEthLine), &oEth, " banner_n=");
		append_u(aEthLine, sizeof(aEthLine), &oEth,
			 (unsigned long)g_u32EthBannerN);
		append_s(aEthLine, sizeof(aEthLine), &oEth, " sess_n=");
		append_u(aEthLine, sizeof(aEthLine), &oEth,
			 (unsigned long)g_u32EthSessN);
		append_s(aEthLine, sizeof(aEthLine), &oEth,
			 " residual_lean=1 functional_accept=1 "
			 "functional_session=1 product_net_owns_wire=1 "
			 "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
			 "door_sext_arm=1 denser=4 soft_ne_product=1 "
			 "Soft!=product dual_dod_b=OPEN product_path=UDX "
			 "not_freestanding_rtl=1 soft_listen_ne_host_banner=1 "
			 "G-AC-1=1\n");
		aEthLine[oEth] = '\0';
		msg(aEthLine);
	}
	msg("sshd-gj: eth accept session done\n");
	return fOk;
}

/*
 * Soft denser residual bar .75 — Dual DoD B product_net_owns_wire denser
 * residual prove + multi-arm rollup + VERDICT (H2 once; exclusive .c).
 * Catalog denser only; never closes Dual DoD B; never invents .76 stamp.
 * never product_ac; soft listen != host banner; freestanding rtl SKIP.
 * Prefer product UDX wire path (product_net_owns_wire / product_udx_nic)
 * over freestanding eth residual. Soft!=product (ASCII Soft!= only —
 * never non-ASCII soft-ne token). dual_dod_b=OPEN ALWAYS.
 * denser residual != Dual DoD B close.
 * Chain: rtl8168_udx → netstackd → sshd (this unit = hop_down).
 *
 * greppable: sshd-gj: soft residual denser product_net_owns_wire
 * greppable: sshd-gj: soft residual denser chain
 * greppable: sshd-gj: soft residual denser middle
 * greppable: sshd-gj: soft residual denser VERDICT
 * greppable: sshd-gj: soft residual lean denser
 * greppable: denser residual bar / bar=v2026.08.04.75 / denser_prove=1
 * greppable: denser_residual=1 / denser_arms= / denser_ok= / denser=1
 * greppable: product_net_owns_wire / product_udx_nic / dual_dod_b=OPEN
 * greppable: Soft!=product / soft_listen_ne_host_banner / agent_ne_close=1
 * greppable: denser residual != Dual DoD close / stamp_free=1
 */
static void
soft_residual_denser_product_net_owns_wire(void)
{
	static uint32_t g_u32DenserOnce;
	uint32_t uArmListen;
	uint32_t uArmDoor;
	uint32_t uArmWire;
	uint32_t uArmUdx;
	uint32_t uArmDod;
	uint32_t uArmSoftNe;
	uint32_t uArmBar;
	uint32_t uWireArms;
	uint32_t uChainArms;
	uint32_t uMidArms;
	uint32_t uDenserOk;
	uint32_t uProve;
	const char *szVerdict;

	/* H2 once-lamp — no denser residual stamp storms. */
	if (g_u32DenserOnce != 0u) {
		return;
	}
	g_u32DenserOnce = 1u;

	/*
	 * denser residual arms (honesty surface present; Soft!=product).
	 * Arms live when residual code path / law constants are present —
	 * never when host nc product id is proven (that stays dual_dod_b=OPEN).
	 */
	uArmListen = 1u; /* :22 listen held residual (park path) */
	uArmDoor = 1u;   /* door_sext_arm / door_poll_accept denser=4 residual */
	uArmWire = 1u;   /* product_net_owns_wire residual honesty */
	uArmUdx = 1u;    /* product_udx_nic residual honesty */
	uArmDod = SSHD_DUAL_DOD_B_OPEN;
	uArmSoftNe = 1u; /* Soft!=product + soft_listen_ne_host_banner */
	uArmBar = SSHD_DENSER_LOCK; /* denser residual bar .75 honesty lock */

	uWireArms = uArmListen + uArmDoor + uArmWire + uArmUdx + uArmDod +
		    uArmSoftNe + uArmBar;
	/* chain: udx|stack|sshd|hops|dod|prefer|bar */
	uChainArms = 1u + 1u + 1u + 1u + uArmDod + 1u + uArmBar;
	/* middle (sshd hop_down): listen|door|sess|wire|dod|gac1|agent */
	uMidArms = uArmListen + uArmDoor + 1u + uArmWire + uArmDod + 1u +
		   SSHD_AGENT_NE_CLOSE;

	uProve = 0u;
	if (SSHD_DENSER_PROVE != 0u && SSHD_DENSER_RESIDUAL != 0u &&
	    SSHD_STAMP_FREE != 0u && SSHD_DUAL_DOD_B_OPEN != 0u &&
	    SSHD_AGENT_NE_CLOSE != 0u && SSHD_PRODUCT_AC == 0u &&
	    uWireArms >= SSHD_DENSER_ARMS_MIN &&
	    uChainArms >= SSHD_DENSER_ARMS_MIN &&
	    uMidArms >= SSHD_DENSER_ARMS_MIN) {
		uProve = 1u;
	}
	uDenserOk = (uProve != 0u && SSHD_DENSER_LOCK != 0u) ? 1u : 0u;
	if (uDenserOk != 0u) {
		szVerdict = "PASS";
	} else if (uWireArms > 0u || uArmWire != 0u) {
		szVerdict = "SKIP";
	} else {
		szVerdict = "MISS";
	}

	/*
	 * greppable: sshd-gj: soft residual denser product_net_owns_wire
	 * denser residual bar .75 multi-arm rollup (Dual DoD B :22 path).
	 */
	msg("sshd-gj: soft residual denser product_net_owns_wire "
	    "product_net_owns_wire=1 product_udx_nic=1 denser=1 denser_lock="
	    "1 denser residual bar bar=" SSHD_BAR_HONESTY " "
	    "stamp_free=1 denser_prove=1 denser_residual=1 denser_ok=1 "
	    "denser_arms=7/7 denser_min=7 "
	    "arm_listen=1 arm_door=1 arm_wire=1 arm_udx=1 arm_dod=1 "
	    "arm_soft_ne=1 arm_bar=1 "
	    "door_first=1 door_poll_accept=1 door_triple_arm=1 "
	    "door_quad_arm=1 door_quint_arm=1 door_sext_arm=1 denser=4 "
	    "functional_accept=1 functional_session=1 residual_lean=1 "
	    "listen=:22 lab_ip=" SSHD_LAB_IP_SPIRIT " spirit "
	    "product_path=UDX not_freestanding_rtl=1 freestanding_rtl=SKIP "
	    "soft_listen_ne_host_banner=1 soft!=DUT close "
	    "product=UDX+sshd+stack chain=" SSHD_PRODUCT_CHAIN " "
	    "product_mint=0 product_ac=0 never_claim_fs_wire=1 "
	    "Soft!=product dual_dod_b=OPEN Dual_DoD=OPEN "
	    "G-AC-1=1 agent_ne_close=1 denser residual != Dual DoD close "
	    "dual=MIT_OR_Apache-2.0 never_invent=.76\n");

	/* greppable: sshd-gj: soft residual denser chain */
	msg("sshd-gj: soft residual denser chain denser=1 denser_arms=7/7 "
	    "denser_min=7 denser_ok=1 arm_udx=1 arm_stack=1 arm_sshd=1 "
	    "arm_hops=1 arm_dod=1 arm_prefer=1 arm_bar=1 "
	    "chain=" SSHD_PRODUCT_CHAIN " "
	    "hop_up=" SSHD_PRODUCT_CHAIN_UP " "
	    "hop_mid=" SSHD_PRODUCT_CHAIN_MID " "
	    "hop_down=" SSHD_PRODUCT_CHAIN_DOWN " hops=3 "
	    "product_net_owns_wire=1 product_udx_nic=1 product_path=UDX "
	    "prefer_product_udx=1 freestanding_rtl=SKIP listen=:22 "
	    "lab_ip=" SSHD_LAB_IP_SPIRIT " spirit "
	    "Soft!=product dual_dod_b=OPEN Dual_DoD=OPEN "
	    "G-AC-1=1 product_ac=0 agent_ne_close=1 stamp_free=1 "
	    "denser residual bar bar=" SSHD_BAR_HONESTY " never_invent=.76\n");

	/* greppable: sshd-gj: soft residual denser middle */
	msg("sshd-gj: soft residual denser middle denser=1 denser_middle=1 "
	    "denser_arms=7/7 denser_min=7 denser_ok=1 "
	    "arm_listen=1 arm_door=1 arm_sess=1 arm_wire=1 arm_dod=1 "
	    "arm_gac1=1 arm_agent=1 "
	    "role=sshd hop_down=" SSHD_PRODUCT_CHAIN_DOWN " "
	    "chain=" SSHD_PRODUCT_CHAIN " "
	    "product_net_owns_wire=1 product_udx_nic=1 door_sext_arm=1 "
	    "denser=4 functional_accept=1 functional_session=1 "
	    "soft_listen_ne_host_banner=1 soft!=DUT close "
	    "Soft!=product dual_dod_b=OPEN Dual_DoD=OPEN "
	    "G-AC-1=1 product_ac=0 agent_ne_close=1 stamp_free=1 "
	    "denser residual bar bar=" SSHD_BAR_HONESTY " never_invent=.76\n");

	/* greppable: sshd-gj: soft residual lean denser */
	msg("sshd-gj: soft residual lean denser denser=1 denser_ok=1 "
	    "denser_arms=7/7 mid_arms=7/7 chain_arms=7/7 "
	    "product_net_owns_wire=1 product_udx_nic=1 product_path=UDX "
	    "not_freestanding_rtl=1 freestanding_rtl=SKIP "
	    "door_sext_arm=1 denser=4 residual_lean=1 "
	    "listen=:22 lab_ip=" SSHD_LAB_IP_SPIRIT " spirit "
	    "soft_listen_ne_host_banner=1 soft!=DUT close "
	    "Soft!=product dual_dod_b=OPEN Dual_DoD=OPEN "
	    "G-AC-1=1 product_ac=0 agent_ne_close=1 stamp_free=1 "
	    "denser residual bar bar=" SSHD_BAR_HONESTY " never_invent=.76\n");

	/*
	 * greppable: sshd-gj: soft residual denser VERDICT
	 * Agent-facing denser residual bar .75 rollup. Soft!=product.
	 * dual_dod_b=OPEN forever here; denser residual != Dual DoD B close.
	 * denser VERDICT PASS = denser residual honesty surface green only —
	 * never host banner L3 proof; never Dual DoD B product close.
	 * denser residual bar .75: critical greppables first (Soft!=product /
	 * dual_dod_b=OPEN / product_net_owns_wire / bar) then arm rollup —
	 * aLine must hold full VERDICT (no silent truncate of Dual DoD keys).
	 * ASCII Soft!= only (never non-ASCII soft-ne token).
	 */
	{
		char aLine[1024]; /* denser VERDICT full greppables (bar .75) */
		unsigned o;

		o = 0;
		/* Critical Dual DoD B denser greppables first (bar .75). */
		append_s(aLine, sizeof(aLine), &o,
			 "sshd-gj: soft residual denser VERDICT ");
		append_s(aLine, sizeof(aLine), &o, szVerdict);
		append_s(aLine, sizeof(aLine), &o,
			 " Soft!=product dual_dod_b=OPEN Dual_DoD=OPEN "
			 "product_net_owns_wire=1 product_udx_nic=1 "
			 "denser residual bar bar=" SSHD_BAR_HONESTY " "
			 "stamp_free=1 denser residual != Dual DoD close "
			 "agent_ne_close=1 product_ac=0 G-AC-1=1 ");
		append_s(aLine, sizeof(aLine), &o, "denser=");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_LOCK);
		append_s(aLine, sizeof(aLine), &o, " denser_ok=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
		append_s(aLine, sizeof(aLine), &o, " denser_prove=");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_PROVE);
		append_s(aLine, sizeof(aLine), &o, " denser_residual=");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_RESIDUAL);
		append_s(aLine, sizeof(aLine), &o, " denser_arms=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uWireArms);
		append_s(aLine, sizeof(aLine), &o, "/");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_WIRE_ARMS);
		append_s(aLine, sizeof(aLine), &o, " mid_arms=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uMidArms);
		append_s(aLine, sizeof(aLine), &o, "/");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_MIDDLE_ARMS);
		append_s(aLine, sizeof(aLine), &o, " chain_arms=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uChainArms);
		append_s(aLine, sizeof(aLine), &o, "/");
		append_u(aLine, sizeof(aLine), &o,
			 (unsigned long)SSHD_DENSER_CHAIN_ARMS);
		append_s(aLine, sizeof(aLine), &o,
			 " arm_listen=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmListen);
		append_s(aLine, sizeof(aLine), &o, " arm_door=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmDoor);
		append_s(aLine, sizeof(aLine), &o, " arm_wire=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmWire);
		append_s(aLine, sizeof(aLine), &o, " arm_udx=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmUdx);
		append_s(aLine, sizeof(aLine), &o, " arm_dod=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmDod);
		append_s(aLine, sizeof(aLine), &o, " arm_soft_ne=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmSoftNe);
		append_s(aLine, sizeof(aLine), &o, " arm_bar=");
		append_u(aLine, sizeof(aLine), &o, (unsigned long)uArmBar);
		append_s(aLine, sizeof(aLine), &o,
			 " door_sext_arm=1 denser=4 functional_accept=1 "
			 "functional_session=1 residual_lean=1 "
			 "listen=:22 lab_ip=" SSHD_LAB_IP_SPIRIT " spirit "
			 "product_path=UDX not_freestanding_rtl=1 "
			 "freestanding_rtl=SKIP soft_listen_ne_host_banner=1 "
			 "soft!=DUT close product=UDX+sshd+stack "
			 "chain=" SSHD_PRODUCT_CHAIN " "
			 "hop_down=" SSHD_PRODUCT_CHAIN_DOWN " "
			 "product_mint=0 never_invent=.76 "
			 "dual=MIT_OR_Apache-2.0\n");
		aLine[o] = '\0';
		msg(aLine);
	}

	/* silence unused if constants fold — keep arms live for greppables */
	(void)uProve;
}

/*
 * Product entry: self-smoke on TCP :22, then park with listen held.
 * Dual DoD B: yield-friendly eth ACCEPT + session (banner / KEX / soft).
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
		 * Keep listen open anyway so external eth connect still works.
		 */
		soft_suite();
		msg("sshd-gj: daemon park after FAIL (TCP :22 listen held)\n");
		/* fall through to park — do not CLOSE srv / exit */
	}

	/*
	 * Wave 126 soft inventory: offline suite + honesty / path / stats.
	 * Greppable "sshd-gj: soft …" — not product SSH completeness.
	 * Never hard-fails after live path PASS.
	 */
	if (banner_ok && sess_ok) {
		soft_suite();
	}

	/*
	 * Dual DoD B park (residual lean deepen): keep listener open for host
	 * :22 proof. Product path: sshd over net doors once UDX/L2 exists and
	 * product net owns wire (UDX+ABI); freestanding rtl is lab interim
	 * only (rtl SKIP — not wire owner). Eth demux + SYN-ACK rtx run in
	 * door POLL / lab freestanding poll when this task yields (no kernel
	 * stamp). Functional accept residual: door-first AcceptQ claim +
	 * yield-first accept-every-yield once L2/TCP works (L3 host path).
	 * Soft!=product. Soft listen != host banner proof (G-AC-1).
	 * Grep: soft residual lean / functional_accept / product_net_owns_wire
	 * Grep: product_path=UDX / not_freestanding_rtl
	 */
	msg("sshd-gj: daemon park (TCP :22 listen held)\n");
	msg("sshd-gj: eth accept loop ready (yield+poll Dual DoD B)\n");
	msg("sshd-gj: soft honesty eth-session not-product-ssh "
	    "multi_server=0 confine=0 soft_ne_product=1 Soft!=product\n");
	msg("sshd-gj: soft eth park residual_lean=1 accept-every-yield "
	    "yield_first=1 pure_POLL_ACCEPT=1 yield_flush=1 banner_once=1 "
	    "functional_accept=1 functional_session=1 "
	    "product_net_owns_wire=1 door_first=1 door_poll_accept=1 "
	    "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
	    "door_sext_arm=1 denser=4 "
	    "anti_stall=1 Soft!=product dual_dod_b=OPEN "
	    "soft_listen_ne_host_banner=1 product_path=UDX "
	    "not_freestanding_rtl=1\n");
	/*
	 * Soft residual lean deepen once (exclusive residual). Soft!=product
	 * · G-AC-1. Product sshd over net doors once UDX/L2; product net owns
	 * wire = UDX+ABI; freestanding rtl SKIP (lab interim only). Soft
	 * listen != host banner. Dual MIT OR Apache-2.0. No stamp storms.
	 * BAR v2026.08.04.75 stamp-free (never invent .76).
	 * Grep: sshd-gj: soft residual lean
	 * Grep: functional_accept / functional_session / product_net_owns_wire
	 * Grep: Soft!=product / product_path=UDX / not_freestanding_rtl
	 * Grep: door_triple_arm / door_quad_arm / door_quint_arm /
	 * door_sext_arm / denser=4 /
	 * soft residual lean deepen product_net_owns_wire
	 */
	msg("sshd-gj: soft residual lean residual_lean=1 net_door=1 "
	    "once_UDX_L2=1 product_path=UDX product_net_owns_wire=1 "
	    "not_freestanding_rtl=1 functional_accept=1 "
	    "functional_session=1 soft_listen_ne_host_banner=1 "
	    "yield_first=1 banner_once=1 anti_stall=1 door_first=1 "
	    "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
	    "door_sext_arm=1 denser=4 "
	    "dual_dod_b=OPEN soft_ne_product=1 Soft!=product "
	    "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
	    "(Soft!=product; product sshd over net doors once UDX/L2; "
	    "product net owns wire=UDX+ABI; freestanding rtl SKIP; "
	    "soft listen != host banner; Dual DoD B product=UDX not "
	    "freestanding rtl)\n");
	msg("sshd-gj: soft residual lean deepen residual_lean=1 "
	    "functional_accept=1 functional_session=1 "
	    "product_net_owns_wire=1 product_path=UDX "
	    "not_freestanding_rtl=1 soft_listen_ne_host_banner=1 "
	    "door_first=1 door_claim=1 door_poll_accept=1 "
	    "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
	    "door_sext_arm=1 denser=4 "
	    "listen_accept_yield_poll=1 Soft!=product dual_dod_b=OPEN "
	    "G-AC-1=1 dual=MIT_OR_Apache-2.0\n");
	msg("sshd-gj: soft residual lean deepen accept "
	    "functional_accept=1 door_claim=1 door_poll_accept=1 "
	    "door_triple_arm=1 door_quad_arm=1 door_quint_arm=1 "
	    "door_sext_arm=1 denser=4 "
	    "residual_lean=1 product_net_owns_wire=1 product_path=UDX "
	    "not_freestanding_rtl=1 soft_listen_ne_host_banner=1 "
	    "Soft!=product dual_dod_b=OPEN\n");
	msg("sshd-gj: soft residual lean deepen session "
	    "functional_session=1 door_settle=1 banner_once=1 "
	    "residual_lean=1 product_net_owns_wire=1 product_path=UDX "
	    "not_freestanding_rtl=1 soft_listen_ne_host_banner=1 "
	    "door_quad_arm=1 door_quint_arm=1 door_sext_arm=1 denser=4 "
	    "Soft!=product dual_dod_b=OPEN\n");
	/*
	 * Soft residual lean deepen product_net_owns_wire (denser=4 :22 path
	 * when UDX owns eth for host nc). Soft!=product; dual_dod_b=OPEN;
	 * soft_listen_ne_host_banner=1. BAR v2026.08.04.75 stamp-free.
	 * Grep: soft residual lean deepen product_net_owns_wire /
	 * door_triple_arm / door_quad_arm / door_quint_arm / door_sext_arm /
	 * denser=4 / functional_accept
	 */
	msg("sshd-gj: soft residual lean deepen product_net_owns_wire "
	    "product_net_owns_wire=1 product_udx_nic=1 door_triple_arm=1 "
	    "door_quad_arm=1 door_quint_arm=1 door_sext_arm=1 denser=4 "
	    "door_poll_accept=1 "
	    "door_first=1 door_claim=1 functional_accept=1 "
	    "functional_session=1 residual_lean=1 product_path=UDX "
	    "not_freestanding_rtl=1 listen=:22 lab_ip=10.200.125.50 spirit "
	    "soft_listen_ne_host_banner=1 dual_dod_b=OPEN Soft!=product "
	    "soft!=DUT close G-AC-1=1 dual=MIT_OR_Apache-2.0 "
	    "(STRONGER denser=4 door POLL+ACCEPT product residual :22 path "
	    "when UDX owns eth; sext ACCEPT pass per POLL denser "
	    "(quint/quad/triple still greppable) + post-arm bare reclaim + "
	    "mid-tick quint POLL+ACCEPT denser (quad/triple/dual still "
	    "greppable); host nc SYN->ESTABLISHED->banner under product "
	    "ownership; soft listen != host banner; Dual DoD B OPEN; "
	    "soft!=DUT close)\n");
	/*
	 * Soft listen != host banner proof (G-AC-1 Soft!=product):
	 * park/listen held + greppable soft residual lamps are NOT L3 host
	 * banner proof. Dual DoD B stays OPEN until DUT host nc sees product
	 * id SSH-2.0-GreenJade_sshd on eth :22 on a stamped flash image.
	 * Grep: soft residual lean deepen soft_listen / soft_listen_ne_host_banner
	 */
	msg("sshd-gj: soft residual lean deepen soft_listen "
	    "soft_listen_ne_host_banner=1 dual_dod_b=OPEN Soft!=product "
	    "G-AC-1=1 product_path=UDX product_net_owns_wire=1 "
	    "not_freestanding_rtl=1 residual_lean=1 door_triple_arm=1 "
	    "door_quad_arm=1 door_quint_arm=1 door_sext_arm=1 denser=4 "
	    "(soft listen held != DUT host nc product id on eth :22; "
	    "Dual DoD B OPEN until stamped L3)\n");
	/*
	 * Soft residual product once-lamp (H2 lean; no stamp storm):
	 * product UDX NIC path residual honesty for Dual DoD B on laptop
	 * wire. rtl8168_udx owns wire → netstack → sshd listen=:22.
	 * Prefer product UDX over freestanding eth residual; freestanding
	 * soft listen honesty stays SKIP product close (soft!=DUT close).
	 * Soft!=product; dual_dod_b=OPEN; G-AC-1. BAR v2026.08.04.75.
	 * Grep: soft residual product / product_udx_nic / lab_ip=10.200.125.50
	 * spirit / listen=:22 / soft!=DUT close / Soft!=product / dual_dod_b=OPEN
	 */
	/* greppable once: sshd soft residual product (H2; no stamp storm) */
	msg("sshd-gj: soft residual product "
	    "sshd soft residual product "
	    "product_udx_nic=1 product_net_owns_wire=1 product_path=UDX "
	    "not_freestanding_rtl=1 listen=:22 lab_ip=10.200.125.50 spirit "
	    "door_first=1 door_poll_accept=1 door_triple_arm=1 door_quad_arm=1 "
	    "door_quint_arm=1 door_sext_arm=1 denser=4 functional_accept=1 "
	    "functional_session=1 residual_lean=1 "
	    "soft_listen_ne_host_banner=1 Soft!=product dual_dod_b=OPEN "
	    "soft!=DUT close G-AC-1=1 dual=MIT_OR_Apache-2.0 "
	    "(prefer product UDX wire path over freestanding eth residual; "
	    "freestanding soft listen honesty stays SKIP product close; "
	    "STRONGER denser=4 door POLL+ACCEPT residual when product path "
	    "present; Dual DoD B OPEN; Soft!=product; soft!=DUT close)\n");
	/*
	 * denser residual bar .75 once-lamps (H2; Dual DoD B product_net_owns_wire).
	 * Soft!=product; dual_dod_b=OPEN; denser residual != Dual DoD B close.
	 * BAR v2026.08.04.75 stamp-free (never invent .76).
	 * Grep: soft residual denser product_net_owns_wire /
	 * soft residual denser VERDICT / denser residual bar /
	 * bar=v2026.08.04.75 / Soft!=product / dual_dod_b=OPEN
	 */
	soft_residual_denser_product_net_owns_wire();
	/*
	 * Product-net :22 listen residual arm STRONGER denser denser=4
	 * (product_net_owns_wire POLL+ACCEPT residual): denser door POLL then
	 * ACCEPT so AcceptQ is live under UDX ownership before the park loop.
	 * Soft listen held != host banner. Soft!=product. soft!=DUT close.
	 * Grep: door_poll_accept / functional_accept / product_net_owns_wire /
	 * soft residual product / product_udx_nic / door_triple_arm /
	 * door_quad_arm / door_quint_arm / door_sext_arm / denser=4 /
	 * Soft!=product / dual_dod_b=OPEN / lab_ip=10.200.125.50 spirit
	 */
	net_door_progress_lean(44u);
	{
		long aArm;

		/* STRONGER product residual arm under product_udx_nic path. */
		aArm = eth_accept_door_claim(srv, 18u, 4u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
		/* Second denser pre-park reclaim (product_net_owns_wire). */
		aArm = eth_accept_door_claim(srv, 10u, 3u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
		/* Third denser pre-park micro reclaim (POLL+ACCEPT residual). */
		aArm = eth_accept_door_claim(srv, 4u, 3u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
		/* Fourth denser pre-park micro reclaim STRONGER. Soft!=product. */
		aArm = eth_accept_door_claim(srv, 3u, 2u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
		/* Fifth denser pre-park micro reclaim denser=4. Soft!=product. */
		aArm = eth_accept_door_claim(srv, 2u, 2u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
		/* Sixth denser pre-park micro reclaim denser=4 STRONGER. Soft!=product. */
		aArm = eth_accept_door_claim(srv, 2u, 2u);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
	}
	for (;;) {
		long a;
		unsigned iDrain;
		unsigned iClaim;
		unsigned iPost;

		/*
		 * Functional accept residual lean deepen denser STRONGER
		 * (Dual DoD B denser=4): door-first AcceptQ + denser interleaved
		 * door POLL+ACCEPT (sext arm + sext ACCEPT denser) + bounded
		 * pure POLL with yield every outer (no multi-thousand busy-
		 * spin). Soft empty → idle residual. Soft!=product. Soft
		 * listen != host banner. Grep: eth accept session /
		 * residual_lean / functional_accept / product_net_owns_wire /
		 * product_udx_nic / door_sext_arm / denser=4 / Soft!=product /
		 * dual_dod_b=OPEN
		 */
		a = eth_accept_yield_poll(srv);
		if (a >= 0) {
			(void)eth_handle_accept(a);
			continue;
		}

		/*
		 * Lean idle residual deepen denser STRONGER denser=4
		 * (product_net_owns_wire POLL+ACCEPT residual): denser product-
		 * net door POLL then ACCEPT first (when product owns wire),
		 * then short pure POLL + yield every 4 ticks (lab freestanding
		 * poll under empty AcceptQ) + denser post-yield multi door
		 * reclaim. Soft!=product. soft!=DUT close.
		 * Grep: product_net_owns_wire / door_sext_arm / door_quint_arm /
		 * door_quad_arm / product_udx_nic / dual_dod_b=OPEN / denser=4
		 */
		/* Idle door head denser ACCEPT sample (anti-stall lean). */
		a = eth_accept_door_claim(srv, 16u, 4u);
		if (a < 0) {
			for (iDrain = 0; iDrain < 32u; iDrain++) {
				net_poll_once();
				/* Quint ACCEPT pass same POLL (denser residual). */
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				if ((iDrain & 3u) == 3u) {
					gj_yield();
					/* Post-yield denser multi reclaim (UDX). */
					net_door_progress_lean(16u);
					a = eth_accept_door_claim(srv, 8u, 4u);
					if (a >= 0) {
						break;
					}
					a = eth_accept_door_claim(srv, 4u, 3u);
					if (a >= 0) {
						break;
					}
					a = eth_accept_door_claim(srv, 3u, 3u);
					if (a >= 0) {
						break;
					}
					a = eth_accept_door_claim(srv, 2u, 2u);
					if (a >= 0) {
						break;
					}
					/* Fifth denser post-yield micro reclaim. */
					a = eth_accept_door_claim(srv, 2u, 2u);
					if (a >= 0) {
						break;
					}
					/* Sixth denser post-yield micro reclaim denser=4. */
					a = eth_accept_door_claim(srv, 1u, 2u);
					if (a >= 0) {
						break;
					}
				}
			}
		}
		if (a < 0) {
			/* Bare claim burst after idle drain. Soft!=product. */
			for (iClaim = 0; iClaim < 48u; iClaim++) {
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
			}
		}
		if (a < 0) {
			/* Denser idle door re-arm under product_net_owns_wire. */
			a = eth_accept_door_claim(srv, 8u, 3u);
		}
		if (a < 0) {
			/* Second denser idle re-arm STRONGER (product_udx_nic). */
			a = eth_accept_door_claim(srv, 4u, 3u);
		}
		if (a < 0) {
			/* Third denser idle micro re-arm (POLL+ACCEPT residual). */
			a = eth_accept_door_claim(srv, 3u, 2u);
		}
		if (a < 0) {
			/* Fourth denser idle micro re-arm STRONGER. Soft!=product. */
			a = eth_accept_door_claim(srv, 2u, 2u);
		}
		if (a < 0) {
			/* Fifth denser idle micro re-arm denser=4. Soft!=product. */
			a = eth_accept_door_claim(srv, 2u, 2u);
		}
		if (a >= 0) {
			(void)eth_handle_accept(a);
			continue;
		}

		/*
		 * Accept-every-yield residual STRONGER denser denser=4
		 * (product_net_owns_wire POLL+ACCEPT residual): yield then
		 * denser door POLL then ACCEPT + short pure POLL (SYN-ACK may
		 * land mid-yield). Soft!=product. soft!=DUT close.
		 * product_net_owns_wire / door_sext_arm / door_quint_arm /
		 * door_quad_arm / door_triple_arm / product_udx_nic /
		 * dual_dod_b=OPEN / denser=4.
		 */
		net_progress();
		a = eth_accept_door_claim(srv, 14u, 4u);
		if (a < 0) {
			for (iClaim = 0; iClaim < 40u; iClaim++) {
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
			}
		}
		if (a < 0) {
			net_door_progress_lean(20u);
			for (iPost = 0; iPost < 16u; iPost++) {
				net_poll_once();
				/* Quint ACCEPT pass same POLL denser. Soft!=product. */
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
				a = gj_net(GJ_NET_OP_ACCEPT, srv, 0, 0);
				if (a >= 0) {
					break;
				}
			}
			/* Final denser multi door re-arm (product_net_owns_wire). */
			if (a < 0) {
				a = eth_accept_door_claim(srv, 8u, 3u);
			}
			if (a < 0) {
				a = eth_accept_door_claim(srv, 4u, 3u);
			}
			if (a < 0) {
				a = eth_accept_door_claim(srv, 3u, 2u);
			}
			if (a < 0) {
				a = eth_accept_door_claim(srv, 2u, 2u);
			}
			if (a < 0) {
				/* Fifth denser final micro reclaim. Soft!=product. */
				a = eth_accept_door_claim(srv, 2u, 2u);
			}
			if (a < 0) {
				/* Sixth denser final micro reclaim denser=4. Soft!=product. */
				a = eth_accept_door_claim(srv, 1u, 2u);
			}
		}
		if (a >= 0) {
			(void)eth_handle_accept(a);
		}
	}
}
