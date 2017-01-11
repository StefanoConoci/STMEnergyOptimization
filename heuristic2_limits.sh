ITERATIONS=5
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"

	echo "Vacation"	

	./set_profile 5 40 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-40.txt
	
	./set_profile 5 50 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-50.txt

	./set_profile 5 60 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-60.txt

	./set_profile 5 70 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-70.txt

	./set_profile 5 80 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-80.txt

	./set_profile 5 90 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-90.txt

	./set_profile 5 100 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-100.txt

	./set_profile 5 110 10000 60 2 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/heuristic_power/vacation-110.txt

	echo "Yada"
	
	./set_profile 5 40 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-40.txt

	./set_profile 5 50 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-50.txt

	./set_profile 5 60 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-60.txt

	./set_profile 5 70 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-70.txt

	./set_profile 5 80 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-80.txt

	./set_profile 5 90 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-90.txt

	./set_profile 5 100 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-100.txt

	./set_profile 5 110 10000 75 2 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/heuristic_power/yada-110.txt


	
	

done 

