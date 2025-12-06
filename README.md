# Modularity Analysis Graph (MAG)

## Opis Projektu

Projekt **MAG (Modularity Analysis Graph)** służy do analizy modularity oraz modularity star. Pozwala generować grafy oraz partycje zbioru ich wierzchołków i badać dla nich wartości modularity i modularity star. Narzędzie zostało przystosowane do poszukiwań grafu, dla którego mod* > mod.

---

## Struktura Projektu

Układ plików źródłowych w projekcie:

* **`core/`**: Główne pliki logiczne programu obliczeniowego.
    * `graph.hpp/cpp`: Struktury danych grafu i obliczanie modularności.
    * `modularity.cpp`: Główny program obliczeniowy (`mod`).
    * `options.hpp/cpp`: Obsługa argumentów linii poleceń i parsowanie plików.
* **`gen/`**: Generatory grafów typu Rój-Ścieżka.
    * `gen1.cpp`: Generator oparty na cyklach Hamiltona.
    * `gen2.cpp`: Generator oparty na gwiazdach.
    * `partitions.hpp/cpp`: Logika tworzenia partycji (P0-P3).
* **`bipartite/`**: Generatory grafów dwudzielnych.
    * `bip1.cpp`, `bip2.cpp`, `bip3.cpp`: Różne warianty grafów dwudzielnych.
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
| `make gen1` / `make gen2` | Kompilacja konkretnego generatora grafów Rój-Ścieżka. |
| `make bip1` / `bip2` / `bip3` | Kompilacja konkretnego generatora grafów dwudzielnych. |
| `make clean` | Usunięcie plików wykonywalnych i obiektowych. |

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
./mod -f <plik_grafu> -p <plik_partycji># Modularity Analysis Graph (MAG)
```

#### B. Generatory Swarms-Path
```bash
./genX <num_swarms> <swarm_size> <swarms_in_cluster> <path_size> <path_frac> <num_connectors> <num_single> <nazwa_pliku>
```

#### Generatory Grafów Dwudzielnych
```bash
./bip1 <p1_size> <p2_size> <cluster1_size> <nazwa_pliku>
./bip2 <p1_size> <p2_size> <cluster1_size> <edges_to_remove> <nazwa_pliku>
./bip3 <p1_size> <p2_size> <edges_to_remove> <nazwa_pliku>
```
