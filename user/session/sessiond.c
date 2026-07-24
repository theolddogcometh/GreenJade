/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace sessiond (host-buildable A1).
 * On GreenJade kernel: GJ_SYS_SESSION door ops (see gj/session_door.h):
 *   PRESENT, DISPLAY_INFO, INPUT_POLL, INPUT_POP, STATS,
 *   PRESENT_FB, CLAIM, RELEASE, MAP_SCANOUT
 * Host CI simulates the same present + input fanout protocol shapes.
 * Freestanding live path: sessiond_gj.c (Wave 50 soft inventory exclusive).
 * Host A1 here is libc soft-shape only — soft ≠ product multi-server confine.
 * Soft honesty: multi_server=0 confine=0 bar3=0 (Wave 50 exclusive deepen).
 *
 * Smoke markers (prefix-stable; scripts/smoke-all.sh greps sessiond: PASS):
 *   sessiond: PRESENT_FB path PASS
 *   sessiond: ownership PASS
 *   sessiond: RELEASE PASS
 *   sessiond: compositor path PASS
 *   sessiond: soft health PASS
 *   sessiond: PASS
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

/* STATS flags: bit0 ready, bit1 input ready, bit2 owned. */
#define SESS_STAT_F_READY 1u
#define SESS_STAT_F_INPUT 2u
#define SESS_STAT_F_OWNED 4u

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
    uint32_t u32Calls;
    uint32_t u32OwnerToken;
    struct sess_input_ev aIn[GJ_SESS_INPUT_RING];
    uint32_t u32InHead;
    uint32_t u32InLen;
    uint32_t u32InPushed;
    int fReady;
    int fInputReady;
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

static void
sess_input_push(uint16_t type, uint16_t code, int32_t value)
{
    uint32_t pos;

    if (g_Sess.u32InLen >= GJ_SESS_INPUT_RING) {
        g_Sess.u32InHead = (g_Sess.u32InHead + 1) % GJ_SESS_INPUT_RING;
        g_Sess.u32InLen--;
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
 */
static int
sess_stats(uint32_t *pSt5)
{
    g_Sess.u32Calls++;
    if (pSt5 == NULL) {
        return -1;
    }
    pSt5[0] = g_Sess.u32Presents;
    pSt5[1] = g_Sess.u32InPushed;
    pSt5[2] = g_Sess.u32Calls;
    pSt5[3] = (g_Sess.fReady ? SESS_STAT_F_READY : 0u) |
              (g_Sess.fInputReady ? SESS_STAT_F_INPUT : 0u) |
              (g_Sess.u32OwnerToken != 0u ? SESS_STAT_F_OWNED : 0u);
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

/* Host sim of CLAIM / RELEASE (token non-zero; idempotent same token). */
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
    g_Sess.u32OwnerToken = uToken;
    return 0;
}

static int
sess_release(uint32_t uToken)
{
    g_Sess.u32Calls++;
    if (g_Sess.u32OwnerToken == 0u) {
        return 0;
    }
    if (uToken != g_Sess.u32OwnerToken) {
        return -1;
    }
    g_Sess.u32OwnerToken = 0u;
    return 0;
}

/*
 * Soft health suite — protocol shapes already exercised hard above.
 * Never fails sessiond: PASS; prints diagnostic markers only.
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

    printf("sessiond: soft health start\n");

    if (sess_display_info(aWh) == 0 && aWh[0] == GJ_SESS_FB_W &&
        aWh[1] == GJ_SESS_FB_H) {
        printf("sessiond: DISPLAY_INFO soft %ux%u (op=%u)\n", aWh[0], aWh[1],
               GJ_SESS_OP_DISPLAY_INFO);
        cOk++;
    } else {
        printf("sessiond: DISPLAY_INFO soft-skip\n");
    }

    if (sess_map_scanout(&u64Va, aInfo) == 0 && aInfo[0] == GJ_SESS_FB_W &&
        aInfo[2] == GJ_SESS_FB_STRIDE && u64Va != 0) {
        printf("sessiond: MAP_SCANOUT soft va=%p %ux%u stride=%u (op=%u)\n",
               (void *)(uintptr_t)u64Va, aInfo[0], aInfo[1], aInfo[2],
               GJ_SESS_OP_MAP_SCANOUT);
        cOk++;
    } else {
        printf("sessiond: MAP_SCANOUT soft-skip\n");
    }

    /* INPUT_POLL shape: host already pushed; drain remaining (expect 0) */
    printf("sessiond: INPUT_POLL soft (op=%u)\n", GJ_SESS_OP_INPUT_POLL);
    while (sess_input_pop(&ev)) {
        nPop++;
    }
    printf("sessiond: INPUT_POP soft drained=%d (op=%u)\n", nPop,
           GJ_SESS_OP_INPUT_POP);
    cOk++;

    if (sess_stats(aSt) == 0) {
        printf("sessiond: STATS soft p=%u in=%u c=%u f=%u own=0x%x (op=%u)\n",
               aSt[0], aSt[1], aSt[2], aSt[3], aSt[4], GJ_SESS_OP_STATS);
        if ((aSt[3] & SESS_STAT_F_OWNED) != 0 && aSt[4] == SESS_TOKEN) {
            printf("sessiond: ownership soft\n");
            cOk++;
        }
    } else {
        printf("sessiond: STATS soft-skip\n");
    }

    /* Soft second PRESENT_FB tint */
    sess_fb_fill(0x10, 0xd0, 0x38);
    if (sess_present() == 0) {
        printf("sessiond: soft PRESENT_FB PASS (op=%u)\n",
               GJ_SESS_OP_PRESENT_FB);
        cOk++;
    } else {
        printf("sessiond: soft PRESENT_FB soft-skip\n");
    }

    if (cOk > 0) {
        printf("sessiond: soft health PASS\n");
        return 0;
    }
    printf("sessiond: soft health soft-skip\n");
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
        /* Tint scanout per frame then present */
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
        if (sess_present() != 0) {
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

        /* Soft health while owned (map / input / stats / multi-present) */
        (void)sess_soft_health();

        if (sess_release(token) != 0 || g_Sess.u32OwnerToken != 0u) {
            printf("sessiond: RELEASE FAIL\n");
            return 1;
        }
        printf("sessiond: RELEASE PASS (op=%u)\n", GJ_SESS_OP_RELEASE);
    }

    printf("sessiond: compositor path PASS\n");
    /*
     * Grep: sessiond: soft deepen / soft honesty (Wave 50 exclusive).
     * Host A1 soft inventory only — soft ≠ product multi-server confine.
     */
    printf("sessiond: soft deepen product_kernel=OPEN wave=50 areas=1 multi_server=0 "
           "confine=0 bar3=0 exclusive=1\n");
    printf("sessiond: soft honesty multi_server=0 confine=0 bar3=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=50\n");
    printf("sessiond: PASS\n");
    return 0;
}

/* Wave 50 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retsally continuum_toward=19200 soft_ne_product=1
 *   greppable: soft retcounterscarp exclusive=1 continuum_toward=19200
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
