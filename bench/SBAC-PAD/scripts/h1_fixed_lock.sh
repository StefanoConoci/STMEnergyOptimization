ITERATIONS=20
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t10194304 -c20 >> bench/SBAC-PAD/results/h1_fixed_lock/vacation-70-stm-h1.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t10194304 -c20 >> bench/SBAC-PAD/results/h1_fixed_lock/vacation-60-stm-h1.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t10194304 -c20 >> bench/SBAC-PAD/results/h1_fixed_lock/vacation-50-stm-h1.txt

	echo "Vacation completed"


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/intruder-70-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/intruder-60-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/intruder-50-stm-h1.txt

	echo "Intruder completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g36384 -s64 -n36777216 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/genome-70-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 3000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g36384 -s64 -n36777216 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/genome-60-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 3000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g36384 -s64 -n36777216 -t20 >> bench/SBAC-PAD/results/h1_fixed_lock/genome-50-stm-h1.txt


	echo "Genome completed"

done

date

echo "Benchmark completed."
