export STM_STATS=1
sudo /home/stefano/scripts/./set_userspace.sh

sudo /home/stefano/scripts/./set_3300.sh
for i in $(seq 1 50) 
do 
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder_3300.txt
done

sudo /home/stefano/scripts/./set_2600.sh
for i in $(seq 1 50) 
do 
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder_2600.txt
done

sudo /home/stefano/scripts/./set_2000.sh
for i in $(seq 1 50) 
do 
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder_2000.txt
done

sudo /home/stefano/scripts/./set_1500.sh
for i in $(seq 1 50) 
do 
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder_1500.txt
done

sudo /home/stefano/scripts/./set_800.sh
for i in $(seq 1 50) 
do 
	./intruder -a10 -l128 -n262144 -s1 -t4 >> intruder_800.txt
done




