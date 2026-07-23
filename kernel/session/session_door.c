/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session door: compositor + input hub + ownership hand-off for sessiond.
 *
 * Opcodes (GJ_SESS_OP_*): present/display, input poll/pop, stats, map
 * scanout, present user FB, claim/release ownership.
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means a
 * userspace sessiond claimed the door. Claim is re-entrant for the same
 * token (idempotent), BUSY for a different token.
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 */
#include <gj/compositor.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/session_door.h>
#include <gj/session_input.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_gpu.h>

/* Match compositor interim clamp (PRESENT_FB / aTmp fallback). */
#define GJ_SESS_MAX_DIM 256u
#define GJ_SESS_TMP_W   64u
#define GJ_SESS_TMP_H   64u

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32UserPresents;

void
session_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32OwnerToken = 0;
    g_u32UserPresents = 0;
    kprintf("session_door: init (present+input+claim)\n");
}

int
session_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
session_door_owner_token(void)
{
    return g_u32OwnerToken;
}

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
sess_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
sess_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

/**
 * Blit user BGRA tile into scanout top-left with independent strides.
 * @u64Src is user/kernel base; @u32SrcStride / @u32DstStride in bytes.
 * Copies @u32CopyW * 4 bytes per row for @u32CopyH rows.
 */
static i64
sess_blit_fb(u8 *pDst, u32 u32DstStride, u64 u64Src, u32 u32SrcStride,
             u32 u32CopyW, u32 u32CopyH)
{
    u32 iRow;
    u32 cbRow = u32CopyW * 4u;
    i64 st;

    if (pDst == NULL || u64Src == 0 || u32CopyW == 0 || u32CopyH == 0 ||
        cbRow == 0 || u32SrcStride < cbRow || u32DstStride < cbRow) {
        return GJ_ERR_INVAL;
    }

    for (iRow = 0; iRow < u32CopyH; iRow++) {
        u64 u64Row = u64Src + (u64)iRow * (u64)u32SrcStride;
        u8 *pRow = pDst + (u64)iRow * (u64)u32DstStride;

        /* Overflow / wrap guards for adversarial huge strides. */
        if (u64Row < u64Src) {
            return GJ_ERR_INVAL;
        }
        st = sess_copy_in(pRow, u64Row, cbRow);
        if (st != 0) {
            return st;
        }
    }
    return 0;
}

i64
session_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        return GJ_ERR_NODEV;
    }
    g_u32Calls++;

    switch (u32Op) {
    case GJ_SESS_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            return GJ_ERR_INVAL;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            return GJ_ERR_BUSY; /* another sessiond */
        }
        g_u32OwnerToken = (u32)u64Arg1;
        kprintf("session_door: CLAIM token=0x%x (userspace owns scanout)\n",
                g_u32OwnerToken);
        return 0;

    case GJ_SESS_OP_RELEASE:
        /* arg1 must match claim token when owned. */
        if (g_u32OwnerToken == 0) {
            return 0; /* already free */
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            return GJ_ERR_INVAL;
        }
        kprintf("session_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        return 0;

    case GJ_SESS_OP_PRESENT:
        /*
         * Present kernel interim scanout. When claimed, sessiond prefers
         * PRESENT_FB; this path remains for bring-up tools.
         */
        if (!session_compositor_ready()) {
            return GJ_ERR_NODEV;
        }
        return session_compositor_present() == 0 ? 0 : GJ_ERR_IO;

    case GJ_SESS_OP_DISPLAY_INFO: {
        /* arg1 → u32[2] {w, h} */
        u32 aWH[2];
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aWH[0] = 0;
        aWH[1] = 0;
        session_compositor_size(&aWH[0], &aWH[1]);
        st = sess_copy_out(u64Arg1, aWH, sizeof(aWH));
        return st;
    }

    case GJ_SESS_OP_INPUT_POLL:
        session_input_poll();
        return 0;

    case GJ_SESS_OP_INPUT_POP: {
        /* arg1 → struct gj_input_event; ret 1 if event, 0 if empty. */
        struct gj_input_event ev;
        int fGot;
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        memset(&ev, 0, sizeof(ev));
        fGot = session_input_pop(&ev);
        if (!fGot) {
            return 0;
        }
        st = sess_copy_out(u64Arg1, &ev, sizeof(ev));
        if (st != 0) {
            return st;
        }
        return 1;
    }

    case GJ_SESS_OP_STATS: {
        /*
         * arg1 → u32[5]:
         *   [0] compositor presents
         *   [1] input events pushed (lifetime)
         *   [2] door call count
         *   [3] flags: bit0 ready, bit1 input ready, bit2 owned,
         *              bits 8..15 pending input (capped 255)
         *   [4] owner token
         * Wire size stays 5 for sessiond / smoke ABI stability.
         * g_u32UserPresents and session_input_dropped() are retained
         * for kernel diagnostics (pending packed into flags).
         */
        u32 aSt[5];
        u32 u32Pend;
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        u32Pend = session_input_pending();
        if (u32Pend > 255u) {
            u32Pend = 255u;
        }
        aSt[0] = session_compositor_present_count();
        aSt[1] = session_input_pushed();
        aSt[2] = g_u32Calls;
        aSt[3] = (session_compositor_ready() ? 1u : 0u) |
                 (session_input_ready() ? 2u : 0u) |
                 (g_u32OwnerToken != 0 ? 4u : 0u) |
                 (u32Pend << 8) |
                 /* bit16: any input drop observed (sticky observability) */
                 (session_input_dropped() != 0 ? (1u << 16) : 0u) |
                 /* bit17: any user PRESENT_FB success */
                 (g_u32UserPresents != 0 ? (1u << 17) : 0u);
        aSt[4] = g_u32OwnerToken;
        st = sess_copy_out(u64Arg1, aSt, sizeof(aSt));
        return st;
    }

    case GJ_SESS_OP_MAP_SCANOUT: {
        /*
         * arg1 → u64 VA hint of kernel scanout (interim; not a true map).
         * arg2 → u32[3] {w, h, stride_bytes}
         */
        u64 u64Va;
        u32 aInfo[3];
        u32 u32W = 0;
        u32 u32H = 0;
        u32 u32Stride;
        void *pFb;
        i64 st;

        if (u64Arg1 == 0 || u64Arg2 == 0) {
            return GJ_ERR_INVAL;
        }
        if (!session_compositor_ready()) {
            return GJ_ERR_NODEV;
        }
        pFb = session_compositor_fb();
        if (pFb == NULL) {
            return GJ_ERR_NODEV;
        }
        session_compositor_size(&u32W, &u32H);
        u32Stride = session_compositor_stride();
        if (u32W == 0 || u32H == 0 || u32Stride == 0) {
            return GJ_ERR_NODEV;
        }
        u64Va = (u64)(uintptr_t)pFb;
        aInfo[0] = u32W;
        aInfo[1] = u32H;
        aInfo[2] = u32Stride;
        st = sess_copy_out(u64Arg1, &u64Va, sizeof(u64Va));
        if (st != 0) {
            return st;
        }
        st = sess_copy_out(u64Arg2, aInfo, sizeof(aInfo));
        return st;
    }

    case GJ_SESS_OP_PRESENT_FB: {
        /*
         * arg1=w arg2=h arg3=BGRA pixel ptr (stride = w*4).
         * When compositor ready: blit top-left into scanout with correct
         * source/dest strides, then present. Else direct virtio-gpu present
         * with a small static temp for user buffers.
         */
        u32 u32ReqW = (u32)u64Arg1;
        u32 u32ReqH = (u32)u64Arg2;
        u32 u32SrcStride;
        u32 u32Bytes;
        u8 *pDst;
        u32 u32Cw = 0;
        u32 u32Ch = 0;
        u32 u32DstStride;
        u32 u32CopyW;
        u32 u32CopyH;
        i64 st;

        if (u64Arg3 == 0 || u32ReqW == 0 || u32ReqH == 0 ||
            u32ReqW > GJ_SESS_MAX_DIM || u32ReqH > GJ_SESS_MAX_DIM) {
            return GJ_ERR_INVAL;
        }
        /* Reject tokens that look like truncated wide values on w/h. */
        if ((u64Arg1 >> 32) != 0 || (u64Arg2 >> 32) != 0) {
            return GJ_ERR_INVAL;
        }

        u32SrcStride = u32ReqW * 4u;
        u32Bytes = u32SrcStride * u32ReqH;
        if (u32Bytes / u32SrcStride != u32ReqH) {
            return GJ_ERR_INVAL; /* overflow guard (defensive) */
        }

        if (!session_compositor_ready()) {
            if (!virtio_gpu_ready()) {
                return GJ_ERR_NODEV;
            }
            if (user_range_ok(u64Arg3, u32Bytes)) {
                static u8 aTmp[GJ_SESS_TMP_W * GJ_SESS_TMP_H * 4u];

                if (u32Bytes > sizeof(aTmp)) {
                    return GJ_ERR_INVAL;
                }
                if (copy_from_user(aTmp, u64Arg3, u32Bytes) != GJ_OK) {
                    return GJ_ERR_FAULT;
                }
                if (virtio_gpu_present(u32ReqW, u32ReqH, aTmp,
                                       u32SrcStride) != 0) {
                    return GJ_ERR_IO;
                }
            } else if (virtio_gpu_present(u32ReqW, u32ReqH,
                                          (void *)(gj_vaddr_t)u64Arg3,
                                          u32SrcStride) != 0) {
                return GJ_ERR_IO;
            }
            g_u32UserPresents++;
            return 0;
        }

        session_compositor_size(&u32Cw, &u32Ch);
        u32DstStride = session_compositor_stride();
        pDst = (u8 *)session_compositor_fb();
        if (pDst == NULL || u32Cw == 0 || u32Ch == 0 || u32DstStride == 0) {
            return GJ_ERR_NODEV;
        }
        /* Clip to scanout; never write past the allocated tile. */
        u32CopyW = u32ReqW;
        u32CopyH = u32ReqH;
        if (u32CopyW > u32Cw) {
            u32CopyW = u32Cw;
        }
        if (u32CopyH > u32Ch) {
            u32CopyH = u32Ch;
        }
        if (u32CopyW == 0 || u32CopyH == 0) {
            return GJ_ERR_INVAL;
        }
        st = sess_blit_fb(pDst, u32DstStride, u64Arg3, u32SrcStride, u32CopyW,
                          u32CopyH);
        if (st != 0) {
            return st;
        }
        if (session_compositor_present() != 0) {
            return GJ_ERR_IO;
        }
        g_u32UserPresents++;
        return 0;
    }

    default:
        return GJ_ERR_NOSUPPORT;
    }
}
