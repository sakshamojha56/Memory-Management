import json, matplotlib.pyplot as plt, sys

FREE = "#7fc97f"
USED = "#fb8072"

def render(frame_json, output_png):
    with open(frame_json) as f:
        data = json.load(f)

    blocks = data["blocks"]
    sizes = [b["size"] for b in blocks]
    colors = [FREE if b["free"] else USED for b in blocks]

    plt.figure(figsize=(12, 1.5))
    plt.barh([0]*len(sizes), sizes, color=colors,
             left=[sum(sizes[:i]) for i in range(len(sizes))])
    plt.axis('off')
    plt.savefig(output_png, dpi=150, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    render(sys.argv[1], sys.argv[2])
