ITERATIONS=20
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"

	echo "Vacation"	

	./set_profile 5 40 10000 55 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_energy/vacation-1.txt
	
	./set_profile 5 50 10000 60 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_energy/vacation-2.txt

	./set_profile 5 60 10000 65 3 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_energy/vacation-3.txt

	echo "Yada"
	
	./set_profile 5 40 10000 65 3 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_energy/yada-1.txt

	./set_profile 5 50 10000 70 3 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_energy/yada-2.txt

	./set_profile 5 60 10000 75 3 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_energy/yada-3.txt

	echo "Intruder"
	
	./set_profile 5 40 2000 5 3 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruder-1.txt 

	./set_profile 5 50 2000 10 3 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruder-2.txt 

	./set_profile 5 60 2000 15 3 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruder-3.txt 


	echo "Intruder-Bound"

	./set_profile 5 40 2000 5 3 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruderbound-1.txt 

	./set_profile 5 50 2000 10 3 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruderbound-2.txt 

	./set_profile 5 60 2000 15 3 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_energy/intruderbound-3.txt 

done 

