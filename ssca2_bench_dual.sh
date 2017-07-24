ITERATIONS=25
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 20000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-70-stm-baseline.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 20000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-70-stm-dualphase.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 20000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-70-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 70.0 20000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-70-stm-h1.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 20000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-60-stm-baseline.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 20000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-60-stm-dualphase.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 20000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-60-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 60.0 20000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-60-stm-h1.txt


	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 20000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-50-stm-baseline.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 20000 50 13 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-50-stm-dualphase.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 20000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-50-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 20000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/ssca2_bench_journal/ssca2-50-stm-h1.txt

done

date

echo "Benchmark completed."
