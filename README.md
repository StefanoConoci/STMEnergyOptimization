*Introduction*

The software in this repository defines a power capping solution that maximizes application performance while operating within power consumption constraints. This solution is based on the results of a preliminary analysis that shows how the throughput curve, when varying the number of cores assigned to a multithreaded applications, preserves the same shape, and same value for the maximum, at different performance states (P-state). Based on this result, the software solution presented in this repository performs an online exploration of the configurations of P-state and assigned cores/threads with the goal of adaptively allocating the power budget (power cap) maximizing application performance.

*Requirements*

This software is developed for Linux operating systems in c programming language. The minimum kernel version required is system specific, as it is the older version that implements for the specific CPU the powercap Linux subsystem, used for runtime low-overhead energy measurements. If used by the specific system, it is also necessary to disable the cpufreq governor intel_pstate (setting "intel_pstate=disable" in the kernel boot line) since it doesn't support software P-state management. 

*Compilation Macros*

- DEBUG_HEURISTICS: if defined puts the software in debug mode during which multiple runtime information are printed to standard output;
- DEBUG_OVERHEAD: if defined prints the time interval spent in the different portions of code specific of the proposed solution and not directly related to the application execution;
- LOCK_BASED_TRANSACTIONS: if defined changes the synchronization paradigm used by the STAMP applications from transactions to coarse-grained locking implemented with spinlocks; 
- TIMELINE_PLOT: if defined produces in the working directory a file that shows for each step (either explorative or exploitative) the selected configuration of threads and P-state, the average power consumption, the value of the powercap at the given time and the average throughput. 

*Compile*

You can use "compile.sh" to compile the power capping solution, TinySTM and the STAMP benchmark suite. 

*Configuration file*

The configuration file named "stm_hope.h" is loaded at the start of the execution and defines different parameters (some of which are currently deprecated). The meaning and supported values for these parameters are displayed as comments in the file "powercap/powercap.h", where each parameter is associated with a variable using the same name.

*Run*

To run an application please refer to the "README" file included in the application path. The configuration file (hope_config.txt) must be present in the working directory of the application.

*Code organization*

The software that constitutes the power capping solution is in the "powercap/" folder. In "powercap.h" and "powercap.c" resides the code that accounts for data structures and global variables initialization, DVFS and thread management, statistics gathering, time and energy sampling. The code in "heuristics.c" implements different exploration strategies based on either a power consumption limit or energy-per-transaction limit. 
The code in "stats_t.h" presents the struct used for statistics gathering. 

The folder "tinySTM" contains the TinySTM transactional memory framework. The different functions and global variables offered by the files in the "powercap/" folder are called and referenced by the file "TinySTM/src/stm.c" at the proper point of the transaction lifecycle. 

The code in "stamp/" contains the applications of the STAMP benchmark suite, modified to support lock-based synchronization which is enabled if the macro LOCK_BASED_TRANSACTIONS is defined at compile time. 

The code in "bench" contains the .sh scripts used for batch testing and the respective results (both raw and aggregated). 

The code in "utils/"" contains multiple utility programs used to parse test results. 

*SBAC-PAD2017 Submission*

In the paper submitted to SBAC-PAD2017 only HEURISTIC_MODE 9-10-11 have been used which do not rely on any offline power consumption profilation and only focus on executions with an average power consumption constraint. Specifically, in the file "powercap/heuristics.c" the function "heuristic_highest_threads" implements the baseline technique, "dynamic_heuristic0" the basic strategy and "dynamic_heuristic1" the enhanced strategy as referenced in Section 5 of the submitted paper. The preliminary analysis has been performed with default implementations of both TinySTM and STAMP, with results located in "bench/SBAC-PAD/results/all_config" and "bench/SBAC-PAD/results/all_config_lock". The results of the experimental analysis, as used for the submitted paper, can be found in "bench/SBAC-PAD/results/heuristic_comparison6" and "bench/SBAC-PAD/results/heuristics_comparison6" and "bench/SBAC-PAD/results/heuristics_lock_comparison1". Unfortunately, there was not enough space to fit a comparison of the different timelines of execution (as offered in the file timeline.txt when TIMELINE_PLOT is defined at compile time) of the same applications with the same parameters, but different exploration policy. For the same space concern, some STAMP applications have been cut from the submitted version of the paper as their results (and throughput curve shapes) were equivalent to those of other considered applications. In addition, results obtained for some of these applications not considered in the submitted paper (ssca2, kmeans) can be found in the "bench\" folder.
