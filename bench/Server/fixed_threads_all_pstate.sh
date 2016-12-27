ITERATIONS=10
export STM_STATS=1

for i in $(seq 0 11)
do
	/home/conoci/scripts/./set_p$i.sh
	
	for b in $(seq 1 $ITERATIONS)	
	do	
		echo "P-state=$i"
        	echo "Iteration $b"
		echo ""
		/home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> all_pstate_folder/vacation-$i.txt
		/home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t6 >> all_pstate_folder/intruder-$i.txt
		/home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g16384 -s64 -n167772162 -t20 >> all_pstate_folder/genome-$i.txt
		/home/conoci/git/STMEnergyOptimization/stamp/bayes/./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t18 >> all_pstate_folder/bayes-$i.txt
		/home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s20 -i1.0 -u1.0 -l3 -p3 -t20 >> all_pstate_folder/ssca2-$i.txt
	done
done

echo "Benchmark completed."
