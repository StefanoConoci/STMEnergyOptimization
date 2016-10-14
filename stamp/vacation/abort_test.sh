echo "Abort test started"
/home/stefano/scripts/./set_userspace.sh
/home/stefano/scripts/./set_3300.sh
echo "Executing at 3300 MHz"
for i in $(seq 1 20)
do	
	echo "Iteration $i at 3300 MHz"
	./vacation -n2 -q90 -u98 -r1048576 -t4194304 -c4 >> result_analysis.txt
done

/home/stefano/scripts/./set_800.sh
echo "Executing at 800 MHz"
for i in $(seq 1 20)
do	
	echo "Iteration $i at 800 MHz"
	./vacation -n2 -q90 -u98 -r1048576 -t4194304 -c4 >> result_analysis.txt
done
echo "Test completed. Check result_anaylis file for results." 
