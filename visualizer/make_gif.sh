#!/bin/bash
mkdir -p frames/png

for json in frames/frame_*.json do
    name=$(baseman $json .json)
    python3 visualizer/render_frame.py "$json" "frames/png/$name.png"
done

convert -delay 10 -loop 0 frames/png/frame_*.png memory.gif
echo "GIF saved as memory.gif"