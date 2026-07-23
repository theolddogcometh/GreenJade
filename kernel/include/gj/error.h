/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel-native error codes (gj_status_t domain).
 *
 * Scope
 * -----
 * Header-only negative status constants for GreenJade native APIs. Pair with
 * gj_status_t from gj/types.h. No strings, no errno tables, no libc.
 *
 * Contract
 * --------
 *   GJ_OK == 0
 *   All GJ_ERR_* are strictly negative small integers.
 *   Values are stable once shipped in a product ABI surface; append new codes
 *   at the end of the enum, never renumber existing ones.
 *
 * Fail-closed security (docs/SECURITY_CORE_DESIGN.md)
 * ---------------------------------------------------
 * Cap use after revoke / gen bump must never succeed. Prefer the specific
 * codes below over a generic INVAL when the failure class is known:
 *   object DEAD after Phase A     → GJ_ERR_DEAD
 *   slot/object generation mismatch → GJ_ERR_STALE_CAP
 *   rights or authority missing   → GJ_ERR_PERM
 *   peer gone mid-IPC             → GJ_ERR_PEER_DEAD
 *   revoke woken waiter           → GJ_ERR_REVOKED (when not PEER_DEAD)
 *
 * See also docs/CAP_ADDRESSING.md (slot resolve table) and G-Q / quota notes
 * in docs/DESIGN_SPEC_COMPLETE.md (hard GJ_ERR_QUOTA is v1 default).
 *
 * Not Linux errno
 * ---------------
 * These are not POSIX errno values and are not interchangeable with the
 * LINUX_E* / -E* numbers used at Linux personality and door i64 edges.
 * Mapping, if any, is the caller's job at the ABI boundary.
 *
 * Usage
 * -----
 *   gj_status_t st = space_map(...);
 *   if (st != GJ_OK) {
 *       return st; // preserve specific code; do not collapse to -1
 *   }
 */
#pragma once

/* ---- Status codes (gj_status_t) ----------------------------------------- */
/*
 * Unnamed enum: constants are int-compatible with typedef int gj_status_t.
 * Order is historical; comments document intended failure class, not "what
 * the name expands to."
 */
enum {
    /** Success. Only non-negative value in this domain. */
    GJ_OK            =  0,

    /**
     * Invalid argument, bad state for the operation, or malformed handle
     * shape (e.g. null required pointer, zero-length where forbidden, slot
     * index out of range before type/gen checks). Prefer more specific codes
     * when the failure is truly NOENT / STALE_CAP / DEAD / PERM.
     */
    GJ_ERR_INVAL     = -1,

    /**
     * Out of memory: page/frame alloc, heap, CNode slot install, or other
     * kernel resource that is exhausted by size rather than by policy quota.
     * Policy/ledger exhaustion is GJ_ERR_QUOTA.
     */
    GJ_ERR_NOMEM     = -2,

    /**
     * No such object/name/slot content: empty/INVALID CNode slot, missing
     * path/name in VFS-facing paths, or lookup that found nothing live.
     * Distinct from STALE_CAP (gen mismatch on an occupied-looking slot).
     */
    GJ_ERR_NOENT     = -3,

    /**
     * Permission / rights failure: capability rights mask insufficient,
     * caller not authorized for the operation, or policy deny that is not
     * merely "object gone."
     */
    GJ_ERR_PERM      = -4,

    /**
     * Temporary unavailability: try again (would block in a non-blocking
     * API, resource not ready yet, soft contention). Not a permanent busy
     * ownership conflict (that is GJ_ERR_BUSY).
     */
    GJ_ERR_AGAIN     = -5,

    /**
     * Operation or feature not supported on this build, arch, or object
     * type (stub device, unimplemented syscall/door op, wrong cap type for
     * the primitive).
     */
    GJ_ERR_NOSUPPORT = -6,

    /**
     * Resource busy: exclusive owner already held (e.g. single session
     * server, single-flight slot still claimed when the API forbids wait).
     * Contended doors product path blocks rather than spinning; BUSY is for
     * explicit exclusive conflicts.
     */
    GJ_ERR_BUSY      = -7,

    /**
     * Bad address / failed user access: copyin/copyout fault, unmapped VA,
     * pager absence leading to fault failure (G-PTR-3: user fault → error,
     * never kernel panic on untrusted addresses).
     */
    GJ_ERR_FAULT     = -8,

    /**
     * Object is DEAD after revoke Phase A (state DEAD + object gen bumped).
     * Cap ops that resolve the object must return DEAD/STALE and never
     * succeed (SECURITY_CORE_DESIGN S1–S2). Prefer over INVAL when DEAD is
     * known so callers can distinguish revoke from bad arguments.
     */
    GJ_ERR_DEAD      = -9,

    /**
     * Slot or object generation mismatch: user handle gen does not match
     * slot gen, or object gen at install no longer matches. Typical after
     * slot reuse or revoke hygiene (CAP_ADDRESSING resolve table). Fail
     * closed — never treat as success even if pObj still looks non-NULL.
     */
    GJ_ERR_STALE_CAP = -10,

    /**
     * Policy quota exhausted: cap-slot ledger, hierarchical quota, or other
     * accounting limit (G-Q-4 hard fail; not soft jetsam). Distinct from
     * GJ_ERR_NOMEM (physical/allocator exhaustion).
     */
    GJ_ERR_QUOTA     = -11,

    /**
     * Deadline expired: timed wait, door_call_timeout mono deadline, or
     * similar. Caller may retry with a new deadline; object may still be LIVE.
     */
    GJ_ERR_TIMEOUT   = -12,

    /**
     * IPC peer died or object marked peer-dead mid-rendezvous. Waiters must
     * wake with PEER_DEAD (no hang — SECURITY_CORE_DESIGN dead-peer rule).
     * Doors may surface this class as Linux -EIO at the personality edge.
     */
    GJ_ERR_PEER_DEAD = -13,

    /**
     * Waiter/operation aborted because the target was revoked (or revoke
     * path woke waiters) when PEER_DEAD is not the better semantic. Use when
     * the local object/capability path was torn down rather than a remote
     * peer exit alone.
     */
    GJ_ERR_REVOKED   = -14,

    /**
     * No device / not attached: probe miss, session/GPU/input backend absent,
     * or door op that requires a registered device that is not present.
     */
    GJ_ERR_NODEV     = -15,

    /**
     * I/O failure: device or transport error after the target exists (bus
     * error, virtio failure, storage/backend short beyond FAULT/NOENT).
     * Not a substitute for FAULT (address) or PEER_DEAD (IPC peer).
     */
    GJ_ERR_IO        = -16,
};
