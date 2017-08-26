ITERATIONS=10
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	

	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 3000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/dynamic_stm_dualphase_fixed/genome-50-stm-dualphase.txt

done

date

echo "Benchmark completed."
