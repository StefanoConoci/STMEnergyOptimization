echo "Abort test started"
/home/stefano/scripts/./set_userspace.sh
/home/stefano/scripts/./set_1500.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 1500 MHz"
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder1500.txt
done

/home/stefano/scripts/./set_2000.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 2000 MHz"
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder2000.txt
done

/home/stefano/scripts/./set_2600.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 2600 MHz"
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder2600.txt
done

echo "Test completed. Check result_anaylis file for results." 
