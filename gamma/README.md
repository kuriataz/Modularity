

## Structure
* **`data/`**
    * `*.csv` - graph edge data downloaded from https://networks.skewed.de/
    * `database.txt` - links to each graph's source, |V|, |E|, (reflections)
* **`graphs/`** - visualizations of the best found partitions for mod and mod*
* **`results/`** - temp data files for `plot.jl`
* **`leiden.jl`** - leiden algorithm finds best partitions (maximizing mod) for different gammas, then calculates their mod score and mod* score
* **`plot.jl`** - finds best gammas for mod and mod*, generates vizualizations

## Usage
julia leiden.jl data/pdz.csv (gamma range should be chosen correctly)
julia plot.jl data/pdz.csv