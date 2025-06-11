// simulation.c
#include <stdio.h>
#include <unistd.h> // for sleep()
#include <stdlib.h>
#include "simulation.h"
#include "constants.h"
#include "police.h"
typedef struct
{
    int id;
    int success;
    int failure;
    int alive_agents;
    int active;
} GangView;

void run_simulation(Gang *gangs, int num_gangs, const Config *config, int max_rounds)
{
    int executed_agents_total = 0;
    int total_successful_plans = 0;
    int total_thwarted_plans = 0;

    printf("\n=== Starting Simulation (%d rounds max) ===\n", max_rounds);

    for (int round = 1; round <= max_rounds; ++round)
    {
        printf("\n--- Round %d ---\n", round);

        // Step 1: Monitor agents (suspicion increases)
        monitor_agents(gangs, num_gangs, config);

        // Step 2: Gang actions
        for (int i = 0; i < num_gangs; ++i)
        {
            Gang *g = &gangs[i];

            // Skip inactive gangs (e.g., in prison)
            if (!g->active)
            {
                g->prison_timer--;
                if (g->prison_timer <= 0)
                {
                    g->active = 1;
                    g->in_prison = 0;
                    printf("Gang #%d has been released from prison.\n", g->id);
                    assign_target_to_gang(g, config);

                    printf("\n=== Simulation Complete ===\n");
                    printf("Total Successes: %d | Thwarted: %d | Agents Executed: %d\n",
                           total_successful_plans, total_thwarted_plans, executed_agents_total);

                    printf(COLOR_YELLOW "\n=== Final Report ===\n" COLOR_RESET);
                    for (int i = 0; i < num_gangs; ++i)
                    {
                        Gang *g = &gangs[i];
                        printf("Gang #%d → ", g->id);
                        printf("Success: %d | Failures: %d | ", g->success_count, g->failure_count);

                        int alive_agents = 0, executed_agents = 0;
                        for (int j = 0; j < g->member_count; ++j)
                        {
                            GangMember *m = &g->members[j];
                            if (m->is_agent)
                            {
                                if (m->alive)
                                    alive_agents++;
                                else
                                    executed_agents++;
                            }
                        }

                        printf("Agents (Alive: %d, Executed: %d)\n", alive_agents, executed_agents);
                    }

                    print_final_gang_summary_table(gangs, num_gangs);
                }
                continue;
            }

            // Simulate members preparing for the target
            simulate_preparation_step(g, config);

            if (is_gang_ready(g))
            {
                printf("Gang #%d is ready to execute target!\n", g->id);
                g->success_count++;
                total_successful_plans++;
                promote_members(g, config);
                simulate_member_casualties(g, g->config);
                assign_target_to_gang(g, config);
            }
        }

        // Step 3: Check agent reports

        int report_flags[num_gangs];
        int confirmed_reports = collect_reports(gangs, num_gangs, config, report_flags);

        if (confirmed_reports > 0)
        {
            arrest_gangs(gangs, num_gangs, report_flags, config);
            for (int i = 0; i < num_gangs; ++i)
            {
                if (report_flags[i])
                {
                    investigate_gang_for_agents(&gangs[i], &executed_agents_total);
                }
            }
            total_thwarted_plans += confirmed_reports;

            int executed = detect_and_execute_agents(gangs, num_gangs, config);
            executed_agents_total += executed;
        }
        print_gang_status_table(gangs, num_gangs);

        // Step 4: Check end conditions
        if (total_successful_plans >= config->max_successful_plans)
        {
            printf("\n=== Simulation ended: Gangs won! ===\n");
            break;
        }
        if (total_thwarted_plans >= config->max_thwarted_plans)
        {
            printf("\n=== Simulation ended: Police won! ===\n");
            break;
        }
        if (executed_agents_total >= config->max_executed_agents)
        {
            printf("\n=== Simulation ended: Agents compromised! ===\n");
            break;
        }

        sleep(1); // delay between rounds
    }

    printf("\n=== Simulation Complete ===\n");
    printf("Total Successes: %d | Thwarted: %d | Agents Executed: %d\n",
           total_successful_plans, total_thwarted_plans, executed_agents_total);
    printf(COLOR_YELLOW "\n=== Final Report ===\n" COLOR_RESET);
    for (int i = 0; i < num_gangs; ++i)
    {
        Gang *g = &gangs[i];
        printf("Gang #%d → ", g->id);
        printf("Success: %d | Failures: %d | ", g->success_count, g->failure_count);

        int alive_agents = 0, executed_agents = 0;
        for (int j = 0; j < g->member_count; ++j)
        {
            GangMember *m = &g->members[j];
            if (m->is_agent)
            {
                if (m->alive)
                    alive_agents++;
                else
                    executed_agents++;
            }
        }

        printf("Agents (Alive: %d, Executed: %d)\n", alive_agents, executed_agents);
    }
    print_final_gang_summary_table(gangs, num_gangs);
}
void print_gang_status_table(Gang *gangs, int num_gangs)
{
    GangView sorted[num_gangs];

    // Copy values for sorting
    for (int i = 0; i < num_gangs; ++i)
    {
        Gang *g = &gangs[i];
        sorted[i].id = g->id;
        sorted[i].success = g->success_count;
        sorted[i].failure = g->failure_count;
        sorted[i].active = g->active;

        int alive_agents = 0;
        for (int j = 0; j < g->member_count; ++j)
        {
            if (g->members[j].is_agent && g->members[j].alive)
                alive_agents++;
        }
        sorted[i].alive_agents = alive_agents;
    }

    // Sort by success count descending
    for (int i = 0; i < num_gangs - 1; ++i)
    {
        for (int j = i + 1; j < num_gangs; ++j)
        {
            if (sorted[j].success > sorted[i].success)
            {
                GangView temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    // Print table header
    printf(COLOR_BOLD "\n%-8s %-14s %-12s %-12s %-10s\n" COLOR_RESET,
           "Gang", "Status", "Success", "Failure", "Agents");

    // Print each gang
    for (int i = 0; i < num_gangs; ++i)
    {
        const char *status_color = sorted[i].active ? COLOR_GREEN : COLOR_RED;
        const char *status_label = sorted[i].active ? "ACTIVE" : "IN PRISON";

        printf(COLOR_BLUE "%-8d" COLOR_RESET " ", sorted[i].id);

        printf("%s%-14s" COLOR_RESET, status_color, status_label);

        printf(COLOR_GREEN "%-12d" COLOR_RESET, sorted[i].success);
        printf(COLOR_RED "%-12d" COLOR_RESET, sorted[i].failure);
        printf(COLOR_CYAN "%-10d" COLOR_RESET, sorted[i].alive_agents);

        printf("\n");
    }
}
void print_final_gang_summary_table(Gang *gangs, int num_gangs)
{
    printf(COLOR_YELLOW "\n=== Final Gang Summary ===\n" COLOR_RESET);
    printf(COLOR_BOLD "%-8s %-10s %-10s %-12s %-12s %-14s\n" COLOR_RESET,
           "Gang", "Success", "Failure", "Agents(OK)", "Agents(DEAD)", "Status");

    for (int i = 0; i < num_gangs; ++i)
    {
        Gang *g = &gangs[i];

        int alive_agents = 0, dead_agents = 0;
        for (int j = 0; j < g->member_count; ++j)
        {
            if (g->members[j].is_agent)
            {
                if (g->members[j].alive)
                    alive_agents++;
                else
                    dead_agents++;
            }
        }

        const char *status_color = g->active ? COLOR_GREEN : COLOR_RED;
        const char *status_text = g->active ? "ACTIVE" : "IN PRISON";

        printf(COLOR_BLUE "%-8d" COLOR_RESET, g->id);
        printf(COLOR_GREEN "%-10d" COLOR_RESET, g->success_count);
        printf(COLOR_RED "%-10d" COLOR_RESET, g->failure_count);
        printf(COLOR_CYAN "%-12d" COLOR_RESET, alive_agents);
        printf(COLOR_RED "%-12d" COLOR_RESET, dead_agents);
        printf("%s%-14s" COLOR_RESET, status_color, status_text);
        printf("\n");
    }
}
