ITERATIONS=5
export STM_STATS=1


/home/stefano/scripts/set_userspace.sh

for i in $(seq 0 15)
do
	/home/stefano/scripts/./set_p$i.sh
	for a in $(seq 1 4)
	do
		for b in $(seq 1 $ITERATIONS)	
		do	
			echo "P-state=$i"
			echo "Threads=$a"
			echo "Iteration=$b"
			echo ""
			/home/stefano/git/stmEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c$a >> vacation/vacation-P$i-TH$a.txt
			/home/stefano/git/stmEnergyOptimization/stamp/genome/./genome -g16384 -s64 -n16777216 -t$a >> genome/genome-P$i-TH$a.txt
			/home/stefano/git/stmEnergyOptimization/stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t$a >> intruder/intruder-P$i-TH$a.txt
			/home/stefano/git/stmEnergyOptimization/stamp/bayes/./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t$a >> bayes/bayes-P$i-TH$a.txt
			/home/stefano/git/stmEnergyOptimization/stamp/ssca2/./ssca2 -s20 -i1.0 -u1.0 -l3 -p3 -t$a >> ssca2/ssca2-P$i-TH$a.txt
		done
	done
done

echo "Benchmark completed. Can shut down the PC. LUCA PUPAZZO!"
