/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-scsi (modern PCI) — pure C, dual license, no GPL.
 * Product SCSI path: control + event + request virtqueues for scsi_mid /
 * scsi_door when virtio-blk is absent. OASIS virtio-scsi; no Linux source.
 */
#pragma once

#include <gj/scsi_mid.h>
#include <gj/types.h>

/*
 * Queue indices (OASIS virtio-scsi fixed layout):
 *   0 control  — TMF / hotplug (reserved; not used on submit path)
 *   1 event    — asynchronous events (one buffer posted at probe)
 *   2 request0 — command submission (blocking poll I/O)
 */
#define GJ_VIRTIO_SCSI_Q_CONTROL 0u
#define GJ_VIRTIO_SCSI_Q_EVENT   1u
#define GJ_VIRTIO_SCSI_Q_REQUEST 2u

/** Max data payload per submit (single-segment bounce). */
#define GJ_VIRTIO_SCSI_DATA_MAX 512u

/** Probe first virtio-scsi; set up ctrl/event/req qs. Returns 0 on success. */
int  virtio_scsi_probe(void);
/** Non-zero when DRIVER_OK and request queue are live. */
int  virtio_scsi_ready(void);

/**
 * Submit one SCSI command on request q2 (blocking used-ring poll).
 * pReq carries CDB, LUN, optional data buffer (data-in or data-out).
 * Returns 0 on transport + SCSI GOOD, -1 on error.
 */
int  virtio_scsi_submit(struct gj_scsi_request *pReq);

/** Completed submit attempts that reached a used-ring completion. */
u32  virtio_scsi_io_count(void);
