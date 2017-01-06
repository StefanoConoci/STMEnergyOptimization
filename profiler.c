
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define ITERATIONS 100000
#define STARTING_VALUE 546723060

int pstate[32];					// Array of p-states initialized at startup with available scaling frequencies 
int max_pstate;					// Maximum index of available pstate for the running machine 
int current_pstate;				// Value of current pstate, index of pstate array which contains frequencies
int nb_cores; 					// Number of cores. Detected at startup and used to set DVFS parameters for all cores
int nb_packages;				// Number of packages in the system



// Returns energy consumption of package 0 cores in micro Joule
long get_energy(){
	
	long energy;
	int i;
	FILE* energy_file;
	long total_energy = 0;
	char fname[64];

	for(i = 0; i<nb_packages; i++){

		// Package energy consumtion
		sprintf(fname, "/sys/class/powercap/intel-rapl/intel-rapl:%d/energy_uj", i);
		energy_file = fopen(fname, "r");
		
		// Cores energy consumption
		//FILE* energy_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");	

		// DRAM module, considered inside the package
		//FILE* energy_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:1/energy_uj", "r");	

		if(energy_file == NULL){
			printf("Error opening energy file\n");		
		}
		fscanf(energy_file,"%ld",&energy);
		fclose(energy_file);
		total_energy+=energy;
	}

	return total_energy;
}

// Return time as a monotomically increasing long expressed as nanoseconds 
long get_time(){
	
	long time =0;
	struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time += (ts.tv_sec*1000000000);
    time += ts.tv_nsec;

	return time;
}

int set_p_state(int input_pstate){
	
	char fname[64];
	FILE* frequency_file;

	if(input_pstate > max_pstate)
		return -1;
	int frequency = pstate[input_pstate];

	for(int i=0; i<nb_cores;i++){
		sprintf(fname, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed", i);
		frequency_file = fopen(fname,"w+");
		if(frequency_file == NULL){
			printf("Error opening cpu%d scaling_setspeed file. Must be superuser\n", i);
			exit(0);		
		}		
		fprintf(frequency_file, "%d", frequency);
		fflush(frequency_file);
		fclose(frequency_file);
	}
	current_pstate = input_pstate;
		
	return 0;
}

// Executed inside stm_init: sets the governor to userspace and sets the highest frequency
int init_DVFS_management(){
	
	char fname[64];
	char* freq_available;
	char* token;
	int frequency;
	FILE* governor_file;
	char* filename;
	int package_last_core;
	FILE* numafile;

	//Set governor to userspace
	nb_cores = sysconf(_SC_NPROCESSORS_ONLN);
	for(int i=0; i<nb_cores;i++){
		sprintf(fname, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", i);
		governor_file = fopen(fname,"w+");
		if(governor_file == NULL){
			printf("Error opening cpu%d scaling_governor file. Must be superuser\n", i);
			exit(0);		
		}		
		fprintf(governor_file, "userspace");
		fflush(governor_file);
		fclose(governor_file);
	}

	
	// Init array of available frequencies
	FILE* available_freq_file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies","r");
	if(available_freq_file == NULL){
		printf("Cannot open scaling_available_frequencies file\n");
		exit(0);
	}
	freq_available = malloc(sizeof(char)*256);
	fgets(freq_available, 256, available_freq_file);
	
	int i = 0; 
	char * end;

	for (frequency = strtol(freq_available, &end, 10); freq_available != end; frequency = strtol(freq_available, &end, 10)){
		pstate[i]=frequency;
		freq_available = end;
  		i++;
	}
  	max_pstate = --i;

  	printf("Found %d p-states in the range from %d MHz to %d MHz\n", max_pstate, pstate[max_pstate]/1000, pstate[0]/1000);
  	fclose(available_freq_file);

  	//init number of packages
	filename = malloc(sizeof(char)*64); 
	sprintf(filename,"/sys/devices/system/cpu/cpu%d/topology/physical_package_id", nb_cores-1);
	numafile = fopen(filename,"r");
	if (numafile == NULL){
		printf("Cannot read number of packages\n");
		exit(1);
	} 
	fscanf(numafile ,"%d", &package_last_core);
	nb_packages = package_last_core+1;
	printf("Number of packages detected: %d\n", nb_packages);
	free(filename);

	return 0;
}

int isPrime(int number) {
    int i;
    for (i=2; i*i<=number; i++) {
        if (number % i == 0) return 0;
    }
    return 1;
}


void* loop_function(void* params){
	
	long start_value = STARTING_VALUE;
	long end_value = STARTING_VALUE+ITERATIONS;

	int cont;

	for(long i=start_value; i<end_value; i++){
		if(isPrime(i))
			cont++;
	}	

}

int main(int argc, char *argv[]){

	pthread_t* pthread_array;
	FILE* profile_file;
	long start_time, end_time, start_energy, end_energy;
	long time_interval;
	long energy_interval;
	int total_threads;
	int threads;
	int i;
	int p;

	if(argc < 2){
		printf("Please define the number of maximum threads\n");
		exit(1);
	}

	total_threads = atoi(argv[1]);

	profile_file = fopen("profile_file.txt","w+");
	
	printf("Power consumption profiler started\nProfiling on %d cores\n", total_threads);	
	init_DVFS_management();
	pthread_array = malloc(sizeof(pthread_t)*total_threads);	
	for( threads = 1; threads <= total_threads; threads++){
		for( p = 0; p<=max_pstate; p++){
			
			printf("Profiling with %d threads at frequency %d\n", threads, pstate[p]);
			set_p_state(p);
			start_time = get_time();
			start_energy = get_energy();

			for(i=0; i<threads; i++){
				if(pthread_create(&pthread_array[i], NULL, loop_function, NULL)){
					printf("Error creating threads\n");	
					return 0;					
				}
			}

			for(i=0; i<threads; i++)
				pthread_join(pthread_array[i], NULL);		

			end_time = get_time();
			end_energy = get_energy();

			time_interval = end_time - start_time;
			energy_interval = end_energy - start_energy;

			double power = ((double) energy_interval) / ( (double) time_interval / 1000 );
			fprintf(profile_file, "%lf\t", power);
		}
	}

	printf("Profiling completed\n");	
	
	return 0;
}
