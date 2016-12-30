export STM_STATS=1

for i in $(seq 1 20)
do
	/home/conoci/scripts/./set_p1.sh
	/home/conoci/scripts/./set_pkg0_p11.sh
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c20 fullP11-mixed.txt
	
	/home/conoci/scripts/./set_p1.sh
	/home/conoci/scripts/./set_pkg0_half_p11.sh >> halfP11-mixed.txt
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c20	
	
	echo "Iteration $i completed"

done

echo "Test done"
