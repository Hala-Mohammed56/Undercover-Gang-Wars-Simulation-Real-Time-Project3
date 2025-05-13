#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Structs (نفس اللي في gang.c)
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

typedef struct {
    int agent_id;
    float suspicion_level;
    int exposed;
} SecretAgent;

// Global
Config* config_ptr;
int gang_id = 0;

// Dummy function to simulate info source (true or false)
int get_info() {
    // 1 = true info, 0 = false info
    return ((float)rand() / RAND_MAX) > config_ptr->false_info_rate;
}

void* agent_thread(void* arg) {
    SecretAgent* agent = (SecretAgent*)arg;

    printf("[Gang %d - Agent %d] Infiltrated gang. Starting...\n", gang_id, agent->agent_id);

    for (int round = 1; round <= config_ptr->target_preparation_time; round++) {
        sleep(config_ptr->interaction_delay);

        int info = get_info(); // simulate info received

        if (info) {
            agent->suspicion_level += config_ptr->suspicion_increase_rate;
            printf("[Gang %d - Agent %d] Got VALID info. Suspicion: %.2f\n", gang_id, agent->agent_id, agent->suspicion_level);
        } else {
            agent->suspicion_level -= config_ptr->suspicion_decrease_rate;
            if (agent->suspicion_level < 0) agent->suspicion_level = 0;
            printf("[Gang %d - Agent %d] Got FAKE info. Suspicion: %.2f\n", gang_id, agent->agent_id, agent->suspicion_level);
        }

        if (agent->suspicion_level >= config_ptr->suspicion_threshold) {
            printf("[Gang %d - Agent %d] Reporting to police! 🚨\n", gang_id, agent->agent_id);
            pthread_exit(NULL); // simulate report & exit
        }
    }

    // Didn't reach suspicion threshold
    printf("[Gang %d - Agent %d] Did not report. Suspicion: %.2f\n", gang_id, agent->agent_id, agent->suspicion_level);

    pthread_exit(NULL);
}

// Callable from gang.c (مثال إدماج لاحق)
void start_secret_agent(int id, Config* config) {
    srand(time(NULL) + id);
    gang_id = id;
    config_ptr = config;

    SecretAgent agent;
    agent.agent_id = 100 + id; // just an ID
    agent.suspicion_level = 0;
    agent.exposed = 0;

    pthread_t t;
    pthread_create(&t, NULL, agent_thread, &agent);
    pthread_join(t, NULL);
}

