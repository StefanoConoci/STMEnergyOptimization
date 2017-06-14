ITERATIONS=20
export STM_STATS=1

		
for b in $(seq 1 $ITERATIONS)	
do	
	echo "Iteration $b"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-80-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-80-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-80-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-50-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-50-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t8194304 -c20 >> bench/SBAC-PAD/results/heuristics_comparison2/vacation-50-stm-h1.txt

	echo "Vacation completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-80-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-80-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder  -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-80-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-50-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-50-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/intruder/./intruder  -a10 -l1024 -n262144 -s1 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/intruder-50-stm-h1.txt

	echo "Intruder completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-80-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-80-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-80-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-50-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-50-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 5000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/genome/./genome -g56384 -s128 -n167772160 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/genome-50-stm-h1.txt

	echo "Genome completed"

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-80-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-80-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 80.0 10000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-80-stm-h1.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 11 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-50-stm-baseline.txt
	
	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 9 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-50-stm-h0.txt

	/home/conoci/git/STMEnergyOptimization/./set_profile 4 0 50.0 10000 50 10 10 2 10 10 150 10.0 10 1.0
	numactl --physcpubind=+0-19 /home/conoci/git/STMEnergyOptimization/stamp/ssca2/./ssca2 -s22 -i1.0 -u1.0 -l3 -p3 -t20 >> bench/SBAC-PAD/results/heuristics_comparison2/ssca2-50-stm-h1.txt

	echo "Ssca2 completed"

done
	

echo "Benchmark completed."
