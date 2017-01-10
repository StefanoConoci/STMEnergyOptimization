ITERATIONS=10
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"
	
	./set_profile 5 80 2000 20 0 15
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruderbound-0.txt

	./set_profile 5 80 2000 20 1 15
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruderbound-1.txt
	
	./set_profile 5 80 2000 20 2 15
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruderbound-2.txt 

	./set_profile 5 80 2000 20 3 15
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruderbound-3.txt 
	
	./set_profile 5 80 2000 25 7 15
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruderbound-7.txt
	
	./set_profile 5 80 10000 75 0 15 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/steps/yada-0.txt
	
	./set_profile 5 80 10000 75 1 15 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/steps/yada-1.txt

	./set_profile 5 80 10000 75 2 15 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/steps/yada-2.txt

	./set_profile 5 80 10000 75 3 15 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/steps/yada-3.txt

	./set_profile 5 80 10000 75 7 15 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/steps/yada-7.txt

done 

