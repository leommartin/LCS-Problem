# Compilador e flags
CC := gcc
CFLAGS := -Wall -fopenmp -O3

# Diretório para executáveis
BIN := bin

# Lista de executáveis
EXE := lcs_seq lcs_parallel lcs_pblocks

# Regra padrão
all: $(EXE)

# Casos genéricos
lcs_parallel: lcs_parallel.c | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $<

lcs_pblocks: lcs_pblocks.c | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $<

# Caso especial: lcs_seq vem de lcs.c
lcs_seq: lcs.c | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $<

# Diretório de saída
$(BIN):
	mkdir -p $(BIN)

# Regra para executar (com argumentos opcionais)
run_%: % 
	@echo "Executando $(BIN)/$*:"
	@$(BIN)/$* $(ARGS)

# Limpeza
clean:
	rm -rf $(BIN)

.PHONY: all clean run
