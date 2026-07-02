#!/usr/bin/env python3
"""
Play a simple "ASCIImation" file in an ANSI-compatible terminal.

Usage
-----
    python3 asciimation-player.py [-h] [--center] [--no-alt] FILE

    positional arguments:
      FILE                Path to the ASCIImation file.

    optional arguments:
      -h, --help          Show this help message and exit.
      -c, --center        Center the (cropped) picture in a larger terminal.
      -n, --no-alt        Do not use the alternate screen buffer.
      -s, --stats         Show statistics about the movie instead of playing it.
      -o, --output FILE   Write statistics to FILE instead of stdout.

Examples
--------

Play a movie in the terminal:
    python3 player.py sample-movie.txtmov

Compare two movies to see if they are identical:
    python3 player.py --stats movie1.txtmov > stats1.txt
    python3 player.py --stats movie2.txtmov > stats2.txt
    diff -u stats1.txt stats2.txt
"""

import argparse
import re
import sys
import time
import hashlib
from pathlib import Path
from shutil import get_terminal_size
from typing import List, Tuple, Optional, TextIO

# ----------------------------------------------------------------------
# ANSI control sequences (used directly - no external library required)
ESC = "\033"
CSI = ESC + "["

HIDE_CURSOR = CSI + "?25l"
SHOW_CURSOR = CSI + "?25h"

ALT_SCREEN_ON = CSI + "?1049h"   # Switch to alternate screen buffer
ALT_SCREEN_OFF = CSI + "?1049l"  # Return to normal screen

CLEAR_SCREEN = CSI + "2J"
CURSOR_HOME = CSI + "H"

# ----------------------------------------------------------------------
def hash_list(lines: List[str]) -> str:
    """
    Compute a hash of the list of strings.
    """
    m = hashlib.sha256()
    for line in lines:
        m.update(line.encode("utf-8"))
        m.update(b"\n")
    return m.hexdigest()[0:48]  # Shorten for readability

# ----------------------------------------------------------------------
def parse_asciimation(path: Path) -> Tuple[int, int, List[Tuple[float, List[str]]]]:
    """
    Parse a file that follows the "ASCIImation" format.

    Returns
    -------
    (orig_width, orig_height, frames)
        *orig_width* and *orig_height* are the dimensions declared in the header.
        *frames* is a list of ``(timestamp, lines)`` where ``lines`` already has
        exactly *orig_height* strings (including trailing spaces).
    """
    with path.open(encoding="utf-8") as f:
        text = f.read().splitlines()

    # Header line:  ASCIImation: 80 x 24
    header_match = re.search(r"ASCIImation:\s*(\d+)\s*x\s*(\d+)", text[0])
    if not header_match:
        raise ValueError("File does not contain a valid ASCIImation header.")
    width, height = map(int, header_match.groups())

    # The file should have exactly 1 + frame_count * (height + 3) lines
    if (len(text) - 1) % (height + 3) != 0:
        raise ValueError("Malformed ASCIImation file, movie size mismatch.")

    num_frames = (len(text) - 1) // (height + 3)
    if num_frames == 0:
        raise ValueError("No frames found in the ASCIImation file.")
    # Split into frames - each frame starts with "Frame N at X seconds:"
    frame_pat = re.compile(
        r"^Frame\s+\d+\s+at\s+([0-9.]+)\s+seconds:\s*$", re.MULTILINE
    )

    frames: List[Tuple[float, List[str]]] = []

    # The frame boundary separator is exactly width '=' characters
    sep_line = "=" * width

    for i in range(num_frames):
        frame_start = 1 + i * (height + 3)
        ts_match = frame_pat.match(text[frame_start])
        if not ts_match:
            raise ValueError(f"Malformed frame header at frame {i+1}.")
        timestamp = float(ts_match.group(1))

        # Check the top and bottom separator lines
        top_boundary = frame_start + 1
        data_start = frame_start + 2
        bottom_boundary = data_start + height
        if text[top_boundary] != sep_line or text[bottom_boundary] != sep_line:
            raise ValueError(f"Malformed frame boundary at frame {i+1}.")

        frame_block = text[data_start : data_start + height]

        # Check all the lines are width long
        for line in frame_block:
            if len(line) > width:
                raise ValueError(f"Frame {i+1} contains a line longer than width.")

        frames.append((timestamp, frame_block))

    return width, height, frames


# ----------------------------------------------------------------------
def crop(
    lines: List[str],
    term_w: int,
    term_h: int,
    center: bool = False
) -> List[str]:
    """
    Crop *lines* to fit the terminal size.
    """
    frame_h = len(lines)
    frame_w = max(len(l) for l in lines) if lines else 0

    # Crop vertically
    visible_h = min(frame_h, term_h)
    top = 0 if not center else max(0, (frame_h - visible_h) // 2)
    cropped_vert = lines[top : top + visible_h]

    # Crop horizontally
    visible_w = min(frame_w, term_w)
    left = 0 if not center else max(0, (frame_w - visible_w) // 2)

    cropped = [row[left : left + visible_w].ljust(visible_w) for row in cropped_vert]

    return cropped

def center_offsets(
    movie_w: int,
    movie_h: int,
    term_w: int,
    term_h: int
) -> Tuple[int, int]:
    """
    Compute the offsets needed to center the movie in the terminal.
    """
    h_offset = (term_h - movie_h) // 2
    v_offset = (term_w - movie_w) // 2
    return h_offset, v_offset

# ----------------------------------------------------------------------
def play(
    movie_w: int,
    movie_h: int,
    frames: List[Tuple[float, List[str]]],
    term_w: int,
    term_h: int,
    *,
    center: bool = False,
    use_alt_screen: bool = True,
) -> None:
    """
    Render the pre-processed frames on the terminal.

    The function hides the cursor, optionally switches to the alternate screen
    buffer and guarantees that the cursor is restored on exit (including Ctrl-C).
    """
    h_off, v_off = center_offsets(movie_w, movie_h, term_w, term_h) if center else (0, 0)
    cursor_move_v = CSI + f"{v_off}B" if v_off else ""
    cursor_move_h = CSI + f"{h_off}C" if h_off else ""
    frame_start = CURSOR_HOME + cursor_move_v + cursor_move_h
    line_tween = "\r\n" + cursor_move_h

    # Pre-process: crop each frame once and store the printable version + offsets
    processed = [
        frame_start + line_tween.join(crop(frame_lines, term_w, term_h, center=center))
        for _, frame_lines in frames
    ]

    # Prepare terminal
    sys.stdout.write(HIDE_CURSOR)
    if use_alt_screen:
        sys.stdout.write(ALT_SCREEN_ON)
    sys.stdout.write(CLEAR_SCREEN)
    sys.stdout.flush()

    try:
        start = time.perf_counter()
        for idx, (timestamp, _) in enumerate(frames):
            target = start + timestamp
            now = time.perf_counter()
            if now < target:
                # We are on schedule - sleep the remaining time.
                time.sleep(target - now)
            # else: we are already late; just continue (run at full speed)

            # Render the frame
            sys.stdout.write(processed[idx])
            sys.stdout.flush()
    finally:
        # Restore terminal state
        if use_alt_screen:
            sys.stdout.write(ALT_SCREEN_OFF)
        sys.stdout.write(SHOW_CURSOR)
        sys.stdout.flush()

# ----------------------------------------------------------------------
def frame_content_bbox(
    lines: List[str]
) -> Tuple[int, int, int, int]:
    """
    Compute the bounding box of the non-space content in the frame.

    Returns (min_x, min_y, max_x, max_y) where (min_x, min_y) is the top-left
    corner and (max_x, max_y) is the bottom-right corner (inclusive).
    If the frame is empty (all spaces), returns (0, 0, 0, 0).
    """
    min_x = min_y = float('inf')
    max_x = max_y = float('-inf')

    for y, line in enumerate(lines):
        for x, char in enumerate(line):
            if char != ' ':
                if x < min_x: min_x = x
                if x > max_x: max_x = x
                if y < min_y: min_y = y
                if y > max_y: max_y = y

    if min_x == float('inf'):
        # Frame is empty
        return 0, 0, 0, 0

    return min_x, min_y, max_x, max_y

def frame_content_hash(
    lines: List[str],
    bbox: Tuple[int, int, int, int]
) -> str:
    """
    Compute a hash of the content within the bounding box.
    """
    min_x, min_y, max_x, max_y = bbox
    cropped = [line[min_x:max_x+1] for line in lines[min_y:max_y+1]]
    return hash_list(cropped)

# ----------------------------------------------------------------------
def show_stats(
    movie_w: int,
    movie_h: int,
    frames: List[Tuple[float, List[str]]],
    file: Optional[TextIO] = None
) -> None:
    """
    Print some statistics about the parsed movie.
    """
    file = file or sys.stdout

    num_frames = len(frames)
    duration = frames[-1][0] if frames else 0.0
    avg_fps = num_frames / duration if duration > 0 else 0.0

    print(f"Movie dimensions: {movie_w} x {movie_h}", file=file)
    print(f"Number of frames: {num_frames}", file=file)
    
    # Compute bounding boxes and hashes and print them
    for idx, (timestamp, lines) in enumerate(frames):
        bbox = frame_content_bbox(lines)
        content_hash = frame_content_hash(lines, bbox)
        print(f"Frame {idx+1:4d}: {bbox}, Hash: {content_hash}", file=file)


# ----------------------------------------------------------------------
def main() -> None:
    parser = argparse.ArgumentParser(
        description="Play an ASCIImation file in a terminal."
    )
    parser.add_argument(
        "file",
        type=Path,
        help="Path to the ASCIImation file.",
    )
    parser.add_argument(
        "--center", "-c",
        action="store_true",
        help="Center the (cropped) picture when the terminal is larger.",
    )
    parser.add_argument(
        "--no-alt", "-n",
        dest="alt_screen",
        action="store_false",
        help="Do not use the alternate screen buffer.",
    )
    parser.add_argument(
        "--stats", "-s",
        action="store_true",
        help="Don't play the movie, just generate stats (for testing).",
    )
    parser.add_argument(
        "--output", "-o",
        type=argparse.FileType('w', encoding='utf-8'),
        default=None,
        help="Write statistics to FILE instead of stdout.",
    )
    args = parser.parse_args()

    if not args.file.is_file():
        sys.exit(f"Error: file not found - {args.file}")

    # Parse the animation
    try:
        movie_w, movie_h, frames = parse_asciimation(args.file)
    except Exception as exc:  # pragma: no cover - defensive
        sys.exit(f"Failed to parse file: {exc}")

    if args.stats:
        show_stats(movie_w, movie_h, frames, file=args.output)
        sys.exit(0)

    # Determine terminal size at start (the player does not handle resize)
    term_sz = get_terminal_size(fallback=(80, 24))
    term_w, term_h = term_sz.columns, term_sz.lines

    try:
        play(
            movie_w,
            movie_h,
            frames,
            term_w,
            term_h,
            center=args.center,
            use_alt_screen=args.alt_screen,
        )
    except KeyboardInterrupt:
        # Graceful exit on Ctrl-C - the finally block in *play* restores everything.
        pass


if __name__ == "__main__":
    main()
