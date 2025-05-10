#!/bin/bash

total_seq=0

for i in {1..20}; do
    echo "Execução $i:"
    output=$(./bin/lcs_pblocks)

    echo "$output"

    # Extrai a linha com "Tempo sequencial" e pega o valor numérico
    tempo_seq=$(echo "$output" | grep "Tempo sequencial" | awk '{print $3}')

    # Soma ao total
    total_seq=$(echo "$total_seq + $tempo_seq" | bc)
done

# Calcula a média
media_seq=$(echo "scale=4; $total_seq / 20" | bc)

echo ""
echo "Média do tempo sequencial em 20 execuções: $media_seq segundos"
