# Generatory Swarms-Path

Moduł ten zawiera narzędzia do generowania specyficznej klasy grafów, zaprojektowanych w celu znalezienia garfów, dla których $mod^* > mod$. Grafy te składają się z gęstych skupisk - rojów ("swarms") oraz ścieżki (path).

## Generatory

* **`gen1.cpp`**: Swarm - gwiazda z dodatkowymi krawędziami zapewniająceymi istnienie cyklu Hamiltona.
* **`gen2.cpp`**: Swarm - gwiazda z dodatkowym wierzchołkiem połączonym z dwoma wierzchołkami gwiazdy.

Oba generatory tworzą plik grafu oraz zestaw 4 plików partycji (`P0`–`P3`).

## Strategie Partycjonowania (`partitions.cpp`)

Plik `partitions.cpp` implementuje logikę automatycznego tworzenia różnych podziałów dla wygenerowanego grafu:

* **P0**: Każdy rój jest osobnym klastrem, ścieżka jest osobnym klastrem.
* **P1**: Roje przejmują część ścieżki. Każdy klaster zawiera jeden rój oraz `path_frac` węzłów ze ścieżki. Pozostałe roje tworzą osobne klastry.
* **P2**: Pewna liczba rojów oraz fragmenty ścieżki tworzą klastry. Pozostałe roje tworzą osobne klastry.
* **P3**: Węzły łączące roje ze ścieżką ("connectors") są grupowane ze ścieżką. Pozostałe roje tworzą osobne klastry.