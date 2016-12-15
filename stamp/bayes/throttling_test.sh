export STM_STATS=1
sudo /home/stefano/scripts/./set_userspace.sh

sudo /home/stefano/scripts/./set_3300.sh
for i in $(seq 1 50) 
do 
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> bayes_3300.txt
done

sudo /home/stefano/scripts/./set_2600.sh
for i in $(seq 1 50) 
do 
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> bayes_2600.txt
done

sudo /home/stefano/scripts/./set_2000.sh
for i in $(seq 1 50) 
do 
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> bayes_2000.txt
done

sudo /home/stefano/scripts/./set_1500.sh
for i in $(seq 1 50) 
do 
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> bayes_1500.txt
done

sudo /home/stefano/scripts/./set_800.sh
for i in $(seq 1 50) 
do 
	./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4 >> bayes_800.txt
done




