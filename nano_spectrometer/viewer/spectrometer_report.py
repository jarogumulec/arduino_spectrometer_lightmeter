#!/usr/bin/env python3
"""Terminal report + live plot for the ESP32 AS7341/AS7343 spectrometer bridge.

Reads the CSV serial protocol from the firmware, prints a table per frame
in the terminal and (optionally) shows a live matplotlib plot of the spectrum.

Usage:
    uv run spectrometer_report.py --port /dev/cu.usbserial-0001
    uv run spectrometer_report.py --port /dev/cu.usbserial-0001 --csv log.csv
    uv run spectrometer_report.py --list            # list serial ports
"""

import argparse
import csv
import sys
from datetime import datetime, timezone

import serial
import serial.tools.list_ports

CLEAR_LABELS = {0: "Clear"}  # wavelength 0 marks unfiltered channels


def parse_args():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--port", help="serial port, e.g. /dev/cu.usbserial-0001")
    p.add_argument("--baud", type=int, default=115200)
    p.add_argument("--csv", metavar="FILE", help="append all frames to CSV file")
    p.add_argument("--interval", type=float, default=1.0,
                   help="seconds between plotted frames (default 1.0)")
    p.add_argument("--list", action="store_true", help="list serial ports and exit")
    p.add_argument("--no-plot", action="store_true", help="terminal output only")
    return p.parse_args()


def open_port(port: str, baud: int) -> serial.Serial:
    return serial.Serial(port, baud, timeout=2)


def handle_line(line: str, state: dict):
    """Parse one protocol line into state; returns 'frame' when a frame ends."""
    if not line or line.startswith("#") or line.startswith("==="):
        print(line)
        return None
    parts = line.split(",")
    kind = parts[0]
    if kind == "ID":
        state["chip"] = parts[1] if len(parts) > 1 else "?"
        print(f"Chip identified: {state['chip']}")
    elif kind == "CONF":
        print(f"Config: gain={parts[1]} atime/astep={parts[2:]}")
    elif kind == "ERR":
        print(f"FIRMWARE ERROR: {','.join(parts[1:])}", file=sys.stderr)
    elif kind == "DATA" and len(parts) >= 4:
        seq = int(parts[1])
        wl = int(parts[2])
        count = int(parts[3])
        if state.get("seq") != seq:
            state["seq"] = seq
            state["channels"] = []
        state["channels"].append((wl, count))
    elif kind == "ENDD":
        return "frame"
    return None


def print_frame(state: dict):
    chs = state.get("channels", [])
    if not chs:
        return
    print(f"\n--- frame seq={state.get('seq')} "
          f"({datetime.now().strftime('%H:%M:%S')}) ---")
    print(f"{'channel':>10} {'count':>8}")
    for wl, count in chs:
        label = CLEAR_LABELS.get(wl, f"{wl} nm")
        print(f"{label:>10} {count:>8}")


def update_plot(ax, state):
    chs = state.get("channels", [])
    if not chs:
        return
    labeled = [(wl, c) for wl, c in chs if wl > 0]
    if not labeled:
        return
    wls = [w for w, _ in labeled]
    counts = [c for _, c in labeled]
    ax.clear()
    ax.plot(wls, counts, "o-", color="#440154")
    ax.set_xlabel("wavelength (nm)")
    ax.set_ylabel("count")
    ax.set_title(f"{state.get('chip', '?')} frame {state.get('seq')}")
    ax.grid(alpha=0.3)


def main():
    args = parse_args()
    if args.list:
        for port in serial.tools.list_ports.comports():
            print(port.device, port.description)
        return
    if not args.port:
        print("no --port given; use --list to see ports", file=sys.stderr)
        sys.exit(1)

    csv_file = None
    csv_writer = None
    if args.csv:
        csv_file = open(args.csv, "a", newline="")
        csv_writer = csv.writer(csv_file)
        csv_writer.writerow(["timestamp_utc", "chip", "seq", "wavelength_nm", "count"])

    fig = ax = None
    if not args.no_plot:
        import matplotlib.pyplot as plt
        plt.ion()
        fig, ax = plt.subplots()

    state = {"seq": None, "channels": [], "chip": "?"}
    try:
        with open_port(args.port, args.baud) as ser:
            print(f"connected to {args.port} @ {args.baud}")
            while True:
                raw = ser.readline().decode(errors="replace").strip()
                if not raw:
                    continue
                if handle_line(raw, state) == "frame":
                    print_frame(state)
                    if csv_writer:
                        ts = datetime.now(timezone.utc).isoformat()
                        for wl, c in state["channels"]:
                            csv_writer.writerow([ts, state["chip"], state["seq"], wl, c])
                        csv_file.flush()
                    if ax is not None:
                        update_plot(ax, state)
                        fig.canvas.draw()
                        fig.canvas.flush_events()
    except KeyboardInterrupt:
        print("\nstopped")
    finally:
        if csv_file:
            csv_file.close()


if __name__ == "__main__":
    main()
