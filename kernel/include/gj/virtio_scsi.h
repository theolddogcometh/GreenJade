/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-scsi (modern PCI) — pure C, dual license, no GPL.
 * Product SCSI path: control + event + request virtqueues for scsi_mid /
 * scsi_door when virtio-blk is absent. OASIS virtio-scsi; no Linux source.
 *
 * Soft path (bring-up):
 *   - ctrl  q0: TMF submit (live poll) or soft-accept LUN reset/abort
 *   - event q1: non-blocking soft poll + repost; empty when no HBA
 *   - req   q2: residual / fail counters / free-desc soft stats
 */
#pragma once

#include <gj/scsi_mid.h>
#include <gj/types.h>

/*
 * Queue indices (OASIS virtio-scsi fixed layout):
 *   0 control  — TMF / AN (soft poll path)
 *   1 event    — asynchronous events (post + soft poll/repost)
 *   2 request0 — command submission (blocking poll I/O)
 */
#define GJ_VIRTIO_SCSI_Q_CONTROL 0u
#define GJ_VIRTIO_SCSI_Q_EVENT   1u
#define GJ_VIRTIO_SCSI_Q_REQUEST 2u

/** Max data payload per submit (single-segment bounce). */
#define GJ_VIRTIO_SCSI_DATA_MAX 512u

/* Controlq type codes (OASIS public). */
#define GJ_VIRTIO_SCSI_T_TMF          0u
#define GJ_VIRTIO_SCSI_T_AN_QUERY     1u
#define GJ_VIRTIO_SCSI_T_AN_SUBSCRIBE 2u

/* TMF subtypes (control q0 soft path). */
#define GJ_VIRTIO_SCSI_TMF_ABORT_TASK         0u
#define GJ_VIRTIO_SCSI_TMF_ABORT_TASK_SET     1u
#define GJ_VIRTIO_SCSI_TMF_CLEAR_ACA          2u
#define GJ_VIRTIO_SCSI_TMF_CLEAR_TASK_SET     3u
#define GJ_VIRTIO_SCSI_TMF_I_T_NEXUS_RESET    4u
#define GJ_VIRTIO_SCSI_TMF_LOGICAL_UNIT_RESET 5u
#define GJ_VIRTIO_SCSI_TMF_QUERY_TASK         6u
#define GJ_VIRTIO_SCSI_TMF_QUERY_TASK_SET     7u

/* Async event codes (event q1 soft path). */
#define GJ_VIRTIO_SCSI_EVT_MISSED          0x80000000u
#define GJ_VIRTIO_SCSI_EVT_NO_EVENT        0u
#define GJ_VIRTIO_SCSI_EVT_TRANSPORT_RESET 1u
#define GJ_VIRTIO_SCSI_EVT_ASYNC_NOTIFY    2u
#define GJ_VIRTIO_SCSI_EVT_PARAM_CHANGE    3u

/** Soft-parsed event snapshot from q1. */
struct gj_virtio_scsi_event {
    u32 u32Event;  /* GJ_VIRTIO_SCSI_EVT_* (low bits; MISSED may or) */
    u32 u32Reason; /* transport-reset reason or AN/param detail */
    u32 u32Lun;    /* single-level LUN byte (aLun[1]) */
    u32 u32Missed; /* 1 if EVENTS_MISSED bit was set */
};

/**
 * Soft queue stats (ctrl/event/req). Safe when not ready: zeros + soft flag.
 */
struct gj_virtio_scsi_qstats {
    u32 u32Ready;         /* 1 = live HBA DRIVER_OK */
    u32 u32Soft;          /* 1 = soft path armed (no HBA) */
    u32 u32IoOk;          /* request q2 GOOD completions */
    u32 u32IoFail;        /* request q2 errors / timeouts / bad resp */
    u32 u32CtrlOk;        /* control q0 TMF success */
    u32 u32CtrlFail;      /* control q0 TMF reject / timeout */
    u32 u32Events;        /* event q1 soft-polled deliveries */
    u32 u32EventPosted;   /* 1 if an event buffer is posted */
    u16 u16FreeCtrl;      /* free descs on control (0 if soft-only) */
    u16 u16FreeEvent;
    u16 u16FreeReq;
    u16 u16Pad;
    u32 u32LastResponse;  /* last request-q virtio response code */
    u32 u32LastResidual;  /* last request-q residual bytes */
    u32 u32LastScsiStatus;/* last SAM status byte */
};

/** Probe first virtio-scsi; set up ctrl/event/req qs. Returns 0 on success. */
int  virtio_scsi_probe(void);
/** Non-zero when DRIVER_OK and request queue are live. */
int  virtio_scsi_ready(void);
/**
 * Non-zero when soft queue path is armed and no live HBA is preferred.
 * Soft path still answers TMF/stats/event_poll; CDBs stay in scsi_mid soft.
 */
int  virtio_scsi_soft_active(void);

/**
 * Submit one SCSI command on request q2 (blocking used-ring poll).
 * pReq carries CDB, LUN, optional data buffer (data-in or data-out).
 * Returns 0 on transport + SCSI GOOD, -1 on error.
 * Soft-only (no HBA): returns -1 (scsi_mid soft LUN owns CDB path).
 */
int  virtio_scsi_submit(struct gj_scsi_request *pReq);

/** Completed request-q submits that reached used-ring + GOOD. */
u32  virtio_scsi_io_count(void);
/** Request-q failures (arg / add / timeout / response / SCSI status). */
u32  virtio_scsi_fail_count(void);

/**
 * Control q0 soft path: Task Management Function.
 * Live HBA: TMF req/resp on control queue (kick + poll).
 * Soft-only: ABORT_* and *_RESET subtypes succeed as nop; others -1.
 * Returns 0 on success, -1 on error.
 */
int  virtio_scsi_ctrl_tmf(u32 u32Subtype, u32 u32Lun, u64 u64Tag);

/**
 * Event q1 soft path: non-blocking poll.
 * Returns 1 and fills *pOut when an event completed (then reposts buffer),
 * 0 when none pending, -1 on hard error.
 * Soft-only / not ready: always 0.
 */
int  virtio_scsi_event_poll(struct gj_virtio_scsi_event *pOut);

/** Lifetime event deliveries (soft-polled). */
u32  virtio_scsi_event_count(void);
/** Lifetime successful control TMF ops. */
u32  virtio_scsi_ctrl_count(void);

/** Fill soft queue stats; null pOut → -1. Works when soft-only. */
int  virtio_scsi_qstats(struct gj_virtio_scsi_qstats *pOut);

/**
 * Free descriptor count for queue index (0 ctrl / 1 event / 2 req).
 * Soft-only or bad index → 0.
 */
u16  virtio_scsi_q_free(u32 u32QIdx);
