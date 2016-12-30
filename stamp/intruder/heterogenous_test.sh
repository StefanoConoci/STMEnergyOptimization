export STM_STATS=1

/home/conoci/scripts/./set_p1.sh

for i in $(seq 1 5)
do
	/home/conoci/scripts/./set_p1.sh
	numactl --cpubind=0 --membind=0 ./intruder -a10 -l128 -n262144 -s1 -t10 >> P1.txt
	
	/home/conoci/scripts/./set_pkg0_half_p11.sh
	numactl --cpubind=0 --membind=0 ./intruder -a10 -l128 -n262144 -s1 -t10 >> P_mixed.txt
	
	/home/conoci/scripts/./set_p11.sh
	numactl --cpubind=0 --membind=0 ./intruder -a10 -l128 -n262144 -s1 -t10 >> P11.txt
	
	echo "Iteration $i completed"

done

echo "Test done"
