#include "police.h"
#include "config_loader.h"
#include <stdio.h>

int main()
{
    Config config;
    if (load_config("config.txt", &config) != 0)
    {
        fprintf(stderr, "Failed to load config\n");
        return 1;
    }

    police_process_fifo_mode(&config);
    return 0;
}
