import json
import matplotlib.pyplot as plt

FREE_COLOR = "#7fc97f"
USED_COLOR = "#fb8072"

with open("memory.json", "r") as f:
    data = json.load(f)

blocks = data["blocks"]

sizes = [block["size"] for block in blocks]
colors = [FREE_COLOR if block["free"] else USED_COLOR for block in blocks]
labels = ["FREE" if block["free"] else "USED" for block in blocks]

y_positions = list(range(len(sizes)))
left_positions = [sum(sizes[:i]) for i in range(len(sizes))]

plt.figure(figsize=(14, 4))
plt.barh(y_positions, sizes, color=colors, left=left_positions)

# labels
for i in range(len(sizes)):
    x = left_positions[i] + sizes[i] / 2
    plt.text(x, y_positions[i], labels[i], ha='center', va='center',
             fontsize=8, color="black")

plt.title("Memory Layout Visualization")
plt.yticks(y_positions, [f"Block {i}" for i in y_positions])
plt.xlabel("Bytes")
plt.tight_layout()
plt.savefig("memory.png", dpi=200)
print("Visualization saved to memory.png")
