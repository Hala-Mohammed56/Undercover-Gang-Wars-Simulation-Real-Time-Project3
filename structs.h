// structs.h
#ifndef STRUCTS_H
#define STRUCTS_H
#include <pthread.h>
#include "config_loader.h"

#define MAX_NAME_LEN 32

typedef enum
{
    BANK_ROBBERY = 0,
    JEWELRY_ROBBERY,
    DRUG_TRAFFICKING,
    ART_THEFT,
    KIDNAPPING,
    BLACKMAIL,
    ARM_TRAFFICKING
} CrimeType;

typedef struct
{
    int id;
    int rank;
    int preparation_level;
    int is_agent;
    float suspicion_level;
    int known_plan_type;
    float knowledge_level;
    int is_newly_joined;
    int alive;
} GangMember;

typedef struct
{
    int id;
    CrimeType type;
    int required_prep_level;
    int preparation_time;
} TargetPlan;

typedef struct
{
    int id;
    GangMember *members;
    int member_count;

    int success_count;
    int failure_count;
    const Config *config; // save simulation configuration
    TargetPlan current_target;
    pthread_t thread_id;  // : id for each gang
    int is_active;        // it will be 0 when simulation is end , or gang arrested
    pthread_mutex_t lock; // to protect gang's info
    int in_prison;
    int prison_timer;
    int active;
    int target_completed;
} Gang;

#endif
