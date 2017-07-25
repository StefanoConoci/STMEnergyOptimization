#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <time.h>
#include <string.h>


// Returns energy consumption of package 0 cores in micro Joule
long get_energy(){
    
    long power, power1;

    // Package 0 power consumtion
    FILE* power_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj", "r");

    // Package 0 power consumtion
    FILE* power_file1 = fopen("/sys/class/powercap/intel-rapl/intel-rapl:1/energy_uj", "r");
    

    // Package 0 cores power consumption
    //FILE* power_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:0/energy_uj", "r");    

    // DRAM module, considered inside the package
    //FILE* power_file = fopen("/sys/class/powercap/intel-rapl/intel-rapl:0/intel-rapl:0:1/energy_uj", "r");    

    if(power_file == NULL){
        printf("Error opening power file\n");       
    }
    fscanf(power_file,"%ld",&power);
    fclose(power_file);
    
    if(power_file1 == NULL){
        printf("Error opening power file\n");
    }
    fscanf(power_file1,"%ld",&power1);
    fclose(power_file1);

    return (power+power1);
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

int main(int argc, char *argv[]){

	long start_time, end_time, start_energy, end_energy;

	double energy_consumed;
	double runtime;
	double avg_power;

	if(argc < 2){
		printf("No input program passed in parameter\n");
		exit(1);
	}

    start_time = get_time();
    start_energy = get_energy();

    pid_t pid=fork();
    if (pid==0) { /* child process */
        execv(argv[1], &(argv[1]));
        exit(127); /* only if execv fails */
    }
    else { /* pid!=0; parent process */
        waitpid(pid,0,0); /* wait for child to exit */
    	end_time = get_time();
    	end_energy = get_energy();

    	runtime = ( (double) end_time - start_time)/1000000000;
    	energy_consumed = ( (double) end_energy - start_energy);
    	avg_power = (energy_consumed / 1000000)/runtime;

    	printf("\nRuntime: %lf seconds - Energy: %lf micro Joule - Average Power: %lf Watt\n", runtime, energy_consumed, avg_power);
    }

    return 0;
}
