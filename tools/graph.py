import networkx as nx
import matplotlib.pyplot as plt

# Pierwszy graf
G1 = nx.Graph()
G1.add_edges_from([(0, 1), (0, 2), (0, 3), (4, 1), (4, 2), (4, 3)])
partition1 = [{1, 2, 0}, {3, 4}]
colors1 = [0 if node in partition1[0] else 1 for node in G1.nodes]
pos1 = {0: (-1, 0), 1: (0, -1), 2: (0, 0), 3: (0, 1), 4: (-1, 1)}

# Drugi graf
G2 = nx.Graph()
G2.add_edges_from([(0, 1), (0, 2), (0, 3), (4, 1), (4, 2), (4, 3), (1, 2)])
partition2 = partition1
colors2 = [0 if node in partition2[0] else 1 for node in G2.nodes]
pos2 = pos1  # Można użyć tej samej siatki pozycji

# Tworzenie dwóch subplotów
fig, axes = plt.subplots(1, 2, figsize=(12, 6))

# Rysuj pierwszy graf
nx.draw(
    G1,
    pos1,
    ax=axes[0],
    with_labels=True,
    node_color=colors1,
    cmap=plt.cm.Set1,
    node_size=600,
)
axes[0].set_title("G1")

# Rysuj drugi graf
nx.draw(
    G2,
    pos2,
    ax=axes[1],
    with_labels=True,
    node_color=colors2,
    cmap=plt.cm.Set1,
    node_size=600,
)
axes[1].set_title("G2")

plt.tight_layout()
plt.show()
