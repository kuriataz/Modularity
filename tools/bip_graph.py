import networkx as nx
import matplotlib.pyplot as plt

G = nx.Graph()
G.add_edges_from([(0, 3), (0, 4), (0, 5), (1,3), (1,6), (2, 5)])

left = {0, 1, 2}
right = {3, 4, 5, 6}

pos = {}
for i, node in enumerate(left):
    pos[node] = (0, -i)
for i, node in enumerate(right):
    pos[node] = (2, -i)

plt.figure(figsize=(6, 4))
nx.draw(
    G,
    pos,
    with_labels=True,
    node_color="grey",
    node_size=800,
    edgecolors="white",
    font_weight="bold",
    font_color="white",
)

plt.axvspan(-0.5, 0.8, color="#1f4b99", alpha=0.6, label=r"$A_1$")
plt.axvspan(1.2, 2.5, color="#ff6b6b", alpha=0.25, label=r"$A_2$")

plt.legend(loc="lower center")
plt.axis("off")
plt.tight_layout()
plt.savefig("graf_dwudzielny.png")
print("Zapisano graf jako graf_dwudzielny.png")
plt.show()