# Makefile

CC = gcc
CFLAGS = -Wall -g -pthread

DEPS = config_loader.h gang.h structs.h constants.h police.h
SRC = main.c gang.c config_loader.c simulation.c police.c
OUT = Run_simulation

# Main simulation build
$(OUT): $(SRC) $(DEPS)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Run main simulation
run: $(OUT)
	./$(OUT)

# Build separate police process
police_process: police_process.c police.c config_loader.c
	$(CC) $(CFLAGS) police_process.c police.c config_loader.c -o police_process

# Run police process manually 
run_police: police_process
	./police_process

# Cleanup all binaries
clean:
	rm -f $(OUT) police_process
