# Modularity Analysis Graph (MAG)

## Opis Projektu

Projekt **MAG (Modularity Analysis Graph)** służy do analizy modularity oraz modularity star. Pozwala generować grafy oraz partycje zbioru ich wierzchołków i badać dla nich wartości modularity i modularity star. Narzędzie zostało przystosowane do poszukiwań grafu, dla którego mod* > mod.

---

## Struktura Projektu

Układ plików źródłowych w projekcie:

* **`core/`**: Główne pliki logiczne programu obliczeniowego.
    * `graph.hpp/cpp`: Struktury danych grafu i obliczanie modularity oraz modularity star.
    * `modularity.cpp`: Główny program obliczeniowy (`mod`).
    * `options.hpp/cpp`: Obsługa argumentów linii poleceń i parsowanie plików.
* **`gen/`**: Generatory grafów typu Swarms-Path.
    * `gen1.cpp`
    * `gen2.cpp`
    * `partitions.hpp/cpp`: Logika tworzenia partycji.
* **`bipartite/`**: Generatory grafów dwudzielnych.
    * `bip1.cpp`, `bip2.cpp`, `bip3.cpp`
* **`Makefile`**: Skrypt kompilacji.
* **`run_mod.sh`**: Skrypt automatyzujący testy.

---

## Kompilacja i Wymagania

Projekt wykorzystuje system `Makefile` do automatyzacji procesu budowania.

### Wymagania
* Kompilator C++ wspierający standard **C++17** (np. `g++`, `clang++`).
* Narzędzie **GNU Make**.

### Instrukcja Budowania

Aby skompilować programy, użyj następujących poleceń w katalogu głównym:

| Polecenie | Opis |
| :--- | :--- |
| `make all` | Kompilacja wszystkich programów (`mod`, generatory `gen` i `bip`). |
| `make mod` | Kompilacja tylko głównego programu obliczeniowego `mod`. |
| `make gen1` / `make gen2` | Kompilacja konkretnego generatora grafów Swarms-Path. |
| `make bip1` / `bip2` / `bip3` | Kompilacja konkretnego generatora grafów dwudzielnych. |
| `make clean` | Usunięcie plików wykonywalnych i obiektowych. |

---

## Format Plików Wejściowych

Program `mod` wykorzystuje specyficzny format tekstowy.

### 1. Plik Grafu (`.txt`)
Pierwsza linia to liczba wierzchołków $N$. Kolejne linie to komendy:

| Prefiks | Składnia | Opis |
| :--- | :--- | :--- |
| *(brak)* | `u v` | Krawędź między `u` i `v`. |
| `m` | `m u start end` | Krawędzie od `u` do wszystkich w `[start, end)`. |
| `k` | `k start end` | Klika (każdy z każdym) w zakresie `[start, end)`. |
| `c` | `c start end` | Cykl w zakresie `[start, end)`. |
| `x` | `x u v` | Usunięcie krawędzi między `u` i `v`. |

### 2. Plik Partycji (`.txt`)
Każda linia definiuje jeden klaster.
* `v1 v2 ...` : Lista wierzchołków.
* `k start end` : Zakres wierzchołków `[start, end)` w klastrze.

---

## Instrukcja Użycia

### 1. Skrypt Automatyzujący (`run_mod.sh`)

Skrypt uruchamia `mod` dla wszystkich partycji wygenerowanych dla danego grafu.

1.  **Wygeneruj graf i partycje:**
    ```bash
    ./gen1 10 10 2 20 5 1 0 test
    ```
2.  **Uruchom obliczenia:**
    (Skrypt automatycznie znajdzie plik `test.txt` oraz wszystkie `testP*.txt`)
    ```bash
    ./run_mod.sh test
    ```

### 2. Ręczne Uruchamianie Programów

#### A. Główny program `mod`
```bash
./mod -f <graph_file> -p <partition_file>
```

#### B. Generatory Swarms-Path
```bash
./genX <num_swarms> <swarm_size> <swarms_in_cluster> <path_size> <path_frac> <num_connectors> <num_single> <file_name>
```

#### Generatory Grafów Dwudzielnych
```bash
./bip1 <p1_size> <p2_size> <cluster1_size> <file_name>
./bip2 <p1_size> <p2_size> <cluster1_size> <edges_to_remove> <file_name>
./bip3 <p1_size> <p2_size> <edges_to_remove> <file_name>
```
