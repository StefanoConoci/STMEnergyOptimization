ITERATIONS=25
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison6/vacation-70-stm-dualphase.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison6/vacation-60-stm-dualphase.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison6/vacation-50-stm-dualphase.txt

	echo "Vacation completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder  -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/intruder-70-stm-dualphase.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder  -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/intruder-60-stm-dualphase.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder  -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/intruder-50-stm-dualphase.txt

	echo "Intruder completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/genome-70-stm-dualphase.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 3000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/genome-60-stm-dualphase.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 3000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison6/genome-50-stm-dualphase.txt

	echo "Genome completed"

done

date

echo "Benchmark completed."
