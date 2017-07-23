ITERATIONS=10
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	echo "Genome completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 10000 50 11 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-70-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 10000 50 9 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-70-stm-h0.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 10000 50 11 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-60-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 10000 50 9 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-60-stm-h0.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 11 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-50-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 9 10 2 10 10 100 10.0 5 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison5/ssca2-50-stm-h0.txt

	echo "Ssca2 completed"

done

date

echo "Benchmark completed."
