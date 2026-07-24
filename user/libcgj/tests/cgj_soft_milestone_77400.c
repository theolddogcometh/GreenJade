/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77400 \
 *   user/libcgj/tests/cgj_soft_milestone_77400.c -ldl
 * Direct: -DCGJ_SOFT_M77400_DIRECT + graph_batch77391.c … graph_batch77400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77400
#define CGJ_SOFT_B0 77391
#define CGJ_SOFT_B1 77392
#define CGJ_SOFT_B2 77393
#define CGJ_SOFT_B3 77394
#define CGJ_SOFT_B4 77395
#define CGJ_SOFT_B5 77396
#define CGJ_SOFT_B6 77397
#define CGJ_SOFT_B7 77398
#define CGJ_SOFT_B8 77399
#include "_cgj_soft_m_template.inc"
