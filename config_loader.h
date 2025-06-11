// config_loader.h
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

typedef struct
{
    int num_gangs;
    int min_gang_members;
    int max_gang_members;
    int max_rank;
    int num_targets;

    int min_prep_time;
    int max_prep_time;
    int min_prep_level;
    int max_prep_level;

    float agent_insertion_prob;
    float max_agent_suspicion;
    float agent_detection_rate;

    float false_info_prob;
    int info_spread_depth;

    float success_rate_factor;
    float kill_probability;
    int prison_duration;

    int confirmation_threshold;
    int arrest_duration;

    int max_successful_plans;
    int max_thwarted_plans;
    int max_executed_agents;
    float member_death_prob;
} Config;

int load_config(const char *filename, Config *config);

#endif
