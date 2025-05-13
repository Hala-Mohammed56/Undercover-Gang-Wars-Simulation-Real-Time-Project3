#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Forward declaration
void* gang_member_thread(void* arg);

// Structs from main.c
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

// Struct for each gang member
typedef struct {
    int member_id;
    int rank;
    int preparation_level;
    int is_alive;
} GangMember;

// Global pointers
GangMember* members;
Config* config_ptr;
int gang_id = 0;

// Function to simulate gang logic
void run_gang_process(int id, Config* config) {
    srand(time(NULL) + id); // seed per process

    gang_id = id;
    config_ptr = config;

    int num_members = config->min_members + rand() % (config->max_members - config->min_members + 1);
    printf("[Gang %d] Starting with %d members.\n", gang_id, num_members);

    members = (GangMember*)malloc(sizeof(GangMember) * num_members);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * num_members);

    // Initialize and start member threads
    for (int i = 0; i < num_members; i++) {
        members[i].member_id = i + 1;
        members[i].rank = 1; // start from lowest rank
        members[i].preparation_level = 0;
        members[i].is_alive = 1;

        pthread_create(&threads[i], NULL, gang_member_thread, &members[i]);
        usleep(100000); // slight delay for print clarity
    }

    // Wait for all members to finish (for demo)
    for (int i = 0; i < num_members; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[Gang %d] All members finished initial round.\n", gang_id);

    free(members);
    free(threads);
}

// Thread function for each gang member
void* gang_member_thread(void* arg) {
    GangMember* m = (GangMember*)arg;

    printf("[Gang %d - Member %d] Joined with rank %d\n", gang_id, m->member_id, m->rank);

    // Simulate preparation phase
    for (int i = 0; i < config_ptr->target_preparation_time; i++) {
        sleep(config_ptr->interaction_delay);
        m->preparation_level++;

        // Simulate random chance of death
        if ((float)rand() / RAND_MAX < config_ptr->kill_rate) {
            m->is_alive = 0;
            printf("[Gang %d - Member %d] KILLED during training\n", gang_id, m->member_id);
            pthread_exit(NULL);
        }

        printf("[Gang %d - Member %d] Preparation level: %d\n", gang_id, m->member_id, m->preparation_level);
    }

    // Rank up if survived
    if (m->is_alive && m->preparation_level >= config_ptr->required_preparation_level) {
        m->rank++;
        printf("[Gang %d - Member %d] Promoted to rank %d\n", gang_id, m->member_id, m->rank);
    }

    pthread_exit(NULL);
}
