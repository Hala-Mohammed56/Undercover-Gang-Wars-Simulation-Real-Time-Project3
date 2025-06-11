#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "gang.h"
#include "constants.h"
#include <sys/stat.h>
#include <string.h>
#define FIFO_PATH "gang_reports.fifo"

// List of crime types
const char *crime_names[] = {
    "Bank Robbery", "Jewelry Robbery", "Drug Trafficking",
    "Art Theft", "Kidnapping", "Blackmail", "Arm Trafficking"};

// Generate random float between 0.0 and 1.0
float randf()
{
    return (float)rand() / RAND_MAX;
}

// Generate random integer between min and max (inclusive)
int randint(int min, int max)
{
    return min + rand() % (max - min + 1);
}

Gang *create_gangs(int num_gangs, const Config *config)
{
    srand(time(NULL));

    Gang *gangs = malloc(num_gangs * sizeof(Gang));
    if (!gangs)
    {
        perror("Failed to allocate memory for gangs");
        exit(1);
    }

    for (int i = 0; i < num_gangs; ++i)
    {
        gangs[i].id = i + 1;
        gangs[i].member_count = randint(config->min_gang_members, config->max_gang_members);
        gangs[i].members = malloc(gangs[i].member_count * sizeof(GangMember));
        if (!gangs[i].members)
        {
            perror("Failed to allocate memory for gang members!");
            exit(1);
        }

        gangs[i].success_count = 0;
        gangs[i].failure_count = 0;
        gangs[i].in_prison = 0;
        gangs[i].prison_timer = 0;
        gangs[i].active = 1;
        gangs[i].target_completed = 0;

        for (int j = 0; j < gangs[i].member_count; ++j)
        {
            gangs[i].members[j].id = j + 1;
            gangs[i].members[j].rank = 0;
            gangs[i].members[j].preparation_level = 0;
            gangs[i].members[j].suspicion_level = 0.0f;
            gangs[i].members[j].alive = 1;
            gangs[i].members[j].is_agent = (randf() < config->agent_insertion_prob) ? 1 : 0;
            gangs[i].members[j].known_plan_type = -1;
            gangs[i].members[j].knowledge_level = 0.0f;
        }
    }

    return gangs;
}
//////////////////////////
void free_gangs(Gang *gangs, int num_gangs)
{
    for (int i = 0; i < num_gangs; ++i)
    {
        free(gangs[i].members);
    }
    free(gangs);
}

void *gang_thread_routine(void *arg)
{
    Gang *g = (Gang *)arg;

    while (g->is_active)
    {
        pthread_mutex_lock(&g->lock);
        simulate_preparation_step(g, g->config);

        if (is_gang_ready(g))
        {
            g->success_count++;
            promote_members(g, g->config);
            assign_target_to_gang(g, g->config);
        }
        pthread_mutex_unlock(&g->lock);

        sleep(1); // delay per round
    }

    return NULL;
}
//////////////////////////////
void print_gangs_summary(Gang *gangs, int num_gangs)
{
    for (int i = 0; i < num_gangs; ++i)
    {
        printf("\n=== Gang #%d ===\n", gangs[i].id);
        printf("Members: %d\n", gangs[i].member_count);

        for (int j = 0; j < gangs[i].member_count; ++j)
        {
            GangMember *m = &gangs[i].members[j];

            const char *alive_status = m->alive ? COLOR_GREEN "YES" COLOR_RESET : COLOR_RED "NO" COLOR_RESET;
            const char *agent_status = m->is_agent ? COLOR_CYAN "YES" COLOR_RESET : "NO";

            printf(" - Member #%d | Rank: %d | Agent: %s | Alive: %s | Prep: %d | Susp: %.2f\n",
                   m->id,
                   m->rank,
                   agent_status,
                   alive_status,
                   m->preparation_level,
                   m->suspicion_level);
        }
    }
}
void assign_target_to_gang(Gang *g, const Config *config)
{
    g->current_target.id = g->id;
    g->current_target.type = (CrimeType)(rand() % 7);
    g->current_target.preparation_time = randint(config->min_prep_time, config->max_prep_time);
    g->current_target.required_prep_level = randint(config->min_prep_level, config->max_prep_level);

    for (int i = 0; i < g->member_count; ++i)
    {
        g->members[i].preparation_level = 0;
    }

    g->target_completed = 0;

    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];
        if (m->is_agent && m->alive)
        {
            if (m->rank < 2)
            {
                m->known_plan_type = (g->current_target.type + 1 + rand() % 3) % NUM_CRIME_TYPES;
            }
            else
            {
                m->known_plan_type = g->current_target.type;
            }
        }
    }
    // ========== Reporting to Police via FIFO ==========
    int agent_count = 0;
    for (int i = 0; i < g->member_count; ++i)
    {
        if (g->members[i].is_agent && g->members[i].alive)
            agent_count++;
    }

    if (agent_count >= 1)
    {
        int fd;
        int attempts = 5;
        while ((fd = open(FIFO_PATH, O_WRONLY)) == -1 && attempts-- > 0)
        {
            perror("[GANG] Failed to open FIFO");
            usleep(200000);
        }

        if (fd != -1)
        {
            char report[64];
            snprintf(report, sizeof(report), "Gang %d reports plan %d", g->id, g->current_target.type);
            write(fd, report, strlen(report));
            close(fd);
        }
        else
        {
            fprintf(stderr, "[GANG] Could not open FIFO after retries. Skipping report.\n");
        }
    }
}

void assign_targets_to_all_gangs(Gang *gangs, int num_gangs, const Config *config)
{
    for (int i = 0; i < num_gangs; ++i)
    {
        assign_target_to_gang(&gangs[i], config);
    }
}

void print_gang_target(const Gang *g)
{
    const TargetPlan *t = &g->current_target;
    printf("Gang #%d Target: %s | Prep Time: %ds | Required Prep Level: %d\n",
           g->id, crime_names[t->type], t->preparation_time, t->required_prep_level);
}

void simulate_preparation_step(Gang *g, const Config *config)
{
    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];
        if (!m->alive)
            continue;

        if (m->preparation_level < g->current_target.required_prep_level)
        {
            m->preparation_level += 1;
        }
    }
}

int is_gang_ready(const Gang *g)
{
    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];
        if (m->alive && m->preparation_level >= g->current_target.required_prep_level)
        {
            return 1;
        }
    }
    return 0;
}

void promote_members(Gang *g, const Config *config)
{
    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];
        if (m->alive && m->rank < config->max_rank)
        {
            m->rank += 1;
        }
    }
}

void investigate_gang_for_agents(Gang *g, int *executed_agents_total)
{
    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];
        if (!m->is_agent || !m->alive)
            continue;

        float suspicion_chance = 0.5f * m->knowledge_level + 0.5f * ((float)m->rank / MAX_RANK);

        if (((float)rand() / RAND_MAX) < suspicion_chance)
        {
            m->alive = 0;
            (*executed_agents_total)++;
            printf(COLOR_RED "Gang #%d executed Agent #%d after internal investigation.\n" COLOR_RESET,
                   g->id, m->id);
        }
    }
}
void gang_report_write(int gang_id, int plan_type)
{
    int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        perror("[GANG] Failed to open FIFO");
        return;
    }

    char message[128];
    snprintf(message, sizeof(message), "Gang %d reports plan %d\n", gang_id, plan_type);

    write(fd, message, strlen(message));
    close(fd);
}

void simulate_member_casualties(Gang *g, const Config *config)
{
    for (int i = 0; i < g->member_count; ++i)
    {
        GangMember *m = &g->members[i];

        // skip dead members
        if (!m->alive)
            continue;

        // death prob after the end of the plan
        if (randf() < config->member_death_prob)
        {
            printf(COLOR_RED "Gang #%d | Member #%d has been killed in action.\n" COLOR_RESET,
                   g->id, m->id);
            m->alive = 0;

            // replace with a new member
            m->rank = 0;
            m->preparation_level = 0;
            m->suspicion_level = 0.0f;
            m->is_agent = (randf() < config->agent_insertion_prob) ? 1 : 0;
            m->alive = 1;
            m->is_newly_joined = 1;

            printf(COLOR_GREEN "A new member has joined Gang #%d instead of the fallen one.\n" COLOR_RESET,
                   g->id);
        }
    }
}
