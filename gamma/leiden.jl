using Graphs
using DataFrames
using CSV
using PythonCall

const ig = pyimport("igraph")
const la = pyimport("leidenalg")

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
    
    println("")
    println("|V|=$(nv(g))")
    println("|E|=$(ne(g))")
    println("")
    
    return g
end

function convert_to_igraph(g::SimpleGraph)
    edge_list = [(src(e)-1, dst(e)-1) for e in edges(g)]
    py_edges = pylist(edge_list)
    ig_graph = ig.Graph(edges=py_edges, directed=false)
    return ig_graph
end

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

function run_leiden_experiment(g::SimpleGraph, gamma_values::Vector{Float64})
    ig_graph = convert_to_igraph(g)
    
    results = DataFrame(
        Gamma = Float64[], 
        Standard_Modularity = Float64[], 
        Mod_Star = Float64[],
        Num_Communities = Int[]
    )
    
    memberships_df = DataFrame(Node = 1:nv(g))
    
    println("gamma,communities,mod,mod*")
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
        
        push!(results, (gamma, std_mod, mod_star, num_comm))
        println("$gamma,$num_comm,$(round(std_mod, digits=4)),$(round(mod_star, digits=4))")
    end
    
    return results, memberships_df
end

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

experiment_data, memberships_data = run_leiden_experiment(g, gammas)

CSV.write("results/$(file_basename)_leiden.csv", experiment_data)
CSV.write("results/$(file_basename)_comm.csv", memberships_data)