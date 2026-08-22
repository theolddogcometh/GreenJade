let meta = #{
    name: "overnight-sshd",
    description: "One Dual DoD B overnight cycle: path-lock, at most one writer file, score vs QEMU51 baseline. Caps identical hangs. Does not flash.",
    when_to_use: "Overnight OpenSSH LINUX work toward Dual DoD B. Prefer this over ad-hoc STOP-loop swarms.",
    phases: [
        #{ title: "Lock", detail: "path lock + newest log vs baseline keys" },
        #{ title: "Write", detail: "at most one named writer file, or skip" },
        #{ title: "Score", detail: "regress vs QEMU51; identical-hang cap" },
        #{ title: "Report", detail: "12-line scoreboard; Dual DoD B OPEN" },
    ],
};

let harvest_schema = #{
    "type": "object", "required": ["listen", "ret271", "accept", "fork_user", "ssh20", "hang_line", "notes"],
    "properties": #{
        "listen": #{ "type": "boolean" },
        "ret271": #{ "type": "boolean" },
        "accept": #{ "type": "boolean" },
        "fork_user": #{ "type": "boolean" },
        "ssh20": #{ "type": "boolean" },
        "hang_line": #{ "type": "string" },
        "notes": #{ "type": "string" },
    },
};
let write_schema = #{
    "type": "object", "required": ["ok", "file", "notes"],
    "properties": #{
        "ok": #{ "type": "boolean" },
        "file": #{ "type": "string" },
        "notes": #{ "type": "string" },
    },
};
let score_schema = #{
    "type": "object", "required": ["regressed", "identical_hang", "next_hole", "notes"],
    "properties": #{
        "regressed": #{ "type": "boolean" },
        "identical_hang": #{ "type": "boolean" },
        "next_hole": #{ "type": "string" },
        "notes": #{ "type": "string" },
    },
};

fn trimmed(s) {
    if type_of(s) == "string" { s.trim(); s } else { "" }
}

fn output_map(r) {
    if r == () { return (); }
    if r.success != true { return (); }
    if r.output == () { return (); }
    if type_of(r.output) != "map" { return (); }
    r.output
}

let hole = "";
let writer_file = "";
let baseline_log = "/tmp/gj-overnight-sshd51.log";
let newest_log = "";
let do_write = false;
if args != () {
    if args.hole != () { hole = trimmed(args.hole); }
    if args.writer_file != () { writer_file = trimmed(args.writer_file); }
    if args.baseline_log != () { baseline_log = trimmed(args.baseline_log); }
    if args.newest_log != () { newest_log = trimmed(args.newest_log); }
    if args.write == true { do_write = true; }
}
if hole == "" {
    pause("verification", "Pass args.hole — the single current Dual DoD B hole (one sentence). Optional: writer_file, write=true, baseline_log, newest_log.");
}
if do_write && writer_file == "" {
    pause("verification", "args.write=true requires args.writer_file — exactly one product path.");
}

phase("Lock");
let lock_p = "";
lock_p += "You are GreenJade honesty-auditor. READ-ONLY. Read AGENTS.md and docs/OVERNIGHT.md. Use grep/read_file/run_terminal_command.\n";
lock_p += "Do not edit. Do not flash. Do not relink DUT. Do not bump stamp.\n";
lock_p += "Named hole (operator): ";
lock_p += hole;
lock_p += "\nBaseline log: ";
lock_p += baseline_log;
lock_p += "\n";
if newest_log != "" {
    lock_p += "Newest log: ";
    lock_p += newest_log;
    lock_p += "\n";
} else {
    lock_p += "Newest log: ls -t /tmp/gj-overnight-sshd*.log | head -1\n";
}
lock_p += "Confirm: GJ_IMAGE_VERSION 0.1.184; DUT build/openssh-dut/sshd entry 0x4000000 size ~7967144; no user/sshd; Dual DoD A/B OPEN.\n";
lock_p += "After last 'Server listening' in baseline AND newest, set booleans: listen, ret271 (271 ret= not ret_later), accept (sys nr=43 ret=), fork_user (linux_fork user child), ssh20 (SSH-2.0).\n";
lock_p += "hang_line = last greppable serial line after Server listening (truncate to 160 chars). notes = facts only.\n";
lock_p += "Reject as current hole: 0x1000000, sshd_gj, listen-not-working, PTY, vfork, dash, Dual DoD close.\n";
let harvested = agent(lock_p, #{
    label: "lock-harvest",
    agent_type: "honesty-auditor",
    capability_mode: "read-only",
    output_schema: harvest_schema,
});
let h = output_map(harvested);
let hang_line = "";
if h != () && h.hang_line != () { hang_line = trimmed(h.hang_line); }
log("lock hang_line=" + hang_line);

phase("Write");
let wrote = false;
let wrote_file = "";
if do_write {
    let wp = "";
    wp += "Read AGENTS.md and docs/OVERNIGHT.md. One writer. no-densify. fail-closed.\n";
    wp += "Named hole: ";
    wp += hole;
    wp += "\nEdit ONLY this file: ";
    wp += writer_file;
    wp += "\nDo not touch any other path. Do not flash. Do not bump GJ_IMAGE_VERSION. Do not relink build/openssh-dut. Do not add kprintf/once-lamps unless this prompt named them (it does not).\n";
    wp += "Baseline must keep QEMU51 keys: Server listening, 271 ret=1, accept, linux_fork user child. Dual DoD B OPEN.\n";
    wp += "If the newest log hang_line matches the previous two QEMU logs and there is no 271 ret after Server listening, do not patch — set ok=false and notes=identical_hang.\n";
    wp += "ok=true only if you edited that one file. file= the path. notes=short.\n";
    let wr = agent(wp, #{
        label: "one-writer",
        capability_mode: "read-write",
        output_schema: write_schema,
    });
    let wo = output_map(wr);
    if wo != () && wo.ok == true { wrote = true; }
    if wo != () && wo.file != () { wrote_file = trimmed(wo.file); }
    if wrote { log("wrote " + wrote_file); } else { log("write skipped or ok=false"); }
} else {
    log("write=false (harvest-only cycle)");
}

phase("Score");
let sp = "";
sp += "You are GreenJade honesty-auditor. READ-ONLY. Read AGENTS.md docs/OVERNIGHT.md.\n";
sp += "Named hole: ";
sp += hole;
sp += "\nBaseline: ";
sp += baseline_log;
sp += "\nCompare newest /tmp/gj-overnight-sshd*.log to baseline after last Server listening.\n";
sp += "regressed=true if baseline had 271 ret= / accept / fork_user and newest does not.\n";
sp += "identical_hang=true if newest hang_line matches the previous log hang (rx demux first / tcp_poll after_first / same last line) AND no 271 ret after Server listening.\n";
sp += "next_hole = one sentence. If identical_hang or regressed: next_hole must say STOP writers / revert to baseline ELF, not stack another cut.\n";
sp += "PTY/vfork/dash/0x1000000/sshd_gj are not next_hole. Dual DoD B OPEN. notes=facts.\n";
let scored = agent(sp, #{
    label: "score",
    agent_type: "honesty-auditor",
    capability_mode: "read-only",
    output_schema: score_schema,
});
let sc = output_map(scored);
let next_hole = hole;
let identical = false;
let regressed = false;
if sc != () {
    if sc.next_hole != () {
        let nh = trimmed(sc.next_hole);
        if nh != "" { next_hole = nh; }
    }
    if sc.identical_hang == true { identical = true; }
    if sc.regressed == true { regressed = true; }
}

phase("Report");
let summary = "";
summary += "Dual DoD B OPEN. hole=";
summary += hole;
summary += " next=";
summary += next_hole;
if identical { summary += " identical_hang=1"; }
if regressed { summary += " regressed=1"; }
if wrote { summary += " wrote=1"; } else { summary += " wrote=0"; }
complete(#{
    summary: summary,
    hole: hole,
    next_hole: next_hole,
    identical_hang: identical,
    regressed: regressed,
    wrote: wrote,
    writer_file: writer_file,
    baseline_log: baseline_log,
    dual_dod_b: "OPEN",
});
