#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stm.h>
#include <mod_stats.h>

//////////////////////////////////////////////////////
// Global module variables 
//////////////////////////////////////////////////////

int job_completed = 0;	// Used to check if the application completed 
int* running_array;		// Array of integers that defines if a thread should be running
pthread_t* pthread_ids;	// Array of pthread id's to be used with signals
int total_threads;		// Total number of threads that could be used by the transcational operation 
int active_threads;		// Number of currently active threads, reflects the number of 1's in running_array
int nb_cores; 			// Number of cores. Detected at startup and used to set DVFS parameters for all cores
int pstate[32];			// Array of p-states initialized at startup with available scaling frequencies 
int max_pstate;			// Maximum number of available pstate for the running machine 
int current_pstate;		// Value of current pstate, index of pstate array which contains frequencies


//////////////////////////////////////////////////////
// DVFS management 
//////////////////////////////////////////////////////

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
		close(frequency_file);
	}
	current_pstate = input_pstate;
	
	printf("Set processor to p-state %d - &d\n", input_pstate, frequency);
	return 0;
}

// Executed at TM_STARTUP: sets the governor to userspace and sets the highest frequency
int init_DVFS_management(){
	
	char fname[64];
	char* freq_available;
	char* token;
	int frequency;
	FILE* governor_file;

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
		close(governor_file);
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
  	close(available_freq_file);


  	// Setting processor to state P0
  	if(set_p_state(0));

	return 0;
}

int set_frequency(int MHz){

}

//////////////////////////////////////////////////////
// Stats management 
//////////////////////////////////////////////////////


// Stats thread function
void* stats_function(void* params){
	
	long power_old = 0;
	long power_new; 
	float current_power = 0;
	
	do{
		// Power calculation
		FILE* power_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");
		if(power_file == NULL){
			printf("Error opening power file\n");		
		}
		fscanf(power_file,"%ld",&power_new);
		fclose(power_file);

		if(power_old != 0)
			current_power = ((float)(power_new-power_old))/1000000;
		power_old = power_new;
			
		if(current_power != 0)
			printf("Power: %lf Watt\n", current_power);
		
		// Throughput calculation
		unsigned long u,v;
        if (stm_get_stats("global_nb_commits", &u) != 0)
            printf("Commits: %lu\n", u); 
        if (stm_get_stats("global_nb_aborts", &v) != 0)
            printf("Aborts: %lu\n", v); 

		sleep(1);
	}while(!job_completed);
	
	return NULL;
}


// Executed at: TM_STARTUP
void start_stats_thread(){
	
	pthread_t stats_thread;
	
	if(pthread_create(&stats_thread, NULL, stats_function, NULL)){
		printf("Error creating stats thread\n");	
		return;					
	}
}


//////////////////////////////////////////////////////
// Thread management  
//////////////////////////////////////////////////////

// SIGUSR1 handler. For now doesn't execute any code. 
void sig_func(int sig){}

// Executed at: TM_STARTUP
void init_thread_management(int threads){
	
	// Init total threads and active threads
	total_threads = threads;
	printf("Set total_threads to %d\n", threads);
	active_threads = total_threads;

	// Init running array with all threads running 	
	running_array = malloc(sizeof(int)*total_threads);
	for(int i=0; i<total_threads; i++){
		running_array[i] = 1;	
		printf("running_array[%d] = 1\n",i);
	}	

	// Allocate memory for pthread_ids
	pthread_ids = malloc(sizeof(pthread_t)*total_threads);

	//Registering SIGUSR1 handler
	signal(SIGUSR1, sig_func);
}

// Executed at: TM_START
void check_running_array(int threadId){
	
	while(running_array[threadId] == 0){
		printf("Pausing thread %d\n", threadId);
		pause();
	}
}

// Executed at: TM_THREAD_ENTER
void set_pthread_id(int threadId){
	pthread_ids[threadId] = pthread_self();
	printf("Setting pthread_ids[%d]=%lu\n", threadId, pthread_ids[threadId]);
}

//////////////////////////////////////////////////////
// Manager thread  
//////////////////////////////////////////////////////

// Manager thread function
void* manager_function(void* params){
	do{
		sleep(5);
		running_array[0] = 0;
		sleep(5);

		printf("Resuming thread 0\n");
		running_array[0] = 1;
		pthread_kill(pthread_ids[0], SIGUSR1);
	} while(!job_completed);

	return NULL;
}


// Executed at: TM_STARTUP
void start_manager_thread(){
	
	pthread_t manager_thread;
	
	if(pthread_create(&manager_thread, NULL, manager_function, NULL)){
		printf("Error creating manager thread\n");	
		return;					
	}
}

//////////////////////////////////////////////////////
// Misc 
//////////////////////////////////////////////////////

// Executed at TM_SHUTDOWN
void set_job_completed(){
	job_completed = 1;
}


