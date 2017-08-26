ITERATIONS=3
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
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t119430 -c$j >> bench/SBAC-PAD/results/all_config_lock/vacation-$i-$j.txt
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s17 -i1.0 -u1.0 -l3 -p3 -t$j >> bench/SBAC-PAD/results/all_config_lock/ssca2-$i-$j.txt
		done
	done 
done

echo "Benchmark completed."
