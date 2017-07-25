
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
			
			#ifdef DEBUG_HEURISTICS
				printf("Pausing a thread already paused\n");
			#endif

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
	  	}else if(heuristic_mode == 12 || heuristic_mode == 13){
	  		set_pstate(max_pstate);
	  		set_threads(1);
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
		new_pstate = 1;
		decreasing = 0;
		stopped_searching = 0;
		steps=0;
		shutdown = 0;
		effective_commits = 0;
		phase = 0; 
		current_exploit_steps = 0;
		barrier_detected = 0;
		pre_barrier_threads = 0;

		high_throughput = -1;
		high_threads = -1;
		high_pstate = -1;

		best_throughput = -1;
		best_pstate = -1;
		best_threads = -1;

		lock_commits = 0; 

		low_throughput = -1;
		low_threads = -1;
		low_pstate = -1;

		current_window_slot = 0;
		window_time = 0;
		window_power = 0;

		net_time_sum = 0;
	    net_energy_sum = 0;
	    net_commits_sum = 0;
		net_aborts_sum = 0;

		net_time_slot_start= 0;
 	    net_energy_slot_start= 0;
	    net_time_accumulator= 0;
		net_error_accumulator= 0; 
		net_discard_barrier= 0;

		min_pstate_search = 0;
		max_pstate_search = max_pstate;

		min_thread_search = 1;
		max_thread_search = total_threads;
		min_thread_search_throughput = -1;
		max_thread_search_throughput = -1;
	}

	// Reset all threads when reaching a barrier
	void setup_before_barrier(){

		int i;

		TX_GET;

		if(tx->thread_number == 0 && !barrier_detected && active_threads!=total_threads) {
		
			#ifdef DEBUG_HEURISTICS
				printf("Thread 0 detected a barrier\n");
			#endif
				
			// Next decision phase should be dropped
			barrier_detected = 1;

			// Dont consider next slot for power_limit error measurements
			net_discard_barrier = 1;

			// Save number of threads that should be restored after the barrier
			pre_barrier_threads = active_threads;

			// Wake up all threads
			for(i=active_threads; i< total_threads; i++){
	  			wake_up_thread(i);
	  		}
		}
	}



	// Used to either enable or disable boosting facilities such as TurboBoost. Boost is disabled whenever the current config goes out of the powercap 
	inline void set_boost(int value){

		int i;
		char fname[64];
		FILE* boost_file;

		#ifdef DEBUG_OVERHEAD
			long time_heuristic_start;
			long time_heuristic_end;
			double time_heuristic_microseconds;

			time_heuristic_start = get_time();
		#endif 
		
		if(value != 0 && value != 1){
			printf("Set_boost parameter invalid. Shutting down application\n");
			exit(1);
		}
		
		boost_file = fopen("/sys/devices/system/cpu/cpufreq/boost", "w+");
		fprintf(boost_file, "%d", value);
		fflush(boost_file);
		fclose(boost_file);

		#ifdef DEBUG_OVERHEAD
			time_heuristic_end = get_time();
			time_heuristic_microseconds = (((double) time_heuristic_end) - ((double) time_heuristic_start))/1000;
			printf("DEBUG OVERHEAD - inside set_boost() %lf microseconds\n", time_heuristic_microseconds);
		#endif 
		
		return 0;
	}

#endif/* ! STM_HOPE */