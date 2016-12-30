
export STM_STATS=1

/home/conoci/scripts/./set_p1.sh

for i in $(seq 1 5) 
do 
	numactl --physcpubind=+0-5 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_6
        numactl --physcpubind=+0-4,10-10 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_5
	numactl --physcpubind=+0-3,10-11 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_4
        numactl --physcpubind=+0-2,10-12 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_3
        numactl --physcpubind=+0-1,10-13 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_2
        numactl --physcpubind=+0-0,10-14 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_1
	numactl --physcpubind=+10-15 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p1_0
done

/home/conoci/scripts/./set_p11.sh
for in $(seq 1 5)
do
        numactl --physcpubind=+0-5 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_6
        numactl --physcpubind=+0-4,10-10 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_5
        numactl --physcpubind=+0-3,10-11 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_4
        numactl --physcpubind=+0-2,10-12 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_3
        numactl --physcpubind=+0-1,10-13 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_2
        numactl --physcpubind=+0-0,10-14 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_1
        numactl --physcpubind=+10-15 ./intruder -a10 -l128 -n262144 -s1 -t6 >> numa/numa_p11_0
done

echo "Test completed"



