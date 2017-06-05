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
 
        double energy_total;
        double avg_energy_total;
        double total_energy_total=0;


        int entries=0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Effective_runtime: %lf  Pkg_power: %lf Energy_total: %lf Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld P-state: %d Best-threads: %d Steps: %d Throughput: %lf", 
                              &effective_runtime, &package_power, &energy_total, &threads, &time, &commits, &aborts, 
                              &pstate, &best_threads, &steps, &throughput);

                if(package_power > 0 && package_power<200){   
                    total_time+=effective_runtime;
                    total_aborts+=aborts;
                    total_commits+=commits;

                    total_throughput+=throughput;

                    total_package_power+=package_power;
                    total_energy_total+=energy_total;

                    total_pstate+=pstate;
                    total_best_threads+=best_threads;
                    total_steps+=steps;
                                    
                    entries++;
                }
         }

        fclose(fp);
        if (line)
                free(line);

        avg_time = total_time/ entries;
        avg_commits = (double) total_commits/ entries;
        avg_aborts = (double) total_aborts/ entries;

        avg_throughput = total_throughput/ entries;
        avg_energy_total = total_energy_total/ entries;

        avg_package_power = total_package_power / entries;
        
        avg_pstate = (double) total_pstate / entries;
        avg_best_threads = (double) total_best_threads / entries;
        avg_steps = (double) total_steps / entries;

        printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
                avg_pstate, avg_best_threads, avg_throughput, avg_package_power, avg_steps, avg_time, avg_commits, avg_aborts, avg_energy_total);

        exit(EXIT_SUCCESS);
}

