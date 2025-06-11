
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "police.h"
#include "constants.h"

#define FIFO_PATH "gang_reports.fifo"

// read reports via FIFO
void police_process_fifo_mode(const Config *config)
{
    int fd;
    char buffer[128];

    int num_gangs = config->num_gangs;
    int report_counts[num_gangs][NUM_CRIME_TYPES];
    int confirmed[num_gangs][NUM_CRIME_TYPES];

    memset(report_counts, 0, sizeof(report_counts));
    memset(confirmed, 0, sizeof(confirmed));

    if (access(FIFO_PATH, F_OK) == -1)
    {
        if (mkfifo(FIFO_PATH, 0666) == -1)
        {
            perror("[POLICE] mkfifo failed");
            exit(EXIT_FAILURE);
        }
    }

    printf("[POLICE] FIFO mode: Waiting for reports from gangs...\n");

    while ((fd = open(FIFO_PATH, O_RDONLY)) == -1)
    {
        perror("[POLICE] Waiting for writer...");
        sleep(1);
    }

    if (fd == -1)
    {
        perror("open fifo");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            int gang_id, plan_type;
            if (sscanf(buffer, "Gang %d reports plan %d", &gang_id, &plan_type) == 2)
            {
                int gi = gang_id - 1;
                int pt = plan_type;

                if (gi >= 0 && gi < num_gangs && pt >= 0 && pt < NUM_CRIME_TYPES)
                {
                    report_counts[gi][pt]++;

                    printf("[POLICE] Report received: Gang #%d → Plan %d | Count = %d\n",
                           gang_id, plan_type, report_counts[gi][pt]);

                    if (report_counts[gi][pt] >= config->confirmation_threshold && !confirmed[gi][pt])
                    {
                        printf("[POLICE] Plan CONFIRMED for Gang #%d → Plan %d (via %d reports)\n",
                               gang_id, plan_type, report_counts[gi][pt]);
                        confirmed[gi][pt] = 1;
                    }
                }
            }
            else
            {
                printf("[POLICE] Invalid report format: %s", buffer);
            }
        }
        usleep(100000); // 100ms
    }

    close(fd);
}

// --- Existing in-process logic below ---

void monitor_agents(Gang *gangs, int num_gangs, const Config *config)
{
    for (int i = 0; i < num_gangs; ++i)
    {
        Gang *g = &gangs[i];
        for (int j = 0; j < g->member_count; ++j)
        {
            GangMember *m = &g->members[j];
            if (m->is_agent && m->alive)
            {
                m->suspicion_level += 0.1f;
                if (m->suspicion_level > 1.0f)
                    m->suspicion_level = 1.0f;
            }
            printf("Agent #%d in Gang #%d | Suspicion: %.2f\n", m->id, g->id, m->suspicion_level);
        }
    }
}

int collect_reports(Gang *gangs, int num_gangs, const Config *config, int *report_flags)
{
    int confirmed_reports = 0;
    for (int i = 0; i < num_gangs; ++i)
    {
        report_flags[i] = 0;
        int plan_votes[NUM_CRIME_TYPES] = {0};
        for (int j = 0; j < gangs[i].member_count; ++j)
        {
            GangMember *m = &gangs[i].members[j];
            if (!m->is_agent || !m->alive)
                continue;
            if (m->suspicion_level >= config->max_agent_suspicion)
            {
                plan_votes[m->known_plan_type]++;
            }
        }
        for (int t = 0; t < NUM_CRIME_TYPES; ++t)
        {
            if (plan_votes[t] >= config->confirmation_threshold)
            {
                report_flags[i] = 1;
                confirmed_reports++;
                printf("Police confirmed report on Gang #%d: Plan type %d confirmed by %d agents.\n",
                       i + 1, t, plan_votes[t]);
                break;
            }
        }
    }
    return confirmed_reports;
}

void arrest_gangs(Gang *gangs, int num_gangs, int *report_flags, const Config *config)
{
    for (int i = 0; i < num_gangs; ++i)
    {
        if (report_flags[i] >= config->confirmation_threshold)
        {
            Gang *g = &gangs[i];
            g->in_prison = 1;
            g->prison_timer = config->arrest_duration;
            g->active = 0;
            g->failure_count++;
            printf(">> Police arrested Gang #%d for suspicious activity!\n", g->id);
        }
    }
}

int detect_and_execute_agents(Gang *gangs, int num_gangs, const Config *config)
{
    int executed_count = 0;
    for (int i = 0; i < num_gangs; ++i)
    {
        Gang *g = &gangs[i];
        if (g->in_prison)
        {
            for (int j = 0; j < g->member_count; ++j)
            {
                GangMember *m = &g->members[j];
                if (m->is_agent && m->alive)
                {
                    float chance = (float)rand() / RAND_MAX;
                    if (chance < config->agent_detection_rate)
                    {
                        m->alive = 0;
                        executed_count++;
                        printf(">> Agent #%d in Gang #%d was discovered and executed!\n", m->id, g->id);
                    }
                }
            }
        }
    }
    return executed_count;
}
