// This file is used for the definition of the different heuristics 

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
			else{ // Not new_state, should check if decreasing or not
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




// Takes decision on frequency and number of active threads based on statistics of current round 
	void heuristic(double throughput, double  abort_rate, double power, double energy_per_tx){
		
		printf("\nHeuristic function called\n");

		switch(heuristic_mode){
			case 0:
				printf("Heuristic mode 0  - power limit no profiler\n");
				break;
			case 1:
				heuristic_power_profiler(throughput, abort_rate, power, energy_per_tx);
				break;
			case 2:
				printf("Heuristic mode 2  - power limit profiler fast exploring\n");
				break;
			case 3:
				printf("Heuristic mode 3  - energy_per_tx limit bidirectional\n");
				break;
			case 4:
				printf("Heuristic mode 3  - energy_per_tx limit unidirectional\n");
				break;	
		}
		
	}