using Graphs
using DataFrames
using CSV
using Plots
using NetworkLayout
using Colors

ENV["GKSwstype"] = "100"

function load_real_network(filepath::String)
    df = CSV.read(filepath, DataFrame, header=false, comment="#")
    if typeof(df[1, 1]) == String && startswith(df[1, 1], "%")
        df = CSV.read(filepath, DataFrame, header=false, comment="%")
    end
    dropmissing!(df)
    
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
    return g
end

function find_peaks(results_filepath::String)
    df = CSV.read(results_filepath, DataFrame)
    
    # Zabezpieczenie: szukamy szczytów tylko wśród wyników z algorytmu Leiden
    if "Algorithm" in names(df)
        df_leiden = df[df.Algorithm .== "Leiden", :]
    else
        df_leiden = df
    end
    
    peak_mod = df_leiden[argmax(df_leiden.Standard_Modularity), :Gamma]
    peak_star  = df_leiden[argmax(df_leiden.Mod_Star), :Gamma]
    
    return peak_mod, peak_star
end

# Zmieniono target_gamma na col_name oraz dostosowano tytuł (title_text)
function plot_expanded_original(g::SimpleGraph, layout_pos, memberships_filepath::String, col_name::String, title_text::String, filepath::String)
    mem_df = CSV.read(memberships_filepath, DataFrame)
    membership = mem_df[!, col_name]

    xs = [p[1] for p in layout_pos]
    ys = [p[2] for p in layout_pos]

    x_margin = (maximum(xs) - minimum(xs)) * 0.05
    y_margin = (maximum(ys) - minimum(ys)) * 0.05

    p = plot(title = title_text,
             size = (3000, 3000), 
             background_color = RGB(0.82, 0.82, 0.82), 
             xlims = (minimum(xs) - x_margin, maximum(xs) + x_margin),
             ylims = (minimum(ys) - y_margin, maximum(ys) + y_margin),
             legend = false,
             grid = false,
             showaxis = false,
             axis = nothing,
             border = :none)

    for e in edges(g)
        u, v = src(e), dst(e)
        plot!(p, [xs[u], xs[v]], [ys[u], ys[v]], 
              linewidth = 0.4, 
              linecolor = RGB(0.3, 0.3, 0.3), 
              linealpha = 0.5)
    end

    scatter!(p, xs, ys,
             markersize = 9, 
             color = membership,
             palette = :Set3_12,
             markerstrokewidth = 0.6,
             markerstrokecolor = :black)
             
    annotations = [(xs[i], ys[i], text(string(membership[i]), 6, :black, :center)) for i in 1:nv(g)]
    annotate!(p, annotations)

    # Sanityzacja nazwy pliku w oparciu o podany tytuł
    safe_title = replace(title_text, " " => "_", "=" => "_", "(" => "", ")" => "")
    filename = "graphs/$(replace(filepath, ".csv" => "", "data/" => ""))_$(safe_title).png"
    savefig(p, filename)
end

if length(ARGS) < 1
    filepath = "data/edges.csv" 
    println("No input filepath, using $(filepath).")
else
    filepath = ARGS[1]
end

file_basename = splitext(basename(filepath))[1]
results_csv = "results/$(file_basename)_leiden.csv"
comm_csv = "results/$(file_basename)_comm.csv"

if !isfile(results_csv) || !isfile(comm_csv)
    println("No results for '$file_basename'.")
    exit(1)
end

g = load_real_network(filepath)

peak_mod, peak_star = find_peaks(results_csv)

pos = spring(g, C=2.0)

# Wykresy dla algorytmu Leiden
plot_expanded_original(g, pos, comm_csv, "Gamma_$peak_mod", "Mod", filepath)
plot_expanded_original(g, pos, comm_csv, "Gamma_$peak_star",  "Mod_Star", filepath)

# Wykres dla autorskiego algorytmu
plot_expanded_original(g, pos, comm_csv, "Custom_ModStar", "ModStar2", filepath)