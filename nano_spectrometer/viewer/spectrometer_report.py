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
SATURATION = 65535          # 16-bit ADC full scale; value = clipped, invalid

# AS7341 irradiance responsivity Re [counts/(uW/cm^2)] at AGAIN=64x, tint=27.8ms,
# calibrated on warm white 2700K LED (datasheet DS000504 v2-00, Figs 7-16).
# Valid only for broadband warm sources; +/-30% for other SPDs.
AS7341_RESPONSIVITY = {415: 55.0, 445: 110.0, 480: 210.0, 515: 390.0,
                       555: 590.0, 590: 840.0, 630: 1350.0, 680: 1070.0,
                       0: 1750.0, 910: 112.0}   # 0 = Clear, 910 = NIR
# Optical gain ratio relative to 64x (datasheet Fig. 16)
AS7341_GAIN_RATIO = {0.5: 0.008, 1: 0.016, 2: 0.032, 4: 0.065, 8: 0.125,
                     16: 0.25, 32: 0.5, 64: 1.0, 128: 2.0, 256: 3.95, 512: 7.75}
REF_TINT_MS = 27.8  # ms, reference integration time of Re values


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


def autodetect_port() -> str | None:
    """Pick a USB-serial adapter by chip vendor ID (FTDI/CH340/CP210x)."""
    known_vids = {0x0403,   # FTDI FT232 (originální Nano)
                  0x1A86,   # WCH CH340 (klony)
                  0x10C4}   # SiLabs CP210x
    ports = [p for p in serial.tools.list_ports.comports() if p.vid in known_vids]
    if len(ports) == 1:
        return ports[0].device
    return ports[0].device if ports else None


def parse_conf(parts: list[str]) -> dict:
    """CONF,gainx,atime=N,astep=N -> gain, atime, astep, tint_ms."""
    conf = {}
    try:
        conf["gain"] = float(parts[1].rstrip("x"))
        for kv in parts[2:]:
            k, _, v = kv.partition("=")
            conf[k] = int(v)
        conf["tint_ms"] = (conf["atime"] + 1) * (conf["astep"] + 1) * 2.78e-3
    except (ValueError, IndexError, KeyError):
        return {}
    return conf


# Confirmed chip + firmware defaults; replaced by ID/CONF lines on board reset
DEFAULT_CONF = {"chip": "AS7341", "gain": 16.0, "atime": 100, "astep": 999,
               "tint_ms": 101 * 1000 * 2.78e-3}


def counts_to_irradiance(count: int, wl: int, conf: dict) -> float | None:
    """Counts -> uW/cm^2 for AS7341 channels; None if not calibratable
    or the channel is saturated (count at ADC full scale)."""
    if conf.get("chip") != "AS7341" or count >= SATURATION:
        return None
    re = AS7341_RESPONSIVITY.get(wl)
    ratio = AS7341_GAIN_RATIO.get(conf.get("gain"))
    tint = conf.get("tint_ms")
    if re is None or ratio is None or tint is None:
        return None
    scale = re * ratio * (tint / REF_TINT_MS)
    if scale <= 0:
        return None
    return count / scale


def handle_line(line: str, state: dict):
    """Parse one protocol line into state; returns 'frame' when a frame ends."""
    if not line or line.startswith("#") or line.startswith("==="):
        print(line)
        return None
    parts = line.split(",")
    kind = parts[0]
    if kind == "ID":
        state["chip"] = parts[1] if len(parts) > 1 else "?"
        if state.get("conf") is not None:
            state["conf"]["chip"] = state["chip"]
        print(f"Chip identified: {state['chip']}")
    elif kind == "CONF":
        state["conf"] = {**parse_conf(parts), "chip": state.get("chip")}
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
    elif kind == "FLICK" and len(parts) >= 3:
        state["flicker_hz"] = int(parts[2])
    elif kind == "ENDD":
        return "frame"
    return None


def print_frame(state: dict):
    chs = state.get("channels", [])
    if not chs:
        return
    conf = state.get("conf", {})
    gain = conf.get("gain", "?")
    tint = conf.get("tint_ms")
    atime = conf.get("atime")
    astep = conf.get("astep")
    tint_str = f"{tint:.1f} ms" if tint else "?"
    conf_str = f"gain {gain}x, tint {tint_str}"
    if atime is not None and astep is not None:
        conf_str += f" (ATIME={atime}, ASTEP={astep})"
    print(f"\n--- frame seq={state.get('seq')} "
          f"({datetime.now().strftime('%H:%M:%S')}) ---")
    print(conf_str)
    flicker = state.get("flicker_hz")
    if flicker is not None:
        flicker_str = ("none/unknown" if flicker == 0 else
                       f"unknown frequency" if flicker == 1 else
                       f"{flicker} Hz")
        print(f"flicker: {flicker_str}")
    print(f"{'channel':>10} {'count':>8} {'uW/cm^2':>10}")
    for wl, count in chs:
        label = CLEAR_LABELS.get(wl, f"{wl} nm")
        if count >= SATURATION:
            print(f"{label:>10} {count:>8} {'SAT':>10}")
            continue
        ee = counts_to_irradiance(count, wl, conf)
        ee_str = f"{ee:.3g}" if ee is not None else "-"
        print(f"{label:>10} {count:>8} {ee_str:>10}")


def update_plot(ax, state):
    chs = state.get("channels", [])
    if not chs:
        return
    conf = state.get("conf", {})
    labeled = [(wl, c) for wl, c in chs if wl > 0]
    if not labeled:
        return
    wls = [w for w, _ in labeled]
    ees = [counts_to_irradiance(c, w, conf) for w, c in labeled]
    ax.clear()
    ax.plot(wls, ees, "o-", color="#440154")
    ax.set_xlabel("wavelength (nm)")
    ax.set_ylabel("irradiance (uW/cm^2)")
    ax.set_title(f"{state.get('chip', '?')} frame {state.get('seq')}")
    ax.grid(alpha=0.3)


def main():
    args = parse_args()
    if args.list:
        for port in serial.tools.list_ports.comports():
            print(port.device, port.description)
        return
    if not args.port:
        port = autodetect_port()
        if not port:
            print("no serial port found; connect the board or use --port", file=sys.stderr)
            sys.exit(1)
        args.port = port
        print(f"auto-detected port: {port}")

    csv_file = None
    csv_writer = None
    if args.csv:
        csv_file = open(args.csv, "a", newline="")
        csv_writer = csv.writer(csv_file)
        csv_writer.writerow(["timestamp_utc", "chip", "seq", "wavelength_nm",
                             "count", "uW_cm2", "flicker_hz", "gain",
                             "tint_ms"])

    fig = ax = None
    if not args.no_plot:
        import matplotlib.pyplot as plt
        plt.ion()
        fig, ax = plt.subplots()

    state = {"seq": None, "channels": [], "chip": "AS7341",
             "conf": dict(DEFAULT_CONF)}
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
                        conf = state.get("conf", {})
                        for wl, c in state["channels"]:
                            ee = counts_to_irradiance(c, wl, conf)
                            csv_writer.writerow([ts, state["chip"], state["seq"], wl, c,
                                                 f"{ee:.6g}" if ee is not None else "SAT" if c >= SATURATION else "",
                                                 state.get("flicker_hz", ""),
                                                 conf.get("gain", ""),
                                                 conf.get("tint_ms", "")])
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
