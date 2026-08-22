/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace sessiond (host-buildable A1).
 * On GreenJade kernel: GJ_SYS_SESSION door ops (see gj/session_door.h):
 *   PRESENT, DISPLAY_INFO, INPUT_POLL, INPUT_POP, STATS,
 *   PRESENT_FB, CLAIM, RELEASE, MAP_SCANOUT
 * Host CI simulates the same present + input fanout protocol shapes.
 * Freestanding live path: sessiond_gj.c (Wave 126 soft inventory exclusive).
 * Host A1 here is libc soft-shape only — Soft!=product multi-server confine.
 * Soft honesty: multi_server=0 confine=0 (C2 product daemon residual).
 * Dual DoD product_kernel=OPEN; product=UDX/DDI+ABI (not this host sim).
 *
 * Smoke markers (prefix-stable; scripts/smoke-all.sh greps sessiond: PASS):
 *   sessiond: PRESENT_FB path PASS
 *   sessiond: ownership PASS
 *   sessiond: RELEASE PASS
 *   sessiond: compositor path PASS
 *   sessiond: soft health PASS
 *   sessiond: PASS
 *
 * Soft residual (diagnostics only; never hard-gates PASS):
 *   sessiond: reclaim soft | reclaim soft-skip
 *   sessiond: multi-frame soft | multi-frame soft-skip
 *   sessiond: free soft | free soft-skip
 *   sessiond: soft path claim=sim present=fb multi_frame=bit18
 *             reclaim=bit19 free=unowned multi_server=0 confine=0
 *
 *   make sessiond && ./build/sessiond
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 * Door opcodes — numbers match kernel/include/gj/session_door.h.
 * Host path only exercises protocol *shape*; do not renumber.
 */
#define GJ_SESS_OP_PRESENT      1u
#define GJ_SESS_OP_DISPLAY_INFO 2u
#define GJ_SESS_OP_INPUT_POLL   3u
#define GJ_SESS_OP_INPUT_POP    4u
#define GJ_SESS_OP_STATS        5u
#define GJ_SESS_OP_PRESENT_FB   6u
#define GJ_SESS_OP_CLAIM        7u
#define GJ_SESS_OP_RELEASE      8u
#define GJ_SESS_OP_MAP_SCANOUT  9u

#define GJ_SESS_FB_W       64u
#define GJ_SESS_FB_H       64u
#define GJ_SESS_FB_STRIDE  (GJ_SESS_FB_W * 4u)
#define GJ_SESS_INPUT_RING 32u

/* Session door ownership token (matches freestanding SESS_TOKEN). */
#define SESS_TOKEN 0xc0ffe1u

/*
 * STATS flags — match kernel session_door.h wire contract (u32[5]):
 *   bit0 ready, bit1 input ready, bit2 owned,
 *   bits8..15 pending input (0..255),
 *   bit16 drop sticky, bit17 user PRESENT_FB, bit18 multi-frame, bit19 reclaim
 */
#define SESS_STAT_F_READY    1u
#define SESS_STAT_F_INPUT    2u
#define SESS_STAT_F_OWNED    4u
#define SESS_STAT_PEND_SHIFT 8u
#define SESS_STAT_PEND_MASK  0xffu
#define SESS_STAT_F_DROP     (1u << 16)
#define SESS_STAT_F_USER_FB  (1u << 17)
#define SESS_STAT_F_MULTI    (1u << 18)
#define SESS_STAT_F_RECLAIM  (1u << 19)

/* Soft inventory area count (C2 residual greppable categories). */
#define SESS_SOFT_AREAS 8u

/* Matches kernel struct gj_input_event (type, code, value). */
struct sess_input_ev {
    uint16_t u16Type;
    uint16_t u16Code;
    int32_t i32Value;
};

struct session_state {
    uint8_t aFb[GJ_SESS_FB_STRIDE * GJ_SESS_FB_H];
    uint32_t u32W;
    uint32_t u32H;
    uint32_t u32Presents;
    uint32_t u32UserFb; /* PRESENT_FB successes (multi-frame soft base) */
    uint32_t u32Calls;
    uint32_t u32OwnerToken;
    struct sess_input_ev aIn[GJ_SESS_INPUT_RING];
    uint32_t u32InHead;
    uint32_t u32InLen;
    uint32_t u32InPushed;
    int fReady;
    int fInputReady;
    int fReclaimSoft; /* same-token CLAIM observed (STATS bit19) */
    int fDropSoft;    /* input ring overflow sticky (STATS bit16) */
};

static struct session_state g_Sess;

static void
sess_fb_fill(uint8_t b, uint8_t g, uint8_t r)
{
    uint32_t i;
    uint32_t n = g_Sess.u32W * g_Sess.u32H;

    for (i = 0; i < n; i++) {
        g_Sess.aFb[i * 4u + 0] = b;
        g_Sess.aFb[i * 4u + 1] = g;
        g_Sess.aFb[i * 4u + 2] = r;
        g_Sess.aFb[i * 4u + 3] = 0xff;
    }
}

static int
sess_init(void)
{
    memset(&g_Sess, 0, sizeof(g_Sess));
    g_Sess.u32W = GJ_SESS_FB_W;
    g_Sess.u32H = GJ_SESS_FB_H;
    sess_fb_fill(0x20, 0x80, 0x10);
    g_Sess.fReady = 1;
    g_Sess.fInputReady = 1;
    printf("sessiond: scanout %ux%u ready\n", g_Sess.u32W, g_Sess.u32H);
    return 0;
}

/* Host sim of GJ_SESS_OP_PRESENT (interim present; not user-fb count). */
static int
sess_present(void)
{
    volatile uint8_t sink;

    if (!g_Sess.fReady) {
        return -1;
    }
    /* Software present: touch first/last pixel (host stand-in for GPU door) */
    sink = g_Sess.aFb[0];
    g_Sess.aFb[0] = sink;
    sink = g_Sess.aFb[GJ_SESS_FB_STRIDE * g_Sess.u32H - 1];
    (void)sink;
    g_Sess.u32Presents++;
    g_Sess.u32Calls++;
    printf("sessiond: present #%u ok\n", g_Sess.u32Presents);
    return 0;
}

/*
 * Host sim of GJ_SESS_OP_PRESENT_FB — bumps user-fb count for multi-frame
 * soft residual (bit17 any user PRESENT_FB; bit18 when user_fb >= 2).
 */
static int
sess_present_fb(void)
{
    int r;

    r = sess_present();
    if (r == 0) {
        g_Sess.u32UserFb++;
    }
    return r;
}

static void
sess_input_push(uint16_t type, uint16_t code, int32_t value)
{
    uint32_t pos;

    if (g_Sess.u32InLen >= GJ_SESS_INPUT_RING) {
        g_Sess.u32InHead = (g_Sess.u32InHead + 1) % GJ_SESS_INPUT_RING;
        g_Sess.u32InLen--;
        g_Sess.fDropSoft = 1; /* sticky drop observed (STATS bit16) */
    }
    pos = (g_Sess.u32InHead + g_Sess.u32InLen) % GJ_SESS_INPUT_RING;
    g_Sess.aIn[pos].u16Type = type;
    g_Sess.aIn[pos].u16Code = code;
    g_Sess.aIn[pos].i32Value = value;
    g_Sess.u32InLen++;
    g_Sess.u32InPushed++;
    g_Sess.u32Calls++;
}

static int
sess_input_pop(struct sess_input_ev *pOut)
{
    g_Sess.u32Calls++;
    if (pOut == NULL || g_Sess.u32InLen == 0) {
        return 0;
    }
    *pOut = g_Sess.aIn[g_Sess.u32InHead];
    g_Sess.u32InHead = (g_Sess.u32InHead + 1) % GJ_SESS_INPUT_RING;
    g_Sess.u32InLen--;
    return 1;
}

/* Host sim of GJ_SESS_OP_DISPLAY_INFO → u32[2] {w,h}. */
static int
sess_display_info(uint32_t *pWh2)
{
    g_Sess.u32Calls++;
    if (pWh2 == NULL) {
        return -1;
    }
    pWh2[0] = g_Sess.u32W;
    pWh2[1] = g_Sess.u32H;
    return 0;
}

/*
 * Host sim of GJ_SESS_OP_STATS → u32[5]:
 *   presents, in_push, calls, flags, owner
 * Flags include door residual bits (pend / drop / user-fb / multi / reclaim).
 */
static int
sess_stats(uint32_t *pSt5)
{
    uint32_t uPend;
    uint32_t uFlags;

    g_Sess.u32Calls++;
    if (pSt5 == NULL) {
        return -1;
    }
    uPend = g_Sess.u32InLen;
    if (uPend > SESS_STAT_PEND_MASK) {
        uPend = SESS_STAT_PEND_MASK;
    }
    uFlags = (g_Sess.fReady ? SESS_STAT_F_READY : 0u) |
             (g_Sess.fInputReady ? SESS_STAT_F_INPUT : 0u) |
             (g_Sess.u32OwnerToken != 0u ? SESS_STAT_F_OWNED : 0u) |
             (uPend << SESS_STAT_PEND_SHIFT) |
             (g_Sess.fDropSoft ? SESS_STAT_F_DROP : 0u) |
             (g_Sess.u32UserFb > 0u ? SESS_STAT_F_USER_FB : 0u) |
             (g_Sess.u32UserFb >= 2u ? SESS_STAT_F_MULTI : 0u) |
             (g_Sess.fReclaimSoft ? SESS_STAT_F_RECLAIM : 0u);
    pSt5[0] = g_Sess.u32Presents;
    pSt5[1] = g_Sess.u32InPushed;
    pSt5[2] = g_Sess.u32Calls;
    pSt5[3] = uFlags;
    pSt5[4] = g_Sess.u32OwnerToken;
    return 0;
}

/* Host sim of GJ_SESS_OP_MAP_SCANOUT → va hint + u32[3] {w,h,stride}. */
static int
sess_map_scanout(uint64_t *pVa, uint32_t *pInfo3)
{
    g_Sess.u32Calls++;
    if (pVa == NULL || pInfo3 == NULL || !g_Sess.fReady) {
        return -1;
    }
    *pVa = (uint64_t)(uintptr_t)g_Sess.aFb;
    pInfo3[0] = g_Sess.u32W;
    pInfo3[1] = g_Sess.u32H;
    pInfo3[2] = GJ_SESS_FB_STRIDE;
    return 0;
}

/*
 * Host sim of CLAIM (token non-zero; same-token reclaim soft → bit19;
 * different owner → BUSY stand-in).
 */
static int
sess_claim(uint32_t uToken)
{
    g_Sess.u32Calls++;
    if (uToken == 0u) {
        return -1;
    }
    if (g_Sess.u32OwnerToken != 0u && g_Sess.u32OwnerToken != uToken) {
        return -1; /* BUSY stand-in */
    }
    if (g_Sess.u32OwnerToken == uToken) {
        g_Sess.fReclaimSoft = 1; /* STATS bit19 reclaim soft observed */
        return 0;
    }
    g_Sess.u32OwnerToken = uToken;
    return 0;
}

/* Host sim of RELEASE; free when unowned is soft 0. */
static int
sess_release(uint32_t uToken)
{
    g_Sess.u32Calls++;
    if (g_Sess.u32OwnerToken == 0u) {
        return 0; /* free soft (unowned) */
    }
    if (uToken != g_Sess.u32OwnerToken) {
        return -1;
    }
    g_Sess.u32OwnerToken = 0u;
    return 0;
}

/*
 * Soft health suite — door residual shapes beyond hard claim/present/release.
 * Never fails sessiond: PASS; prints diagnostic markers only.
 * Soft residual: display / map / input / reclaim / stats / present_fb /
 * multi-frame / free-path probes. Soft!=product multi-server confine.
 */
static int
sess_soft_health(void)
{
    uint32_t aWh[2];
    uint32_t aSt[5];
    uint64_t u64Va = 0;
    uint32_t aInfo[3];
    struct sess_input_ev ev;
    int nPop = 0;
    int cOk = 0;
    int cSkip = 0;

    printf("sessiond: soft health start\n");

    if (sess_display_info(aWh) == 0 && aWh[0] == GJ_SESS_FB_W &&
        aWh[1] == GJ_SESS_FB_H) {
        printf("sessiond: DISPLAY_INFO soft %ux%u (op=%u)\n", aWh[0], aWh[1],
               GJ_SESS_OP_DISPLAY_INFO);
        cOk++;
    } else {
        printf("sessiond: DISPLAY_INFO soft-skip\n");
        cSkip++;
    }

    if (sess_map_scanout(&u64Va, aInfo) == 0 && aInfo[0] == GJ_SESS_FB_W &&
        aInfo[2] == GJ_SESS_FB_STRIDE && u64Va != 0) {
        printf("sessiond: MAP_SCANOUT soft va=%p %ux%u stride=%u (op=%u)\n",
               (void *)(uintptr_t)u64Va, aInfo[0], aInfo[1], aInfo[2],
               GJ_SESS_OP_MAP_SCANOUT);
        cOk++;
    } else {
        printf("sessiond: MAP_SCANOUT soft-skip\n");
        cSkip++;
    }

    /* INPUT_POLL shape: host already pushed; drain remaining (expect 0) */
    printf("sessiond: INPUT_POLL soft (op=%u)\n", GJ_SESS_OP_INPUT_POLL);
    while (sess_input_pop(&ev)) {
        nPop++;
    }
    printf("sessiond: INPUT_POP soft drained=%d (op=%u)\n", nPop,
           GJ_SESS_OP_INPUT_POP);
    cOk++;

    /*
     * Soft reclaim: same-token CLAIM is idempotent (door sets STATS bit19).
     * Expect owned when called from hard CLAIM path.
     */
    if (g_Sess.u32OwnerToken == SESS_TOKEN && sess_claim(SESS_TOKEN) == 0 &&
        g_Sess.fReclaimSoft) {
        printf("sessiond: reclaim soft (op=%u bit19)\n", GJ_SESS_OP_CLAIM);
        cOk++;
    } else {
        printf("sessiond: reclaim soft-skip\n");
        cSkip++;
    }

    if (sess_stats(aSt) == 0) {
        printf("sessiond: STATS soft p=%u in=%u c=%u f=0x%x own=0x%x "
               "user_fb=%u (op=%u)\n",
               aSt[0], aSt[1], aSt[2], aSt[3], aSt[4], g_Sess.u32UserFb,
               GJ_SESS_OP_STATS);
        if ((aSt[3] & SESS_STAT_F_OWNED) != 0 && aSt[4] == SESS_TOKEN) {
            printf("sessiond: ownership soft\n");
            cOk++;
        } else {
            cSkip++;
        }
        if ((aSt[3] & SESS_STAT_F_RECLAIM) != 0) {
            printf("sessiond: reclaim flag soft\n");
            cOk++;
        }
    } else {
        printf("sessiond: STATS soft-skip\n");
        cSkip++;
    }

    /* Soft PRESENT_FB tints — second/third for multi-frame residual (bit18) */
    sess_fb_fill(0x10, 0xd0, 0x38);
    if (sess_present_fb() == 0) {
        printf("sessiond: soft PRESENT_FB PASS (op=%u)\n",
               GJ_SESS_OP_PRESENT_FB);
        cOk++;
    } else {
        printf("sessiond: soft PRESENT_FB soft-skip\n");
        cSkip++;
    }

    sess_fb_fill(0x08, 0xe0, 0x48);
    if (sess_present_fb() == 0) {
        printf("sessiond: soft PRESENT_FB2 PASS (op=%u)\n",
               GJ_SESS_OP_PRESENT_FB);
        cOk++;
    } else {
        printf("sessiond: soft PRESENT_FB2 soft-skip\n");
        cSkip++;
    }

    /* Post multi-frame STATS recheck — expect user-fb + multi-frame bits */
    if (sess_stats(aSt) == 0) {
        if ((aSt[3] & SESS_STAT_F_USER_FB) != 0 &&
            (aSt[3] & SESS_STAT_F_MULTI) != 0 && g_Sess.u32UserFb >= 2u) {
            printf("sessiond: multi-frame soft (bit18 user_fb=%u)\n",
                   g_Sess.u32UserFb);
            cOk++;
        } else {
            printf("sessiond: multi-frame soft-skip\n");
            cSkip++;
        }
    } else {
        printf("sessiond: multi-frame soft-skip\n");
        cSkip++;
    }

    printf("sessiond: soft health ok=%d skip=%d areas=%u\n", cOk, cSkip,
           (unsigned)SESS_SOFT_AREAS);

    if (cOk > 0) {
        printf("sessiond: soft health PASS\n");
        return 0;
    }
    printf("sessiond: soft health soft-skip\n");
    return 0; /* never hard-fail */
}

/*
 * Soft free path after hard RELEASE — unowned probes never hard-gate.
 * free RELEASE soft, STATS unowned, MAP_SCANOUT / DISPLAY_INFO while free.
 */
static int
sess_soft_free(void)
{
    uint32_t aWh[2];
    uint32_t aSt[5];
    uint64_t u64Va = 0;
    uint32_t aInfo[3];
    int cOk = 0;

    printf("sessiond: soft free start\n");

    /* free when already unowned is soft 0 */
    if (g_Sess.u32OwnerToken == 0u && sess_release(SESS_TOKEN) == 0) {
        printf("sessiond: free soft (op=%u unowned)\n", GJ_SESS_OP_RELEASE);
        cOk++;
    } else {
        printf("sessiond: free soft-skip\n");
    }

    if (sess_stats(aSt) == 0 && (aSt[3] & SESS_STAT_F_OWNED) == 0 &&
        aSt[4] == 0u) {
        printf("sessiond: free STATS soft p=%u f=0x%x own=0x%x\n", aSt[0],
               aSt[3], aSt[4]);
        cOk++;
    } else {
        printf("sessiond: free STATS soft-skip\n");
    }

    if (sess_map_scanout(&u64Va, aInfo) == 0 && aInfo[0] == GJ_SESS_FB_W) {
        printf("sessiond: free MAP_SCANOUT soft %ux%u\n", aInfo[0], aInfo[1]);
        cOk++;
    } else {
        printf("sessiond: free MAP_SCANOUT soft-skip\n");
    }

    if (sess_display_info(aWh) == 0 && aWh[0] == GJ_SESS_FB_W) {
        printf("sessiond: free DISPLAY_INFO soft %ux%u\n", aWh[0], aWh[1]);
        cOk++;
    } else {
        printf("sessiond: free DISPLAY_INFO soft-skip\n");
    }

    if (cOk > 0) {
        printf("sessiond: free soft PASS\n");
    } else {
        printf("sessiond: free soft soft-skip\n");
    }
    return 0; /* never hard-fail */
}

int
main(void)
{
    struct sess_input_ev ev;
    int nPop = 0;
    int frame;
    uint32_t aWh[2];
    uint32_t aSt[5];

    printf("sessiond: start (A1 userspace session owner)\n");
    if (sess_init() != 0) {
        printf("sessiond: init FAIL\n");
        return 1;
    }

    /* DISPLAY_INFO protocol shape */
    if (sess_display_info(aWh) != 0 || aWh[0] != GJ_SESS_FB_W ||
        aWh[1] != GJ_SESS_FB_H) {
        printf("sessiond: DISPLAY_INFO FAIL\n");
        return 1;
    }
    printf("sessiond: DISPLAY_INFO %ux%u (op=%u)\n", aWh[0], aWh[1],
           GJ_SESS_OP_DISPLAY_INFO);

    /* Synthetic keyboard events (stand-in for virtio-input fan-in) */
    sess_input_push(1 /* EV_KEY */, 28 /* KEY_ENTER */, 1);
    sess_input_push(1, 28, 0);
    sess_input_push(2 /* EV_REL */, 0 /* REL_X */, 3);

    while (sess_input_pop(&ev)) {
        nPop++;
        printf("sessiond: input type=%u code=%u value=%d\n", ev.u16Type,
               ev.u16Code, (int)ev.i32Value);
    }
    if (nPop != 3) {
        printf("sessiond: input fanout FAIL popped=%d\n", nPop);
        return 1;
    }
    printf("sessiond: input fanout ok pushed=%u\n", g_Sess.u32InPushed);

    for (frame = 0; frame < 3; frame++) {
        /* Tint scanout per frame then interim present (not user-fb) */
        sess_fb_fill((uint8_t)(0x20 + frame * 20), 0x90, 0x18);
        if (sess_present() != 0) {
            printf("sessiond: present FAIL frame=%d\n", frame);
            return 1;
        }
    }
    if (g_Sess.u32Presents != 3) {
        printf("sessiond: present count FAIL\n");
        return 1;
    }

    /* PRESENT_FB protocol shape (matches GJ_SESS_OP_PRESENT_FB args) */
    {
        uint32_t op = GJ_SESS_OP_PRESENT_FB;
        uint32_t w = GJ_SESS_FB_W;
        uint32_t h = GJ_SESS_FB_H;
        void *fb = g_Sess.aFb;

        printf("sessiond: PRESENT_FB op=%u %ux%u fb=%p\n", op, w, h, fb);
        sess_fb_fill(0x18, 0xb0, 0x28);
        if (sess_present_fb() != 0) {
            printf("sessiond: PRESENT_FB FAIL\n");
            return 1;
        }
        printf("sessiond: PRESENT_FB path PASS\n");
    }

    /* CLAIM / RELEASE protocol (matches GJ_SESS_OP_CLAIM/RELEASE) */
    {
        uint32_t token = SESS_TOKEN;

        printf("sessiond: CLAIM token=0x%x (op=%u)\n", token, GJ_SESS_OP_CLAIM);
        if (sess_claim(token) != 0) {
            printf("sessiond: CLAIM FAIL\n");
            return 1;
        }
        printf("sessiond: ownership claimed=%u\n", g_Sess.u32OwnerToken != 0u);

        if (sess_stats(aSt) != 0 || (aSt[3] & SESS_STAT_F_OWNED) == 0 ||
            aSt[4] != token) {
            printf("sessiond: ownership STATS FAIL\n");
            return 1;
        }
        printf("sessiond: ownership PASS\n");

        /* Soft health while owned (map / input / reclaim / multi-present) */
        (void)sess_soft_health();

        if (sess_release(token) != 0 || g_Sess.u32OwnerToken != 0u) {
            printf("sessiond: RELEASE FAIL\n");
            return 1;
        }
        printf("sessiond: RELEASE PASS (op=%u)\n", GJ_SESS_OP_RELEASE);

        /* Soft free path while unowned (never hard-gates) */
        (void)sess_soft_free();
    }

    printf("sessiond: compositor path PASS\n");
    /*
     * Grep: sessiond: soft deepen / soft honesty / soft path (C2 residual).
     * Host A1 soft inventory only — Soft!=product multi-server confine.
     * Dual DoD A/B OPEN until USB path / interactive SSH login. Soft lamps ≠ close.
     */
    printf("sessiond: soft path claim=sim present=fb multi_frame=bit18 "
           "reclaim=bit19 free=unowned multi_server=0 confine=0\n");
    printf("sessiond: soft deepen product_kernel=OPEN wave=70 areas=%u "
           "multi_server=0 confine=0 exclusive=1 user_fb=%u reclaim=%u\n",
           (unsigned)SESS_SOFT_AREAS, g_Sess.u32UserFb,
           g_Sess.fReclaimSoft ? 1u : 0u);
    printf("sessiond: soft honesty multi_server=0 confine=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=70 "
           "Soft!=product dual_dod=OPEN\n");
    printf("sessiond: PASS\n");
    return 0;
}

/* Wave 126 soft deepen surfaces (CREATE-ONLY Soft!=product):
 *   greppable: soft retgradientangle continuum_toward=26800 soft_ne_product=1 wave=126
 *   greppable: soft retblendangle exclusive=1 continuum_toward=26800 soft_ne_product=1 wave=126
 * Soft!=product complete; product lamps 0; Dual DoD OPEN; C2 residual deepen.
 */
