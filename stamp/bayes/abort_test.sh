echo "Abort test started"
/home/stefano/scripts/./set_userspace.sh
/home/stefano/scripts/./set_3300.sh
echo "3300 MHz" >> result_analysis.txt
for i in $(seq 1 20)
do	
	echo "Iteration $i at 3300 MHz"
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> result_analysis.txt
done

echo"" >> result_analysis.txt

/home/stefano/scripts/./set_800.sh
echo "Executing at 800 MHz" >>  result_analysis.txt
for i in $(seq 1 20)
do	
	echo "Iteration $i at 800 MHz"
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> result_analysis.txt
done
echo "Test completed. Check result_anaylis file for results." 
