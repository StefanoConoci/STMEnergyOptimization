ITERATIONS=10
export STM_STATS=1

cd ..

for i in $(seq 0 11)
do
	/home/conoci/scripts/./set_p$i.sh
	echo "Set to p-state $i"
	for a in $(seq 1 20)
	do
		for b in $(seq 1 $ITERATIONS)	
		do	
                	echo "Iteration $b"
			/home/conoci/git/STMEnergyOptimization/./energy_monitor /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c$a
		done
		
		echo " "
	done
done
