ITERATIONS=5
export STM_STATS=1

for i in $(seq 0 11)
do
	for j in $(seq 1 20)
	do
		/home/conoci/git/STMEnergyOptimization/./set_profile $j $i 80.0 10000 50 8 10 0 10 10
		for b in $(seq 1 $ITERATIONS)	
		do	
			echo "P-state=$i"
			echo  "Threads=$j"
	        echo "Iteration $b"
			echo ""
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c$j >> all_config/vacation-$i-$j.txt
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t$j >> all_config/intruder-$i-$j.txt
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g16384 -s64 -n167772162 -t$j >> all_config/genome-$i-$j.txt
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/bayes/./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t$j >> all_config/bayes-$i-$j.txt
			numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s20 -i1.0 -u1.0 -l3 -p3 -t$j >> all_config/ssca2-$i-$j.txt
		done
	done 
done

echo "Benchmark completed."
