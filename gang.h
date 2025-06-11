// gang.h
#ifndef GANG_H
#define GANG_H
#include "structs.h"
#include "config_loader.h"

// Create all gangs and their members dynamically
Gang *create_gangs(int num_gangs, const Config *config);

// Free all dynamically allocated memory used by gangs
void free_gangs(Gang *gangs, int num_gangs);

// Print a summary of all gangs and their members
void print_gangs_summary(Gang *gangs, int num_gangs);

// Assign a target to a gang
void assign_target_to_gang(Gang *g, const Config *config);

// Assign targets to all gangs
void assign_targets_to_all_gangs(Gang *gangs, int num_gangs, const Config *config);

// Print a gang's assigned target
void print_gang_target(const Gang *g);

// Simulate preparation step: increase members' readiness
void simulate_preparation_step(Gang *g, const Config *config);

// Check if at least one member is ready for execution
int is_gang_ready(const Gang *g);

// Promote gang members after success
void promote_members(Gang *g, const Config *config);

void investigate_gang_for_agents(Gang *g, int *executed_agents_total);
void *gang_thread_routine(void *arg);
void simulate_member_casualties(Gang *g, const Config *config);

#endif
