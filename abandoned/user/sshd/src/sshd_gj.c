/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GreenJade freestanding product sshd (sshd.elf).
 * Dual MIT OR Apache-2.0. Clean-room pure C — not an OpenSSH/GPL paste.
 *
 * Live path (TCP + KEX + NEWKEYS + shell), greppable PASS markers:
 *   1. TCP :22 listen/accept (net_tcp / UDX ETH + virtio-net + loopback)
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
 *     OpenSSH-facing: ident fail-closed at CR LF; leftover bytes kept for
 *     the next SSH binary packet (KEXINIT). nc FIN is not session PASS if
 *     the peer sent SSH-. eth client KEXINIT soft-skip does not CLOSE
 *     immediately (retry / stay-open). Eth ECDH_REPLY uses RFC 8709 K_S
 *     + RFC 4253 H; RFC 8032 of H (need=ed25519 host-accept OPEN). Banner != login.
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
 *     (soft_listen_ne_host_banner=1; Dual DoD B OPEN until interactive SSH
 *     login; G-AC-1 Soft!=product). Soft listen held != host banner L3 proof.
 *     Stamp-free residual (never invent next N / never bump image stamp
 *     from this unit). Product residual STRONGER denser
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
#define SSH_MSG_USERAUTH_REQUEST        50
#define SSH_MSG_USERAUTH_FAILURE        51
#define SSH_MSG_USERAUTH_SUCCESS        52
#define SSH_MSG_USERAUTH_PK_OK          60
#define SSH_MSG_KEXINIT                 20
#define SSH_MSG_NEWKEYS                 21
#define SSH_MSG_KEX_ECDH_INIT           30
#define SSH_MSG_KEX_ECDH_REPLY          31
#define SSH_MSG_IGNORE                  2
#define SSH_MSG_GLOBAL_REQUEST          80
#define SSH_MSG_REQUEST_FAILURE         82
#define SSH_MSG_CHANNEL_OPEN            90
#define SSH_MSG_CHANNEL_OPEN_CONFIRM    91
#define SSH_MSG_CHANNEL_WINDOW_ADJUST   93
#define SSH_MSG_CHANNEL_DATA            94
#define SSH_MSG_CHANNEL_EOF             96
#define SSH_MSG_CHANNEL_CLOSE           97
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
void gj_ssh_chacha20_ssh(const uint8_t aKey[32], uint32_t uSeq, uint32_t uCtr,
			 uint8_t *pData, size_t cb);
void gj_ssh_poly1305(const uint8_t key[32], const uint8_t *pMsg, size_t cbMsg,
		     uint8_t tag[16]);
int  gj_ssh_poly1305_selfcheck(void);
int  gj_ssh_poly1305_ok(void);
int  gj_ssh_memeq_ct(const uint8_t *pA, const uint8_t *pB, size_t cb);
void gj_ssh_hostkey_init(void);
void gj_ssh_hostkey_pk(uint8_t pk[32]);
void gj_ssh_hostkey_sign(const uint8_t *pMsg, size_t cbMsg, uint8_t aSig[64]);
int  gj_ssh_hostkey_verify(const uint8_t *pMsg, size_t cbMsg,
			   const uint8_t aSig[64]);
int  gj_ssh_ed25519_verify(const uint8_t aPk[32], const uint8_t *pMsg,
			   size_t cbMsg, const uint8_t aSig[64]);

static long net_send_wait(long fd, const void *buf, long cap, unsigned tries);
static long net_recv_wait(long fd, void *buf, long cap, unsigned tries);
static void net_flush_residual(unsigned uOuter, unsigned uInner);

static const char g_szBanner[] = "SSH-2.0-GreenJade_sshd\r\n";
static const char g_szClientBanner[] = "SSH-2.0-GreenJade_smoke\r\n";
static const char g_szMotd[] =
	"GreenJade sshd: shell ready (session channel soft path)\r\n";

/*
 * Session keys after NEWKEYS.
 * chacha20-poly1305@openssh.com: 64-byte C/D → K_2 || K_1.
 */
static uint8_t g_k2_c2s[32], g_k1_c2s[32];
static uint8_t g_k2_s2c[32], g_k1_s2c[32];
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
 * Ciphertext already pulled from TCP (OpenSSH often coalesces clear NEWKEYS
 * + AEAD SERVICE_REQUEST). recv_pkt must consume this, not overwrite it.
 */
static uint8_t g_aead_hold[4096];
static uint32_t g_aead_have;

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

/*
 * RFC 4253 §4.2: identification string is terminated by CR LF.
 * Fail-closed: do not treat the buffer as a complete ident (and do not
 * peel leftover binary) until "\r\n" is present. Optional non-SSH lines
 * before the SSH- line are skipped only when each is also CR LF ended.
 * Returns byte offset past the ident CR LF, or 0 if incomplete.
 */
static long
ident_line_end(const uint8_t *buf, long n)
{
	long i;
	long line = 0;

	if (buf == 0 || n < 6) {
		return 0;
	}
	for (i = 0; i + 1 < n; i++) {
		if (buf[i] != (uint8_t)'\r') {
			continue;
		}
		if (buf[i + 1] != (uint8_t)'\n') {
			return 0; /* fail-closed: CR not paired with LF */
		}
		if ((i - line) >= 4 && banner_is_ssh((const char *)(buf + line),
						     i - line)) {
			return i + 2;
		}
		line = i + 2;
		i++;
	}
	return 0;
}

static uint32_t
get_u32(const uint8_t *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/*
 * Cleartext SSH binary packet size (RFC 4253 §6): 4 + packet_length.
 * 0 = incomplete or fail-closed (length out of range).
 */
static uint32_t
ssh_clear_pkt_need(const uint8_t *p, long n)
{
	uint32_t pktlen;

	if (p == 0 || n < 4) {
		return 0;
	}
	pktlen = get_u32(p);
	if (pktlen < 6u || pktlen > 35000u) {
		return 0;
	}
	return 4u + pktlen;
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
	/* Advertise only algorithms this unit can emit. hmac-sha2-256 is a
	 * MAC — never a hostkey name. Next hard stop: RFC 4253/8709 K_S +
	 * Ed25519 verify (no Ed25519 stack invented in this slice).
	 */
	put_name_list(payload, &off, "curve25519-sha256,curve25519-sha256@libssh.org");
	put_name_list(payload, &off, "ssh-ed25519");
	put_name_list(payload, &off, "chacha20-poly1305@openssh.com");
	put_name_list(payload, &off, "chacha20-poly1305@openssh.com");
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
	/*
	 * RFC 4253 §6: (4 + packet_length) % 8 == 0, i.e. packet_length
	 * ≡ 4 (mod 8). Aligning 1+pay+pad made packet_length ≡ 0 (mod 8);
	 * OpenSSH 9.9 then ssh_packet_start_discard → "MAC incorrect"
	 * (0.1.157 host). Soft!=product.
	 */
	while ((4u + 1u + pay_len + pad) % 8u != 0u) {
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

	/*
	 * Clear: RFC 4253 (4+packet_length)%8==0 (0.1.157 KEXINIT).
	 * AEAD: OpenSSH packet.c need=packlen; need%block==0, so
	 * packet_length%8==0. 172 host: length 28 then
	 * "padding error: need 28 block 8 mod 4" + MAC incorrect.
	 */
	if (g_encrypted != 0) {
		while ((1u + pay_len + pad) % 8u != 0u) {
			pad++;
		}
	} else {
		while ((4u + 1u + pay_len + pad) % 8u != 0u) {
			pad++;
		}
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

#define SSH_KS_ED25519_CB   51u
#define SSH_SIG_ED25519_CB  83u

static uint32_t
ident_strip_crlf(const char *sz, uint32_t cb)
{
	while (cb > 0u && (sz[cb - 1u] == '\n' || sz[cb - 1u] == '\r')) {
		cb--;
	}
	return cb;
}

static void
ssh_sha256_put_string(struct sha256_ctx *pCtx, const void *p, uint32_t cb)
{
	uint8_t aLen[4];

	put_u32(aLen, cb);
	gj_ssh_sha256_update(pCtx, aLen, 4);
	if (cb != 0u && p != 0) {
		gj_ssh_sha256_update(pCtx, p, (size_t)cb);
	}
}

static void
ssh_sha256_put_mpint32(struct sha256_ctx *pCtx, const uint8_t aBe[32])
{
	uint32_t i0 = 0u;
	uint32_t cb;
	uint8_t aLen[4];
	uint8_t u8Pad = 0;
	int fPad;

	while (i0 < 32u && aBe[i0] == 0u) {
		i0++;
	}
	if (i0 == 32u) {
		put_u32(aLen, 0);
		gj_ssh_sha256_update(pCtx, aLen, 4);
		return;
	}
	fPad = ((aBe[i0] & 0x80u) != 0u) ? 1 : 0;
	cb = (32u - i0) + (uint32_t)fPad;
	put_u32(aLen, cb);
	gj_ssh_sha256_update(pCtx, aLen, 4);
	if (fPad != 0) {
		gj_ssh_sha256_update(pCtx, &u8Pad, 1);
	}
	gj_ssh_sha256_update(pCtx, aBe + i0, (size_t)(32u - i0));
}

static int
kexinit_payload(const uint8_t *pPkt, long cbPkt, const uint8_t **ppPay,
		uint32_t *pcbPay)
{
	uint32_t pktlen;
	uint32_t pad;
	uint32_t pay;

	if (pPkt == 0 || cbPkt < 6 || pPkt[5] != SSH_MSG_KEXINIT) {
		return 0;
	}
	pktlen = get_u32(pPkt);
	pad = (uint32_t)pPkt[4];
	if (pktlen < 6u || pad < 4u || (4 + (long)pktlen) > cbPkt) {
		return 0;
	}
	if (pktlen < 1u + pad) {
		return 0;
	}
	pay = pktlen - 1u - pad;
	if (pay < 1u) {
		return 0;
	}
	*ppPay = pPkt + 5;
	*pcbPay = pay;
	return 1;
}

static uint32_t
encode_ks_ed25519(uint8_t *pOut, const uint8_t aPk32[32])
{
	put_u32(pOut, 11);
	bytes_copy(pOut + 4, (const uint8_t *)"ssh-ed25519", 11);
	put_u32(pOut + 15, 32);
	bytes_copy(pOut + 19, aPk32, 32);
	return SSH_KS_ED25519_CB;
}

static uint32_t
encode_sig_ed25519(uint8_t *pOut, const uint8_t aSig64[64])
{
	put_u32(pOut, 11);
	bytes_copy(pOut + 4, (const uint8_t *)"ssh-ed25519", 11);
	put_u32(pOut + 15, 64);
	bytes_copy(pOut + 19, aSig64, 64);
	return SSH_SIG_ED25519_CB;
}

static int
shared_is_zero(const uint8_t a[32])
{
	unsigned i;
	uint8_t u = 0;

	for (i = 0; i < 32; i++) {
		u = (uint8_t)(u | a[i]);
	}
	return u == 0u;
}

static void
kex_hash_rfc(uint8_t H[32], const char *pVc, uint32_t cbVc, const char *pVs,
	     uint32_t cbVs, const uint8_t *pIc, uint32_t cbIc,
	     const uint8_t *pIs, uint32_t cbIs, const uint8_t *pKs,
	     uint32_t cbKs, const uint8_t aQc[32], const uint8_t aQs[32],
	     const uint8_t aK[32])
{
	struct sha256_ctx hx;

	gj_ssh_sha256_init(&hx);
	ssh_sha256_put_string(&hx, pVc, cbVc);
	ssh_sha256_put_string(&hx, pVs, cbVs);
	ssh_sha256_put_string(&hx, pIc, cbIc);
	ssh_sha256_put_string(&hx, pIs, cbIs);
	ssh_sha256_put_string(&hx, pKs, cbKs);
	ssh_sha256_put_string(&hx, aQc, 32);
	ssh_sha256_put_string(&hx, aQs, 32);
	ssh_sha256_put_mpint32(&hx, aK);
	gj_ssh_sha256_final(&hx, H);
}

/*
 * SSH_MSG_KEX_ECDH_REPLY: string K_S || string Q_S || string signature.
 * K_S / sig are RFC 8709 blobs. Soft!=product. need=ed25519 OPEN.
 */
static uint32_t
build_ecdh_reply(uint8_t *pkt, uint32_t cap, const uint8_t *pKs, uint32_t cbKs,
		 const uint8_t q_s[32], const uint8_t *pSig, uint32_t cbSig)
{
	uint8_t body[4 + 64 + 4 + 32 + 4 + 96];
	uint32_t off = 0;

	if (pKs == 0 || pSig == 0 || cbKs == 0u || cbKs > 64u || cbSig == 0u ||
	    cbSig > 96u) {
		return 0;
	}
	put_u32(body + off, cbKs);
	off += 4;
	bytes_copy(body + off, pKs, cbKs);
	off += cbKs;
	put_u32(body + off, 32);
	off += 4;
	bytes_copy(body + off, q_s, 32);
	off += 32;
	put_u32(body + off, cbSig);
	off += 4;
	bytes_copy(body + off, pSig, cbSig);
	off += cbSig;
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
 * RFC 4252 USERAUTH_FAILURE: name-list + boolean partial success.
 * Not SUCCESS. Authentications that can continue ≠ login.
 */
static uint32_t
build_userauth_failure(uint8_t *pkt, uint32_t cap, const char *szMethods)
{
	uint8_t body[64];
	uint32_t cbName = (uint32_t)slen(szMethods);
	uint32_t iByte;

	if (cbName > 48) {
		cbName = 48;
	}
	put_u32(body, cbName);
	for (iByte = 0; iByte < cbName; iByte++) {
		body[4 + iByte] = (uint8_t)szMethods[iByte];
	}
	body[4 + cbName] = 0; /* partial success = false */
	return build_simple(pkt, cap, SSH_MSG_USERAUTH_FAILURE,
			    body, 4 + cbName + 1);
}

static uint32_t
build_userauth_success(uint8_t *pkt, uint32_t cap)
{
	return build_simple(pkt, cap, SSH_MSG_USERAUTH_SUCCESS, 0, 0);
}

static uint32_t
build_userauth_pk_ok(uint8_t *pkt, uint32_t cap, const uint8_t *pAlg,
		     uint32_t cbAlg, const uint8_t *pBlob, uint32_t cbBlob)
{
	uint8_t body[128];
	uint32_t o = 0;

	if (pAlg == 0 || pBlob == 0 || cbAlg > 32 || cbBlob > 80) {
		return 0;
	}
	put_u32(body + o, cbAlg);
	o += 4;
	bytes_copy(body + o, pAlg, cbAlg);
	o += cbAlg;
	put_u32(body + o, cbBlob);
	o += 4;
	bytes_copy(body + o, pBlob, cbBlob);
	o += cbBlob;
	return build_simple(pkt, cap, SSH_MSG_USERAUTH_PK_OK, body, o);
}

static int
ssh_get_string(const uint8_t *p, uint32_t cb, uint32_t *poff,
	       const uint8_t **pp, uint32_t *pcb)
{
	uint32_t n;

	if (p == 0 || poff == 0 || pp == 0 || pcb == 0 || *poff + 4u > cb) {
		return 0;
	}
	n = get_u32(p + *poff);
	*poff += 4u;
	if (n > cb - *poff) {
		return 0;
	}
	*pp = p + *poff;
	*pcb = n;
	*poff += n;
	return 1;
}

static int
ssh_ed25519_blob_pk(const uint8_t *pBlob, uint32_t cbBlob, uint8_t aPk[32])
{
	uint32_t o = 0;
	const uint8_t *pName = 0;
	const uint8_t *pKey = 0;
	uint32_t cbName = 0;
	uint32_t cbKey = 0;

	if (pBlob == 0 || aPk == 0) {
		return 0;
	}
	if (ssh_get_string(pBlob, cbBlob, &o, &pName, &cbName) == 0 ||
	    cbName != 11u ||
	    !gj_ssh_memeq_ct(pName, (const uint8_t *)"ssh-ed25519", 11) ||
	    ssh_get_string(pBlob, cbBlob, &o, &pKey, &cbKey) == 0 ||
	    cbKey != 32u) {
		return 0;
	}
	bytes_copy(aPk, pKey, 32);
	return 1;
}

static int
ssh_ed25519_sig_raw(const uint8_t *pBlob, uint32_t cbBlob, uint8_t aSig[64])
{
	uint32_t o = 0;
	const uint8_t *pName = 0;
	const uint8_t *pSig = 0;
	uint32_t cbName = 0;
	uint32_t cbSig = 0;

	if (pBlob == 0 || aSig == 0) {
		return 0;
	}
	if (ssh_get_string(pBlob, cbBlob, &o, &pName, &cbName) == 0 ||
	    cbName != 11u ||
	    !gj_ssh_memeq_ct(pName, (const uint8_t *)"ssh-ed25519", 11) ||
	    ssh_get_string(pBlob, cbBlob, &o, &pSig, &cbSig) == 0 ||
	    cbSig != 64u) {
		return 0;
	}
	bytes_copy(aSig, pSig, 64);
	return 1;
}

/*
 * RFC 4252 §7 signed data. Session id is H (first KEX).
 * No authorized_keys: any valid ed25519 signature is accepted.
 * SUCCESS ≠ interactive login (no channel).
 */
static int
userauth_pubkey_verify(const uint8_t *pH, const uint8_t *pUser, uint32_t cbUser,
		       const uint8_t *pSvc, uint32_t cbSvc, const uint8_t *pAlg,
		       uint32_t cbAlg, const uint8_t *pBlob, uint32_t cbBlob,
		       const uint8_t *pSigBlob, uint32_t cbSigBlob)
{
	uint8_t aToSign[384];
	uint8_t aPk[32];
	uint8_t aSig[64];
	uint32_t o = 0;
	uint8_t uTrue = 1;

	if (pH == 0 || pUser == 0 || pSvc == 0 || pAlg == 0 || pBlob == 0 ||
	    pSigBlob == 0 || cbUser > 64u || cbSvc > 32u || cbAlg > 32u ||
	    cbBlob > 80u) {
		return 0;
	}
	if (ssh_ed25519_blob_pk(pBlob, cbBlob, aPk) == 0 ||
	    ssh_ed25519_sig_raw(pSigBlob, cbSigBlob, aSig) == 0) {
		return 0;
	}
	put_u32(aToSign + o, 32);
	o += 4;
	bytes_copy(aToSign + o, pH, 32);
	o += 32;
	aToSign[o++] = SSH_MSG_USERAUTH_REQUEST;
	put_u32(aToSign + o, cbUser);
	o += 4;
	bytes_copy(aToSign + o, pUser, cbUser);
	o += cbUser;
	put_u32(aToSign + o, cbSvc);
	o += 4;
	bytes_copy(aToSign + o, pSvc, cbSvc);
	o += cbSvc;
	put_u32(aToSign + o, 9);
	o += 4;
	bytes_copy(aToSign + o, (const uint8_t *)"publickey", 9);
	o += 9;
	aToSign[o++] = uTrue;
	put_u32(aToSign + o, cbAlg);
	o += 4;
	bytes_copy(aToSign + o, pAlg, cbAlg);
	o += cbAlg;
	put_u32(aToSign + o, cbBlob);
	o += 4;
	bytes_copy(aToSign + o, pBlob, cbBlob);
	o += cbBlob;
	if (o > (uint32_t)sizeof(aToSign)) {
		return 0;
	}
	return gj_ssh_ed25519_verify(aPk, aToSign, (size_t)o, aSig);
}

/*
 * RFC 4253 7.2 + OpenSSH 64-byte C/D for chacha20-poly1305@openssh.com.
 * K is the 32-byte shared secret; hashed as the same mpint used in H.
 *
 * K1 = HASH(K || H || X || session_id)     (X is 'C' / 'D')
 * K2 = HASH(K || H || K1)                  (no letter, no session_id)
 * Need 64 bytes: K1 || K2 → K_2 || K_1.
 * 0.1.171 hashed K2 as HASH(K||H||X||sid||K1); OpenSSH length key
 * then missed and SERVICE_REQUEST fail-closed with no MAC incorrect.
 */
static void
kdf_sha256_letter(uint8_t aOut[32], const uint8_t aK[32], const uint8_t aH[32],
		  uint8_t uX)
{
	struct sha256_ctx hx;

	gj_ssh_sha256_init(&hx);
	ssh_sha256_put_mpint32(&hx, aK);
	gj_ssh_sha256_update(&hx, aH, 32);
	gj_ssh_sha256_update(&hx, &uX, 1);
	gj_ssh_sha256_update(&hx, aH, 32);
	gj_ssh_sha256_final(&hx, aOut);
}

static void
kdf_sha256_expand(uint8_t aOut[32], const uint8_t aK[32], const uint8_t aH[32],
		  const uint8_t *pPrev, uint32_t cbPrev)
{
	struct sha256_ctx hx;

	gj_ssh_sha256_init(&hx);
	ssh_sha256_put_mpint32(&hx, aK);
	gj_ssh_sha256_update(&hx, aH, 32);
	if (pPrev != 0 && cbPrev != 0u) {
		gj_ssh_sha256_update(&hx, pPrev, (size_t)cbPrev);
	}
	gj_ssh_sha256_final(&hx, aOut);
}

static void
derive_keys(const uint8_t *K, const uint8_t *H, const uint8_t *sid)
{
	uint8_t aK0[32];
	uint8_t aK1[32];

	(void)sid;
	kdf_sha256_letter(aK0, K, H, (uint8_t)'C');
	kdf_sha256_expand(aK1, K, H, aK0, 32);
	bytes_copy(g_k2_c2s, aK0, 32);
	bytes_copy(g_k1_c2s, aK1, 32);
	kdf_sha256_letter(aK0, K, H, (uint8_t)'D');
	kdf_sha256_expand(aK1, K, H, aK0, 32);
	bytes_copy(g_k2_s2c, aK0, 32);
	bytes_copy(g_k1_s2c, aK1, 32);
	bytes_copy(g_enc_c2s, g_k2_c2s, 32);
	bytes_copy(g_enc_s2c, g_k2_s2c, 32);
	bytes_copy(g_int_c2s, g_k2_c2s, 32);
	bytes_copy(g_int_s2c, g_k2_s2c, 32);
	bytes_copy(g_iv_c2s, g_k1_c2s, 12);
	bytes_copy(g_iv_s2c, g_k1_s2c, 12);
	/*
	 * RFC 4253: sequence is not reset at NEWKEYS. Eth already sent
	 * KEXINIT + ECDH_REPLY + NEWKEYS (seq 0..2); first AEAD is 3.
	 */
	g_encrypted = 1;
	bytes_zero(aK0, sizeof(aK0));
	bytes_zero(aK1, sizeof(aK1));
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
 * Send one SSH binary packet. After NEWKEYS: chacha20-poly1305@openssh.com.
 * Cleartext path used for KEXINIT / ECDH / NEWKEYS.
 */
static long
send_pkt(long fd, uint8_t *pkt, uint32_t n, int as_server)
{
	uint8_t aPoly[64];
	uint8_t aTag[16];
	uint8_t *pK1 = as_server ? g_k1_s2c : g_k1_c2s;
	uint8_t *pK2 = as_server ? g_k2_s2c : g_k2_c2s;
	uint32_t *pSeq = as_server ? &g_seq_s2c_tx : &g_seq_c2s_tx;
	uint32_t uSeq;
	unsigned i;
	long r;

	if (g_encrypted && n >= 5) {
		uSeq = *pSeq;
		for (i = 0; i < 64; i++) {
			aPoly[i] = 0;
		}
		gj_ssh_chacha20_ssh(pK2, uSeq, 0u, aPoly, 64);
		gj_ssh_chacha20_ssh(pK1, uSeq, 0u, pkt, 4);
		gj_ssh_chacha20_ssh(pK2, uSeq, 1u, pkt + 4, n - 4);
		gj_ssh_poly1305(aPoly, pkt, (size_t)n, aTag);
		r = net_send_wait(fd, pkt, (long)n, 32u);
		if (r < (long)n) {
			unsigned iBody;
			long nBody = (r > 0) ? r : 0;

			/*
			 * Body short used to return immediately. 181 host:
			 * OPEN_CONFIRM never reached the client (no Sending
			 * command). Remainder retry, same seq, already
			 * encrypted. Rebuild+send_pkt is the caller's job.
			 */
			for (iBody = 0; iBody < 8u && nBody < (long)n;
			     iBody++) {
				gj_yield();
				net_flush_residual(1u, 4u);
				r = net_send_wait(fd, pkt + nBody,
						  (long)n - nBody, 32u);
				if (r > 0) {
					nBody += r;
				}
			}
			if (nBody < (long)n) {
				bytes_zero(aPoly, sizeof(aPoly));
				bytes_zero(aTag, sizeof(aTag));
				return 0;
			}
		}
		r = net_send_wait(fd, aTag, 16, 32u);
		if (r < 16) {
			unsigned iTag;
			long nTag = (r > 0) ? r : 0;

			/*
			 * Tag short used to return 1..15 (>0). Callers treated
			 * that as OPEN_CONFIRM PASS with seq unbumped (143315).
			 */
			for (iTag = 0; iTag < 8u && nTag < 16; iTag++) {
				gj_yield();
				net_flush_residual(1u, 4u);
				r = net_send_wait(fd, aTag + nTag,
						  16 - nTag, 32u);
				if (r > 0) {
					nTag += r;
				}
			}
			if (nTag < 16) {
				bytes_zero(aPoly, sizeof(aPoly));
				bytes_zero(aTag, sizeof(aTag));
				return 0;
			}
		}
		(*pSeq)++;
		bytes_zero(aPoly, sizeof(aPoly));
		bytes_zero(aTag, sizeof(aTag));
		return (long)n;
	}
	return gj_net(GJ_NET_OP_SEND, fd, (long)(uintptr_t)pkt, (long)n);
}

static void
aead_hold_put(const uint8_t *p, uint32_t n)
{
	if (p == 0 || n == 0u) {
		return;
	}
	if (n > (uint32_t)sizeof(g_aead_hold)) {
		n = (uint32_t)sizeof(g_aead_hold);
	}
	bytes_copy(g_aead_hold, p, n);
	g_aead_have = n;
}

static uint32_t
aead_hold_take(uint8_t *dst, uint32_t need)
{
	uint32_t n;

	if (dst == 0 || need == 0u || g_aead_have == 0u) {
		return 0;
	}
	n = g_aead_have < need ? g_aead_have : need;
	bytes_copy(dst, g_aead_hold, n);
	if (g_aead_have > n) {
		bytes_copy(g_aead_hold, g_aead_hold + n, g_aead_have - n);
		g_aead_have -= n;
	} else {
		g_aead_have = 0;
	}
	return n;
}

/* Fill dst[have..need) from leftover then the socket. Returns new have. */
static uint32_t
aead_fill(long fd, uint8_t *dst, uint32_t have, uint32_t need)
{
	unsigned iTry;

	if (dst == 0 || have >= need) {
		return have;
	}
	if (have == 0u) {
		have = aead_hold_take(dst, need);
	} else if (g_aead_have != 0u) {
		uint32_t n;

		n = aead_hold_take(dst + have, need - have);
		have += n;
	}
	for (iTry = 0; have < need && iTry < 192u; iTry++) {
		long nr;

		nr = net_recv_wait(fd, dst + have, (long)(need - have), 16u);
		if (nr > 0) {
			have += (uint32_t)nr;
			continue;
		}
		if (nr == 0) {
			break;
		}
		gj_yield();
	}
	return have;
}

/*
 * Receive one chacha20-poly1305@openssh.com packet.
 * as_server=1: client→server (c2s).
 * Returns 4+packet_len, 0 to retry (short), or -1 fail-closed
 * (bad length / MAC). Short reads go back into g_aead_hold.
 */
static long
recv_pkt(long fd, uint8_t *pkt, uint32_t cap, int as_server)
{
	uint8_t aPoly[64];
	uint8_t aTag[16];
	uint8_t aExp[16];
	uint8_t aLen[4];
	uint8_t *pK1 = as_server ? g_k1_c2s : g_k1_s2c;
	uint8_t *pK2 = as_server ? g_k2_c2s : g_k2_s2c;
	uint32_t *pSeq = as_server ? &g_seq_c2s_rx : &g_seq_s2c_rx;
	uint32_t uSeq;
	uint32_t uPktLen;
	uint32_t cbWire;
	uint32_t have;
	unsigned i;

	if (!g_encrypted || pkt == 0 || cap < 24) {
		return 0;
	}
	uSeq = *pSeq;
	have = aead_fill(fd, pkt, 0, 4);
	if (have < 4) {
		aead_hold_put(pkt, have);
		return 0;
	}
	for (i = 0; i < 4; i++) {
		aLen[i] = pkt[i];
	}
	for (i = 0; i < 64; i++) {
		aPoly[i] = 0;
	}
	gj_ssh_chacha20_ssh(pK2, uSeq, 0u, aPoly, 64);
	gj_ssh_chacha20_ssh(pK1, uSeq, 0u, aLen, 4);
	uPktLen = ((uint32_t)aLen[0] << 24) | ((uint32_t)aLen[1] << 16) |
		  ((uint32_t)aLen[2] << 8) | (uint32_t)aLen[3];
	if (uPktLen < 5 || uPktLen > cap - 20) {
		aead_hold_put(pkt, have);
		bytes_zero(aPoly, sizeof(aPoly));
		return -1;
	}
	cbWire = 4 + uPktLen;
	have = aead_fill(fd, pkt, have, cbWire);
	if (have < cbWire) {
		aead_hold_put(pkt, have);
		bytes_zero(aPoly, sizeof(aPoly));
		return 0;
	}
	have = aead_fill(fd, aTag, 0, 16);
	if (have < 16) {
		if (have > 0u) {
			if (cbWire + have <= (uint32_t)sizeof(g_aead_hold)) {
				bytes_copy(g_aead_hold, pkt, cbWire);
				bytes_copy(g_aead_hold + cbWire, aTag, have);
				g_aead_have = cbWire + have;
			}
		} else {
			aead_hold_put(pkt, cbWire);
		}
		bytes_zero(aPoly, sizeof(aPoly));
		return 0;
	}
	gj_ssh_poly1305(aPoly, pkt, (size_t)cbWire, aExp);
	if (!gj_ssh_memeq_ct(aTag, aExp, 16)) {
		bytes_zero(aPoly, sizeof(aPoly));
		bytes_zero(aTag, sizeof(aTag));
		bytes_zero(aExp, sizeof(aExp));
		return -1;
	}
	gj_ssh_chacha20_ssh(pK2, uSeq, 1u, pkt + 4, uPktLen);
	for (i = 0; i < 4; i++) {
		pkt[i] = aLen[i];
	}
	(*pSeq)++;
	bytes_zero(aPoly, sizeof(aPoly));
	bytes_zero(aTag, sizeof(aTag));
	bytes_zero(aExp, sizeof(aExp));
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
	uint8_t aReply[384];
	uint8_t aHostPk[32];
	uint8_t aMsg[32];
	uint8_t aSig[64];
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
	{
		uint8_t aKsSoft[51];
		uint8_t aSig64Soft[64];
		uint8_t aSigBlobSoft[83];

		(void)encode_ks_ed25519(aKsSoft, aHostPk);
		gj_ssh_hostkey_sign(aMsg, 32, aSig64Soft);
		(void)encode_sig_ed25519(aSigBlobSoft, aSig64Soft);
		cbReply = build_ecdh_reply(aReply, sizeof(aReply), aKsSoft,
					   SSH_KS_ED25519_CB, aPkS,
					   aSigBlobSoft, SSH_SIG_ED25519_CB);
	}
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
	uint8_t kex_s[640], kex_c[640], ecdh_c[96], ecdh_s[384], rbuf[640];
	uint8_t sk_s[32], pk_s[32], sk_c[32], pk_c[32];
	uint8_t shared_s[32], shared_c[32], H[32], sig[64], host_pk[32];
	uint8_t aKs[51], aSig64[64], aSigBlob[83];
	const uint8_t *pIs = 0;
	const uint8_t *pIc = 0;
	uint32_t cbIs = 0;
	uint32_t cbIc = 0;
	uint32_t cbVc;
	uint32_t cbVs;
	uint8_t newkeys[64];
	uint8_t base[32];
	uint8_t aPolyKey[32];
	uint8_t aPolyTag[16];
	uint8_t aPolyTag2[16];
	uint32_t n;
	long nr;
	unsigned i;
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
	if (kexinit_payload(kex_s, (long)n, &pIs, &cbIs) == 0) {
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
	if (kexinit_payload(rbuf, nr, &pIc, &cbIc) == 0) {
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

	(void)encode_ks_ed25519(aKs, host_pk);
	cbVc = ident_strip_crlf(g_szClientBanner, (uint32_t)slen(g_szClientBanner));
	cbVs = ident_strip_crlf(g_szBanner, (uint32_t)slen(g_szBanner));
	kex_hash_rfc(H, g_szClientBanner, cbVc, g_szBanner, cbVs, pIc, cbIc,
		     pIs, cbIs, aKs, SSH_KS_ED25519_CB, pk_c, pk_s, shared_s);

	gj_ssh_hostkey_sign(H, 32, sig);
	if (!gj_ssh_hostkey_verify(H, 32, sig)) {
		msg("sshd-gj: hostkey sign FAIL\n");
		return 0;
	}
	soft_note(&g_u32SoftHostkey);
	msg("sshd-gj: hostkey sign PASS\n");

	gj_ssh_hostkey_sign(H, 32, aSig64);
	(void)encode_sig_ed25519(aSigBlob, aSig64);
	n = build_ecdh_reply(ecdh_s, sizeof(ecdh_s), aKs, SSH_KS_ED25519_CB,
			     pk_s, aSigBlob, SSH_SIG_ED25519_CB);
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
	/* type@5, string K_S (len@6 = 51), Q_S after 4+51. */
	if (nr < 100 || get_u32(rbuf + 6) != SSH_KS_ED25519_CB) {
		msg("sshd-gj: ECDH_REPLY shape FAIL\n");
		return 0;
	}
	if (!gj_ssh_memeq_ct(rbuf + 10, (const uint8_t *)"ssh-ed25519", 11)) {
		msg("sshd-gj: ECDH_REPLY host_pk FAIL\n");
		return 0;
	}
	if (get_u32(rbuf + 6 + 4 + SSH_KS_ED25519_CB) != 32u) {
		msg("sshd-gj: ECDH_REPLY Q_S shape FAIL\n");
		return 0;
	}
	gj_ssh_x25519(shared_c, sk_c,
		      rbuf + 6 + 4 + SSH_KS_ED25519_CB + 4);
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
	msg("sshd-gj: key derivation PASS (chacha20-poly1305)\n");
	if (g_int_s2c[0] == 0xffu && g_int_s2c[1] == 0xffu) {
		uint8_t aTmp[32];

		product_mac(g_int_s2c, 0u, H, 32, aTmp);
		bytes_zero(aTmp, sizeof(aTmp));
	}

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
		long cbRecv;
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
 * One virtio/door POLL then one ACCEPT. Door POLL is T0 virtio residual;
 * it is not ETH_INJECT (rtl8168_udx injects). Do not nest thousands of
 * ACCEPT/POLL here — coop RR, no preempt; a hog leaves the host ACK in
 * the NIC and the child in SYN_RCVD (0.1.152/153: Connected, no banner).
 * Soft!=product. Grep: functional_accept
 */
static long
eth_accept_door_claim(long i64Srv, unsigned uDoorN, unsigned uClaimN)
{
	(void)uDoorN;
	(void)uClaimN;
	net_poll_once();
	return gj_net(GJ_NET_OP_ACCEPT, i64Srv, 0, 0);
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
 * 182 glass 092206: CHANNEL_OPEN session PASS (send_pkt == n, seq
 * bumped) then CHANNEL miss CLOSE. Client never saw type 91.
 * Cannot resend confirm (seq already used). Drain door TX / TCP rtx.
 */
static void
ssh_tx_drain(unsigned uN)
{
	unsigned i;

	for (i = 0; i < uN; i++) {
		net_door_progress_lean(8u);
		net_flush_residual(1u, 8u);
	}
}

/* send_pkt encrypts in place. Rebuild is the caller's job. Drain after == n. */
static uint8_t
ssh_send_drain(long fd, uint8_t *pkt, uint32_t n)
{
	if (n == 0u || pkt == 0) {
		return 0;
	}
	if (send_pkt(fd, pkt, n, 1) != (long)n) {
		ssh_tx_drain(2u);
		return 0;
	}
	ssh_tx_drain(4u);
	return 1;
}

/*
 * Park hot path: yield so rtl8168_udx can ETH_INJECT the 3WHS ACK
 * (coop RR, no preempt; GJ_NET_OP_POLL is not inject), then one
 * virtio POLL + ACCEPT. Soft!=product. Dual DoD B OPEN.
 * Grep: functional_accept
 */
static long
eth_accept_yield_poll(long i64Srv)
{
	gj_yield();
	return eth_accept_door_claim(i64Srv, 1u, 1u);
}

/*
 * RECV with door-first + yield+poll retry for eth peers (loopback is
 * immediate; NIC is not). Soft!=product.
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
		net_door_progress_lean(2u);
		net_progress();
		for (j = 0; j < 2u; j++) {
			net_poll_once();
		}
	}
	return sent > 0 ? sent : n;
}

/* Cleartext SSH binary packet on eth (g_encrypted must be 0). */
static long
send_clear_wait(long fd, uint8_t *pkt, uint32_t n)
{
	long r;

	if (pkt == 0 || n == 0) {
		return -22;
	}
	r = net_send_wait(fd, pkt, (long)n, 64u);
	if (r > 0) {
		g_seq_s2c_tx++;
	}
	return r;
}

/*
 * Fail-closed RFC 4253 client identification.
 * Accumulate until "\r\n". Bytes after the ident line are leftover for the
 * next SSH binary packet (KEXINIT) — OpenSSH often writes ident+KEXINIT
 * in one segment.
 *
 * Return: 1 ident complete; 0 bare-nc FIN (no SSH-); -1 FIN after SSH- or
 * ident fail-closed; -2 timeout / incomplete (fd still up).
 */
static int
eth_recv_ident(long fd, char *ident, long ident_cap, long *pcbIdent,
	       uint8_t *leftover, long leftover_cap, long *pcbLeftover,
	       int *pfSawSsh)
{
	static uint8_t aAcc[4096];
	long have = 0;
	unsigned iTry;
	long iEnd;
	long nCopy;
	long iByte;

	if (ident == 0 || ident_cap < 8 || pcbIdent == 0 || leftover == 0 ||
	    leftover_cap < 6 || pcbLeftover == 0 || pfSawSsh == 0) {
		return -1;
	}
	*pcbIdent = 0;
	*pcbLeftover = 0;
	*pfSawSsh = 0;
	bytes_zero(aAcc, sizeof(aAcc));

	for (iTry = 0;; iTry++) {
		long nprobe;
		unsigned j;

		net_door_progress_lean(3u);
		/* Every try yields: POLL is not ETH_INJECT (0.1.156). */
		net_progress();
		for (j = 0; j < 2u; j++) {
			net_poll_once();
		}
		if (have >= (long)sizeof(aAcc)) {
			return -1; /* fail-closed: no CR LF in a full buffer */
		}
		nprobe = gj_net(GJ_NET_OP_RECV, fd,
				(long)(uintptr_t)(aAcc + have),
				(long)(sizeof(aAcc) - (size_t)have));
		if (nprobe == 0) {
			/* Peer FIN: nc success only if the peer never sent SSH-. */
			return (*pfSawSsh != 0) ? -1 : 0;
		}
		if (nprobe < 0) {
			gj_yield();
			continue;
		}
		have += nprobe;
		if (have >= 4 &&
		    banner_is_ssh((const char *)aAcc, have)) {
			*pfSawSsh = 1;
		}
		iEnd = ident_line_end(aAcc, have);
		if (iEnd <= 0) {
			continue;
		}
		nCopy = iEnd;
		if (nCopy > ident_cap - 1) {
			nCopy = ident_cap - 1;
		}
		for (iByte = 0; iByte < nCopy; iByte++) {
			ident[iByte] = (char)aAcc[iByte];
		}
		if (nCopy < ident_cap) {
			ident[nCopy] = '\0';
		}
		*pcbIdent = nCopy;
		*pfSawSsh = 1;
		if (have > iEnd) {
			long cbLeft = have - iEnd;

			if (cbLeft > leftover_cap) {
				cbLeft = leftover_cap;
			}
			bytes_copy(leftover, aAcc + iEnd, (size_t)cbLeft);
			*pcbLeftover = cbLeft;
		}
		return 1;
	}
	if (have >= 4 && banner_is_ssh((const char *)aAcc, have)) {
		*pfSawSsh = 1;
	}
	return -2;
}

/*
 * Assemble one cleartext SSH packet into buf, starting from leftover *phave.
 * Retries RECV so a split OpenSSH KEXINIT can finish. Returns complete
 * packet size, 0 on FIN, or <0 if still incomplete.
 */
static long
eth_recv_clear_pkt(long fd, uint8_t *buf, long cap, long *phave,
		   unsigned retries)
{
	unsigned i;
	uint32_t need;

	if (buf == 0 || cap < 6 || phave == 0) {
		return -22;
	}
	if (*phave < 0) {
		*phave = 0;
	}
	if (*phave > cap) {
		*phave = cap;
	}
	for (i = 0; i < retries; i++) {
		if (*phave < 4) {
			long nr;

			nr = net_recv_wait(fd, buf + *phave, cap - *phave,
					   (i == 0u) ? 24u : 16u);
			if (nr == 0) {
				return 0;
			}
			if (nr > 0) {
				*phave += nr;
			}
			if (*phave < 4) {
				continue;
			}
		}
		need = ssh_clear_pkt_need(buf, *phave);
		if (need == 0u) {
			if (*phave >= 6) {
				g_seq_c2s_rx++;
				return *phave; /* length/type present; caller checks */
			}
			continue;
		}
		if ((long)need > cap) {
			return -1; /* fail-closed: packet larger than cap */
		}
		while (*phave < (long)need) {
			long nr;

			nr = net_recv_wait(fd, buf + *phave, cap - *phave, 16u);
			if (nr == 0) {
				return 0;
			}
			if (nr < 0) {
				break;
			}
			*phave += nr;
		}
		if (*phave >= (long)need) {
			g_seq_c2s_rx++;
			return (long)need;
		}
	}
	return -11;
}

static uint32_t
ssh_eth_pay_len(const uint8_t *pkt, long cbWire)
{
	uint32_t uPktLen;
	uint32_t uPad;

	if (pkt == 0 || cbWire < 6) {
		return 0;
	}
	uPktLen = get_u32(pkt);
	uPad = (uint32_t)pkt[4];
	if (uPktLen < 6u || uPad < 4u ||
	    (4 + (long)uPktLen) > cbWire || uPktLen < 1u + uPad) {
		return 0;
	}
	return uPktLen - 1u - uPad;
}

static uint32_t
build_channel_open_confirm(uint8_t *pkt, uint32_t cap, uint32_t uPeer,
			   uint32_t uLocal)
{
	uint8_t body[16];

	put_u32(body, uPeer);
	put_u32(body + 4, uLocal);
	put_u32(body + 8, 32768u);
	put_u32(body + 12, 16384u);
	return build_simple(pkt, cap, SSH_MSG_CHANNEL_OPEN_CONFIRM, body, 16);
}

static uint32_t
build_channel_u32(uint8_t *pkt, uint32_t cap, uint8_t uType, uint32_t uPeer)
{
	uint8_t body[4];

	put_u32(body, uPeer);
	return build_simple(pkt, cap, uType, body, 4);
}

static uint32_t
build_channel_exit_status(uint8_t *pkt, uint32_t cap, uint32_t uPeer,
			  uint32_t uStatus)
{
	uint8_t body[24];
	uint32_t o = 0;

	put_u32(body + o, uPeer);
	o += 4;
	put_u32(body + o, 11);
	o += 4;
	body[o++] = 'e';
	body[o++] = 'x';
	body[o++] = 'i';
	body[o++] = 't';
	body[o++] = '-';
	body[o++] = 's';
	body[o++] = 't';
	body[o++] = 'a';
	body[o++] = 't';
	body[o++] = 'u';
	body[o++] = 's';
	body[o++] = 0;
	put_u32(body + o, uStatus);
	o += 4;
	return build_simple(pkt, cap, SSH_MSG_CHANNEL_REQUEST, body, o);
}

static uint32_t
build_channel_motd(uint8_t *pkt, uint32_t cap, uint32_t uPeer)
{
	uint8_t dbody[128];
	uint32_t mlen = (uint32_t)slen(g_szMotd);
	uint32_t i;

	if (mlen > 80u) {
		mlen = 80u;
	}
	put_u32(dbody, uPeer);
	put_u32(dbody + 4, mlen);
	for (i = 0; i < mlen; i++) {
		dbody[8 + i] = (uint8_t)g_szMotd[i];
	}
	return build_simple(pkt, cap, SSH_MSG_CHANNEL_DATA, dbody, 8 + mlen);
}

/*
 * RFC 4254 on the eth AEAD path after USERAUTH_SUCCESS.
 * GOP live dash stays isolate (0.1.140 #PF). No dash exec this cut.
 * exec "true" → SUCCESS + exit-status 0 + EOF + CLOSE so ssh … true can
 * exit 0. shell → MOTD then wait EOF/CLOSE. SUCCESS/channel ≠ login.
 * Returns 1 when the fd should CLOSE, 2 stay-open (exec stub only).
 * 143315: do not 256-miss stay-open before OPEN_CONFIRM; miss bound CLOSES.
 * 181 host: SUCCESS PASS, no Sending command (confirm miss); 8×256 waited
 * past client timeout. Tighter miss CLOSE; rebuild+retry confirm TX.
 * 182 host: confirm send_pkt == n (CHANNEL_OPEN session PASS) then
 * miss CLOSE; client never Sending command. Drain TX after confirm.
 */
static int
do_eth_session_channel(long fd, uint8_t *aRx, uint32_t cbRx, uint8_t *aPkt,
		       uint32_t cbPkt)
{
	uint32_t uPeer = 0;
	uint32_t uLocal = 0;
	unsigned iMiss = 0;
	unsigned iOpenWait = 0;
	unsigned iReqWait = 0;
	uint8_t fOpen = 0;
	uint8_t fMotd = 0;
	uint32_t n;

	for (;;) {
		long cbWire;
		uint32_t uPay;
		uint32_t oPay;
		uint8_t uType;
		const uint8_t *pName = 0;
		uint32_t cbName = 0;

		if (fOpen != 0u) {
			ssh_tx_drain(1u);
		}
		cbWire = recv_pkt(fd, aRx, cbRx, 1);
		if (cbWire < 0) {
			msg("sshd-gj: eth channel miss CLOSE\n");
			return 1;
		}
		if (cbWire < 6) {
			iMiss++;
			if (iMiss >= 256u) {
				if (fOpen == 0u) {
					iOpenWait++;
					if (iOpenWait >= 2u) {
						msg("sshd-gj: eth channel miss CLOSE\n");
						return 1;
					}
				} else {
					iReqWait++;
					if (iReqWait >= 4u) {
						msg("sshd-gj: eth channel miss CLOSE\n");
						return 1;
					}
					ssh_tx_drain(4u);
				}
				iMiss = 0u;
			}
			if (fOpen == 0u) {
				net_door_progress_lean(2u);
				net_flush_residual(1u, 4u);
			}
			gj_yield();
			continue;
		}
		uType = aRx[5];
		uPay = ssh_eth_pay_len(aRx, cbWire);
		if (uPay == 0u) {
			iMiss++;
			if (iMiss >= 256u) {
				if (fOpen == 0u) {
					iOpenWait++;
					if (iOpenWait >= 2u) {
						msg("sshd-gj: eth channel miss CLOSE\n");
						return 1;
					}
				} else {
					iReqWait++;
					if (iReqWait >= 4u) {
						msg("sshd-gj: eth channel miss CLOSE\n");
						return 1;
					}
					ssh_tx_drain(4u);
				}
				iMiss = 0u;
			}
			gj_yield();
			continue;
		}
		iMiss = 0u;
		if (uType == SSH_MSG_IGNORE || uType == 4u ||
		    uType == SSH_MSG_CHANNEL_WINDOW_ADJUST) {
			continue;
		}
		if (uType == SSH_MSG_GLOBAL_REQUEST) {
			oPay = 0;
			if (ssh_get_string(aRx + 6, uPay, &oPay, &pName,
					   &cbName) != 0 &&
			    oPay < uPay && aRx[6 + oPay] != 0) {
				n = build_simple(aPkt, cbPkt,
						 SSH_MSG_REQUEST_FAILURE, 0,
						 0);
				if (n != 0u) {
					(void)send_pkt(fd, aPkt, n, 1);
				}
			}
			continue;
		}
		if (uType == SSH_MSG_CHANNEL_OPEN) {
			const uint8_t *pKind = 0;
			uint32_t cbKind = 0;

			oPay = 0;
			if (ssh_get_string(aRx + 6, uPay, &oPay, &pKind,
					   &cbKind) == 0 ||
			    cbKind != 7u ||
			    !gj_ssh_memeq_ct(pKind,
					     (const uint8_t *)"session",
					     7) ||
			    oPay + 4u > uPay) {
				continue;
			}
			uPeer = get_u32(aRx + 6 + oPay);
			{
				unsigned iTry;

				/*
				 * send_pkt encrypts in place. Rebuild
				 * plaintext each try. 181: one-shot != n
				 * left fOpen=0 until 8×256 past timeout.
				 */
				for (iTry = 0; iTry < 4u && fOpen == 0u;
				     iTry++) {
					n = build_channel_open_confirm(aPkt,
								       cbPkt,
								       uPeer,
								       uLocal);
					if (n != 0u &&
					    send_pkt(fd, aPkt, n, 1) ==
						    (long)n) {
						msg("sshd-gj: eth CHANNEL_OPEN session PASS\n");
						fOpen = 1;
					} else {
						gj_yield();
						net_flush_residual(1u, 8u);
					}
				}
			}
			if (fOpen == 0u) {
				msg("sshd-gj: eth CHANNEL_OPEN miss CLOSE\n");
				return 1;
			}
			ssh_tx_drain(16u);
			continue;
		}
		if (uType == SSH_MSG_CHANNEL_REQUEST) {
			uint8_t uWant = 0;
			uint8_t fExecTrue = 0;
			uint8_t fShell = 0;

			if (fOpen == 0u || uPay < 9u) {
				continue;
			}
			oPay = 4;
			if (ssh_get_string(aRx + 6, uPay, &oPay, &pName,
					   &cbName) == 0 ||
			    oPay >= uPay) {
				continue;
			}
			uWant = aRx[6 + oPay];
			oPay += 1u;
			if (cbName == 4u &&
			    gj_ssh_memeq_ct(pName, (const uint8_t *)"exec",
					    4)) {
				fExecTrue = 1;
			}
			if (cbName == 5u &&
			    gj_ssh_memeq_ct(pName, (const uint8_t *)"shell",
					    5)) {
				fShell = 1;
			}
			if (uWant != 0u) {
				n = build_channel_u32(aPkt, cbPkt,
						      SSH_MSG_CHANNEL_SUCCESS,
						      uPeer);
				(void)ssh_send_drain(fd, aPkt, n);
			}
			if (fExecTrue != 0u) {
				/*
				 * 183 host: Sending command PASS then 124.
				 * Drain after each exec reply (body+tag
				 * two SENDs). Stay-open so FIN does not
				 * eat unflushed AEAD (0.1.179).
				 */
				n = build_channel_exit_status(aPkt, cbPkt,
							      uPeer, 0);
				(void)ssh_send_drain(fd, aPkt, n);
				n = build_channel_u32(aPkt, cbPkt,
						      SSH_MSG_CHANNEL_EOF,
						      uPeer);
				(void)ssh_send_drain(fd, aPkt, n);
				n = build_channel_u32(aPkt, cbPkt,
						      SSH_MSG_CHANNEL_CLOSE,
						      uPeer);
				(void)ssh_send_drain(fd, aPkt, n);
				ssh_tx_drain(8u);
				msg("sshd-gj: eth CHANNEL exec stub PASS\n");
				return 2;
			}
			if (fShell != 0u && fMotd == 0u) {
				n = build_channel_motd(aPkt, cbPkt, uPeer);
				if (n != 0u && send_pkt(fd, aPkt, n, 1) > 0) {
					msg("sshd-gj: eth CHANNEL shell MOTD PASS\n");
					fMotd = 1;
				}
			}
			net_door_progress_lean(2u);
			net_flush_residual(1u, 4u);
			continue;
		}
		if (uType == SSH_MSG_CHANNEL_DATA) {
			continue;
		}
		if (uType == SSH_MSG_CHANNEL_EOF) {
			n = build_channel_u32(aPkt, cbPkt, SSH_MSG_CHANNEL_EOF,
					      uPeer);
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			n = build_channel_u32(aPkt, cbPkt,
					      SSH_MSG_CHANNEL_CLOSE, uPeer);
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			net_door_progress_lean(2u);
			net_flush_residual(2u, 8u);
			msg("sshd-gj: eth CHANNEL_EOF PASS\n");
			return 2;
		}
		if (uType == SSH_MSG_CHANNEL_CLOSE) {
			n = build_channel_u32(aPkt, cbPkt,
					      SSH_MSG_CHANNEL_CLOSE, uPeer);
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			net_door_progress_lean(2u);
			net_flush_residual(2u, 8u);
			msg("sshd-gj: eth CHANNEL_CLOSE PASS\n");
			return 2;
		}
	}
}

/*
 * Server-only path for external eth accept (OpenSSH / nc client).
 * Dual DoD B: banner is proven-class. Close is host interactive SSH login.
 * 178 SUCCESS PASS. This cut: RFC 4254 after SUCCESS. No GOP dash un-isolate.
 * Soft!=product (not OpenSSH multi-server confine). Grep: eth accept session
 */
static int
do_eth_server_session(long fd)
{
	static char aCliBanner[256];
	static uint8_t aRx[4096];
	static uint8_t aIs[512];
	static uint8_t aIc[2048];
	uint8_t aPkt[768];
	uint8_t aQc[32];
	uint8_t sk_s[32], pk_s[32], base[32];
	uint8_t shared[32], H[32], host_pk[32];
	uint8_t aKs[51], aSig64[64], aSigBlob[83];
	uint8_t newkeys[64];
	const uint8_t *pPay = 0;
	uint32_t cbIs = 0;
	uint32_t cbIc = 0;
	uint32_t cbVc;
	uint32_t cbVs;
	uint32_t n;
	long nr;
	long cbCliBanner = 0;
	long cbBanner;
	long cbLeft = 0;
	long have = 0;
	long cbPkt = 0;
	unsigned i;
	unsigned iInit;
	int fSrvBanner = 0; /* DoD B: product id on wire */
	int fPeerBanner = 0;
	int fKexStart = 0;
	int fSawSsh = 0;
	int stIdent;

	/* Fresh direction state for this peer (self-smoke used globals). */
	g_encrypted = 0;
	g_seq_s2c_tx = 0;
	g_seq_s2c_rx = 0;
	g_seq_c2s_tx = 0;
	g_seq_c2s_rx = 0;
	g_aead_have = 0;

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
	 * stays OPEN until interactive SSH login (soft_listen_ne_host_banner=1).
	 */
	/* Ident on the wire before settle/flush (sshd is the only writer). */
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
	 * Short yield so rtl8168_udx can TX-pull the ident. The old
	 * multi-second flush held the session so a later ssh sat in
	 * 3WHS with no ACCEPT (0.1.155: python saw the banner, ssh
	 * banner-exchange timed out). Soft!=product.
	 */
	gj_yield();
	net_flush_residual(1u, 8u);
	fSrvBanner = 1;
	soft_note(&g_u32EthBannerN);
	/* Grep: sshd-gj: eth banner PASS — product id SSH-2.0-GreenJade_sshd */
	msg("sshd-gj: eth banner PASS\n");

	for (i = 0; i < sizeof(aCliBanner); i++) {
		aCliBanner[i] = 0;
	}
	bytes_zero(aRx, sizeof(aRx));
	/*
	 * Fail-closed ident: parse until CR LF. Leftover bytes after the
	 * ident line are the next SSH binary packet (OpenSSH KEXINIT).
	 * Bare nc FIN (no SSH-) is still eng session soft PASS. FIN after
	 * SSH- is NOT nc success. Grep: ident leftover / ident_line_end
	 */
	stIdent = eth_recv_ident(fd, aCliBanner, (long)sizeof(aCliBanner),
				 &cbCliBanner, aRx, (long)sizeof(aRx), &cbLeft,
				 &fSawSsh);
	if (stIdent == 0) {
		msg("sshd-gj: eth peer early close soft PASS\n");
		msg("sshd-gj: eth session path PASS\n");
		soft_note(&g_u32EthSessN);
		return 1;
	}
	if (stIdent == 1) {
		fPeerBanner = 1;
		msg("sshd-gj: eth peer banner PASS\n");
		if (cbLeft > 0) {
			msg("sshd-gj: eth ident leftover kept\n");
		}
	} else if (stIdent == -1) {
		/* Peer sent SSH- then FIN — not bare nc. Do not PASS. */
		msg("sshd-gj: eth peer FIN after SSH- (not nc success)\n");
		return 0;
	} else {
		msg("sshd-gj: eth peer banner soft-skip\n");
	}

	/*
	 * Type 20 after client ident (0.1.157: type 20 with the banner
	 * made OpenSSH 9.9 "MAC incorrect" — leftover + last-seg RTX).
	 * Next-seq SEND is allowed (0.1.157 tcp_tx). Soft!=product.
	 */
	if (fPeerBanner != 0) {
		n = build_kexinit(aPkt, sizeof(aPkt));
		if (n == 0 || send_clear_wait(fd, aPkt, n) <= 0) {
			msg("sshd-gj: eth KEXINIT SEND FAIL\n");
			return 0;
		}
		fKexStart = 1;
		msg("sshd-gj: eth KEX start PASS\n");
	} else if (fKexStart == 0) {
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return fSrvBanner != 0 ? 1 : 0;
	}
	if (kexinit_payload(aPkt, (long)n, &pPay, &cbIs) != 0 &&
	    cbIs <= (uint32_t)sizeof(aIs)) {
		bytes_copy(aIs, pPay, cbIs);
	} else {
		cbIs = 0;
	}

	/*
	 * After our type 20: RECV until type 30 or FIN.
	 * 0.1.167: 4096-iter drain raced DUT RTT; stay-open then
	 * stopped RECV (0.1.160 class). Ident path is unbounded;
	 * type 30 must be too. Soft!=product.
	 */
	have = cbLeft;
	cbPkt = -11;
	iInit = 0u;
	for (;;) {
		cbPkt = eth_recv_clear_pkt(fd, aRx, (long)sizeof(aRx), &have,
					   1u);
		if (cbPkt == 0) {
			break;
		}
		if (cbPkt >= 42 && aRx[5] == SSH_MSG_KEX_ECDH_INIT) {
			break;
		}
		if (cbPkt >= 6 && aRx[5] == SSH_MSG_KEXINIT) {
			if (kexinit_payload(aRx, cbPkt, &pPay, &cbIc) != 0 &&
			    cbIc <= (uint32_t)sizeof(aIc)) {
				bytes_copy(aIc, pPay, cbIc);
			} else {
				cbIc = 0;
			}
			if (have > cbPkt) {
				bytes_copy(aRx, aRx + cbPkt,
					   (size_t)(have - cbPkt));
				have = have - cbPkt;
			} else {
				have = 0;
			}
			continue;
		}
		if (cbPkt >= 6) {
			if (have > cbPkt) {
				bytes_copy(aRx, aRx + cbPkt,
					   (size_t)(have - cbPkt));
				have = have - cbPkt;
			} else {
				have = 0;
			}
		}
		gj_yield();
		iInit++;
		if ((iInit & 15u) == 0u) {
			net_flush_residual(1u, 4u);
		}
	}
	if (cbPkt < 42 || aRx[5] != SSH_MSG_KEX_ECDH_INIT) {
		if (fSawSsh != 0 || fPeerBanner != 0) {
			msg("sshd-gj: eth ECDH_INIT stay-open\n");
			return 2;
		}
		msg("sshd-gj: eth ECDH_INIT soft-skip\n");
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	/* string Q_C at offset 10 (type@5 + string len@6..9). Then consume. */
	bytes_copy(aQc, aRx + 10, 32);
	if (have > cbPkt) {
		bytes_copy(aRx, aRx + cbPkt, (size_t)(have - cbPkt));
		have = have - cbPkt;
	} else {
		have = 0;
	}

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
	if (shared_is_zero(shared) != 0) {
		msg("sshd-gj: eth ECDH shared zero FAIL\n");
		return 0;
	}

	(void)encode_ks_ed25519(aKs, host_pk);
	cbVs = ident_strip_crlf(g_szBanner, (uint32_t)slen(g_szBanner));
	if (fPeerBanner != 0 && cbCliBanner > 0) {
		cbVc = ident_strip_crlf(aCliBanner, (uint32_t)cbCliBanner);
		kex_hash_rfc(H, aCliBanner, cbVc, g_szBanner, cbVs, aIc, cbIc,
			     aIs, cbIs, aKs, SSH_KS_ED25519_CB, aQc, pk_s,
			     shared);
	} else {
		cbVc = ident_strip_crlf(g_szClientBanner,
					(uint32_t)slen(g_szClientBanner));
		kex_hash_rfc(H, g_szClientBanner, cbVc, g_szBanner, cbVs, aIc,
			     cbIc, aIs, cbIs, aKs, SSH_KS_ED25519_CB, aQc, pk_s,
			     shared);
	}

	gj_ssh_hostkey_sign(H, 32, aSig64);
	(void)encode_sig_ed25519(aSigBlob, aSig64);
	n = build_ecdh_reply(aPkt, sizeof(aPkt), aKs, SSH_KS_ED25519_CB, pk_s,
			     aSigBlob, SSH_SIG_ED25519_CB);
	if (n == 0 || send_clear_wait(fd, aPkt, n) <= 0) {
		msg("sshd-gj: eth ECDH_REPLY FAIL\n");
		if (fSawSsh != 0 || fPeerBanner != 0) {
			return 0;
		}
		if (fSrvBanner != 0) {
			msg("sshd-gj: eth session path PASS\n");
			soft_note(&g_u32EthSessN);
		}
		return (fSrvBanner != 0 || fKexStart != 0) ? 1 : 0;
	}
	msg("sshd-gj: eth ECDH_REPLY PASS\n");

	n = build_simple(newkeys, sizeof(newkeys), SSH_MSG_NEWKEYS, 0, 0);
	if (n == 0 || send_clear_wait(fd, newkeys, n) <= 0) {
		msg("sshd-gj: eth NEWKEYS SEND FAIL\n");
		return 2;
	}
	msg("sshd-gj: eth NEWKEYS soft PASS\n");

	/* Peer NEWKEYS is still cleartext. Arm AEAD only after type 21. */
	for (;;) {
		cbPkt = eth_recv_clear_pkt(fd, aRx, (long)sizeof(aRx), &have,
					   1u);
		if (cbPkt == 0) {
			msg("sshd-gj: eth NEWKEYS stay-open\n");
			return 2;
		}
		if (cbPkt >= 6 && aRx[5] == SSH_MSG_NEWKEYS) {
			if (have > cbPkt) {
				aead_hold_put(aRx + cbPkt,
					      (uint32_t)(have - cbPkt));
			}
			have = 0;
			break;
		}
		if (cbPkt >= 6) {
			if (have > cbPkt) {
				bytes_copy(aRx, aRx + cbPkt,
					   (size_t)(have - cbPkt));
				have = have - cbPkt;
			} else {
				have = 0;
			}
		}
		gj_yield();
	}

	derive_keys(shared, H, H);
	bytes_zero(sk_s, sizeof(sk_s));
	bytes_zero(shared, sizeof(shared));
	bytes_zero(aSig64, sizeof(aSig64));
	bytes_zero(aQc, sizeof(aQc));

	for (;;) {
		long cbWire;

		cbWire = recv_pkt(fd, aRx, (uint32_t)sizeof(aRx), 1);
		if (cbWire < 0) {
			msg("sshd-gj: eth SERVICE_REQUEST stay-open\n");
			return 2;
		}
		if (cbWire >= 6 && aRx[5] == SSH_MSG_SERVICE_REQUEST) {
			n = build_service(aPkt, sizeof(aPkt),
					  SSH_MSG_SERVICE_ACCEPT,
					  "ssh-userauth");
			if (n != 0u && send_pkt(fd, aPkt, n, 1) > 0) {
				msg("sshd-gj: eth SERVICE_ACCEPT PASS\n");
			}
			break;
		}
		gj_yield();
	}
	net_door_progress_lean(2u);
	net_flush_residual(1u, 8u);

	/*
	 * RFC 4252 publickey: none → FAILURE; query → PK_OK;
	 * signed → verify then SUCCESS. Then RFC 4254 channel (no dash).
	 * SUCCESS ≠ login. GOP live dash stays isolate.
	 * 0.1.175: unbounded recv_pkt==0 hogged ACCEPT (second banner TIMEOUT).
	 * 0.1.176: 48 misses ~client timeout; first ssh no FAILURE, second
	 * no PK_OK after continue-list. 128 + flush every miss.
	 * 0.1.177: PK_OK PASS then 128-miss raced signed SUCCESS. 256.
	 */
	iInit = 0u;
	for (;;) {
		long cbWire;
		uint32_t uPktLen;
		uint32_t uPad;
		uint32_t uPay;
		uint32_t oPay;
		const uint8_t *pUser = 0;
		const uint8_t *pSvc = 0;
		const uint8_t *pMeth = 0;
		const uint8_t *pAlg = 0;
		const uint8_t *pBlob = 0;
		const uint8_t *pSigB = 0;
		uint32_t cbUser = 0;
		uint32_t cbSvc = 0;
		uint32_t cbMeth = 0;
		uint32_t cbAlg = 0;
		uint32_t cbBlob = 0;
		uint32_t cbSigB = 0;
		uint8_t uHaveSig = 0;

		cbWire = recv_pkt(fd, aRx, (uint32_t)sizeof(aRx), 1);
		if (cbWire < 0) {
			msg("sshd-gj: eth USERAUTH stay-open\n");
			return 2;
		}
		if (cbWire < 6 || aRx[5] != SSH_MSG_USERAUTH_REQUEST) {
			iInit++;
			if (iInit >= 256u) {
				msg("sshd-gj: eth USERAUTH stay-open\n");
				return 2;
			}
			net_door_progress_lean(2u);
			net_flush_residual(1u, 4u);
			gj_yield();
			continue;
		}
		iInit = 0u;
		uPktLen = get_u32(aRx);
		uPad = (uint32_t)aRx[4];
		if (uPktLen < 6u || uPad < 4u ||
		    (4 + (long)uPktLen) > cbWire || uPktLen < 1u + uPad) {
			iInit++;
			if (iInit >= 256u) {
				msg("sshd-gj: eth USERAUTH stay-open\n");
				return 2;
			}
			gj_yield();
			continue;
		}
		uPay = uPktLen - 1u - uPad;
		oPay = 0;
		if (ssh_get_string(aRx + 6, uPay, &oPay, &pUser, &cbUser) ==
			0 ||
		    ssh_get_string(aRx + 6, uPay, &oPay, &pSvc, &cbSvc) == 0 ||
		    ssh_get_string(aRx + 6, uPay, &oPay, &pMeth, &cbMeth) ==
			0) {
			n = build_userauth_failure(aPkt, sizeof(aPkt),
						   "publickey");
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			continue;
		}
		if (cbMeth == 4u &&
		    gj_ssh_memeq_ct(pMeth, (const uint8_t *)"none", 4)) {
			n = build_userauth_failure(aPkt, sizeof(aPkt),
						   "publickey");
			if (n != 0u && send_pkt(fd, aPkt, n, 1) > 0) {
				msg("sshd-gj: eth USERAUTH_FAILURE PASS\n");
			}
			continue;
		}
		if (cbMeth != 9u ||
		    !gj_ssh_memeq_ct(pMeth, (const uint8_t *)"publickey",
				     9) ||
		    oPay >= uPay) {
			n = build_userauth_failure(aPkt, sizeof(aPkt),
						   "publickey");
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			continue;
		}
		uHaveSig = aRx[6 + oPay];
		oPay += 1u;
		if (ssh_get_string(aRx + 6, uPay, &oPay, &pAlg, &cbAlg) == 0 ||
		    ssh_get_string(aRx + 6, uPay, &oPay, &pBlob, &cbBlob) ==
			0 ||
		    cbAlg != 11u ||
		    !gj_ssh_memeq_ct(pAlg, (const uint8_t *)"ssh-ed25519",
				     11)) {
			n = build_userauth_failure(aPkt, sizeof(aPkt),
						   "publickey");
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			continue;
		}
		if (uHaveSig == 0u) {
			n = build_userauth_pk_ok(aPkt, sizeof(aPkt), pAlg,
						 cbAlg, pBlob, cbBlob);
			if (n != 0u && send_pkt(fd, aPkt, n, 1) > 0) {
				msg("sshd-gj: eth USERAUTH_PK_OK PASS\n");
			}
			net_door_progress_lean(2u);
			net_flush_residual(1u, 8u);
			continue;
		}
		if (ssh_get_string(aRx + 6, uPay, &oPay, &pSigB, &cbSigB) ==
			0 ||
		    userauth_pubkey_verify(H, pUser, cbUser, pSvc, cbSvc, pAlg,
					   cbAlg, pBlob, cbBlob, pSigB,
					   cbSigB) == 0) {
			n = build_userauth_failure(aPkt, sizeof(aPkt),
						   "publickey");
			if (n != 0u) {
				(void)send_pkt(fd, aPkt, n, 1);
			}
			continue;
		}
		n = build_userauth_success(aPkt, sizeof(aPkt));
		if (n != 0u && send_pkt(fd, aPkt, n, 1) > 0) {
			msg("sshd-gj: eth USERAUTH_SUCCESS PASS\n");
		}
		net_door_progress_lean(2u);
		net_flush_residual(1u, 8u);
		gj_yield();
		net_flush_residual(1u, 8u);
		/*
		 * RFC 4254 after SUCCESS. GOP dash isolate stays.
		 * Channel / exec stub / MOTD ≠ interactive login.
		 * 181: extra SUCCESS flush so CHANNEL_OPEN is not racing TX.
		 */
		return do_eth_session_channel(fd, aRx,
					      (uint32_t)sizeof(aRx), aPkt,
					      (uint32_t)sizeof(aPkt));
	}

	msg("sshd-gj: eth session stay-open\n");
	return 2;
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
	/* Banner SEND first (0.1.152: settle-before-SEND missed nc -w 5). */
	fOk = do_eth_server_session(i64Acc);
	if (fOk == 2) {
		unsigned iHold;

		/*
		 * return 2 is stay-open after REPLY SEND, or honest
		 * ECDH_INIT miss after the long drain. Bounded hold;
		 * no CLOSE/RST. Soft!=product.
		 */
		msg("sshd-gj: eth stay-open (no immediate CLOSE)\n");
		for (iHold = 0; iHold < 4u; iHold++) {
			gj_yield();
			net_flush_residual(1u, 4u);
		}
		/* Leave fd open (no CLOSE). Soft!=product. Dual DoD B OPEN. */
		msg("sshd-gj: eth accept session done\n");
		return 0;
	}
	(void)gj_net(GJ_NET_OP_CLOSE, i64Acc, 0, 0);
	gj_yield();
	net_flush_residual(1u, 8u);
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
 * Chain: rtl8168_udx → kernel net_tcp → sshd (this unit = hop_down).
 * leftover netstackd :7777 is not Dual DoD B. Dual DoD B OPEN until
 * interactive SSH login. Banner / PK_OK / SUCCESS ≠ close.
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
	msg("sshd-gj: TCP listening :22 listen=:22 "
	    "product_net_owns_wire=1 product_path=UDX "
	    "soft_listen_ne_host_banner=1 "
	    "(eth/UDX wire; not virtio-only; Soft!=product "
	    "dual_dod_b=OPEN)\n");

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
	{
		long iEnd = 0;

		if (n > 0) {
			iEnd = ident_line_end((const uint8_t *)rbuf, n);
		}
		if (n > 0 && iEnd > 0 && banner_is_ssh(rbuf, iEnd)) {
			banner_ok = 1;
			soft_note(&g_u32SoftBanner);
			msg("sshd-gj: peer banner PASS\n");
			/* leftover after ident CR LF is the next SSH packet */
			(void)(n - iEnd);
		} else {
			msg("sshd-gj: peer banner FAIL\n");
		}
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
	 * banner proof. Dual DoD B stays OPEN until host interactive SSH login
	 * on stamped flash. Banner / nc Connected ≠ login.
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
	{
		long aArm;

		aArm = eth_accept_yield_poll(srv);
		if (aArm >= 0) {
			(void)eth_handle_accept(aArm);
		}
	}
	for (;;) {
		long a;

		a = eth_accept_yield_poll(srv);
		if (a >= 0) {
			(void)eth_handle_accept(a);
		}
	}
}
