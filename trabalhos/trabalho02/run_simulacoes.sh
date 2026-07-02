#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

make compila

if [ "$#" -eq 0 ]; then
    algoritmos=("LRU" "NRU" "CLOCK" "OTIMO")
    logs=("compilador.log" "compressor.log" "matriz.log" "simulador.log")
    tamanhos_pagina=(4 8)
    tamanhos_memoria=(1 2 4)

    for algoritmo in "${algoritmos[@]}"; do
        for arquivo in "${logs[@]}"; do
            for tamanho_pagina in "${tamanhos_pagina[@]}"; do
                for tamanho_memoria in "${tamanhos_memoria[@]}"; do
                    echo "========================================"
                    echo "Executando: ./sim-virtual $algoritmo $arquivo $tamanho_pagina $tamanho_memoria"
                    ./sim-virtual "$algoritmo" "$arquivo" "$tamanho_pagina" "$tamanho_memoria"
                done
            done
        done
    done
else
    algoritmo="${1:-OTIMO}"
    arquivo="${2:-compilador.log}"
    tamanho_pagina="${3:-4}"
    tamanho_memoria="${4:-1}"

    echo "Executando: ./sim-virtual $algoritmo $arquivo $tamanho_pagina $tamanho_memoria"
    ./sim-virtual "$algoritmo" "$arquivo" "$tamanho_pagina" "$tamanho_memoria"
fi
