import networkx as nx
import matplotlib.pyplot as plt

def draw_schematic_graph():
    G = nx.Graph()
    pos = {}
    
    # --- 1. Ścieżka (4 wierzchołki) ---
    core_nodes = ['C0', 'C1', 'C2', 'C3']
    nx.add_path(G, core_nodes)
    
    # Pozycjonowanie ścieżki centralnie pod każdą z 4 głównych grup
    pos['C0'] = (1, 0)
    pos['C1'] = (5, 0)
    pos['C2'] = (9, 0)
    pos['C3'] = (13, 0)

    # --- 2. Konfiguracja Partycjonowania (Słownik Węzeł -> Kolor) ---
    community_colors = {}
    
    main_colors = ['#FF99CC', '#33CC66', '#3399FF', '#9966CC'] 
    for i, c_node in enumerate(core_nodes):
        community_colors[c_node] = main_colors[i]

    # Współrzędne X dla reprezentatywnych diamentów
    # Indeksy 0-7: 4 główne społeczności (po 2 diamenty rozdzielone trzykropkiem)
    # Indeksy 8-11: Osobne społeczności (pojedyncze diamenty na końcu)
    x_positions = [0, 2, 4, 6, 8, 10, 12, 14, 16, 17, 20, 21]
    
    distinct_colors = ['#FF0000', '#FFFF00', '#0033CC', '#FF9933'] 

    # --- 3. Budowa Diamentów ---
    for i, x in enumerate(x_positions):
        top, left, right, bottom = f'D{i}_T', f'D{i}_L', f'D{i}_R', f'D{i}_B'
        
        G.add_edges_from([(left, top), (right, top), (left, bottom), (right, bottom), (left, right)])
        
        pos[bottom] = (x, 10)
        pos[left] = (x - 0.45, 11)
        pos[right] = (x + 0.45, 11)
        pos[top] = (x, 12)
        
        for c in core_nodes:
            G.add_edge(bottom, c)
            
        # Logika przypisywania kolorów
        if i < 8:
            # Dwa diamenty na każdą główną społeczność (np. i=0 i i=1 to pierwsza grupa)
            color = main_colors[i // 2]
        else:
            # Osobne społeczności na końcu
            color = distinct_colors[i - 8]
            
        community_colors[top] = color
        community_colors[left] = color
        community_colors[right] = color
        community_colors[bottom] = color

    # --- 4. Renderowanie ---
    plt.figure(figsize=(26, 10))
    
    node_colors_list = [community_colors[node] for node in G.nodes()]
    
    nx.draw_networkx_edges(G, pos, alpha=0.6, width=1.5, edge_color='black')
    nx.draw_networkx_nodes(G, pos, node_color=node_colors_list, 
                           node_size=200, edgecolors='black', linewidths=1.2)
                           
    # Rysowanie trzykropków dla 4 głównych społeczności
    plt.text(1, 11, '...', fontsize=40, ha='center', va='center', fontweight='bold', color='black')
    plt.text(5, 11, '...', fontsize=40, ha='center', va='center', fontweight='bold', color='black')
    plt.text(9, 11, '...', fontsize=40, ha='center', va='center', fontweight='bold', color='black')
    plt.text(13, 11, '...', fontsize=40, ha='center', va='center', fontweight='bold', color='black')
    
    # Rysowanie trzykropka oddzielającego pojedyncze diamenty
    plt.text(18.5, 11, '...', fontsize=50, ha='center', va='center', fontweight='bold', color='black')
    
    plt.axis('off')
    plt.tight_layout()
    plt.savefig("modularity_schematic_updated.png", dpi=300, bbox_inches='tight')
    plt.show()

draw_schematic_graph()