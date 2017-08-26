ITERATIONS=20
export STM_STATS=1

for i in $(seq 0 11)
do
	for j in $(seq 1 20)
	do
		/home/conoci/git/STMEnergyOptimization/./set_profile $j $i 80.0 10000 50 8 10 0 10 10 100 10.0 10 1.0
		for b in $(seq 1 $ITERATIONS)	
		do	
			echo "P-state=$i"
			echo  "Threads=$j"
	        echo "Iteration $b"
			echo ""
			/home/conoci/git/STMEnergyOptimization/stamp/kmeans/./kmeans -m40 -n40 -t0.00001 -i stamp/kmeans/inputs/random-n16384-d24-c16.txt -p$j >> bench/SBAC-PAD/results/all_config_lock/kmeans-$i-$j.txt
		done
	done 
done

echo "Benchmark completed."
