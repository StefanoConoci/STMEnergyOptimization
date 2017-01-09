/////////////////////////////////////////////////////////////////
//	Global variables
/////////////////////////////////////////////////////////////////


int* running_array;				// Array of integers that defines if a thread should be running
pthread_t* pthread_ids;			// Array of pthread id's to be used with signals
int total_threads;				// Total number of threads that could be used by the transcational operation 
volatile int active_threads;	// Number of currently active threads, reflects the number of 1's in running_array
int nb_cores; 					// Number of cores. Detected at startup and used to set DVFS parameters for all cores
int nb_packages;				// Number of system package. Necessary to monitor energy consumption of all packages in th system
int cache_line_size;			// Size in byte of the cache line. Detected at startup and used to alloc memory cache aligned 
int* pstate;					// Array of p-states initialized at startup with available scaling frequencies 
int max_pstate;					// Maximum index of available pstate for the running machine 
int current_pstate;				// Value of current pstate, index of pstate array which contains frequencies
int total_commits_round; 		// Number of total commits for each heuristics step 
int starting_threads;			// Number of threads running at the start of the heuristic search. Defined in hope_config.txt
int steps;						// Number of steps required for the heuristic to converge 
stats_t** stats_array;			// Pointer to pointers of struct stats_s, one for each thread 	
volatile int round_completed;   // Defines if round completed and thread 0 should collect stats and call the heuristic function 
double** power_profile; 		// Power consumption matrix of the machine. Precomputed using profiler.c included in root folder.
								// Rows are threads, columns are p-states. It has total_threads+1 rows as first row is filled with 0 for the profile with 0 threads
double power_limit;				// Maximum power that should be used by the application expressed in Watt. Defined in hope_config.txt
double energy_per_tx_limit;		// Maximum energy per tx that should be drawn by the application expressed in micro Joule. Defined in hope_config.txt 
int heuristic_mode;				// Used to switch between different heuristics mode. Can be set from 0 to 4. 
double jump_percentage;			// Used by heuristic mode 2. It defines how near power_limit we expect the optimal configuration to be
volatile int shutdown;			// Used to check if should shutdown

// Statistics of the last heuristic round
double old_throughput;		
double old_power;			
double old_abort_rate; 		
double old_energy_per_tx;	

// Variables that define the currently best configuration
double best_throughput; 
int best_threads;
int best_pstate;

// Variables that define the current level best configuration. Used by HEURISTIC_MODE 0, 3 and 4
double level_best_throughput; 
int level_best_threads;
int level_best_pstate;
int level_starting_threads;
int level_starting_energy_per_tx;

// Variables used to define the state of the search 
int new_pstate;					// Used to check if just arrived to a new p_state in the heuristic search
int decreasing;					// If 0 heuristic should remove threads until it reaches the limit  
int stopped_searching;			// While 1 the algorithm searches for the best configuration, if 0 the algorithm moves to monitoring mode 

/////////////////////////////////////////////////////////////////
//	Function declerations
/////////////////////////////////////////////////////////////////

int set_pstate(int);
int init_DVFS_management();
void sig_func(int);
void init_thread_management(int);
inline void check_running_array(int);
inline int wake_up_thread(int);
inline int pause_thread(int);
void init_stats_array_pointer(int);
stats_t* alloc_stats_buffer(int);
void load_profile_file();
long get_energy();
long get_time();
inline void set_threads(int);
inline void update_best_config(double);
inline void stop_searching();
int profiler_isoenergy(int, int, int*);
void heuristic(double, double, double, double);