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
        int pstate;
        int best_threads;
        int steps;

        double avg_time;
        double avg_commits;
        double avg_aborts;
        double avg_throughput;
        double avg_pstate;
        int avg_best_threads;
        int avg_steps;

        int entries=0;
        double total_time=0;
        long total_commits=0;
        long total_aborts=0;
        double total_throughput=0;
        int total_pstate = 0;
        int total_best_threads = 0;
        int total_steps = 0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld P-state: %d Best-threads: %d Steps: %d Throughput: %lf", 
                            &threads, &time, &commits, &aborts, &pstate, &best_threads, &steps, &throughput);
                total_time+=time;
                total_commits+=commits;
                total_aborts+=aborts;
                total_throughput+=throughput;
                total_pstate+=pstate;
                total_best_threads+=best_threads;
                total_steps+=steps;
                entries++;
         }

        fclose(fp);
        if (line)
                free(line);

        avg_time = total_time/ entries;
        avg_commits = (double) total_commits/ entries;
        avg_aborts = (double) total_aborts/ entries;
        avg_throughput = total_throughput/ entries;
        avg_pstate = (double) total_pstate/ entries;
        avg_best_threads = (double) total_best_threads/ entries;
        avg_steps = (double) total_steps/ entries;

        printf("%d,%lf,%lf,%lf\n", 
                threads, avg_time, avg_throughput, avg_commits);

        exit(EXIT_SUCCESS);
}

