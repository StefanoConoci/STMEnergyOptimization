
///////////////////////////////////////////////////////////////
// Utility functions
///////////////////////////////////////////////////////////////

// Checks if the current config is better than the currently best config and if that's the case update it 
inline void update_best_config(double throughput){
	
	if(throughput > best_throughput){
		best_throughput = throughput;
		best_pstate = current_pstate;
		best_threads = active_threads;
	}
}

inline int update_level_best_config(double throughput){
	if(throughput > level_best_throughput){
		level_best_throughput = throughput;
		level_best_threads = active_threads;
		level_best_pstate = current_pstate;
		return 1;
	}
	else return 0;
}

inline void compare_best_level_config(){
	if(level_best_throughput > best_throughput){
		best_throughput = level_best_throughput;
		best_pstate = level_best_pstate;
		best_threads = level_best_threads;
	}
}

// Stop searching and set the best configuration 
inline void stop_searching(){

	decreasing = 0;
	new_pstate = 1;
	stopped_searching = 1;

	set_pstate(best_pstate);
	set_threads(best_threads);

	// DEBUG
	printf("Stopped searching. Best configuration: %d threads at p-state %d\n", best_threads, best_pstate);
}

// Returns a configuration at the given pstate with power consumption less than the one of the current configuration 
// All the power related data is retrieved from the profiler_matrix and is only used as a starting point, as this values change based on the workload
// If no configuration at the given pstate uses less than the configuration power the algorithm returns the lowest consuming configuration 
// as long as it draws less than power_limit. If this configuration can't be found or the pstate is invalid the function returns -1 
int profiler_isoenergy(int from_threads, int pstate, int* threads){
	
	if(pstate < 0 || pstate > max_pstate)
		return -1;

	double old_power = power_profile[from_threads][current_pstate];

	// There could be no number of threads > 0 such that the power consumption is less than old_power 
	
	/*if(power_profile[1][pstate] > old_power){
		if (power_profile[1][pstate] < power_limit){
			*threads = 1;
			return 0;
		}
		else return -1;
	}*/

	// If no configuration at requested pstate consumes less power than the last configuration return the configuration with 1 thread 
	if(power_profile[1][pstate] > old_power){
		*threads = 1;
		return 0;
	}

	int i = 1;
	while( i<=total_threads && power_profile[i][pstate] < old_power)
		i++;

	*threads = --i;
	printf("Isoenergy configuration: %d threads at %d pstate\n", *threads, pstate);
	return 0;
}  


///////////////////////////////////////////////////////////////
// Heuristic search functions
///////////////////////////////////////////////////////////////


// HEURISTIC_MODE 0
// This heuristics converges to the configuration with the highest throughput that draws less power than power_limit 
// It starts from the bottom and finds the best configuration at each p-state that is optimal and compares it with the current best
// It needs to explore bidictionally as this makes no assumptions other that the explored surface is continous and that decreasing pstate increases power
// as well as increaasing number of active threads increaseas power   
void heuristic_power(double throughput, double abort_rate, double power, double energy_per_tx){
	
	if(!stopped_searching){

		if(new_pstate){
			new_pstate = 0;
			if( power > power_limit ){
				decreasing = 1;
				if(active_threads > 1)
					pause_thread(active_threads-1);
				else stop_searching();
			}
			else{
				update_level_best_config(throughput);
				if(active_threads < total_threads){
					wake_up_thread(active_threads);
				}
				else{
					decreasing = 1;
					if(active_threads > 1)
						pause_thread(active_threads-1);
					else stop_searching();
				}
			}
			
		}
		else{	//Not new state
			
			// Increasing number of threads 
			if(!decreasing){
				if( throughput < old_throughput || power > power_limit || active_threads >= total_threads){	// Increasing should stop 
					if(power<power_limit)
						update_level_best_config(throughput);

					// If already increased once or cannot reduce wrt to starting threads there is no reason to explore descreasing threads 
					if( (active_threads-level_starting_threads)>1 || (level_starting_threads-1)<1  ){
						new_pstate = 1;
						compare_best_level_config();
						if( current_pstate-1 < 0 )
							stop_searching();
						else{
							set_threads(level_best_threads);
							set_pstate(current_pstate-1);
							decreasing = 0;
							level_starting_threads = level_best_threads;
							level_best_throughput = 0;
							level_best_threads = 0;
							level_best_pstate = 0;
						}
					}
					else{
						decreasing = 1;
						set_threads(level_starting_threads-1);
					}
				}
				else{	// Should continue increasing 
					update_level_best_config(throughput);
					wake_up_thread(active_threads);
				}

			}
			// Decreasing number of threads
			else{
				if(throughput < level_best_throughput|| active_threads == 1 ){
					new_pstate = 1;
					compare_best_level_config();
					if( current_pstate-1 < 0 )
						stop_searching();
					else{
						set_threads(level_best_threads);
						set_pstate(current_pstate-1);
						decreasing = 0;
						level_starting_threads = level_best_threads;
						level_best_throughput = 0;
						level_best_threads = 0;
						level_best_pstate = 0;
					}
				}
				else{
					if(power < power_limit){
						update_level_best_config(throughput);
					}
					pause_thread(active_threads-1);
				}
			}
		}


		//Update old global variables 
		old_throughput = throughput;
		old_abort_rate = abort_rate;
		old_power = power;
		old_energy_per_tx = energy_per_tx;

		printf("Switched to: #threads %d - pstate %d\n", active_threads, current_pstate);

	}
	else{
		if( throughput > (best_throughput*1.1) || throughput < (best_throughput*0.9) || power > (power_limit *1.05) ){
			stopped_searching = 0;
			set_pstate(max_pstate);
			set_threads(starting_threads);
			best_throughput = 0;
			best_threads = starting_threads;
			best_pstate = max_pstate;
			printf("Workload change detected. Restarting heuristic search\n");
		}
	}
}





// HEURISTIC_MODE 1 
// This heuristic uses the power profiler to achieve a faster convergence to the optimal value.
// It finds the configuration with the highest throughput that drawns less power than power_limit defined in hope_config file 
void heuristic_power_profiler(double throughput, double abort_rate, double power, double energy_per_tx){
	
	if(!stopped_searching){
			
		if(new_pstate){
			new_pstate = 0;
			if(power > power_limit){	// Power is higher than limit
				decreasing = 1;
				if(active_threads > 1){
					pause_thread(active_threads-1);
				}
				else 
					stop_searching();
			}else{	// Power lower than limit
				decreasing = 0;
				update_best_config(throughput);

				if(active_threads < total_threads)
					wake_up_thread(active_threads);
				else{
					int threads;
					if(profiler_isoenergy(active_threads, current_pstate-1, &threads) != -1){
						set_pstate(current_pstate-1);
						set_threads(threads);
						new_pstate = 1;
					}
					else stop_searching();
				}
			}
		}
		else{ // Not new_pstate, should check if decreasing or not
			if(decreasing){
				if(power < power_limit){
					update_best_config(throughput);
					decreasing = 0;
					int threads;
					if(profiler_isoenergy(active_threads, current_pstate-1, &threads) != -1){
						set_pstate(current_pstate-1);
						set_threads(threads);
						new_pstate = 1;
					}
					else stop_searching();
				}
				else{
					if(active_threads >1)
						pause_thread(active_threads-1);
					else stop_searching();
				}
			}
			else{	// Increasing number of threads 
				int improved = 0;
				if(throughput > old_throughput && power < power_limit)
					improved = 1;

				if(improved){
					update_best_config(throughput);
					if(active_threads < total_threads){
						wake_up_thread(active_threads);
					}
					else{
						int threads;
						if(profiler_isoenergy(active_threads, current_pstate-1, &threads) != -1){
							set_pstate(current_pstate-1);
							set_threads(threads);
							new_pstate = 1;
						}
						else stop_searching();
					}
				}
				else{ // not improved
					int threads;
						if(profiler_isoenergy(active_threads-1, current_pstate-1, &threads) != -1){
							set_pstate(current_pstate-1);
							set_threads(threads);
							new_pstate = 1;
						}
						else stop_searching();
				}

			}	
		}

		//Update old global variables 
		old_throughput = throughput;
		old_abort_rate = abort_rate;
		old_power = power;
		old_energy_per_tx = energy_per_tx;

		printf("Switched to: #threads %d - pstate %d\n", active_threads, current_pstate);
	}
	else{	//Check if workload changed and if that's the case restart the searching 
		if( throughput > (best_throughput*1.1) || throughput < (best_throughput*0.9) || power > (power_limit *1.05) ){
			stopped_searching = 0;
			set_pstate(max_pstate);
			set_threads(starting_threads);
			best_throughput = 0;
			best_threads = starting_threads;
			best_pstate = max_pstate;
			printf("Workload change detected. Restarting heuristic search\n");
		}
	}
}

// HEURISTIC_MODE 2. This heuristics converges to the configuration with the highest throughput that consumes less than energy_per_tx on average for transaction
// Hypothesis no local max on throughput. 
void heuristic_energy_bidirectional(double throughput, double abort_rate, double power, double energy_per_tx){
	
	if(!stopped_searching){

		if(new_pstate){
			new_pstate = 0;
			level_starting_energy_per_tx = energy_per_tx;
			
			if(energy_per_tx < energy_per_tx_limit)
				update_level_best_config(throughput);			

			if(active_threads < total_threads){
				decreasing = 0;
				wake_up_thread(active_threads);
			}
			else{
				if(active_threads != 1 && (level_starting_threads-1) > 0) {
					decreasing = 1;
					pause_thread(active_threads-1);
				}
				else{
					new_pstate = 1;
					decreasing = 0;
					compare_best_level_config();
					if(current_pstate == 0 ){
						stop_searching();
					}
					else{
						if(level_best_threads != 0)
							level_starting_threads = level_best_threads;
						level_best_threads = 0;
						level_best_throughput = 0;
						level_best_pstate = 0;
						set_threads(level_starting_threads);
						set_pstate(current_pstate-1);
					}
				}
			}
		}
		else{
			if(!decreasing){	// Increasing
				if(energy_per_tx < energy_per_tx_limit){
					update_level_best_config(throughput);
					if(active_threads == total_threads || old_throughput > throughput){ // Switch to decreasing if possible or move to next p-state
						if(active_threads != 1 && (level_starting_threads-1) > 0) {
							decreasing = 1;
							pause_thread(active_threads-1);
						}
						else{
							new_pstate = 1;
							decreasing = 0;
							compare_best_level_config();
							if(current_pstate == 0 ){
								stop_searching();
							}
							else{
								if(level_best_threads != 0)
									level_starting_threads = level_best_threads;
								level_best_threads = 0;
								level_best_throughput = 0;
								level_best_pstate = 0;
								set_threads(level_starting_threads);
								set_pstate(current_pstate-1);
							}
						}
					}
					else wake_up_thread(active_threads);
				}
				else{	// Energy higher than limit 
					if(energy_per_tx > old_energy_per_tx || active_threads == total_threads){	// Switch to decreasing if possible or move to next p-state
						if(active_threads != 1 && (level_starting_threads-1) > 0) {
							decreasing = 1;
							pause_thread(active_threads-1);
						}
						else{
							new_pstate = 1;
							decreasing = 0;
							compare_best_level_config();
							if(current_pstate == 0 ){
								stop_searching();
							}
							else{
								if(level_best_threads != 0)
									level_starting_threads = level_best_threads;
								level_best_threads = 0;
								level_best_throughput = 0;
								level_best_pstate = 0;
								set_threads(level_starting_threads);
								set_pstate(current_pstate-1);
							}
						}
					}
					else wake_up_thread(active_threads);
				}
			}
			else{	// Decreasing
				if(energy_per_tx < energy_per_tx_limit){
					update_level_best_config(throughput);
					if(active_threads == 1 || level_best_throughput > throughput){
						new_pstate = 1;
						decreasing = 0;
						compare_best_level_config();
						if(current_pstate == 0){
							stop_searching();
						}	
						else{
							if(level_best_threads != 0)
								level_starting_threads = level_best_threads;
							level_best_threads = 0;
							level_best_throughput = 0;
							level_best_pstate = 0;
							set_threads(level_starting_threads);
							set_pstate(current_pstate-1);
						}
					}
					else pause_thread(active_threads-1);
				}
				else{	// Energy higher than limit 
					if(active_threads == (level_starting_threads-1))	// In the first decreasing step last energy is related to the increasing phase
						old_energy_per_tx = level_starting_energy_per_tx;
					if(energy_per_tx > old_energy_per_tx || active_threads == 1){
						new_pstate = 1;
						decreasing = 0;
						compare_best_level_config();
						if(current_pstate == 0){
							stop_searching();
						}	
						else{
							if(level_best_threads != 0)
								level_starting_threads = level_best_threads;
							level_best_threads = 0;
							level_best_throughput = 0;
							level_best_pstate = 0;
							set_threads(level_starting_threads);
							set_pstate(current_pstate-1);
						}
					}
					else pause_thread(active_threads-1);
				}
			}
		}

		//Update old global variables 
		old_throughput = throughput;
		old_abort_rate = abort_rate;
		old_power = power;
		old_energy_per_tx = energy_per_tx;

		printf("Switched to: #threads %d - pstate %d\n", active_threads, current_pstate);
	}
	else{
		if( throughput > (best_throughput*1.1) || throughput < (best_throughput*0.9) || power > (power_limit *1.05) ){
			stopped_searching = 0;
			set_pstate(max_pstate);
			set_threads(starting_threads);
			best_throughput = 0;
			best_threads = starting_threads;
			best_pstate = max_pstate;
			printf("Workload change detected. Restarting heuristic search\n");
		}
	}
}


// Used for testing all possible configurations pstate/threads
void explore_all_configurations(double throughput, double  abort_rate, double power, double energy_per_tx){

	if(active_threads == total_threads){
		if( current_pstate > 0 ){
			set_pstate(current_pstate-1);
			set_threads(1);
			printf("\n");
		}
	}
	else set_threads(active_threads+1);

	printf("Switched to: #threads %d - pstate %d\n", active_threads, current_pstate);
}


///////////////////////////////////////////////////////////////
// Main heuristic function
///////////////////////////////////////////////////////////////


// Takes decision on frequency and number of active threads based on statistics of current round 
	void heuristic(double throughput, double  abort_rate, double power, double energy_per_tx){
		
		printf("\nHeuristic function called\n");

		switch(heuristic_mode){
			case 0:
				heuristic_power(throughput, abort_rate, power, energy_per_tx);
				break;
			case 1:
				heuristic_power_profiler(throughput, abort_rate, power, energy_per_tx);
				break;
			case 2:
				printf("Heuristic mode 2  - power limit profiler fast exploring\n");
				break;
			case 3:
				heuristic_energy_bidirectional(throughput, abort_rate, power, energy_per_tx);
				break;
			case 4:
				printf("Heuristic mode 4  - energy_per_tx limit unidirectional\n");
				break;	
			case 5:
				explore_all_configurations(throughput, abort_rate, power, energy_per_tx);
				break;
		}
		
	}