ITERATIONS=10
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"

	echo "Vacation"	

	./set_profile 2 40 10000 30 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> bench/Desktop/final_energy/vacation-1.txt
	
	./set_profile 2 40 10000 40 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> bench/Desktop/final_energy/vacation-2.txt

	./set_profile 2 40 10000 50 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> bench/Desktop/final_energy/vacation-3.txt

	./set_profile 2 40 10000 60 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> bench/Desktop/final_energy/vacation-4.txt

	./set_profile 2 50 10000 50 7 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> bench/Desktop/final_energy/vacation-TS.txt

	echo "Yada"
	
	./set_profile 2 40 10000 25 3 20 
	stamp/yada/./yada -a20 -i stamp/yada/inputs/ttimeu1000000.2 -t4 >> bench/Desktop/final_energy/yada-1.txt

	./set_profile 2 40 10000 30 3 20 
	stamp/yada/./yada -a20 -i stamp/yada/inputs/ttimeu1000000.2 -t4 >> bench/Desktop/final_energy/yada-2.txt

	./set_profile 2 40 10000 35 3 20 
	stamp/yada/./yada -a20 -i stamp/yada/inputs/ttimeu1000000.2 -t4 >> bench/Desktop/final_energy/yada-3.txt

	./set_profile 2 40 10000 40 3 20 
	stamp/yada/./yada -a20 -i stamp/yada/inputs/ttimeu1000000.2 -t4 >> bench/Desktop/final_energy/yada-4.txt

	./set_profile 2 50 10000 37 7 20 
	stamp/yada/./yada -a20 -i stamp/yada/inputs/ttimeu1000000.2 -t4 >> bench/Desktop/final_energy/yada-TS.txt


done 

