ITERATIONS=20
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"

	echo "Vacation"	

	./set_profile 5 40 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-40.txt
	
	./set_profile 5 50 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-50.txt

	./set_profile 5 60 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-60.txt

	./set_profile 5 70 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-70.txt

	./set_profile 5 80 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-80.txt

	./set_profile 5 90 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-90.txt

	./set_profile 5 100 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-100.txt

	./set_profile 5 110 10000 60 0 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-110.txt

	/set_profile 5 110 10000 60 7 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-TS.txt

	/set_profile 20 110 10000 60 6 20 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/final_power_0/vacation-STATIC.txt

	echo "Yada"
	
	./set_profile 5 40 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-40.txt

	./set_profile 5 50 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-50.txt

	./set_profile 5 60 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-60.txt

	./set_profile 5 70 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-70.txt

	./set_profile 5 80 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-80.txt

	./set_profile 5 90 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-90.txt

	./set_profile 5 100 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-100.txt

	./set_profile 5 110 10000 75 0 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-110.txt

	./set_profile 5 110 10000 75 7 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-TS.txt

	./set_profile 20 110 10000 75 6 20 
	stamp/yada/./yada -a30 -i stamp/yada/inputs/ttimeu1000000.2 -t20 >> bench/Server/final_power_0/yada-STATIC.txt

	echo "Intruder"
	
	./set_profile 5 40 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-40.txt 

	./set_profile 5 50 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-50.txt 

	./set_profile 5 60 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-60.txt 

	./set_profile 5 70 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-70.txt 

	./set_profile 5 80 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-80.txt 
	
	./set_profile 5 90 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-90.txt  
	
	./set_profile 5 100 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-100.txt 

	./set_profile 5 110 2000 20 0 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-110.txt 

	./set_profile 5 110 2000 20 7 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-TS.txt  
	
	./set_profile 20 110 2000 20 6 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruder-STATIC.txt  

	echo "Intruder-Bound"

	./set_profile 5 40 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-40.txt 

	./set_profile 5 50 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-50.txt 

	./set_profile 5 60 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-60.txt 

	./set_profile 5 70 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-70.txt 

	./set_profile 5 80 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-80.txt 
	
	./set_profile 5 90 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-90.txt  
	
	./set_profile 5 100 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-100.txt 

	./set_profile 5 110 2000 20 0 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-110.txt 

	./set_profile 5 110 2000 20 7 20
	numactl --physcpubind=+0-9 stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-TS.txt  
	
	./set_profile 20 110 2000 20 6 20
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/final_power_0/intruderbound-STATIC.txt  

	
	

done 

