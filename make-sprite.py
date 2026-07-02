#!/usr/bin/env python3

# Generates a sprite file from a human-readable text file.
# Usage: python3 gensprite.py input.txt -o output.sprite
# optional arguments:
#   -h, --help            show this help message and exit
#   -o|--output OUTPUT,   write to OUTPUT (default: stdout)
#   -W|--width WIDTH,     sprite WIDTH (default: fit-to-content)
#   -H|--height HEIGHT,   sprite HEIGHT (default: fit-to-content)
#   -f|--flood-fill       flood-fill the shape (default: no flood-fill)
#   -p|--fill-point X,Y   start point for flood-fill (default: all edges)
#   -c|--fill-char CHAR   character to use for flood-fill (default: 'B')
#   -b|--blank-char CHAR  character to treat as blank for flood-fill (default: ' ')
#   -t|--text-output      output in text format (default: sprite format)

MAX_SPRITE_AREA = 800 # Maximum allowed area for a sprite

import argparse
import sys
import os
from collections import deque # For flood-fill algorithm

def flood_fill(canvas, width, height, start_points, target_char=' ', replacement_char='B'):
    """Flood-fill algorithm to fill connected areas of target_char starting from (x, y) with replacement_char."""
    if target_char == replacement_char:
        return 0

    queue = deque()
    for (x, y) in start_points:
        if x < 0 or x >= width or y < 0 or y >= height:
            continue
        if canvas[y * width + x] != target_char:
            continue
        queue.append((x, y))
    filled_count = 0

    while queue:
        cx, cy = queue.popleft()
        if cx < 0 or cx >= width or cy < 0 or cy >= height:
            continue
        if canvas[cy * width + cx] != target_char:
            continue

        canvas[cy * width + cx] = replacement_char
        filled_count += 1

        # Add neighboring pixels to the queue
        queue.append((cx + 1, cy))
        queue.append((cx - 1, cy))
        queue.append((cx, cy + 1))
        queue.append((cx, cy - 1))

    return filled_count

def file_to_canvas(infile, width=None, height=None):
    """Read input file and create a canvas of specified width and height, filling with spaces as needed."""
    lines = [line.rstrip('\n') for line in infile]
    max_line_length = max(len(line) for line in lines)
    num_lines = len(lines)

    if width is None:
        width = max_line_length
    if height is None:
        height = num_lines

    if width < max_line_length:
        print(f"Some lines are longer than the specified width {width}, image will be clipped.", file=sys.stderr)
        width = max_line_length
    if height < num_lines:
        print(f"More lines than the specified height {height}, image will be clipped.", file=sys.stderr)
        height = num_lines

    # Create a blank canvas
    canvas = [' '] * (width * height)

    # Copy the content into the canvas
    for r in range(min(num_lines, height)):
        for c in range(min(len(lines[r]), width)):
            canvas[r * width + c] = lines[r][c]

    return canvas, width, height

def validate_canvas(canvas):
    """Validate that all characters in the canvas are printable ASCII characters, replacing invalid ones with '?' and returning a count and a set of invalid characters."""
    invalid_chars = set()
    num_invalid = 0
    for i, char in enumerate(canvas):
        if not (32 <= ord(char) <= 126):  # Check if char is a printable ASCII character
            invalid_chars.add(char)
            num_invalid += 1
            canvas[i] = '?'  # Replace invalid char with '?'
    return num_invalid, invalid_chars

def canvas_to_file(canvas, width, height, outfile):
    # Write to output file
    outfile.write(f"{width}\n{height}\n")
    outfile.write(''.join(canvas))
    outfile.write('\n')
    outfile.flush()

    return width * height

def print_canvas(canvas, width, height, outfile):
    for r in range(height):
        outfile.write(''.join(canvas[r * width:(r + 1) * width]) + '\n')
    outfile.flush()

def main():
    parser = argparse.ArgumentParser(description="Generate a sprite file from a text file.")
    parser.add_argument("input", nargs='?', type=argparse.FileType('r'), default=sys.stdin, help="Input text file (default: stdin)")
    parser.add_argument("-o", "--output", type=argparse.FileType('w'), default=sys.stdout, help="Output sprite file (default: stdout)")
    parser.add_argument("-W", "--width", type=int, default=None, help="Width of the sprite (default: fit-to-content)")
    parser.add_argument("-H", "--height", type=int, default=None, help="Height of the sprite (default: fit-to-content)")
    parser.add_argument("-f", "--flood-fill", action='store_true', help="Flood-fill the shape (default: no flood-fill)")
    parser.add_argument("-p", "--fill-point", type=str, default=None, help="Start point for flood-fill as X,Y (default: all edges)")
    parser.add_argument("-c", "--fill-char", type=str, default='B', help="Character to use for flood-fill (default: 'B')")
    parser.add_argument("-b", "--blank-char", type=str, default=' ', help="Character to treat as blank for flood-fill (default: ' ')")
    parser.add_argument("-t", "--text-output", action='store_true', help="Output in text format (default: sprite format)")

    args = parser.parse_args()

    canvas, w, h = file_to_canvas(args.input, width=args.width, height=args.height)

    invcount, invalid_chars = validate_canvas(canvas)

    if args.flood_fill:
        try:
            if args.fill_point:
                x_str, y_str = args.fill_point.split(',')
                x, y = int(x_str), int(y_str)
                filled = flood_fill(canvas, w, h, [(x, y)], target_char=args.blank_char, replacement_char=args.fill_char)
                def flood_fill_report():
                    print(f"- Flood-filled {filled} pixels starting from ({x},{y})", file=sys.stderr)
            else:
                # Flood-fill from all edges
                edge_points = []
                for i in range(w):
                    edge_points.append((i, 0))        # Top edge
                    edge_points.append((i, h - 1))    # Bottom edge
                for j in range(h):
                    edge_points.append((0, j))        # Left edge
                    edge_points.append((w - 1, j))    # Right edge
                filled = flood_fill(canvas, w, h, edge_points, target_char=args.blank_char, replacement_char=args.fill_char)
                def flood_fill_report():
                    print(f"- Flood-filled {filled} pixels starting from all edges", file=sys.stderr)
        except Exception as e:
            print(f"Error in flood-fill parameters: {e}", file=sys.stderr)
            def flood_fill_report():
                print("- Flood-fill failed due to error.", file=sys.stderr)
    else:
        def flood_fill_report():
            pass

    if args.text_output:
        print_canvas(canvas, w, h, args.output)
        chars = w * h
        verb = 'Displayed'
    else:
        chars = canvas_to_file(canvas, w, h, args.output)
        verb = 'Converted'

    print(f"{verb} '{args.input.name}' to '{args.output.name}'", file=sys.stderr)
    if invalid_chars:
        plural = 's' if invcount != 1 else ''
        print(f"Warning: Found {invcount} non-ASCII character{plural} {invalid_chars!r}, replaced with '?'.", file=sys.stderr)
    print(f"- Sprite dimensions: {w}x{h}", file=sys.stderr)
    print(f"- Total characters: {chars}", file=sys.stderr)
    flood_fill_report()

    if chars > MAX_SPRITE_AREA:
        print(f"Warning: Sprite area {chars} exceeds maximum allowed area of {MAX_SPRITE_AREA}.", file=sys.stderr)

if __name__ == "__main__":
    main()
