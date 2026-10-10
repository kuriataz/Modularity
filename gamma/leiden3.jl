using Graphs
using DataFrames
using CSV
using PythonCall
using Random

const ig = pyimport("igraph")
const la = pyimport("leidenalg")

# --- Wczytywanie i konwersja grafu ---

function load_real_network(filepath::String)
    df = CSV.read(filepath, DataFrame, header=false, comment="#")
    
    if typeof(df[1, 1]) == String && startswith(df[1, 1], "%")
        df = CSV.read(filepath, DataFrame, header=false, comment="%")
    end

    dropmissing!(df)
    
    if ncol(df) < 2
        error("Error: only one column.")
    end

    unique_nodes = unique(vcat(df[:, 1], df[:, 2]))
    num_nodes = length(unique_nodes)
    
    node_map = Dict(orig_id => new_id for (new_id, orig_id) in enumerate(unique_nodes))
    g = SimpleGraph(num_nodes)
    
    for row in eachrow(df)
        u = node_map[row[1]]
        v = node_map[row[2]]
        if u != v 
            add_edge!(g, u, v)
        end
    end
    
    println("\n|V| = $(nv(g))")
    println("|E| = $(ne(g))\n")
    
    return g
end

function convert_to_igraph(g::SimpleGraph)
    edge_list = [(src(e)-1, dst(e)-1) for e in edges(g)]
    py_edges = pylist(edge_list)
    ig_graph = ig.Graph(edges=py_edges, directed=false)
    return ig_graph
end

# --- Obliczanie metryk (Modularity & Mod*) ---

function calculate_mod_star(g::SimpleGraph, membership::Vector{Int})
    m = ne(g)
    if m == 0 
        return 0.0 
    end
    
    mod_star = 0.0
    communities = unique(membership)
    
    for c in communities
        nodes_in_comm = findall(==(c), membership)
        size_A = length(nodes_in_comm)
        
        subg, _ = induced_subgraph(g, nodes_in_comm)
        
        e_A = ne(subg)
        vol_A = sum(degree(g, v) for v in nodes_in_comm)
        
        Q_A = (e_A / m) - (vol_A / (2 * m))^2
        
        max_deg = size_A > 0 ? maximum(degree(subg)) : 0
        star_factor = (max_deg + 1) / size_A
        
        mod_star += Q_A * star_factor
    end
    
    return mod_star
end

function calc_single_comm_mod_star(g::SimpleGraph, nodes_in_comm::Vector{Int}, m::Int)
    size_A = length(nodes_in_comm)
    if size_A == 0
        return 0.0
    end
    
    subg, _ = induced_subgraph(g, nodes_in_comm)
    e_A = ne(subg)
    vol_A = sum(degree(g, v) for v in nodes_in_comm)
    
    Q_A = (e_A / m) - (vol_A / (2 * m))^2
    
    max_deg = size_A > 0 ? maximum(degree(subg)) : 0
    star_factor = (max_deg + 1) / size_A
    
    return Q_A * star_factor
end
function find_partition_mod_star(g::SimpleGraph; max_iter::Int=10)
    n = nv(g)
    m = ne(g)
    if m == 0
        return collect(1:n)
    end
    
    # Na starcie każdy oryginalny węzeł tworzy swój własny superwęzeł
    # supernodes[i] przechowuje wektor oryginalnych ID węzłów tworzących dany blok
    supernodes = [[i] for i in 1:n]
    
    # Globalny podział oryginalnych węzłów
    current_membership = collect(1:n)
    
    global_improved = true
    level = 0
    
    while global_improved && level < max_iter
        level += 1
        global_improved = false
        println("   -> [Leiden Poziom $level] Przetwarzam $(length(supernodes)) superwęzłów...")
        
        # =================================================================
        # FAZA 1: Lokalna optymalizacja superwęzłów (Ruchy blokowe)
        # =================================================================
        num_s = length(supernodes)
        s_membership = zeros(Int, num_s)
        
        # Inicjalizacja: superwęzły dziedziczą społeczność z poprzedniego poziomu
        for i in 1:num_s
            s_membership[i] = current_membership[supernodes[i][1]]
        end
        
        phase1_improved = true
        while phase1_improved
            phase1_improved = false
            s_indices = shuffle(1:num_s)
            
            for i in s_indices
                curr_c = s_membership[i]
                nodes_in_s = supernodes[i]
                
                # Znajdź oryginalne węzły sąsiadujące z naszym superwęzłem
                all_neigh_nodes = Int[]
                for u in nodes_in_s
                    append!(all_neigh_nodes, neighbors(g, u))
                end
                unique!(all_neigh_nodes)
                
                # Zbierz społeczności, do których należą sąsiednie superwęzły
                neighbor_comms = Int[]
                for idx in 1:num_s
                    if idx != i && any(u -> u in supernodes[idx], all_neigh_nodes)
                        push!(neighbor_comms, s_membership[idx])
                    end
                end
                unique!(neighbor_comms)
                filter!(c -> c != curr_c, neighbor_comms)
                
                if isempty(neighbor_comms) continue end
                
                # Kopia zapasowa globalnego podziału do testów nieliniowego Mod*
                orig_mem_temp = copy(current_membership)
                best_c = curr_c
                best_score = calculate_mod_star(g, orig_mem_temp)
                
                # Testujemy przeniesienie CAŁEGO superwęzła do sąsiednich społeczności
                for c_cand in neighbor_comms
                    for u in nodes_in_s
                        orig_mem_temp[u] = c_cand
                    end
                    
                    score_cand = calculate_mod_star(g, orig_mem_temp)
                    
                    # Dokładne porównanie zmienionego Mod* całego grafu
                    if score_cand > best_score + 1e-7
                        best_score = score_cand
                        best_c = c_cand
                        phase1_improved = true
                        global_improved = true
                    end
                    
                    # Przywracamy stan do kolejnego testu
                    for u in nodes_in_s
                        orig_mem_temp[u] = curr_c
                    end
                end
                
                # Jeśli znaleziono lepszą społeczność, aktualizujemy stan permanentnie
                if best_c != curr_c
                    s_membership[i] = best_c
                    for u in nodes_in_s
                        current_membership[u] = best_c
                    end
                end
            end
        end
        
        # Czyszczenie i normalizacja indeksów społeczności po Fazie 1
        unique_c = unique(current_membership)
        c_map = Dict(old => new for (new, old) in enumerate(unique_c))
        for u in 1:n
            current_membership[u] = c_map[current_membership[u]]
        end
        
        # Jeśli Faza 1 na tym poziomie nie przyniosła żadnej poprawy, kończymy całościowy algorytm
        if !global_improved
            break
        end
        
        # =================================================================
        # FAZA 2: Refinement (Wyrafinowanie podziału)
        # =================================================================
        # Zaczynamy od nowa jako singletony, ale optymalizujemy wewnątrz klastrów Fazy 1
        refine_membership = collect(1:n)
        refine_improved = true
        refine_loop = 0
        
        while refine_improved && refine_loop < 5
            refine_improved = false
            refine_loop += 1
            nodes = shuffle(1:n)
            
            for v in nodes
                v_phase1_comm = current_membership[v]
                v_refine_comm = refine_membership[v]
                
                neighs = neighbors(g, v)
                
                # Warunek klasycznego Leidena: węzeł może przejść do innej podspołeczności 
                # TYLKO wtedy, gdy ta podspołeczność w całości leży wewnątrz jego społeczności z Fazy 1
                candidate_refine_comms = Int[]
                for u in neighs
                    if current_membership[u] == v_phase1_comm && refine_membership[u] != v_refine_comm
                        push!(candidate_refine_comms, refine_membership[u])
                    end
                end
                unique!(candidate_refine_comms)
                
                if isempty(candidate_refine_comms) continue end
                
                orig_mem_temp = copy(refine_membership)
                best_r_c = v_refine_comm
                best_score = calculate_mod_star(g, orig_mem_temp)
                
                for r_c in candidate_refine_comms
                    orig_mem_temp[v] = r_c
                    score_cand = calculate_mod_star(g, orig_mem_temp)
                    
                    if score_cand > best_score + 1e-7
                        best_score = score_cand
                        best_r_c = r_c
                        refine_improved = true
                    end
                    orig_mem_temp[v] = v_refine_comm
                end
                
                if best_r_c != v_refine_comm
                    refine_membership[v] = best_r_c
                end
            end
        end
        
        # =================================================================
        # FAZA 3: Agregacja (Przebudowa superwęzłów na kolejny poziom hierarchii)
        # =================================================================
        # Nowe bloki budujemy na podstawie podziału z fazy wyrafinowania (Refinement)
        unique_refine = unique(refine_membership)
        new_supernodes = Vector{Int}[]
        for r_c in unique_refine
            push!(new_supernodes, findall(==(r_c), refine_membership))
        end
        supernodes = new_supernodes
    end
    
    return current_membership
end

# --- Główna pętla eksperymentalna ---

function run_all_experiments(g::SimpleGraph, gamma_values::Vector{Float64})
    ig_graph = convert_to_igraph(g)
    
    # Dodana kolumna "Algorithm", aby rozróżnić Leiden od Twojej metody
    results = DataFrame(
        Algorithm = String[],
        Gamma = Float64[], 
        Standard_Modularity = Float64[], 
        Mod_Star = Float64[],
        Num_Communities = Int[]
    )
    
    memberships_df = DataFrame(Node = 1:nv(g))
    
    println("algorithm,gamma,communities,mod,mod*")
    
    # 1. Eksperymenty z użyciem Leiden
    for gamma in gamma_values
        partition = la.find_partition(
            ig_graph, 
            la.RBConfigurationVertexPartition, 
            resolution_parameter=gamma
        )
        
        membership = pyconvert(Vector{Int}, partition.membership) .+ 1
        num_comm = length(unique(membership))
        
        std_mod = modularity(g, membership)
        mod_star = calculate_mod_star(g, membership)
        
        memberships_df[!, "Gamma_$(gamma)"] = membership
        
        push!(results, ("Leiden", gamma, std_mod, mod_star, num_comm))
        println("Leiden,$gamma,$num_comm,$(round(std_mod, digits=4)),$(round(mod_star, digits=4))")
    end
    
    # 2. Eksperyment z użyciem zachłannego algorytmu Mod*
    println("\nRozpoczynam autorską optymalizację Mod*...")
    custom_membership = find_partition_mod_star(g, max_iter=15)
    
    custom_num_comm = length(unique(custom_membership))
    custom_std_mod = modularity(g, custom_membership)
    custom_mod_star_score = calculate_mod_star(g, custom_membership)
    
    memberships_df[!, "Custom_ModStar"] = custom_membership
    push!(results, ("Custom", 0.0, custom_std_mod, custom_mod_star_score, custom_num_comm))
    
    println("Custom,0.0,$custom_num_comm,$(round(custom_std_mod, digits=4)),$(round(custom_mod_star_score, digits=4))")
    
    return results, memberships_df
end

# --- Blok uruchomieniowy skryptu ---

if length(ARGS) < 1
    filepath = "data/edges.csv" 
    println("No input filepath, using $(filepath).")
else
    filepath = ARGS[1]
end

file_basename = splitext(basename(filepath))[1]
g = load_real_network(filepath)

gammas = collect(5.0:5.0:100.0)
if !(1.0 in gammas)
    push!(gammas, 1.0)
    sort!(gammas)
end

# Uruchomienie wszystkich analiz na raz
experiment_data, memberships_data = run_all_experiments(g, gammas)

# Zapis do plików CSV (włączając nową kolumnę Algorithm i kolumnę Custom_ModStar)
CSV.write("results/$(file_basename)_leiden.csv", experiment_data)
CSV.write("results/$(file_basename)_comm.csv", memberships_data)

println("\nZakończono! Wyniki zapisano w results/$(file_basename)_leiden.csv oraz _comm.csv")