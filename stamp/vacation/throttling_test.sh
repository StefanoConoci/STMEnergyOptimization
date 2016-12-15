export STM_STATS=1
sudo /home/stefano/scripts/./set_userspace.sh

sudo /home/stefano/scripts/./set_3300.sh
for i in $(seq 1 50) 
do 
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> vacation_3300.txt
done

sudo /home/stefano/scripts/./set_2600.sh
for i in $(seq 1 50) 
do 
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> vacation_2600.txt
done

sudo /home/stefano/scripts/./set_2000.sh
for i in $(seq 1 50) 
do 
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> vacation_2000.txt
done

sudo /home/stefano/scripts/./set_1500.sh
for i in $(seq 1 50) 
do 
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> vacation_1500.txt
done

sudo /home/stefano/scripts/./set_800.sh
for i in $(seq 1 50) 
do 
	./vacation -n4 -q60 -u90 -r1048576 -t4194304 -c4 >> vacation_800.txt
done




