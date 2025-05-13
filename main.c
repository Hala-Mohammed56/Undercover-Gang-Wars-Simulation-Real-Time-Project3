#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Struct to hold configuration values
typedef struct {
    int num_gangs;
    int min_members;
    int max_members;
    int num_ranks;

    int target_preparation_time;
    int required_preparation_level;
    float plan_success_rate;

    float false_info_rate;
    int info_spread_depth;

    float agent_infiltration_rate;
    float suspicion_increase_rate;
    float suspicion_decrease_rate;
    float suspicion_threshold;

    int confirmation_required;
    int prison_duration;
    int thwart_threshold;

    int successful_plans_threshold;

    int max_executed_agents;
    float agent_discovery_prob;

    float kill_rate;

    int round_duration;
    int interaction_delay;
} Config;

// Function to load config values from file
void load_config(const char* filename, Config* cfg) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || strlen(line) < 3) continue;

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        if (!key || !value) continue;

        #define MATCH(k) strcmp(key, k) == 0

        if (MATCH("NUM_GANGS")) cfg->num_gangs = atoi(value);
        else if (MATCH("MIN_MEMBERS_PER_GANG")) cfg->min_members = atoi(value);
        else if (MATCH("MAX_MEMBERS_PER_GANG")) cfg->max_members = atoi(value);
        else if (MATCH("NUM_RANKS")) cfg->num_ranks = atoi(value);
        else if (MATCH("TARGET_PREPARATION_TIME")) cfg->target_preparation_time = atoi(value);
        else if (MATCH("REQUIRED_PREPARATION_LEVEL")) cfg->required_preparation_level = atoi(value);
        else if (MATCH("PLAN_SUCCESS_RATE")) cfg->plan_success_rate = atof(value);
        else if (MATCH("FALSE_INFO_RATE")) cfg->false_info_rate = atof(value);
        else if (MATCH("INFO_SPREAD_DEPTH")) cfg->info_spread_depth = atoi(value);
        else if (MATCH("AGENT_INFILTRATION_RATE")) cfg->agent_infiltration_rate = atof(value);
        else if (MATCH("SUSPICION_INCREASE_RATE")) cfg->suspicion_increase_rate = atof(value);
        else if (MATCH("SUSPICION_DECREASE_RATE")) cfg->suspicion_decrease_rate = atof(value);
        else if (MATCH("SUSPICION_THRESHOLD")) cfg->suspicion_threshold = atof(value);
        else if (MATCH("CONFIRMATION_REQUIRED")) cfg->confirmation_required = atoi(value);
        else if (MATCH("PRISON_DURATION")) cfg->prison_duration = atoi(value);
        else if (MATCH("THWART_THRESHOLD")) cfg->thwart_threshold = atoi(value);
        else if (MATCH("SUCCESSFUL_PLANS_THRESHOLD")) cfg->successful_plans_threshold = atoi(value);
        else if (MATCH("MAX_EXECUTED_AGENTS")) cfg->max_executed_agents = atoi(value);
        else if (MATCH("AGENT_DISCOVERY_PROB")) cfg->agent_discovery_prob = atof(value);
        else if (MATCH("KILL_RATE")) cfg->kill_rate = atof(value);
        else if (MATCH("ROUND_DURATION")) cfg->round_duration = atoi(value);
        else if (MATCH("INTERACTION_DELAY")) cfg->interaction_delay = atoi(value);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s config.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    Config config;
    load_config(argv[1], &config);

    printf("Simulation Starting with %d gangs...\n", config.num_gangs);

    for (int i = 0; i < config.num_gangs; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process: here will later call gang simulation logic
            printf("[Gang %d] Process started. PID: %d\n", i + 1, getpid());
            // Placeholder for gang.c logic
            exit(0);
        }
    }

    // Parent waits for all child processes to finish
    for (int i = 0; i < config.num_gangs; i++) {
        wait(NULL);
    }

    printf("Simulation ended. All gang processes completed.\n");
    return 0;
}

