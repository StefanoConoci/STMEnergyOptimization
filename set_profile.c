#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	int starting_threads;
	int static_pstate;
	double power_limit;
	int total_commits_round;
	double energy_per_tx_limit;
	int heuristic_mode;
	double jump_percentage;
	int detection_mode;
	double detection_tp_threshold;
	double detection_pwr_threshold; 
	int exploit_steps; 
	double extra_range_percentage;
	int window_size;
	double hysteresis;

	int output_starting_threads;
	int output_static_pstate;
	double output_power_limit;
	int output_total_commits_round;
	double output_energy_per_tx_limit;
	int output_heuristic_mode;
	double output_jump_percentage;
	int output_detection_mode;
	double output_detection_tp_threshold;
	double output_detection_pwr_threshold; 
	int output_exploit_steps;
	double output_extra_range_percentage;
	int output_window_size;
	double output_hysteresis;

	if(argc < 14){
		printf("Not enough parameters passed in input\n");
		exit(1);
	}

	output_starting_threads = atoi(argv[1]);
	output_static_pstate = atoi(argv[2]);
	output_power_limit = atof(argv[3]);
	output_total_commits_round = atoi(argv[4]);
	output_energy_per_tx_limit = atof(argv[5]);
	output_heuristic_mode = atoi(argv[6]);
	output_jump_percentage = atof(argv[7]);
	output_detection_mode = atoi(argv[8]);
	output_detection_tp_threshold = atof(argv[9]);
	output_detection_pwr_threshold = atof(argv[10]);
	output_exploit_steps = atoi(argv[11]);
	output_extra_range_percentage = atof(argv[12]);
	output_window_size = atoi(argv[13]);
	output_hysteresis = atof(argv[14]);

	// Load config file 
	FILE* config_file;
	if ((config_file = fopen("hope_config.txt", "r")) == NULL) {
		printf("Error opening STM_HOPE configuration file.\n");
		exit(1);
	}
	if (fscanf(config_file, "STARTING_THREADS=%d STATIC_PSTATE=%d POWER_LIMIT=%lf COMMITS_ROUND=%d ENERGY_PER_TX_LIMIT=%lf HEURISTIC_MODE=%d JUMP_PERCENTAGE=%lf DETECTION_MODE=%d DETECTION_TP_THRESHOLD=%lf DETECTION_PWR_THRESHOLD=%lf EXPLOIT_STEPS=%d EXTRA_RANGE_PERCENTAGE=%lf WINDOW_SIZE=%d HYSTERESIS=%lf", 
				 &starting_threads, &static_pstate, &power_limit, &total_commits_round, &energy_per_tx_limit, &heuristic_mode, &jump_percentage, &detection_mode, &detection_tp_threshold, &detection_pwr_threshold, &exploit_steps, &extra_range_percentage, &window_size, &hysteresis)!=14) {
		printf("The number of input parameters of the STM_HOPE configuration file does not match the number of required parameters.\n");
		exit(1);
	}
	fclose(config_file);

	if ((config_file = fopen("hope_config.txt", "w+")) == NULL) {
		printf("Error opening STM_HOPE configuration file.\n");
		exit(1);
	}
	fprintf(config_file, "STARTING_THREADS=%d\nSTATIC_PSTATE=%d\nPOWER_LIMIT=%lf\nCOMMITS_ROUND=%d\nENERGY_PER_TX_LIMIT=%lf\nHEURISTIC_MODE=%d\nJUMP_PERCENTAGE=%lf\nDETECTION_MODE=%d\nDETECTION_TP_THRESHOLD=%lf\nDETECTION_PWR_THRESHOLD=%lf\nEXPLOIT_STEPS=%d\nEXTRA_RANGE_PERCENTAGE=%lf\nWINDOW_SIZE=%d\nHYSTERESIS=%lf",
			output_starting_threads, output_static_pstate, output_power_limit, output_total_commits_round, output_energy_per_tx_limit, output_heuristic_mode, output_jump_percentage, output_detection_mode, output_detection_tp_threshold, output_detection_pwr_threshold, output_exploit_steps, output_extra_range_percentage, output_window_size, output_hysteresis);
	fflush(config_file);
	printf("File hope_config.txt modified\n");
	
	return 0;

}