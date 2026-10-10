# Generatory Grafów Dwudzielnych

Narzędzia do generowania grafów dwudzielnych (lub prawie dwudzielnych) i badania wpływu usuwania krawędzi na ich modularity oraz modularity star.

## Opis Generatorów

### `bip1.cpp`
Generuje kompletny graf dwudzielny $K_{s,t}$.
* Nie usuwa krawędzi.
* Generuje dwie partycje:
    * **P0**: Horyzontalna.
    * **P1**: Naiwna.
### `bip2.cpp`
Generuje graf dwudzielny, a następnie **usuwa** z niego pewną liczbę krawędzi w sposób deterministyczny (pomiędzy zdefiniowanymi klastrami).
* Pozwala użytkownikowi określić rozmiar pierwszego klastra (`cluster1_size`).
* Generuje dwie partycje:
    * **P0**: Oparta na podanym rozmiarze klastra.
    * **P1**: Dzieli graf idealnie na pół (połowa wierzchołków z każdej grupy).

### `bip3.cpp`
Uproszczona wersja `bip2`.
* Zawsze zakłada podział na klastry typu "pół na pół" ($\lceil size/2 \rceil$).
* Służy do szybkiego testowania wpływu usuwania krawędzi przy symetrycznym podziale, bez konieczności definiowania rozmiaru klastra ręcznie.
