ITERATIONS=20
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"

	echo "Intruder"
	
	./set_profile 5 60 2000 20 2 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruder-60.txt 
	
	./set_profile 5 80 2000 20 2 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruder-90.txt  

	./set_profile 5 110 2000 20 2 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruder-110.txt 

	./set_profile 5 110 2000 20 7 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruder-TS.txt  
	
	./set_profile 20 110 2000 20 6 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruder-STATIC.txt  
	
	echo "Intruder bound"

	./set_profile 5 60 2000 20 2 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruderbound-60.txt 
	
	./set_profile 5 80 2000 20 2 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruderbound-90.txt  

	./set_profile 5 110 2000 20 2 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruderbound-110.txt 

	./set_profile 5 110 2000 20 7 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruderbound-TS.txt  
	
	./set_profile 20 110 2000 20 6 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/heuristic_power/intruderbound-STATIC.txt  

	echo "Vacation"
	
	./set_profile 5 60 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-60.txt
	
	./set_profile 5 80 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-90.txt

	./set_profile 5 110 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-110.txt

	./set_profile 5 110 10000 60 7 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-TS.txt

	./set_profile 20 110 10000 60 6 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-STATIC.txt
	
	echo "Yada"
	
	./set_profile 5 60 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-60.txt

	./set_profile 5 80 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-90.txt

	./set_profile 5 110 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-110.txt

	./set_profile 5 110 10000 75 7 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-TS.txt

	./set_profile 20 110 10000 75 6 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-STATIC.txt
	

done 

