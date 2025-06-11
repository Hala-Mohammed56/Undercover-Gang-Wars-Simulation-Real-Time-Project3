#include <stdio.h>
#include "config_loader.h"
#include "gang.h"
#include "simulation.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> // for unlink()

#define FIFO_PATH "gang_reports.fifo"

int main()
{
    // Create FIFO for police communication
    unlink(FIFO_PATH); // remove if it already exists
    if (mkfifo(FIFO_PATH, 0666) == -1)
    {
        perror("Failed to create FIFO");
    }

    Config config;

    if (load_config("config.txt", &config) != 0)
    {
        return 1;
    }

    // 1. Generate gangs
    Gang *gangs = create_gangs(config.num_gangs, &config);
    for (int i = 0; i < config.num_gangs; ++i)
    {
        gangs[i].is_active = 1;
        gangs[i].config = &config;
        pthread_mutex_init(&gangs[i].lock, NULL);
        pthread_create(&gangs[i].thread_id, NULL, gang_thread_routine, &gangs[i]);
    }

    // 2. Assign targets
    assign_targets_to_all_gangs(gangs, config.num_gangs, &config);

    // 3. Start simulation (10 rounds)
    run_simulation(gangs, config.num_gangs, &config, 10);

    // 4. Free memory
    free_gangs(gangs, config.num_gangs);

    return 0;
}
