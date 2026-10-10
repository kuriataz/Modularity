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

# --- Autorski algorytm zachłanny dla Mod* ---

function find_partition_mod_star(g::SimpleGraph; max_iter::Int=20)
    n = nv(g)
    m = ne(g)
    if m == 0
        return collect(1:n)
    end
    
    # Inicjalizacja: każdy węzeł jako osobna społeczność
    membership = collect(1:n)
    global_improvement = true
    outer_iter = 0
    
    while global_improvement && outer_iter < max_iter
        outer_iter += 1
        global_improvement = false
        
        println("   -> Custom Leiden-Mod* (Poziom $outer_iter):")
        
        # =================================================================
        # FAZA 1: Optymalizacja na poziomie pojedynczych węzłów (Mikroruchy)
        # =================================================================
        node_improvement = true
        node_iter = 0
        while node_improvement && node_iter < 10
            node_improvement = false
            node_iter += 1
            nodes = shuffle(1:n)
            
            for v in nodes
                current_comm = membership[v]
                neighs = neighbors(g, v)
                
                neighbor_comms = unique(membership[u] for u in neighs)
                filter!(c -> c != current_comm, neighbor_comms)
                
                if isempty(neighbor_comms) continue end
                
                nodes_current = findall(==(current_comm), membership)
                score_current_old = calc_single_comm_mod_star(g, nodes_current, m)
                
                nodes_current_new = filter(x -> x != v, nodes_current)
                score_current_new = calc_single_comm_mod_star(g, nodes_current_new, m)
                
                best_comm = current_comm
                best_increase = 0.0
                
                for candidate_comm in neighbor_comms
                    nodes_cand = findall(==(candidate_comm), membership)
                    score_cand_old = calc_single_comm_mod_star(g, nodes_cand, m)
                    
                    nodes_cand_new = copy(nodes_cand)
                    push!(nodes_cand_new, v)
                    score_cand_new = calc_single_comm_mod_star(g, nodes_cand_new, m)
                    
                    delta = (score_current_new + score_cand_new) - (score_current_old + score_cand_old)
                    
                    if delta > best_increase
                        best_increase = delta
                        best_comm = candidate_comm
                    end
                end
                
                if best_comm != current_comm
                    membership[v] = best_comm
                    node_improvement = true
                    global_improvement = true
                end
            end
        end
        
        # =================================================================
        # FAZA 2: Agregacja i optymalizacja społeczności (Makroruchy)
        # =================================================================
        comm_improvement = true
        comm_iter = 0
        while comm_improvement && comm_iter < 5
            comm_improvement = false
            comm_iter += 1
            
            unique_comms = shuffle(unique(membership))
            if length(unique_comms) <= 1 break end
            
            for c_old in unique_comms
                # Sprawdźmy, czy ta społeczność w ogóle jeszcze istnieje 
                # (mogła zostać wchłonięta w tej samej iteracji)
                nodes_c_old = findall(==(c_old), membership)
                if isempty(nodes_c_old) continue end
                
                # Znajdź sąsiednie społeczności dla CAŁEJ społeczności c_old
                all_neighbors = Int[]
                for v in nodes_c_old
                    append!(all_neighbors, neighbors(g, v))
                end
                neighbor_comms = unique(membership[u] for u in all_neighbors)
                filter!(c -> c != c_old, neighbor_comms)
                
                if isempty(neighbor_comms) continue end
                
                # Koszt obecny: wynik c_old osobno
                score_c_old = calc_single_comm_mod_star(g, nodes_c_old, m)
                
                best_target_comm = c_old
                best_comm_increase = 0.0
                
                # Symulujemy połączenie społeczności c_old ze społecznością kandydacką
                for c_cand in neighbor_comms
                    nodes_c_cand = findall(==(c_cand), membership)
                    score_c_cand = calc_single_comm_mod_star(g, nodes_c_cand, m)
                    
                    # Połączenie grup
                    nodes_merged = vcat(nodes_c_old, nodes_c_cand)
                    score_merged = calc_single_comm_mod_star(g, nodes_merged, m)
                    
                    # Delta = Nowy połączony wynik - dwa stare osobne wyniki
                    delta = score_merged - (score_c_old + score_c_cand)
                    
                    if delta > best_comm_increase
                        best_comm_increase = delta
                        best_target_comm = c_cand
                    end
                end
                
                # Jeśli opłaca się scalić całe społeczności
                if best_target_comm != c_old
                    membership[nodes_c_old] .= best_target_comm
                    comm_improvement = true
                    global_improvement = true
                end
            end
        end
        
        # Czyszczenie identyfikatorów społeczności (opcjonalne, dla porządku)
        # Zamienia np. [10, 10, 45, 45] na [1, 1, 2, 2]
        unique_mapping = unique(membership)
        comm_map = Dict(old_id => new_id for (new_id, old_id) in enumerate(unique_mapping))
        membership = [comm_map[c] for c in membership]
    end
    
    return membership
end

# function find_partition_mod_star(g::SimpleGraph; max_iter::Int=10)
#     n = nv(g)
#     m = ne(g)
#     if m == 0
#         return collect(1:n)
#     end
    
#     membership = collect(1:n)
#     improvement = true
#     iter = 0
    
#     while improvement && iter < max_iter
#         improvement = false
#         iter += 1
#         println("   -> Custom ModStar: Iteracja $iter...")
        
#         nodes = shuffle(1:n)
        
#         for v in nodes
#             current_comm = membership[v]
#             neighs = neighbors(g, v)
            
#             neighbor_comms = unique(membership[u] for u in neighs)
#             filter!(c -> c != current_comm, neighbor_comms)
            
#             if isempty(neighbor_comms)
#                 continue
#             end
            
#             nodes_current = findall(==(current_comm), membership)
#             score_current_old = calc_single_comm_mod_star(g, nodes_current, m)
            
#             nodes_current_new = filter(x -> x != v, nodes_current)
#             score_current_new = calc_single_comm_mod_star(g, nodes_current_new, m)
            
#             best_comm = current_comm
#             best_increase = 0.0
            
#             for candidate_comm in neighbor_comms
#                 nodes_cand = findall(==(candidate_comm), membership)
#                 score_cand_old = calc_single_comm_mod_star(g, nodes_cand, m)
                
#                 nodes_cand_new = copy(nodes_cand)
#                 push!(nodes_cand_new, v)
#                 score_cand_new = calc_single_comm_mod_star(g, nodes_cand_new, m)
                
#                 delta = (score_current_new + score_cand_new) - (score_current_old + score_cand_old)
                
#                 if delta > best_increase
#                     best_increase = delta
#                     best_comm = candidate_comm
#                 end
#             end
            
#             if best_comm != current_comm
#                 membership[v] = best_comm
#                 improvement = true
#             end
#         end
#     end
    
#     return membership
# end

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

gammas = collect(1.0:0.1:6.0)
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