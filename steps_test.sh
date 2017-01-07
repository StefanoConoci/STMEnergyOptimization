ITERATIONS=10
export STM_STATS=1

for i in $(seq 1 $ITERATIONS)
do
	echo "Iteration $i"
	
	./set_profile 5 80 2500 20 0 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-0.txt

	./set_profile 5 80 2500 20 1 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-1.txt
	
	./set_profile 5 80 2500 20 2 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-2.txt 

	./set_profile 5 80 2500 20 3 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-3.txt 
	
	./set_profile 5 80 2500 25 4 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-4.txt 
	
	./set_profile 5 80 2500 25 5 15
	stamp/intruder/./intruder -a10 -l128 -n262144 -s1 -t20 >> bench/Server/steps/intruder-5.txt
	
	./set_profile 5 80 10000 60 0 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-0.txt
	
	./set_profile 5 80 10000 60 1 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-1.txt

	./set_profile 5 80 10000 60 2 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-2.txt

	./set_profile 5 80 10000 60 3 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-3.txt

	./set_profile 5 80 10000 60 4 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-4.txt
	
	./set_profile 5 80 10000 60 5 15 
	stamp/vacation/./vacation -n4 -q60 -u90 -r1048576 -t41943040 -c20 >> bench/Server/steps/vacation-5.txt

done 

