
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
CONSOLE_OBJ = $(OBJ_DIR)/app/console.o

# 4. Fichiers objets des tests
TEST_SIGNALS_OBJ = $(OBJ_DIR)/tests/test_signals.o
TEST_RINGBUF_OBJ = $(OBJ_DIR)/tests/test_ringbuf.o
TEST_CONSOLE_OBJ = $(OBJ_DIR)/tests/test_console.o

# 5. Executables des tests
TEST_SIGNALS = $(BUILD_DIR)/test_signals
TEST_RINGBUF = $(BUILD_DIR)/test_ringbuf
TEST_CONSOLE = $(BUILD_DIR)/test_console
TEST_GPIO = $(BUILD_DIR)/test_gpio

TEST_BINS = $(TEST_SIGNALS) \
            $(TEST_RINGBUF) \
            $(TEST_CONSOLE) \
            $(TEST_GPIO)

# 6. Cible par defaut
.DEFAULT_GOAL := all

.PHONY: all test test_signals test_ringbuf test_console test_gpio map clean help

all: $(TEST_BINS)

# 7. Compilation des modules applicatifs
$(OBJ_DIR)/app/%.o: src/app/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# 8. Compilation des fichiers de tests
$(OBJ_DIR)/tests/%.o: tests/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# 9. Edition des liens des tests applicatifs
$(TEST_SIGNALS): $(SIGNALS_OBJ) $(TEST_SIGNALS_OBJ)
	$(CC) $^ -o $@

$(TEST_RINGBUF): $(RINGBUF_OBJ) $(TEST_RINGBUF_OBJ)
	$(CC) $^ -o $@

$(TEST_CONSOLE): $(CONSOLE_OBJ) $(TEST_CONSOLE_OBJ)
	$(CC) $^ -o $@

# 10. Compilation du test GPIO avec stub STM32
$(TEST_GPIO): src/hal/gpio.c src/hal/gpio.h tests/test_gpio.c tests/stubs/stm32h7xx.h
	mkdir -p $(BUILD_DIR)
	$(CC) -Itests/stubs $(CPPFLAGS) $(CFLAGS) \
	    src/hal/gpio.c tests/test_gpio.c \
	    -o $@

# 11. Executer tous les tests
test: all
	./$(TEST_SIGNALS)
	./$(TEST_RINGBUF)
	./$(TEST_CONSOLE)
	./$(TEST_GPIO)

# 12. Executer chaque module separement
test_signals: $(TEST_SIGNALS)
	./$(TEST_SIGNALS)

test_ringbuf: $(TEST_RINGBUF)
	./$(TEST_RINGBUF)

test_console: $(TEST_CONSOLE)
	./$(TEST_CONSOLE)

test_gpio: $(TEST_GPIO)
	./$(TEST_GPIO)

# 13. Generation du rapport memoire
map: $(SIGNALS_OBJ) $(TEST_SIGNALS_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(SIGNALS_OBJ) $(TEST_SIGNALS_OBJ) \
	    -Wl,-Map=$(BUILD_DIR)/test_signals.map \
	    -o $(TEST_SIGNALS)
	@echo "MAP genere : $(BUILD_DIR)/test_signals.map"

# 14. Nettoyage
clean:
	rm -rf $(BUILD_DIR)

# 15. Aide
help:
	@echo "Commandes disponibles :"
	@echo "  make              : Compiler tous les tests"
	@echo "  make test         : Executer tous les tests"
	@echo "  make test_signals : Tester le module Signals"
	@echo "  make test_ringbuf : Tester le Ring Buffer"
	@echo "  make test_console : Tester la console UART"
	@echo "  make test_gpio    : Tester le module GPIO"
	@echo "  make map          : Generer le fichier MAP"
	@echo "  make clean        : Supprimer build/"
	@echo "  make help         : Afficher cette aide"

# 16. Fichiers de dependances
DEPFILES = $(SIGNALS_OBJ:.o=.d) \
           $(RINGBUF_OBJ:.o=.d) \
           $(CONSOLE_OBJ:.o=.d) \
           $(TEST_SIGNALS_OBJ:.o=.d) \
           $(TEST_RINGBUF_OBJ:.o=.d) \
           $(TEST_CONSOLE_OBJ:.o=.d)

-include $(DEPFILES)
