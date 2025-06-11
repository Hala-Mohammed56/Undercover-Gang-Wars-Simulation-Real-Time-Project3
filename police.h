// police.h
#ifndef POLICE_H
#define POLICE_H

#include "structs.h"

#include "config_loader.h"
int collect_reports(Gang *gangs, int num_gangs, const Config *config, int *report_flags);

// Called each round: agents interact with others and adjust suspicion
void monitor_agents(Gang *gangs, int num_gangs, const Config *config);

// Check which agents reported to police (returns total reports)
int collect_reports(Gang *gangs, int num_gangs, const Config *config, int *report_flags);

// Arrest gangs based on confirmed reports
void arrest_gangs(Gang *gangs, int num_gangs, int *report_flags, const Config *config);

// Check if agents are discovered by gangs after plan failure
int detect_and_execute_agents(Gang *gangs, int num_gangs, const Config *config);
void police_process_fifo_mode(const Config *config);

#endif
