/*
 * File:
 *   stm.c
 * Author(s):
 *   Pascal Felber <pascal.felber@unine.ch>
 *   Patrick Marlier <patrick.marlier@unine.ch>
 * Description:
 *   STM functions.
 *
 * Copyright (c) 2007-2012.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This program has a dual license and can also be distributed
 * under the terms of the MIT license.
 */

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <sched.h>

#include "stm.h"
#include "stm_internal.h"

#include "utils.h"
#include "atomic.h"
#include "gc.h"
#include "../../rapl-power/rapl.h"

#ifdef STM_HOPE
	#include <time.h>
	#include "stm_hope.h"
	#include "heuristics.c"
#endif

#ifdef ENERGY_DESKTOP
	#include <time.h>
#endif

/* ################################################################### *
 * DEFINES
 * ################################################################### */

#ifdef PRINT_STATS_INFO
#define PRINT_STATS(...)			printf(__VA_ARGS__)
#else
#define PRINT_STATS(...)
#endif

/* Indexes are defined in stm_internal.h  */
static const char *design_names[] = {
  /* 0 */ "WRITE-BACK (ETL)",
  /* 1 */ "WRITE-BACK (CTL)",
  /* 2 */ "WRITE-THROUGH",
  /* 3 */ "WRITE-MODULAR"
};

static const char *cm_names[] = {
  /* 0 */ "SUICIDE",
  /* 1 */ "DELAY",
  /* 2 */ "BACKOFF",
  /* 3 */ "MODULAR"
};

/* Global variables */
global_t _tinystm =
    { .nb_specific = 0
    , .initialized = 0
#ifdef IRREVOCABLE_ENABLED
    , .irrevocable = 0
#endif /* IRREVOCABLE_ENABLED */
    };


/* ################################################################### *
 * STM_HOPE FUNCTIONS
 * ################################################################### */

#ifdef STM_HOPE

	// Used by the heuristic 
	int set_pstate(int input_pstate){
		
		int i;
		char fname[64];
		FILE* frequency_file;

		#ifdef DEBUG_OVERHEAD
			long time_heuristic_start;
			long time_heuristic_end;
			double time_heuristic_microseconds;

			time_heuristic_start = get_time();
		#endif 
		
		if(input_pstate > max_pstate)
			return -1;
			

		if(current_pstate != input_pstate){
			int frequency = pstate[input_pstate];

			for(i=0; i<nb_cores; i++){
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
		}

		#ifdef DEBUG_OVERHEAD
			time_heuristic_end = get_time();
			time_heuristic_microseconds = (((double) time_heuristic_end) - ((double) time_heuristic_start))/1000;
			printf("DEBUG OVERHEAD - inside set_pstate() %lf microseconds\n", time_heuristic_microseconds);
		#endif 
		
		return 0;
	}

	// Executed inside stm_init: sets the governor to userspace and sets the highest frequency
	int init_DVFS_management(){
		
		char fname[64];
		char* freq_available;
		int frequency, i;
		FILE* governor_file;

		//Set governor to userspace
		nb_cores = sysconf(_SC_NPROCESSORS_ONLN);
		for(i=0; i<nb_cores;i++){
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
		
		pstate = malloc(sizeof(int)*32);
		i = 0; 
		char * end;

		for (frequency = strtol(freq_available, &end, 10); freq_available != end; frequency = strtol(freq_available, &end, 10)){
			pstate[i]=frequency;
			freq_available = end;
	  		i++;
		}
	  	max_pstate = --i;

		#ifdef DEBUG_HEURISTICS
	  		printf("Found %d p-states in the range from %d MHz to %d MHz\n", max_pstate, pstate[max_pstate]/1000, pstate[0]/1000);
	  	#endif
	  	fclose(available_freq_file);


	  	set_pstate(max_pstate);

		return 0;
	}

	// SIGUSR1 handler. Doesn't need to execute any code
	void sig_func(int sig){}

	// Executed inside stm_init
	void init_thread_management(int threads){

		char* filename;
		FILE* numafile;
		int package_last_core;
		int i;

		// Init total threads and active threads
		total_threads = threads;

		#ifdef DEBUG_HEURISTICS
			printf("Set total_threads to %d\n", threads);
		#endif

		active_threads = total_threads;

		// Init running array with all threads running 	
		running_array = malloc(sizeof(int)*total_threads);
		for(i=0; i<total_threads; i++)
			running_array[i] = 1;	
		

		// Allocate memory for pthread_ids
		pthread_ids = malloc(sizeof(pthread_t)*total_threads);

		//Registering SIGUSR1 handler
		signal(SIGUSR1, sig_func);

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

		#ifdef DEBUG_HEURISTICS
			printf("Number of packages detected: %d\n", nb_packages);
		#endif
	}


	// Executed inside stm_init
	inline void check_running_array(int threadId){
		
		while(running_array[threadId] == 0){
			pause();
		}
	}

	// Used by the heuristics to tune the number of active threads 
	inline int wake_up_thread(int thread_id){
		
		if( running_array[thread_id] == 1){
			printf("Waking up a thread already running\n");
			return -1;
		}

		running_array[thread_id] = 1;
		pthread_kill(pthread_ids[thread_id], SIGUSR1);
		active_threads++;
		return 0;
	}

	// Used by the heuristics to tune the number of active threads 
	inline int pause_thread(int thread_id){

		if( running_array[thread_id] == 0 ){
			printf("Pausing a thread already paused\n");
			return -1;
		}

		running_array[thread_id] = 0;
		active_threads--;
		return active_threads;
	}

	// Executed inside stm_init
	void init_stats_array_pointer(int threads){

		// Allocate memory for the pointers of stats_t
		stats_array = malloc(sizeof(stats_t*)*threads); 

		cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

		#ifdef DEBUG_HEURISTICS
			printf("D1 cache line size: %d bytes\n", cache_line_size);
		#endif
	}

	// Executed by each thread inside stm_pre_init_thread
	stats_t* alloc_stats_buffer(int thread_number){
		
		stats_t* stats_ptr = stats_array[thread_number];

		int ret = posix_memalign(((void**) &stats_ptr), cache_line_size, sizeof(stats_t));
		if ( ret != 0 ){ printf("Error allocating stats_t for thread %d\n", thread_number);
			exit(0);
		}

		stats_ptr->total_commits = total_commits_round/active_threads;
		stats_ptr->commits = 0;
		stats_ptr->aborts = 0;
		stats_ptr->nb_tx = 0;
		stats_ptr->start_energy = 0;
		stats_ptr->end_energy = 0;
		stats_ptr->start_time = 0;
		stats_ptr->end_time = 0;

		stats_array[thread_number] = stats_ptr;

		return stats_ptr;
	}

	// Executed inside stm_init to load the precomputed power profile for the current machine and put inside power_profile
	// Rows are threads, columns are p-states. It has total_threads+1 rows as first row is filled with 0 for the profile with 0 threads 
	void load_profile_file(){

		double power;
		int i;

		// Allocate the matrix 
		power_profile = (double**) malloc(sizeof(double*) * (total_threads+1)); 
		for (i = 0; i < (total_threads+1); i++)
  	   		power_profile[i] = (double *) malloc(sizeof(double) * (max_pstate+1));

  	   	// Init first row with all zeros 
  	   	for(i =0; i<=max_pstate; i++){
  	   		power_profile[0][i] = 0;
  	   	}

  	   	// Open file and copy to string
		FILE* profile_file = fopen("profile_file.txt","r");
		if(profile_file == NULL){
			printf("Cannot open profile_file. Execute the profiler in root folder to profile the machine power consumption\n");
			exit(0);
		}
		char* profile_string = malloc(sizeof(char)*8192);
		fgets(profile_string, 8192, profile_file);
  	   	
  	   	char * end;
  	   	i=1;
  	   	int j=0; 
		for (power = strtod(profile_string, &end); profile_string != end && i <= total_threads; power = strtod(profile_string, &end)){
			power_profile[i][j] = power;
			profile_string = end;
	  		if( j == max_pstate){
	  			i++;
	  			j = 0;
	  		}
	  		else j++;
		}

		#ifdef DEBUG_HEURISTICS
			printf("Power consumption profile loaded\n");
		#endif
	}

	void load_config_file(){
		
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
		if(detection_tp_threshold < 0.0 || detection_tp_threshold > 100.0 || detection_pwr_threshold < 0.0 || detection_pwr_threshold >100.0){
			printf("The detection percentage thresholds are set to values outside their valid range.\n");
			exit(1);
		}

	  	if(extra_range_percentage < 0 || extra_range_percentage > 100){
	  		printf("Extra_range_percentage value is not a percentage. Should be a floating point number in the range from 0 to 100\n");
	  		exit(1);
	  	}


		if(hysteresis < 0 || hysteresis > 100){
	  		printf("Hysteresis value is not a percentage. Should be a floating point number in the range from 0 to 100\n");
	  		exit(1);
	  	}

	  	// Necessary for the static execution in order to avoid running for the first step with a different frequency than manually set in hope_config.txt
	  	if(heuristic_mode == 8){
	  		if(static_pstate >= 0 && static_pstate <= max_pstate)
	  			set_pstate(static_pstate);
	  		else 
	  			printf("The parameter manual_pstate is set outside of the valid range for this CPU. Setting the CPU to the slowest frequency/voltage\n");
	  	}

		fclose(config_file);
	}


	// Returns energy consumption of package 0 cores in micro Joule
	long get_energy(){


		#ifdef DEBUG_OVERHEAD
			long time_heuristic_start;
			long time_heuristic_end;
			double time_heuristic_microseconds;

			time_heuristic_start = get_time();
		#endif 
		
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

		#ifdef DEBUG_OVERHEAD
			time_heuristic_end = get_time();
			time_heuristic_microseconds = (((double) time_heuristic_end) - ((double) time_heuristic_start))/1000;
			printf("DEBUG OVERHEAD -  Inside get_energy(): %lf microseconds\n", time_heuristic_microseconds);
		#endif 

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

	// Function used to set the number of running threads. Based on active_threads and threads might wake up or pause some threads 
	inline void set_threads(int to_threads){

		#ifdef DEBUG_OVERHEAD
			long time_heuristic_start;
			long time_heuristic_end;
			double time_heuristic_microseconds;

			time_heuristic_start = get_time();
		#endif 

		int i;
		int starting_threads = active_threads;

		if(starting_threads != to_threads){
			if(starting_threads > to_threads){
				for(i = to_threads; i<starting_threads; i++)
					pause_thread(i);
			}
			else{
				for(i = starting_threads; i<to_threads; i++)
					wake_up_thread(i);
			}
		}

		#ifdef DEBUG_OVERHEAD
			time_heuristic_end = get_time();
			time_heuristic_microseconds = (((double) time_heuristic_end) - ((double) time_heuristic_start))/1000;
			printf("DEBUG OVERHEAD -  Inside set_threads(): %lf microseconds\n", time_heuristic_microseconds);
		#endif 
	}

	// Initialization of global variables 
	inline void init_global_variables(){

		#ifdef DEBUG_HEURISTICS
			printf("Initializing global variables\n");
		#endif

		round_completed=0;
		old_throughput = -1;
		old_power = -1;
		old_energy_per_tx = -1;
		level_best_throughput = -1; 
		level_best_threads = 0;
		level_starting_threads = starting_threads;
		best_throughput = -1;
		new_pstate = 1;
		decreasing = 0;
		stopped_searching = 0;
		steps=0;
		shutdown = 0;
		effective_commits = 0;
		phase = 0; 
		current_exploit_steps = 0;

		high_throughput = -1;
		high_threads = -1;
		high_pstate = -1;

		lock_commits = 0; 

		low_throughput = -1;
		low_threads = -1;
		low_pstate = -1;

		current_window_slot = 0;
		window_time = 0;
		window_power = 0;
	}

#endif/* ! STM_HOPE */


/* ################################################################### *
 * ENERGY_DESKTOP
 * ################################################################### */

#ifdef ENERGY_DESKTOP

	long start_package_energy, end_package_energy;
	long start_core_energy, end_core_energy;
	long start_dram_energy, end_dram_energy; 

	long start_time, end_time;

	// Return time as a monotomically increasing long expressed as nanoseconds 
	long read_time_energy(){

		long time =0;
		struct timespec ts;

		clock_gettime(CLOCK_MONOTONIC, &ts);
		time += (ts.tv_sec*1000000000);
		time += ts.tv_nsec;

		return time;
	}


	// Overhead around 0.13 ms 
	void set_start_energy_counters(){
		long power;

		// Package 0 power consumtion
		FILE* package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		start_package_energy = power;
		
		// Package 0 cores power consumption
		FILE* core_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");	
		if(core_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(core_file,"%ld",&power);
		fclose(core_file);
		start_core_energy = power;

		// Package 0 DRAM module, considered as a child of the package
		FILE* dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:1/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		start_dram_energy = power;

		start_time = read_time_energy();
	}

	// Energy consumed is expressed in Joule, power expressed in Watt. 
	void print_energy_counters(){
		
		double runtime; 
		double package_energy_consumed, core_energy_consumed, dram_energy_consumed;
		double package_power, core_power, dram_power;
		double effective_throughput;
		double effective_energy_per_tx;

		long power; 

		end_time = read_time_energy();

		// Package 0 power consumtion
		FILE* package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		end_package_energy = power;
		
		// Package 0 cores power consumption
		FILE* core_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");	
		if(core_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(core_file,"%ld",&power);
		fclose(core_file);
		end_core_energy = power;

		// Package 0 DRAM module, considered as a child of the package
		FILE* dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:1/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		end_dram_energy = power;

		// Computing aggregated results 
		runtime = ( (double) end_time - start_time)/1000000000;
    	
    	package_energy_consumed = ( (double) end_package_energy - start_package_energy)/1000000;
    	core_energy_consumed = ( (double) end_core_energy - start_core_energy)/1000000;
    	dram_energy_consumed = ( (double) end_dram_energy - start_dram_energy)/1000000;

   		package_power = (package_energy_consumed)/runtime;
   		core_power = (core_energy_consumed)/runtime;
   		dram_power = (dram_energy_consumed)/runtime;

   		effective_throughput = effective_commits/runtime;
   		effective_energy_per_tx = (package_energy_consumed/effective_commits)*1000000; //Expressed in microJoule
   		
   		#ifdef REDUCED_VERBOSITY
   			printf("Effective_runtime: %lf\tEffective_commits: %ld\tEffective_throughput: %lf\tEffective_energy_per_tx: %lf\tPkg_power: %lf\t",
   				   runtime, effective_commits, effective_throughput, effective_energy_per_tx, package_power);
   		#else
   			printf("Effective_runtime: %lf\tEffective_commits: %ld\tEffective_throughput: %lf\tEffective_energy_per_tx: %lf\tPkg_energy: %lf\tCore_energy: %lf\tDram_energy: %lf\tPkg_power: %lf\tCore_power: %lf\tDram_power: %lf\t",
   				   runtime, effective_commits, effective_throughput, effective_energy_per_tx, package_energy_consumed, core_energy_consumed, dram_energy_consumed, package_power, core_power, dram_power);
   		#endif   		
	
	}

#endif /* ! ENERGY_DESKTOP */


/* ################################################################### *
 * ENERGY_SERVER
 * ################################################################### */

#ifdef ENERGY_SERVER

	long start_package0_energy, end_package0_energy;
	long start_package1_energy, end_package1_energy;

	long start_dram0_energy, end_dram0_energy;
	long start_dram1_energy, end_dram1_energy; 

	long start_time, end_time;

	// Return time as a monotomically increasing long expressed as nanoseconds 
	long read_time_energy(){

		long time =0;
		struct timespec ts;

		clock_gettime(CLOCK_MONOTONIC, &ts);
		time += (ts.tv_sec*1000000000);
		time += ts.tv_nsec;

		return time;
	}


	// Overhead not yet computed for server 
	void set_start_energy_counters(){
		long power;

		// Package 0 power consumption
		FILE* package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:1/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		start_package0_energy = power;

		// Package 1 power consumption
		package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		start_package1_energy = power;

		//Package 0 DRAM module, considered as a child of the package
		FILE* dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:1/intel-rapl:1:0/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		start_dram0_energy = power;

		// Package 1 DRAM module, considered as a child of the package
		dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		start_dram1_energy = power;

		start_time = read_time_energy();
	}

	// Energy consumed is expressed in Joule, power expressed in Watt. 
	void print_energy_counters(){
		
		double runtime; 
		double package0_energy_consumed, package1_energy_consumed, dram0_energy_consumed, dram1_energy_consumed;
		double package0_power, package1_power;

		#ifndef REDUCED_VERBOSITY
			double dram0_power, dram1_power;
		#endif

		long power; 

		double effective_throughput;
		double effective_energy_per_tx;
		double summed_power;
		double summed_energy;

		end_time = read_time_energy();

		// Package 0 power consumption
		FILE* package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:1/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		end_package0_energy = power;

		// Package 1 power consumption
		package_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");
		if(package_file == NULL){
			printf("Error opening package power file\n");		
		}
		fscanf(package_file,"%ld",&power);
		fclose(package_file);
		end_package1_energy = power;

		//Package 0 DRAM module, considered as a child of the package
		FILE* dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:1/intel-rapl:1:0/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		end_dram0_energy = power;

		// Package 1 DRAM module, considered as a child of the package
		dram_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");	
		if(dram_file == NULL){
			printf("Error opening core power file\n");		
		}
		fscanf(dram_file,"%ld",&power);
		fclose(dram_file);
		end_dram1_energy = power;

		// Computing aggregated results 
		runtime = ( (double) end_time - start_time)/1000000000;
    	
    	package0_energy_consumed = ( (double) end_package0_energy - start_package0_energy)/1000000;
    	package1_energy_consumed = ( (double) end_package1_energy - start_package1_energy)/1000000;

    	#ifndef REDUCED_VERBOSITY
    		dram0_energy_consumed = ( (double) end_dram0_energy - start_dram0_energy)/1000000;
    		dram1_energy_consumed = ( (double) end_dram1_energy - start_dram1_energy)/1000000;
    	#endif

   		package0_power = (package0_energy_consumed)/runtime;
   		package1_power = (package1_energy_consumed)/runtime;

   		#ifndef REDUCED_VERBOSITY
   			dram0_power = (dram0_energy_consumed)/runtime;
   			dram1_power = (dram1_energy_consumed)/runtime;
   		#endif

   		summed_energy = package0_energy_consumed+package1_energy_consumed;
   		summed_power = package0_power+package1_power;

   		effective_throughput = effective_commits/runtime;
   		effective_energy_per_tx = (summed_energy/effective_commits)*1000000; //Expressed in microJoule
   		
   		#ifdef REDUCED_VERBOSITY
   			printf("Effective_runtime: %lf\tEffective_commits: %ld\tEffective_throughput: %lf\tEffective_energy_per_tx: %lf\tPkg_power: %lf\t",
   				   runtime, effective_commits, effective_throughput, effective_energy_per_tx, summed_power);
   		#else
   				printf("Effective_runtime: %lf\tPkg0_energy: %lf\tDram0_energy: %lf\tPkg1_energy: %lf\tDram1_energy: %lf\tPkg0_power: %lf\tDram0_power: %lf\tPkg1_power: %lf\tDram1_power: %lf",
   				   runtime,package0_energy_consumed, dram0_energy_consumed, package1_energy_consumed, dram1_energy_consumed, package0_power, dram0_power, package1_power, dram1_power);
	
   		#endif   		
	}




#endif /* ! ENERGY_SERVER */


/* ################################################################### *
 * TYPES
 * ################################################################### */


/*
 * Transaction nesting is supported in a minimalist way (flat nesting):
 * - When a transaction is started in the context of another
 *   transaction, we simply increment a nesting counter but do not
 *   actually start a new transaction.
 * - The environment to be used for setjmp/longjmp is only returned when
 *   no transaction is active so that it is not overwritten by nested
 *   transactions. This allows for composability as the caller does not
 *   need to know whether it executes inside another transaction.
 * - The commit of a nested transaction simply decrements the nesting
 *   counter. Only the commit of the top-level transaction will actually
 *   carry through updates to shared memory.
 * - An abort of a nested transaction will rollback the top-level
 *   transaction and reset the nesting counter. The call to longjmp will
 *   restart execution before the top-level transaction.
 * Using nested transactions without setjmp/longjmp is not recommended
 * as one would need to explicitly jump back outside of the top-level
 * transaction upon abort of a nested transaction. This breaks
 * composability.
 */

/*
 * Reading from the previous version of locked addresses is implemented
 * by peeking into the write set of the transaction that owns the
 * lock. Each transaction has a unique identifier, updated even upon
 * retry. A special "commit" bit of this identifier is set upon commit,
 * right before writing the values from the redo log to shared memory. A
 * transaction can read a locked address if the identifier of the owner
 * does not change between before and after reading the value and
 * version, and it does not have the commit bit set.
 */

/* ################################################################### *
 * THREAD-LOCAL
 * ################################################################### */

#if defined(TLS_POSIX) || defined(TLS_DARWIN)
/* TODO this may lead to false sharing. */
/* TODO this could be renamed with tinystm prefix */
pthread_key_t thread_tx;
pthread_key_t thread_gc;
#elif defined(TLS_COMPILER)
__thread stm_tx_t* thread_tx = NULL;
__thread long thread_gc = 0;
#endif /* defined(TLS_COMPILER) */

/* ################################################################### *
 * STATIC
 * ################################################################### */

#if CM == CM_MODULAR
/*
 * Kill other.
 */
static int
cm_aggressive(struct stm_tx *me, struct stm_tx *other, int conflict)
{
  return KILL_OTHER;
}

/*
 * Kill self.
 */
static int
cm_suicide(struct stm_tx *me, struct stm_tx *other, int conflict)
{
  return KILL_SELF;
}

/*
 * Kill self and wait before restart.
 */
static int
cm_delay(struct stm_tx *me, struct stm_tx *other, int conflict)
{
  return KILL_SELF | DELAY_RESTART;
}

/*
 * Oldest transaction has priority.
 */
static int
cm_timestamp(struct stm_tx *me, struct stm_tx *other, int conflict)
{
  if (me->timestamp < other->timestamp)
    return KILL_OTHER;
  if (me->timestamp == other->timestamp && (uintptr_t)me < (uintptr_t)other)
    return KILL_OTHER;
  return KILL_SELF | DELAY_RESTART;
}

/*
 * Transaction with more work done has priority.
 */
static int
cm_karma(struct stm_tx *me, struct stm_tx *other, int conflict)
{
  unsigned int me_work, other_work;

  me_work = (me->w_set.nb_entries << 1) + me->r_set.nb_entries;
  other_work = (other->w_set.nb_entries << 1) + other->r_set.nb_entries;

  if (me_work < other_work)
    return KILL_OTHER;
  if (me_work == other_work && (uintptr_t)me < (uintptr_t)other)
    return KILL_OTHER;
  return KILL_SELF;
}

struct {
  const char *name;
  int (*f)(stm_tx_t *, stm_tx_t *, int);
} cms[] = {
  { "aggressive", cm_aggressive },
  { "suicide", cm_suicide },
  { "delay", cm_delay },
  { "timestamp", cm_timestamp },
  { "karma", cm_karma },
  { NULL, NULL }
};
#endif /* CM == CM_MODULAR */

#ifdef SIGNAL_HANDLER
/*
 * Catch signal (to emulate non-faulting load).
 */
static void
signal_catcher(int sig)
{
  sigset_t block_signal;
  stm_tx_t *tx = tls_get_tx();

  /* A fault might only occur upon a load concurrent with a free (read-after-free) */
  PRINT_DEBUG("Caught signal: %d\n", sig);

  /* TODO: TX_KILLED should be also allowed */
  if (tx == NULL || tx->attr.no_retry || GET_STATUS(tx->status) != TX_ACTIVE) {
    /* There is not much we can do: execution will restart at faulty load */
    fprintf(stderr, "Error: invalid memory accessed and no longjmp destination\n");
    exit(1);
  }

  /* Unblock the signal since there is no return to signal handler */
  sigemptyset(&block_signal);
  sigaddset(&block_signal, sig);
  pthread_sigmask(SIG_UNBLOCK, &block_signal, NULL);

  /* Will cause a longjmp */
  stm_rollback(tx, STM_ABORT_SIGNAL);
}
#endif /* SIGNAL_HANDLER */


/* ################################################################### *
 * STM FUNCTIONS
 * ################################################################### */

/*
 * Called once (from main) to initialize STM infrastructure.
 */
#  ifdef STM_HOPE

void stm_init(int threads) {
	
	int i;

	#ifdef DEBUG_HEURISTICS
		printf("TinySTM - STM_HOPE mode started\n");
	#endif

	/* This seems to be useless, its all already on node 0 expect stats arrays which should be local
	// Set mem_policy to numa node 0 
	if(numa_available() < 0){
		printf("System doesn't support NUMA API\n")
	} else{
		numa_set_preferred(0);
		printf("Set node 0 as preferred numa node for memory allocation\n");
	}*/

	init_DVFS_management();
	init_thread_management(threads);
	load_profile_file();
	init_stats_array_pointer(threads);
	load_config_file();
	init_global_variables();

	#ifdef LOCK_BASED_TRANSACTIONS
		int ret_spin_init = pthread_spin_init(&spinlock_variable, PTHREAD_PROCESS_PRIVATE);
		if(ret_spin_init != 0){
			printf("Error initiliazing the spinlock. Aborting application\n");
			exit(1);
		}
	#endif

	#ifdef DEBUG_HEURISTICS
		printf("Heuristic mode: %d\n", heuristic_mode);
	#endif

	if(starting_threads > total_threads){
		printf("Starting threads set higher than total threads. Please modify this value in hope_config.txt\n");
		exit(1);
	}
	
	// Set active_threads to starting_threads
	for(i = starting_threads; i<total_threads;i++){
		pause_thread(i);
	}
	
	
#else

void stm_init(){

#endif /* ! STM_HOPE */

#if CM == CM_MODULAR
  char *s;
#endif /* CM == CM_MODULAR */

#ifdef SIGNAL_HANDLER
  struct sigaction act;
#endif /* SIGNAL_HANDLER */

  PRINT_DEBUG("==> stm_init()\n");

  if (_tinystm.initialized)
    return;

  PRINT_DEBUG("\tsizeof(word)=%d\n", (int)sizeof(stm_word_t));

  PRINT_DEBUG("\tVERSION_MAX=0x%lx\n", (unsigned long)VERSION_MAX);

  COMPILE_TIME_ASSERT(sizeof(stm_word_t) == sizeof(void *));
  COMPILE_TIME_ASSERT(sizeof(stm_word_t) == sizeof(atomic_t));

#ifdef EPOCH_GC
  gc_init(stm_get_clock);
#endif /* EPOCH_GC */

#if CM == CM_MODULAR
  s = getenv(VR_THRESHOLD);
  if (s != NULL)
    _tinystm.vr_threshold = (int)strtol(s, NULL, 10);
  else
    _tinystm.vr_threshold = VR_THRESHOLD_DEFAULT;
  PRINT_DEBUG("\tVR_THRESHOLD=%d\n", _tinystm.vr_threshold);
#endif /* CM == CM_MODULAR */

  /* Set locks and clock but should be already to 0 */
  memset((void *)_tinystm.locks, 0, LOCK_ARRAY_SIZE * sizeof(stm_word_t));
#ifdef INVISIBLE_TRACKING
  memset((void* )_tinystm.last_id_tx_class,-1,LOCK_ARRAY_SIZE * sizeof(stm_word_t));
#endif
  CLOCK = 0;

  stm_quiesce_init();

  tls_init();

#ifdef SIGNAL_HANDLER
  if (getenv(NO_SIGNAL_HANDLER) == NULL) {
    /* Catch signals for non-faulting load */
    act.sa_handler = signal_catcher;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGBUS, &act, NULL) < 0 || sigaction(SIGSEGV, &act, NULL) < 0) {
      perror("sigaction");
      exit(1);
    }
  }
#endif /* SIGNAL_HANDLER */

  _tinystm.initialized = 1;
}

/*
 * Called once (from main) to clean up STM infrastructure.
 */
_CALLCONV void
stm_exit(void)
{
  PRINT_DEBUG("==> stm_exit()\n");

  if (!_tinystm.initialized)
    return;
  tls_exit();
  stm_quiesce_exit();

  terminate_rapl();

  #ifdef STM_HOPE
  	printf("\tP-state: %d\tBest-threads: %d\tSteps: %d", best_pstate, best_threads, steps);
  #endif


	#ifdef EPOCH_GC
  	gc_exit();
	#endif /* EPOCH_GC */
  _tinystm.initialized = 0;
}

/*
 * Called by the CURRENT thread to initialize thread-local STM data.
 */

_CALLCONV stm_tx_t *
stm_init_thread(void)
{
	return int_stm_init_thread();
}

/*
 * Called by the CURRENT thread to cleanup thread-local STM data.
 */
_CALLCONV void
stm_exit_thread(void)
{
  
  TX_GET;
  #ifdef STM_HOPE
  	int i;

  	// When thread 0 completes wake up all threads 
  	if(tx->thread_number == 0){
  		shutdown = 1;

  		#ifdef ENERGY_DESKTOP
  			print_energy_counters();
  		#endif

  		#ifdef ENERGY_SERVER
  			print_energy_counters();
  		#endif

  		for(i=active_threads; i< total_threads; i++){
  			wake_up_thread(i);
  		}	
  	}
  #endif

  int_stm_exit_thread(tx);
}

_CALLCONV void
stm_exit_thread_tx(stm_tx_t *tx)
{

  int_stm_exit_thread(tx);
}

/*
 * Called by the CURRENT thread to start a transaction.
 */
_CALLCONV sigjmp_buf *
stm_start(stm_tx_attr_t attr)
{

  TX_GET;
  sigjmp_buf * ret;
  
  #ifdef STM_HOPE
  	int i;
  	stm_wait(tx->thread_number);

  	#ifdef LOCK_BASED_TRANSACTIONS

	  	if(tx->thread_number == 0 && lock_commits >= total_commits_round){
	  		
	  		pthread_spin_lock(&spinlock_variable);

	  		double throughput;		// Expressed as commit per second
			double power;			// Expressed in Watt
			double abort_rate;
			double energy_per_tx;	// Expressed in micro Joule 

			long time_interval, energy_interval;

			lock_end_time = get_time();
			lock_end_energy = get_energy();

			time_interval = lock_end_time - lock_start_time; //Expressed in nano seconds 
			energy_interval = lock_end_energy - lock_start_energy; // Expressed in micro Joule

			//DEBUG
			printf("Start energy: %ld\nEnd energy: %ld\n", lock_start_energy, lock_end_energy );
			//

			throughput = ((double) lock_commits) / (((double) time_interval)/ 1000000000);
			abort_rate = 0;
			power = ((double) energy_interval) / (((double) time_interval)/ 1000);
			energy_per_tx = ((double) energy_interval) / (lock_commits);

			effective_commits += lock_commits;

			// We don't call the heuristic if the energy results are out or range due to an overflow 
			if(power > 0 && energy_per_tx > 0)
					heuristic(throughput, abort_rate, power, energy_per_tx, time_interval);

			//Setup next round
			lock_commits = 0;
			lock_start_energy = get_energy();
			lock_start_time = get_time();
			lock_end_energy = 0;
			lock_start_energy = 0;

			pthread_spin_unlock(&spinlock_variable);
	  	}

  	#else 

	  	// Retrive stats if collector 
	  	if(tx->stats_ptr->collector == 1){

	  		// If thread 0 and round completed should collect stats and call heuristic
	  		if(tx->thread_number == 0 && round_completed){

				// Compute aggregated statistics for the current round
				
				double throughput;		// Expressed as commit per second
				double power;			// Expressed in Watt
				double abort_rate;
				double energy_per_tx;	// Expressed in micro Joule 


				long energy_sum = 0;	// Expressed in micro Joule
				long time_sum = 0;		// Expressed in nano seconds 
				long aborts_sum = 0; 
				long commits_sum = 0;

				for(i=0; i<active_threads; i++){
					energy_sum += ((stats_array[i]->end_energy) - (stats_array[i]->start_energy));
					time_sum += ((stats_array[i]->end_time) - (stats_array[i]->start_time));
					aborts_sum += stats_array[i]->aborts;
					commits_sum += stats_array[i]->commits;
				}

				throughput = (((double) commits_sum)*active_threads) / ( ((double) time_sum) / 1000000000 );
				abort_rate = (((double) aborts_sum) / (aborts_sum+commits_sum))*100; 
				power = ((double) energy_sum) / ( (double) time_sum / 1000 );
				energy_per_tx = ((double) energy_sum) / (commits_sum*active_threads);

				effective_commits+= (commits_sum*active_threads);

				// We don't call the heuristic if the energy results are out or range due to an overflow 
				if(power > 0 && energy_per_tx > 0)
					heuristic(throughput, abort_rate, power, energy_per_tx, time_sum);
			
				//Setup next round
				int slice = total_commits_round/active_threads;
				for(i=0; i<active_threads; i++){
					stats_array[i]->nb_tx = 0;
					stats_array[i]->total_commits = slice; 
				}
				round_completed = 0;
	  		}

	  		stats_t* stats = tx->stats_ptr;
	  		
	  		// First tx for this round
	  		if(stats->nb_tx == 0){ 
	  			
	  			stats->commits = 0;
	  			stats->aborts = 0;

	  			stats->start_energy = get_energy();
	  			stats->start_time = get_time();
	  		}

	  		stats->nb_tx++;
	  	}

  	#endif

  #endif

  #if defined(STM_HOPE) && defined (LOCK_BASED_TRANSACTIONS)
  		pthread_spin_lock(&spinlock_variable);
  		return NULL;
  #else
	  ret=int_stm_start(tx, attr);
	  return ret;
  #endif
}


_CALLCONV stm_tx_t *stm_pre_init_thread(int id){
	#ifdef STM_HOPE
		stm_tx_t *tx;
		tx=stm_init_thread();

		tx->thread_number = id;
		pthread_ids[id] = pthread_self();
		tx->stats_ptr = alloc_stats_buffer(id);

		// Thread 0 sets itself as a collector and inits global variables or init global variables if lock based
		if( id == 0){

			#ifdef LOCK_BASED_TRANSACTIONS
				lock_start_energy = get_energy();
				lock_start_time = get_time();
			#else 
				tx->stats_ptr->collector = 1;
			#endif
		
			#ifdef ENERGY_DESKTOP
  			set_start_energy_counters();
			#endif 

			#ifdef ENERGY_SERVER
  			set_start_energy_counters();
			#endif 
		}

		return tx;
	
	#else
		return stm_init_thread();
	#endif
}

inline void stm_wait(int id) {
	#ifdef STM_HOPE
  		check_running_array(id);
  #endif
}



_CALLCONV sigjmp_buf *
stm_start_tx(stm_tx_t *tx, stm_tx_attr_t attr)
{
  return int_stm_start(tx, attr);
}

/*
 * Called by the CURRENT thread to commit a transaction.
 */
__thread unsigned int inc;

_CALLCONV int
stm_commit(void)
{
	TX_GET;
	int ret;

	#if defined(STM_HOPE) && defined(LOCK_BASED_TRANSACTIONS)

		lock_commits++;
		pthread_spin_unlock(&spinlock_variable);

		/* Set status to COMMITTED */
  		SET_STATUS(tx->status, TX_COMMITTED);
		 /* Callbacks */
		 if (likely(_tinystm.nb_commit_cb != 0)) {
		    unsigned int cb;
		    for (cb = 0; cb < _tinystm.nb_commit_cb; cb++)
		      _tinystm.commit_cb[cb].f(_tinystm.commit_cb[cb].arg);
		}

		ret=0;
	#else
		ret=int_stm_commit(tx);
	#endif

	#ifdef STM_HOPE && !defined(LOCK_BASED_TRANSACTIONS)
		// Retrive stats if collector 
	  	if(tx->stats_ptr->collector == 1){
	  		
	  		stats_t* stats = tx->stats_ptr;
	  		stats->commits++;

	  		// Round completed 
	  		if(stats->commits == stats->total_commits){

	  			stats->end_energy = get_energy();
	  			stats->end_time = get_time();
	  			stats->collector = 0;

	  			if(tx->thread_number == (active_threads-1)){
	  				stats_array[0]->collector = 1;
	  				round_completed = 1;
	  			}
	  			else{  				
	  				int next = (tx->thread_number)+1;
	  				stats_array[next]->collector = 1;
	  			}
	  		}
	  	}
	#endif

	return ret;
}

_CALLCONV int
stm_commit_tx(stm_tx_t *tx)
{
  return int_stm_commit(tx);
}

/*
 * Called by the CURRENT thread to abort a transaction.
 */
_CALLCONV void
stm_abort(int reason)
{
  TX_GET;
  stm_rollback(tx, reason | STM_ABORT_EXPLICIT);
}

_CALLCONV void
stm_abort_tx(stm_tx_t *tx, int reason)
{
  stm_rollback(tx, reason | STM_ABORT_EXPLICIT);
}

/*
 * Called by the CURRENT thread to load a word-sized value.
 */
_CALLCONV ALIGNED stm_word_t
stm_load(volatile stm_word_t *addr)
{
  TX_GET;
  return int_stm_load(tx, addr);
}

_CALLCONV stm_word_t
stm_load_tx(stm_tx_t *tx, volatile stm_word_t *addr)
{
  return int_stm_load(tx, addr);
}

/*
 * Called by the CURRENT thread to store a word-sized value.
 */
_CALLCONV ALIGNED void
stm_store(volatile stm_word_t *addr, stm_word_t value)
{
  TX_GET;
  int_stm_store(tx, addr, value);
}

_CALLCONV void
stm_store_tx(stm_tx_t *tx, volatile stm_word_t *addr, stm_word_t value)
{
  int_stm_store(tx, addr, value);
}

/*
 * Called by the CURRENT thread to store part of a word-sized value.
 */
_CALLCONV ALIGNED void
stm_store2(volatile stm_word_t *addr, stm_word_t value, stm_word_t mask)
{
  TX_GET;
  int_stm_store2(tx, addr, value, mask);
}

_CALLCONV void
stm_store2_tx(stm_tx_t *tx, volatile stm_word_t *addr, stm_word_t value, stm_word_t mask)
{
  int_stm_store2(tx, addr, value, mask);
}

/*
 * Called by the CURRENT thread to inquire about the status of a transaction.
 */
_CALLCONV int
stm_active(void)
{
  TX_GET;
  return int_stm_active(tx);
}

_CALLCONV int
stm_active_tx(stm_tx_t *tx)
{
  return int_stm_active(tx);
}

/*
 * Called by the CURRENT thread to inquire about the status of a transaction.
 */
_CALLCONV int
stm_aborted(void)
{
  TX_GET;
  return int_stm_aborted(tx);
}

_CALLCONV int
stm_aborted_tx(stm_tx_t *tx)
{
  return int_stm_aborted(tx);
}

/*
 * Called by the CURRENT thread to inquire about the status of a transaction.
 */
_CALLCONV int
stm_irrevocable(void)
{
  TX_GET;
  return int_stm_irrevocable(tx);
}

_CALLCONV int
stm_irrevocable_tx(stm_tx_t *tx)
{
  return int_stm_irrevocable(tx);
}

/*
 * Called by the CURRENT thread to obtain an environment for setjmp/longjmp.
 */
_CALLCONV sigjmp_buf *
stm_get_env(void)
{
  TX_GET;
  return int_stm_get_env(tx);
}

_CALLCONV sigjmp_buf *
stm_get_env_tx(stm_tx_t *tx)
{
  return int_stm_get_env(tx);
}

/*
 * Get transaction attributes.
 */
_CALLCONV stm_tx_attr_t
stm_get_attributes(void)
{
  TX_GET;
  assert (tx != NULL);
  return tx->attr;
}

/*
 * Get transaction attributes from a specifc transaction.
 */
_CALLCONV stm_tx_attr_t
stm_get_attributes_tx(struct stm_tx *tx)
{
  assert (tx != NULL);
  return tx->attr;
}

/*
 * Return statistics about a thread/transaction.
 */
_CALLCONV int
stm_get_stats(const char *name, void *val)
{
  TX_GET;
  return int_stm_get_stats(tx, name, val);
}

_CALLCONV int
stm_get_stats_tx(stm_tx_t *tx, const char *name, void *val)
{
  return int_stm_get_stats(tx, name, val);
}

/*
 * Return STM parameters.
 */
_CALLCONV int
stm_get_parameter(const char *name, void *val)
{
  if (strcmp("contention_manager", name) == 0) {
    *(const char **)val = cm_names[CM];
    return 1;
  }
  if (strcmp("design", name) == 0) {
    *(const char **)val = design_names[DESIGN];
    return 1;
  }
  if (strcmp("initial_rw_set_size", name) == 0) {
    *(int *)val = RW_SET_SIZE;
    return 1;
  }
#if CM == CM_BACKOFF
  if (strcmp("min_backoff", name) == 0) {
    *(unsigned long *)val = MIN_BACKOFF;
    return 1;
  }
  if (strcmp("max_backoff", name) == 0) {
    *(unsigned long *)val = MAX_BACKOFF;
    return 1;
  }
#endif /* CM == CM_BACKOFF */
#if CM == CM_MODULAR
  if (strcmp("vr_threshold", name) == 0) {
    *(int *)val = _tinystm.vr_threshold;
    return 1;
  }
#endif /* CM == CM_MODULAR */
#ifdef COMPILE_FLAGS
  if (strcmp("compile_flags", name) == 0) {
    *(const char **)val = XSTR(COMPILE_FLAGS);
    return 1;
  }
#endif /* COMPILE_FLAGS */
  return 0;
}

/*
 * Set STM parameters.
 */
_CALLCONV int
stm_set_parameter(const char *name, void *val)
{
#if CM == CM_MODULAR
  int i;

  if (strcmp("cm_policy", name) == 0) {
    for (i = 0; cms[i].name != NULL; i++) {
      if (strcasecmp(cms[i].name, (const char *)val) == 0) {
        _tinystm.contention_manager = cms[i].f;
        return 1;
      }
    }
    return 0;
  }
  if (strcmp("cm_function", name) == 0) {
    _tinystm.contention_manager = (int (*)(stm_tx_t *, stm_tx_t *, int))val;
    return 1;
  }
  if (strcmp("vr_threshold", name) == 0) {
    _tinystm.vr_threshold = *(int *)val;
    return 1;
  }
#endif /* CM == CM_MODULAR */
  return 0;
}

/*
 * Create transaction-specific data (return -1 on error).
 */
_CALLCONV int
stm_create_specific(void)
{
  if (_tinystm.nb_specific >= MAX_SPECIFIC) {
    fprintf(stderr, "Error: maximum number of specific slots reached\n");
    return -1;
  }
  return _tinystm.nb_specific++;
}

/*
 * Store transaction-specific data.
 */
_CALLCONV void
stm_set_specific(int key, void *data)
{
  int_stm_set_specific(key, data);
}

/*
 * Fetch transaction-specific data.
 */
_CALLCONV void *
stm_get_specific(int key)
{
  return int_stm_get_specific(key);
}

/*
 * Register callbacks for an external module (must be called before creating transactions).
 */
_CALLCONV int
stm_register(void (*on_thread_init)(void *arg),
             void (*on_thread_exit)(void *arg),
             void (*on_start)(void *arg),
             void (*on_precommit)(void *arg),
             void (*on_commit)(void *arg),
             void (*on_abort)(void *arg),
             void *arg)
{
  if ((on_thread_init != NULL && _tinystm.nb_init_cb >= MAX_CB) ||
      (on_thread_exit != NULL && _tinystm.nb_exit_cb >= MAX_CB) ||
      (on_start != NULL && _tinystm.nb_start_cb >= MAX_CB) ||
      (on_precommit != NULL && _tinystm.nb_precommit_cb >= MAX_CB) ||
      (on_commit != NULL && _tinystm.nb_commit_cb >= MAX_CB) ||
      (on_abort != NULL && _tinystm.nb_abort_cb >= MAX_CB)) {
    fprintf(stderr, "Error: maximum number of modules reached\n");
    return 0;
  }
  /* New callback */
  if (on_thread_init != NULL) {
    _tinystm.init_cb[_tinystm.nb_init_cb].f = on_thread_init;
    _tinystm.init_cb[_tinystm.nb_init_cb++].arg = arg;
  }
  /* Delete callback */
  if (on_thread_exit != NULL) {
    _tinystm.exit_cb[_tinystm.nb_exit_cb].f = on_thread_exit;
    _tinystm.exit_cb[_tinystm.nb_exit_cb++].arg = arg;
  }
  /* Start callback */
  if (on_start != NULL) {
    _tinystm.start_cb[_tinystm.nb_start_cb].f = on_start;
    _tinystm.start_cb[_tinystm.nb_start_cb++].arg = arg;
  }
  /* Pre-commit callback */
  if (on_precommit != NULL) {
    _tinystm.precommit_cb[_tinystm.nb_precommit_cb].f = on_precommit;
    _tinystm.precommit_cb[_tinystm.nb_precommit_cb++].arg = arg;
  }
  /* Commit callback */
  if (on_commit != NULL) {
    _tinystm.commit_cb[_tinystm.nb_commit_cb].f = on_commit;
    _tinystm.commit_cb[_tinystm.nb_commit_cb++].arg = arg;
  }
  /* Abort callback */
  if (on_abort != NULL) {
    _tinystm.abort_cb[_tinystm.nb_abort_cb].f = on_abort;
    _tinystm.abort_cb[_tinystm.nb_abort_cb++].arg = arg;
  }

  return 1;
}

/*
 * Called by the CURRENT thread to load a word-sized value in a unit transaction.
 */
_CALLCONV stm_word_t
stm_unit_load(volatile stm_word_t *addr, stm_word_t *timestamp)
{
#ifdef UNIT_TX
  volatile stm_word_t *sca_serializing_lock;
  stm_word_t l, l2, value;

  PRINT_DEBUG2("==> stm_unit_load(a=%p)\n", addr);

  /* Get reference to lock */
  sca_serializing_lock = GET_LOCK(addr);

  /* Read lock, value, lock */
 restart:
  l = ATOMIC_LOAD_ACQ(sca_serializing_lock);
 restart_no_load:
  if (LOCK_GET_OWNED(l)) {
    /* Locked: wait until lock is free */
#ifdef WAIT_YIELD
    sched_yield();
#endif /* WAIT_YIELD */
    goto restart;
  }
  /* Not locked */
  value = ATOMIC_LOAD_ACQ(addr);
  l2 = ATOMIC_LOAD_ACQ(sca_serializing_lock);
  if (l != l2) {
    l = l2;
    goto restart_no_load;
  }

  if (timestamp != NULL)
    *timestamp = LOCK_GET_TIMESTAMP(l);

  return value;
#else /* ! UNIT_TX */
  fprintf(stderr, "Unit transaction is not enabled\n");
  exit(-1);
  return 1;
#endif /* ! UNIT_TX */
}

/*
 * Store a word-sized value in a unit transaction.
 */
static INLINE int
stm_unit_write(volatile stm_word_t *addr, stm_word_t value, stm_word_t mask, stm_word_t *timestamp)
{
#ifdef UNIT_TX
  volatile stm_word_t *sca_serializing_lock;
  stm_word_t l;

  PRINT_DEBUG2("==> stm_unit_write(a=%p,d=%p-%lu,m=0x%lx)\n",
               addr, (void *)value, (unsigned long)value, (unsigned long)mask);

  /* Get reference to lock */
  sca_serializing_lock = GET_LOCK(addr);

  /* Try to acquire lock */
 restart:
  l = ATOMIC_LOAD_ACQ(sca_serializing_lock);
  if (LOCK_GET_OWNED(l)) {
    /* Locked: wait until lock is free */
#ifdef WAIT_YIELD
    sched_yield();
#endif /* WAIT_YIELD */
    goto restart;
  }
  /* Not locked */
  if (timestamp != NULL && LOCK_GET_TIMESTAMP(l) > *timestamp) {
    /* Return current timestamp */
    *timestamp = LOCK_GET_TIMESTAMP(l);
    return 0;
  }
  /* TODO: would need to store thread ID to be able to kill it (for wait freedom) */
  if (ATOMIC_CAS_FULL(sca_serializing_lock, l, LOCK_UNIT) == 0)
    goto restart;
  ATOMIC_STORE(addr, value);
  /* Update timestamp with newer value (may exceed VERSION_MAX by up to MAX_THREADS) */
  l = FETCH_INC_CLOCK + 1;
  if (timestamp != NULL)
    *timestamp = l;
  /* Make sure that lock release becomes visible */
  ATOMIC_STORE_REL(sca_serializing_lock, LOCK_SET_TIMESTAMP(l));
  if (unlikely(l >= VERSION_MAX)) {
    /* Block all transactions and reset clock (current thread is not in active transaction) */
    stm_quiesce_barrier(NULL, rollover_clock, NULL);
  }
  return 1;
#else /* ! UNIT_TX */
  fprintf(stderr, "Unit transaction is not enabled\n");
  exit(-1);
  return 1;
#endif /* ! UNIT_TX */
}

/*
 * Called by the CURRENT thread to store a word-sized value in a unit transaction.
 */
_CALLCONV int
stm_unit_store(volatile stm_word_t *addr, stm_word_t value, stm_word_t *timestamp)
{
  return stm_unit_write(addr, value, ~(stm_word_t)0, timestamp);
}

/*
 * Called by the CURRENT thread to store part of a word-sized value in a unit transaction.
 */
_CALLCONV int
stm_unit_store2(volatile stm_word_t *addr, stm_word_t value, stm_word_t mask, stm_word_t *timestamp)
{
  return stm_unit_write(addr, value, mask, timestamp);
}

/*
 * Enable or disable extensions and set upper bound on snapshot.
 */
static INLINE void
int_stm_set_extension(stm_tx_t *tx, int enable, stm_word_t *timestamp)
{
#ifdef UNIT_TX
  tx->attr.no_extend = !enable;
  if (timestamp != NULL && *timestamp < tx->end)
    tx->end = *timestamp;
#else /* ! UNIT_TX */
  fprintf(stderr, "Unit transaction is not enabled\n");
  exit(-1);
#endif /* ! UNIT_TX */
}

_CALLCONV void
stm_set_extension(int enable, stm_word_t *timestamp)
{
  TX_GET;
  int_stm_set_extension(tx, enable, timestamp);
}

_CALLCONV void
stm_set_extension_tx(stm_tx_t *tx, int enable, stm_word_t *timestamp)
{
  int_stm_set_extension(tx, enable, timestamp);
}

/*
 * Get curent value of global clock.
 */
_CALLCONV stm_word_t
stm_get_clock(void)
{
  return GET_CLOCK;
}

/*
 * Get current transaction descriptor.
 */
_CALLCONV stm_tx_t *
stm_current_tx(void)
{
  return tls_get_tx();
}

/* ################################################################### *
 * UNDOCUMENTED STM FUNCTIONS (USE WITH CARE!)
 * ################################################################### */

#ifdef CONFLICT_TRACKING
/*
 * Get thread identifier of other transaction.
 */
_CALLCONV int
stm_get_thread_id(stm_tx_t *tx, pthread_t *id)
{
  *id = tx->thread_id;
  return 1;
}

/*
 * Set global conflict callback.
 */
_CALLCONV int
stm_set_conflict_cb(void (*on_conflict)(int id1, int id2))
{
  _tinystm.conflict_cb = on_conflict;
  return 1;
}
#endif /* CONFLICT_TRACKING */

/*
 * Set the CURRENT transaction as irrevocable.
 */
static INLINE int
int_stm_set_irrevocable(stm_tx_t *tx, int serial)
{
#ifdef IRREVOCABLE_ENABLED
# if CM == CM_MODULAR
  stm_word_t t;
# endif /* CM == CM_MODULAR */

  if (!IS_ACTIVE(tx->status) && serial != -1) {
    /* Request irrevocability outside of a transaction or in abort handler (for next execution) */
    tx->irrevocable = 1 + (serial ? 0x08 : 0);
    return 0;
  }

  /* Are we already in irrevocable mode? */
  if ((tx->irrevocable & 0x07) == 3) {
    return 1;
  }

  if (tx->irrevocable == 0) {
    /* Acquire irrevocability for the first time */
    tx->irrevocable = 1 + (serial ? 0x08 : 0);
    /* Try acquiring global lock */
    if (_tinystm.irrevocable == 1 || ATOMIC_CAS_FULL(&_tinystm.irrevocable, 0, 1) == 0) {
      /* Transaction will acquire irrevocability after rollback */
      stm_rollback(tx, STM_ABORT_IRREVOCABLE);
      return 0;
    }
    /* Success: remember we have the lock */
    tx->irrevocable++;
    /* Try validating transaction */
#if DESIGN == WRITE_BACK_ETL
    if (!stm_wbetl_validate(tx,0)) {
      stm_rollback(tx, STM_ABORT_VALIDATE);
      return 0;
    }
#elif DESIGN == WRITE_BACK_CTL
    if (!stm_wbctl_validate(tx)) {
      stm_rollback(tx, STM_ABORT_VALIDATE);
      return 0;
    }
#elif DESIGN == WRITE_THROUGH
    if (!stm_wt_validate(tx)) {
      stm_rollback(tx, STM_ABORT_VALIDATE);
      return 0;
    }
#elif DESIGN == MODULAR
    if ((tx->attr.id == WRITE_BACK_CTL && stm_wbctl_validate(tx))
       || (tx->attr.id == WRITE_THROUGH && stm_wt_validate(tx))
       || (tx->attr.id != WRITE_BACK_CTL && tx->attr.id != WRITE_THROUGH && stm_wbetl_validate(tx))) {
      stm_rollback(tx, STM_ABORT_VALIDATE);
      return 0;
    }
#endif /* DESIGN == MODULAR */

# if CM == CM_MODULAR
   /* We might still abort if we cannot set status (e.g., we are being killed) */
    t = tx->status;
    if (GET_STATUS(t) != TX_ACTIVE || ATOMIC_CAS_FULL(&tx->status, t, t + (TX_IRREVOCABLE - TX_ACTIVE)) == 0) {
      stm_rollback(tx, STM_ABORT_KILLED);
      return 0;
    }
# endif /* CM == CM_MODULAR */
    if (serial && tx->w_set.nb_entries != 0) {
      /* TODO: or commit the transaction when we have the irrevocability. */
      /* Don't mix transactional and direct accesses => restart with direct accesses */
      stm_rollback(tx, STM_ABORT_IRREVOCABLE);
      return 0;
    }
  } else if ((tx->irrevocable & 0x07) == 1) {
    /* Acquire irrevocability after restart (no need to validate) */
    while (_tinystm.irrevocable == 1 || ATOMIC_CAS_FULL(&_tinystm.irrevocable, 0, 1) == 0)
      ;
    /* Success: remember we have the lock */
    tx->irrevocable++;
  }
  assert((tx->irrevocable & 0x07) == 2);

  /* Are we in serial irrevocable mode? */
  if ((tx->irrevocable & 0x08) != 0) {
    /* Stop all other threads */
    if (stm_quiesce(tx, 1) != 0) {
      /* Another thread is quiescing and we are active (trying to acquire irrevocability) */
      assert(serial != -1);
      stm_rollback(tx, STM_ABORT_IRREVOCABLE);
      return 0;
    }
  }

  /* We are in irrevocable mode */
  tx->irrevocable++;

#else /* ! IRREVOCABLE_ENABLED */
  fprintf(stderr, "Irrevocability is not supported in this configuration\n");
  exit(-1);
#endif /* ! IRREVOCABLE_ENABLED */
  return 1;
}

_CALLCONV NOINLINE int
stm_set_irrevocable(int serial)
{
  TX_GET;
  return int_stm_set_irrevocable(tx, serial);
}

_CALLCONV NOINLINE int
stm_set_irrevocable_tx(stm_tx_t *tx, int serial)
{
  return int_stm_set_irrevocable(tx, serial);
}

/*
 * Increment the value of global clock (Only for TinySTM developers).
 */
void
stm_inc_clock(void)
{
  FETCH_INC_CLOCK;
}


