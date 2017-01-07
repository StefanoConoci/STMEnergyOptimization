#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	int starting_threads;
	double power_limit;
	int total_commits_round;
	double energy_per_tx_limit;
	int heuristic_mode;
	double jump_percentage;

	int output_starting_threads;
	double output_power_limit;
	int output_total_commits_round;
	double output_energy_per_tx_limit;
	int output_heuristic_mode;
	double output_jump_percentage;

	if(argc < 7){
		printf("Not enough parameters passed in input\n");
		exit(1);
	}

	output_starting_threads = atoi(argv[1]);
	output_power_limit = atof(argv[2]);
	output_total_commits_round = atoi(argv[3]);
	output_energy_per_tx_limit = atof(argv[4]);
	output_heuristic_mode = atoi(argv[5]);
	output_jump_percentage = atof(argv[6]);

	// Load config file 
	FILE* config_file;
	if ((config_file = fopen("hope_config.txt", "r")) == NULL) {
		printf("Error opening STM_HOPE configuration file.\n");
		exit(1);
	}
	if (fscanf(config_file, "STARTING_THREADS=%d POWER_LIMIT=%lf COMMITS_ROUND = %d ENERGY_PER_TX_LIMIT = %lf HEURISTIC_MODE = %d JUMP_PERCENTAGE = %lf", 
			 &starting_threads, &power_limit, &total_commits_round, &energy_per_tx_limit, &heuristic_mode, &jump_percentage)!=6) {
		printf("The number of input parameters of the STM_HOPE configuration file does not match the number of required parameters.\n");
		exit(1);
	}
	fclose(config_file);

	if ((config_file = fopen("hope_config.txt", "w+")) == NULL) {
		printf("Error opening STM_HOPE configuration file.\n");
		exit(1);
	}
	fprintf(config_file, "STARTING_THREADS=%d\nPOWER_LIMIT=%lf\nCOMMITS_ROUND=%d\nENERGY_PER_TX_LIMIT=%lf\nHEURISTIC_MODE=%d\nJUMP_PERCENTAGE=%lf",
			output_starting_threads, output_power_limit, output_total_commits_round, output_energy_per_tx_limit, output_heuristic_mode, output_jump_percentage);
	fflush(config_file);
	printf("File hope_config.txt modified\n");
	
	return 0;

}