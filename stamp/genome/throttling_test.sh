export STM_STATS=1
sudo /home/stefano/scripts/./set_userspace.sh

sudo /home/stefano/scripts/./set_3300.sh
for i in $(seq 1 50) 
do 
	./genome -g16384 -s64 -n16777216 -t4 >> genome_3300.txt
done

sudo /home/stefano/scripts/./set_2600.sh
for i in $(seq 1 50) 
do 
	./genome -g16384 -s64 -n16777216 -t4 >> genome_2600.txt
done

sudo /home/stefano/scripts/./set_2000.sh
for i in $(seq 1 50) 
do 
	./genome -g16384 -s64 -n16777216 -t4 >> genome_2000.txt
done

sudo /home/stefano/scripts/./set_1500.sh
for i in $(seq 1 50) 
do 
	./genome -g16384 -s64 -n16777216 -t4 >> genome_1500.txt
done

sudo /home/stefano/scripts/./set_800.sh
for i in $(seq 1 50) 
do 
	./genome -g16384 -s64 -n16777216 -t4 >> genome_800.txt
done




