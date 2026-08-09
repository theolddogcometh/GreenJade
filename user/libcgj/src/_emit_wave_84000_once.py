#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
"""One-shot CREATE-ONLY: graph_batch80001.c .. graph_batch84000.c via O_EXCL.
Pattern: graph_batch25850 simple + decade lamps (xx91-xx00) at each *00.
Matches high-range rewire style used for 76001..80000 (template from 25850/25891-25900).
"""
from __future__ import annotations

import errno
import os
import sys
from pathlib import Path

OUT = Path(__file__).resolve().parent
LO, HI = 80001, 84000

# Load templates once
T_SIMPLE = (OUT / "graph_batch25850.c").read_text(encoding="utf-8")
T_LAMPS = {
    rem: (OUT / f"graph_batch{25800 + rem}.c").read_text(encoding="utf-8")
    for rem in range(91, 100)
}
T_CAP = (OUT / "graph_batch25900.c").read_text(encoding="utf-8")


def next_hw(n: int) -> int:
    return ((n + 99) // 100) * 100


def body_for(n: int) -> str:
    rem = n % 100
    if rem == 0:
        # Capstone: 25900 -> n; keep 25891-25899 decade refs as template style
        return T_CAP.replace("25900", str(n))
    if 91 <= rem <= 99:
        m = next_hw(n)
        src_batch = 25800 + rem
        # Order matters: replace longer/more-specific first
        text = T_LAMPS[rem]
        text = text.replace(str(src_batch), str(n))
        text = text.replace("25900", str(m))
        return text
    # Simple identity (25850 template)
    return T_SIMPLE.replace("25850", str(n))


def create_excl(path: Path, data: str) -> str:
    flags = os.O_CREAT | os.O_EXCL | os.O_WRONLY
    try:
        fd = os.open(path, flags, 0o644)
    except OSError as e:
        if e.errno == errno.EEXIST:
            return "exists"
        raise
    try:
        os.write(fd, data.encode("utf-8"))
    finally:
        os.close(fd)
    return "created"


def main() -> int:
    created = 0
    exists = 0
    for n in range(LO, HI + 1):
        path = OUT / f"graph_batch{n}.c"
        status = create_excl(path, body_for(n))
        if status == "created":
            created += 1
        else:
            exists += 1
    planned = HI - LO + 1
    line = (
        f"created={created} already_existed={exists} "
        f"planned={planned} range={LO}..{HI}"
    )
    print(line)
    (OUT / "_emit_wave_84000_result.txt").write_text(
        line
        + "\npattern=25850 simple identity + decade lamps (*91-*00) at each hundred\n"
        + "note=CREATE-ONLY O_EXCL. No Makefile/kernel/docs touch. Soft only; "
        + "bar3_ready=0 product_score=0 at each *00 decade.\n",
        encoding="utf-8",
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
