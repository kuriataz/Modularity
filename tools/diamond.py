import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


def draw_graph(ax, clusters, title=None):
    G = nx.Graph()

    diamond_edges = [
        ('a', 'b'),
        ('a', 'c'),
        ('b', 'c'),
        ('a', 'd'),
        ('b', 'd')
    ]
    G.add_edges_from(diamond_edges)

    path_edges = [
        ('p1', 'p2'),
        ('p2', 'p3'),
        ('p3', 'p4')
    ]
    G.add_edges_from(path_edges)

    edges_from_d = [
        ('d', 'p1'),
        ('d', 'p2'),
        ('d', 'p3'),
        ('d', 'p4')
    ]
    G.add_edges_from(edges_from_d)

    pos = {
        'c': (0, 1.2),
        'a': (-1.2, 0),
        'b': (1.2, 0),
        'd': (0, -1.2),
        'p1': (-2.25, -2.5),
        'p2': (-0.75, -2.5),
        'p3': (0.75, -2.5),
        'p4': (2.25, -2.5)
    }

    cluster_colors = {
        'C1': "#307CC3",
        'C2': "#0B823B",
        'C3': "#B42249",
        'X': "#0C0A0A"
    }

    node_colors = [
        cluster_colors[clusters[n]]
        for n in G.nodes()
    ]

    nx.draw_networkx_edges(
        G, pos, ax=ax,
        edgelist=diamond_edges,
        width=2,
        edge_color='#2D3748'
    )

    nx.draw_networkx_edges(
        G, pos, ax=ax,
        edgelist=path_edges,
        width=2,
        edge_color='#2D3748'
    )

    nx.draw_networkx_edges(
        G, pos, ax=ax,
        edgelist=edges_from_d,
        width=2,
        edge_color='#2D3748'
    )

    nx.draw_networkx_nodes(
        G, pos, ax=ax,
        node_color=node_colors,
        node_size=1600
    )

    labels = {
        'c': r'$c$',
        'a': r'$a$',
        'b': r'$b$',
        'd': r'$d$',
        'p1': r'$p_1$',
        'p2': r'$p_2$',
        'p3': r'$p_3$',
        'p4': r'$p_4$'
    }

    nx.draw_networkx_labels(
        G, pos,
        labels=labels,
        ax=ax,
        font_size=11,
        font_color='white'
    )

    if title:
        ax.set_title(title, fontsize=14, pad=15)

    ax.axis('off')
    ax.set_xlim(-3.0, 3.0)
    ax.set_ylim(-3.8, 1.8)


def visualise_proof_scenarios(save_path="t2a.png"):

    clusters_1 = {
        'a': 'C2',
        'c': 'C2',
        'b': 'C1',
        'd': 'C1',
        'p1': 'X',
        'p2': 'X',
        'p3': 'X',
        'p4': 'X'
    }
    clusters_2 = {
        'a': 'C2',
        'c': 'C2',
        'b': 'C2',
        'd': 'C1',
        'p1': 'X',
        'p2': 'X',
        'p3': 'X',
        'p4': 'X'
    }

    fig, axes = plt.subplots(
        1, 2,
        figsize=(16, 8),
        dpi=300
    )

    draw_graph(
        axes[0],
        clusters_1,
        title=r"$\mathcal{C}$"
    )

    draw_graph(
        axes[1],
        clusters_2,
        title=r"$\mathcal{C}^{\prime}$"
    )

    legend_handles = [
        mpatches.Patch(
            color="#307CC3",
            label=r'$C_1$'
        ),
        mpatches.Patch(
            color="#0B823B",
            label=r'$C_2$'
        ),
        mpatches.Patch(
            color="#0C0A0A",
            label=r'$V \setminus (C_1 \cup C_2)$'
        )
    ]

    fig.legend(
        handles=legend_handles,
        loc='upper center',
        ncol=2,
        fontsize=11,
        frameon=True,
        facecolor='white',
        edgecolor='#CBD5E0'
    )

    plt.tight_layout(rect=[0, 0, 1, 0.92])

    plt.savefig(
        save_path,
        dpi=300,
        bbox_inches='tight'
    )

    plt.close()


if __name__ == "__main__":
    visualise_proof_scenarios()