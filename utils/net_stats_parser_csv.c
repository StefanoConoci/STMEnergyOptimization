#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
        FILE * fp;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;

        int threads;
        double time;
        long commits;
        long aborts;
        double throughput;

        double avg_time;
        double avg_commits;
        double avg_aborts;
        double avg_throughput;

        double package_energy;
        double core_energy;
        double dram_energy;

        double package_power;
        double core_power;
        double dram_power;

        double avg_package_power;
        double avg_core_power;
        double avg_dram_power;


        double avg_package_energy;
        double avg_core_energy;
        double avg_dram_energy;

        double total_time=0;
        long total_commits=0;
        long total_aborts=0;
        double total_throughput=0;

        double total_package_energy=0;
        double total_core_energy=0;
        double total_dram_energy=0;

        double total_package_power=0;
        double total_core_power=0;
        double total_dram_power=0;

        double effective_runtime;
        long effective_commits;
        double effective_throughput;
        double effective_energy_per_tx;

        double total_energy_per_tx=0;
        double avg_energy_per_tx;

        int pstate;
        int best_threads;
        int steps;

        int total_pstate=0;
        int total_best_threads=0;
        int total_steps=0;

        double avg_pstate;
        double avg_best_threads;
        double avg_steps;

        double net_runtime, net_throughput, net_power, net_error;
        long net_commits, net_aborts; 

        double total_error;
        double avg_error;
 


        int entries=0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld Net_runtime: %lf Net_throughput: %lf Net_power: %lf Net_commits: %ld Net_aborts: %ld Net_error: %lf P-state: %d Best-threads: %d Steps: %d Throughput: %lf", 
                              &threads, &time, &commits, &aborts, 
                              &net_runtime, &net_throughput, &net_power, &net_commits, &net_aborts, &net_error,
                              &pstate, &best_threads, &steps, &throughput);

                if(net_power > 0 && net_error <= 100 && net_error >= 0){   
                    total_throughput+=net_throughput;
                    total_error+=net_error;
                                    
                    entries++;
                }
         }

        fclose(fp);
        if (line)
                free(line);

        avg_throughput = total_throughput/ entries;
        avg_error = total_error/entries;

        printf("%lf,%lf\n", avg_throughput, avg_error);

        exit(EXIT_SUCCESS);
}

