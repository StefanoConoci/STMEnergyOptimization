ITERATIONS=10
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 10 10 2 10 10 150 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/5000-5-1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/5000-10-1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 5000 50 10 10 2 10 10 150 10.0 5 0.5
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/5000-5-05.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 10 10 2 10 10 150 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/3000-5-1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/3000-10-1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 10 10 2 10 10 150 10.0 5 0.5
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/3000-5-05.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 3000 50 9 10 2 10 10 150 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t40194304 -c20 >> bench/SBAC-PAD/results/h1_fine_tuning/h0-3000.txt	

done

date

echo "Benchmark completed."
