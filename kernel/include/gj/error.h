/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

enum {
    GJ_OK            =  0,
    GJ_ERR_INVAL     = -1,
    GJ_ERR_NOMEM     = -2,
    GJ_ERR_NOENT     = -3,
    GJ_ERR_PERM      = -4,
    GJ_ERR_AGAIN     = -5,
    GJ_ERR_NOSUPPORT = -6,
    GJ_ERR_BUSY      = -7,
    GJ_ERR_FAULT     = -8,
    GJ_ERR_DEAD      = -9,  /* object DEAD after revoke */
    GJ_ERR_STALE_CAP = -10, /* slot/object generation mismatch */
    GJ_ERR_QUOTA     = -11,
    GJ_ERR_TIMEOUT   = -12,
    GJ_ERR_PEER_DEAD = -13,
    GJ_ERR_REVOKED   = -14,
    GJ_ERR_NODEV     = -15,
    GJ_ERR_IO        = -16,
};
