
# ============================================
# MINI-ECU PROJECT - MAKEFILE
# Compilation et tests sur Ubuntu WSL
# ============================================

# 1. Compilateur et options
CC = gcc

CPPFLAGS = -Isrc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O0 -g

DEPFLAGS = -MMD -MP

# 2. Repertoires
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# 3. Fichiers objets des modules
SIGNALS_OBJ = $(OBJ_DIR)/app/signals.o
RINGBUF_OBJ = $(OBJ_DIR)/app/ringbuf.o

# 4. Fichiers objets des tests
TEST_SIGNALS_OBJ = $(OBJ_DIR)/tests/test_signals.o
TEST_RINGBUF_OBJ = $(OBJ_DIR)/tests/test_ringbuf.o

# 5. Executables
TEST_SIGNALS = $(BUILD_DIR)/test_signals
TEST_RINGBUF = $(BUILD_DIR)/test_ringbuf

TEST_BINS = $(TEST_SIGNALS) $(TEST_RINGBUF)

# 6. Cible par defaut
.DEFAULT_GOAL := all

.PHONY: all test map clean help

all: $(TEST_BINS)

# 7. Compilation des modules app
$(OBJ_DIR)/app/%.o: src/app/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# 8. Compilation des fichiers de test
$(OBJ_DIR)/tests/%.o: tests/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# 9. Edition des liens
$(TEST_SIGNALS): $(SIGNALS_OBJ) $(TEST_SIGNALS_OBJ)
	$(CC) $^ -o $@

$(TEST_RINGBUF): $(RINGBUF_OBJ) $(TEST_RINGBUF_OBJ)
	$(CC) $^ -o $@

# 10. Execution des tests
test: all
	./$(TEST_SIGNALS)
	./$(TEST_RINGBUF)

# 11. Generation du rapport memoire
map: all
	$(CC) $(SIGNALS_OBJ) $(TEST_SIGNALS_OBJ) \
	    -Wl,-Map=$(BUILD_DIR)/test_signals.map \
	    -o $(TEST_SIGNALS)
	@echo "MAP genere : $(BUILD_DIR)/test_signals.map"

# 12. Nettoyage
clean:
	rm -rf $(BUILD_DIR)

# 13. Aide
help:
	@echo "Commandes disponibles :"
	@echo "  make       : Compiler les tests"
	@echo "  make test  : Executer les tests"
	@echo "  make map   : Generer le fichier MAP"
	@echo "  make clean : Supprimer build/"
	@echo "  make help  : Afficher cette aide"

# 14. Fichiers de dependances
DEPFILES = $(SIGNALS_OBJ:.o=.d) \
           $(RINGBUF_OBJ:.o=.d) \
           $(TEST_SIGNALS_OBJ:.o=.d) \
           $(TEST_RINGBUF_OBJ:.o=.d)

-include $(DEPFILES)

