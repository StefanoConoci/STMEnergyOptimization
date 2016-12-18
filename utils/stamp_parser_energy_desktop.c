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


        int entries=0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld Pkg_energy: %lf Core_energy: %lf Dram_energy: %lf Pkg_power: %lf Core_power: %lf Dram_power: %lf  Throughput: %lf ", 
                              &threads, &time, &commits, &aborts, 
                              &package_energy, &core_energy, &dram_energy,
                              &package_power, &core_power, &dram_power, &throughput);
                total_time+=time;
                total_commits+=commits;
                total_aborts+=aborts;
                
                total_package_energy+=package_energy;
                total_core_energy+=core_energy;
                total_dram_energy+=dram_energy;

                total_package_power+=package_power;
                total_core_power+=core_power;
                total_dram_power+=dram_power;
                total_throughput+=throughput;
                
                entries++;
         }

        fclose(fp);
        if (line)
                free(line);

        avg_time = total_time/ entries;
        avg_commits = (double) total_commits/ entries;
        avg_aborts = (double) total_aborts/ entries;
        avg_throughput = total_throughput/ entries;

        avg_package_energy = total_package_energy / entries;
        avg_core_energy = total_core_energy / entries;
        avg_dram_energy = total_dram_energy / entries;

        avg_package_power = total_package_power / entries;
        avg_core_power = total_core_power / entries;
        avg_dram_power = total_dram_power / entries;

        printf("Entries: %d - Threads: %d - Runtime: %lf - Commits: %lf - Aborts: %lf - Throughput: %lf - Package_energy: %lf - Core_energy: %lf - Dram_energy: %lf - Package_power: %lf - Core_power %lf - Dram_power %lf\n",
                entries, threads, avg_time, avg_commits, avg_aborts, avg_throughput, avg_package_energy, avg_core_energy, avg_dram_energy, avg_package_power, avg_core_power, avg_dram_power);

        exit(EXIT_SUCCESS);
}

