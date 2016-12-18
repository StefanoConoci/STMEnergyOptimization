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

        int entries=0;
        double total_time=0;
        long total_commits=0;
        long total_aborts=0;
        double total_throughput=0;

        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
                sscanf(line, "Threads: %d Elapsed time: %lf Commits: %ld Aborts: %ld Throughput: %lf", &threads, &time, &commits, &aborts, &throughput);
                total_time+=time;
                total_commits+=commits;
                total_aborts+=aborts;
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

        printf("Entries: %d - Threads: %d - AVG Runtime: %lf - AVG Commits: %lf - AVG Aborts: %lf - AVG Throughput: %lf\n", entries, threads, avg_time, avg_commits, avg_aborts, avg_throughput);

        exit(EXIT_SUCCESS);
}

