ITERATIONS=5
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
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a20 -l16 -n127680 -s1 -t$j >> /home/conoci/git/STMEnergyOptimization/results/all_config_lock/intruder-$i-$j.txt
		done
	done 
done

echo "Benchmark completed."
