/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch225: shortest path on a binary grid via BFS.
 *
 * Surface (unique symbols):
 *   int gj_bfs_grid(const unsigned char *grid, size_t rows, size_t cols,
 *                   size_t sr, size_t sc, size_t tr, size_t tc);
 *     — 4-connected BFS from (sr, sc) to (tr, tc) on a row-major grid
 *       of size rows × cols. Cells with value 0 are walkable; any non-zero
 *       cell is blocked. Returns the shortest path length in edges, or -1
 *       if unreachable / invalid (NULL grid, empty dims, OOB endpoints,
 *       blocked start/target, or rows*cols exceeds the freestanding cap).
 *       When start == target and the cell is walkable, returns 0.
 *   __gj_bfs_grid  (alias)
 *   __libcgj_batch225_marker = "libcgj-batch225"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. Stack-only queue + bitset
 * visited (capped at B225_MAX_CELLS). No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch225_marker[] = "libcgj-batch225";

/* Max cells for stack queue + visited bitset (e.g. 64×64). */
#define B225_MAX_CELLS 4096u
/* Visited bitset words (64-bit). */
#define B225_VIS_WORDS ((B225_MAX_CELLS + 63u) / 64u)

/* 4-connected deltas: up, down, left, right. */
static const int b225_dr[4] = { -1, 1, 0, 0 };
static const int b225_dc[4] = { 0, 0, -1, 1 };

/* ---- bitset helpers ---------------------------------------------------- */

static void
b225_vis_clear(uint64_t *aVis)
{
	size_t i;

	for (i = 0u; i < B225_VIS_WORDS; i++) {
		aVis[i] = 0u;
	}
}

static int
b225_vis_test(const uint64_t *aVis, size_t i)
{
	return (aVis[i >> 6] >> (i & 63u)) & 1u;
}

static void
b225_vis_set(uint64_t *aVis, size_t i)
{
	aVis[i >> 6] |= (uint64_t)1 << (i & 63u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bfs_grid — shortest 4-connected path length on a binary grid.
 *
 * grid: row-major, length rows*cols; 0 = walkable, non-zero = blocked.
 * (sr, sc) / (tr, tc): start / target row and column (0-based).
 *
 * Returns edge-count of the shortest path, 0 when start equals target
 * and walkable, or -1 on failure / unreachable.
 */
int
gj_bfs_grid(const unsigned char *grid, size_t rows, size_t cols,
            size_t sr, size_t sc, size_t tr, size_t tc)
{
	uint64_t aVis[B225_VIS_WORDS];
	uint16_t aQ[B225_MAX_CELLS];
	size_t nCells;
	size_t iStart;
	size_t iTarget;
	size_t qHead;
	size_t qTail;
	int nDist;

	if (grid == NULL || rows == 0u || cols == 0u) {
		return -1;
	}
	if (sr >= rows || sc >= cols || tr >= rows || tc >= cols) {
		return -1;
	}
	/* Guard rows*cols overflow and freestanding stack cap. */
	if (rows > (size_t)B225_MAX_CELLS / cols) {
		return -1;
	}
	nCells = rows * cols;
	if (nCells > (size_t)B225_MAX_CELLS) {
		return -1;
	}

	iStart = sr * cols + sc;
	iTarget = tr * cols + tc;

	if (grid[iStart] != 0u) {
		return -1;
	}
	if (iStart == iTarget) {
		return 0;
	}
	if (grid[iTarget] != 0u) {
		return -1;
	}

	b225_vis_clear(aVis);
	b225_vis_set(aVis, iStart);

	qHead = 0u;
	qTail = 0u;
	aQ[qTail++] = (uint16_t)iStart;
	nDist = 0;

	/*
	 * Level-order BFS: each outer iteration advances path length by 1.
	 * Queue holds cell indices only; distance is the level counter.
	 */
	while (qHead < qTail) {
		size_t cLevel = qTail - qHead;
		size_t k;

		nDist++;
		for (k = 0u; k < cLevel; k++) {
			size_t iCur = (size_t)aQ[qHead++];
			size_t r = iCur / cols;
			size_t c = iCur % cols;
			unsigned d;

			for (d = 0u; d < 4u; d++) {
				long nR = (long)r + (long)b225_dr[d];
				long nC = (long)c + (long)b225_dc[d];
				size_t iNxt;

				if (nR < 0 || nC < 0 ||
				    (size_t)nR >= rows || (size_t)nC >= cols) {
					continue;
				}
				iNxt = (size_t)nR * cols + (size_t)nC;
				if (grid[iNxt] != 0u) {
					continue;
				}
				if (b225_vis_test(aVis, iNxt)) {
					continue;
				}
				if (iNxt == iTarget) {
					return nDist;
				}
				b225_vis_set(aVis, iNxt);
				aQ[qTail++] = (uint16_t)iNxt;
			}
		}
	}

	return -1;
}

int __gj_bfs_grid(const unsigned char *grid, size_t rows, size_t cols,
                  size_t sr, size_t sc, size_t tr, size_t tc)
    __attribute__((alias("gj_bfs_grid")));
