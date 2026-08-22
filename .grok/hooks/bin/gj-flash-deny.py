#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
"""PreToolUse: deny agent-driven USB flash and embed-rewrites.

Fail-open on parse errors (hook contract). Deny is explicit JSON on stdout.
Override: parent environment GJ_FLASH_ALLOW=1 (not a token in the command).
"""
from __future__ import annotations

import json
import os
import re
import sys

DENY_RES = (
    re.compile(r"\bdd\b.*\bof=/dev/(sd|hd|vd|nvme|mmcblk|loop)", re.I | re.S),
    re.compile(r"\bof=/dev/(sd[a-z]|hd[a-z]|vd[a-z]|nvme\d|mmcblk\d)", re.I),
    re.compile(r"install-hwtest-usb\.sh", re.I),
    re.compile(r"install-linux-hwtest", re.I),
    re.compile(r"(^|[\s;&|])\./scripts/install-usb\.sh\b", re.I),
    re.compile(r"\bmake\b[^\n]*\binstall-hwtest-usb\b", re.I),
    re.compile(r"\bmake\b[^\n]*\binstall-linux-hwtest\b", re.I),
    re.compile(r"\bmake\b[^\n]*\binstall-usb\b", re.I),
    re.compile(r"embed-linux-fw\.sh", re.I),
    re.compile(r"embed-linux-mod\.sh", re.I),
    re.compile(r"\bmkfs\.[a-z0-9]+\b[^\n]*/dev/(sd|nvme|mmcblk)", re.I),
)


def allow(msg: str | None = None) -> int:
    out = {"decision": "allow"}
    if msg:
        out["reason"] = msg
    sys.stdout.write(json.dumps(out) + "\n")
    return 0


def deny(reason: str) -> int:
    sys.stdout.write(json.dumps({"decision": "deny", "reason": reason}) + "\n")
    return 2


def collect_text(obj) -> str:
    parts: list[str] = []

    def walk(x) -> None:
        if isinstance(x, str):
            parts.append(x)
        elif isinstance(x, dict):
            for v in x.values():
                walk(v)
        elif isinstance(x, list):
            for v in x:
                walk(v)

    walk(obj)
    return "\n".join(parts)


def main() -> int:
    try:
        raw = sys.stdin.read()
        ev = json.loads(raw) if raw.strip() else {}
    except Exception:
        return allow()

    if not isinstance(ev, dict):
        return allow()

    if os.environ.get("GJ_FLASH_ALLOW", "") == "1":
        return allow("GJ_FLASH_ALLOW=1")

    blob = collect_text(ev.get("toolInput"))
    if not blob:
        return allow()

    for rx in DENY_RES:
        if rx.search(blob):
            return deny(
                "Flash/embed blocked for agents (AGENTS.md). "
                "Flash in a real shell: sudo ./scripts/install-hwtest-usb.sh /dev/sdX. "
                "To override, start Grok with GJ_FLASH_ALLOW=1. "
                "Matched: " + rx.pattern
            )
    return allow()


if __name__ == "__main__":
    raise SystemExit(main())
