// config_loader.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_loader.h"

int load_config(const char *filename, Config *config)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open config file");
        return -1;
    }

    char line[128];
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        char key[64], value[64];
        if (sscanf(line, "%[^=]=%s", key, value) != 2)
            continue;

#define MATCH(k) strcmp(key, k) == 0

        if (MATCH("NUM_GANGS"))
            config->num_gangs = atoi(value);
        else if (MATCH("MIN_GANG_MEMBERS"))
            config->min_gang_members = atoi(value);
        else if (MATCH("MAX_GANG_MEMBERS"))
            config->max_gang_members = atoi(value);
        else if (MATCH("MAX_RANK"))
            config->max_rank = atoi(value);
        else if (MATCH("NUM_TARGETS"))
            config->num_targets = atoi(value);
        else if (MATCH("MIN_PREP_TIME"))
            config->min_prep_time = atoi(value);
        else if (MATCH("MAX_PREP_TIME"))
            config->max_prep_time = atoi(value);
        else if (MATCH("MIN_PREP_LEVEL"))
            config->min_prep_level = atoi(value);
        else if (MATCH("MAX_PREP_LEVEL"))
            config->max_prep_level = atoi(value);
        else if (MATCH("AGENT_INSERTION_PROB"))
            config->agent_insertion_prob = atof(value);
        else if (MATCH("MAX_AGENT_SUSPICION"))
            config->max_agent_suspicion = atof(value);
        else if (MATCH("AGENT_DETECTION_RATE"))
            config->agent_detection_rate = atof(value);
        else if (MATCH("FALSE_INFO_PROB"))
            config->false_info_prob = atof(value);
        else if (MATCH("INFO_SPREAD_DEPTH"))
            config->info_spread_depth = atoi(value);
        else if (MATCH("SUCCESS_RATE_FACTOR"))
            config->success_rate_factor = atof(value);
        else if (MATCH("KILL_PROBABILITY"))
            config->kill_probability = atof(value);
        else if (MATCH("PRISON_DURATION"))
            config->prison_duration = atoi(value);
        else if (MATCH("CONFIRMATION_THRESHOLD"))
            config->confirmation_threshold = atoi(value);
        else if (MATCH("ARREST_DURATION"))
            config->arrest_duration = atoi(value);
        else if (MATCH("MAX_SUCCESSFUL_PLANS"))
            config->max_successful_plans = atoi(value);
        else if (MATCH("MAX_THWARTED_PLANS"))
            config->max_thwarted_plans = atoi(value);
        else if (MATCH("MAX_EXECUTED_AGENTS"))
            config->max_executed_agents = atoi(value);
        else if (strcmp(key, "MEMBER_DEATH_PROB") == 0)
            config->member_death_prob = atof(value);
    }

    fclose(file);
    return 0;
}
