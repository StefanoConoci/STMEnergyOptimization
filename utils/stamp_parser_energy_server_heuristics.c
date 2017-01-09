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

        double total_time=0;
        long total_commits=0;
        long total_aborts=0;
        double total_throughput=0;

		//Package 0
        double package0_energy;
        double dram0_energy;

        double package0_power;
        double dram0_power;
        
        double avg_package0_power;
        double avg_dram0_power;

        double avg_package0_energy;
        double avg_dram0_energy;

        double total_package0_energy=0;
        double total_dram0_energy=0;

        double total_package0_power=0;
        double total_dram0_power=0;

       	//Package 1
        double package1_energy;
        double dram1_energy;

        double package1_power;
        double dram1_power;
        
        double avg_package1_power;
        double avg_dram1_power;

        double avg_package1_energy;
        double avg_dram1_energy;

        double total_package1_energy=0;
        double total_dram1_energy=0;

        double total_package1_power=0;
        double total_dram1_power=0;

        int pstate;
        int best_threads;
        int steps;

        int total_pstate=0;
        int total_best_threads=0;
        int total_steps=0;

        double avg_pstate;
        double avg_best_threads;
        double avg_steps;


        int entries=0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld Pkg0_energy: %lf Dram0_energy: %lf Pkg1_energy: %lf Dram1_energy: %lf Pkg0_power: %lf Dram0_power: %lf Pkg1_power: %lf Dram1_power: %lf P_state: %d Best_threads: %d Steps: %d Throughput: %lf ", 
                              &threads, &time, &commits, &aborts, 
                              &package0_energy, &dram0_energy, &package1_energy, &dram1_energy,
                              &package0_power, &dram0_power, &package1_power, &dram1_power, 
                              &pstate, &best_threads, &steps, &throughput);

                total_time+=time;
                total_commits+=commits;
                total_aborts+=aborts;
                
                total_package0_energy+=package0_energy;
                total_dram0_energy+=dram0_energy;
                total_package1_energy+=package1_energy;
                total_dram1_energy+=dram1_energy;

                total_package0_power+=package0_power;
                total_dram0_power+=dram0_power;
                total_package1_power+=package1_power;
                total_dram1_power+=dram1_power;
               
                total_pstate+=pstate;
                total_best_threads+=best_threads;
                total_steps+=steps;
                total_throughput+=throughput;

                entries++;
         }

        fclose(fp);
        if (line)
                free(line);

        avg_time = total_time/ entries;
        avg_commits = (double) total_commits/ entries;
        avg_aborts = (double) total_aborts/ entries;

        avg_package0_energy = total_package0_energy / entries;
        avg_dram0_energy = total_dram0_energy / entries;
		avg_package1_energy = total_package1_energy / entries;
        avg_dram1_energy = total_dram1_energy / entries;
		        
        avg_package0_power = total_package0_power / entries;
        avg_dram0_power = total_dram0_power / entries;
        avg_package1_power = total_package1_power / entries;
        avg_dram1_power = total_dram1_power / entries;


        avg_pstate = (double) total_pstate / entries;
        avg_best_threads = (double) total_best_threads / entries;
        avg_steps = (double) total_steps / entries;
        avg_throughput = total_throughput/ entries;


        printf("Entries: %d - Threads: %d - Runtime: %lf - P_state: %lf - Best_threads: %lf - Steps: %lf - Commits: %lf - Aborts: %lf - Throughput: %lf - Package0_energy: %lf - Dram0_energy: %lf - Package1_energy: %lf - Dram1_energy: %lf - Package0_power: %lf - Dram0_power %lf - Package1_power: %lf - Dram1_power %lf\n",
                entries, threads, avg_time, avg_pstate, avg_best_threads, avg_steps, avg_commits, avg_aborts, avg_throughput, avg_package0_energy, avg_dram0_energy, avg_package1_energy, avg_dram1_energy, avg_package0_power, avg_dram0_power, avg_package1_power, avg_dram1_power);

        exit(EXIT_SUCCESS);
}

