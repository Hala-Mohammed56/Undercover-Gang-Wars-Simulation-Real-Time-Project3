// simulation.h
#ifndef SIMULATION_H
#define SIMULATION_H

#include "structs.h"
#include "config_loader.h"
#include "gang.h"

// Run full simulation loop for all gangs
void run_simulation(Gang *gangs, int num_gangs, const Config *config, int max_rounds);
void print_gang_status_table(Gang *gangs, int num_gangs);
void print_final_gang_summary_table(Gang *gangs, int num_gangs);

#endif
