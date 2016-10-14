echo "Abort test started"
/home/stefano/scripts/./set_userspace.sh

/home/stefano/scripts/./set_800.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 800 MHz"
	./labyrinth -i inputs/random-x512-y512-z7-n512.txt -t4 >> labyrinth800.txt
	echo "" >> labyrinth800.txt
done

/home/stefano/scripts/./set_1500.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 1500 MHz"
	./labyrinth -i inputs/random-x512-y512-z7-n512.txt -t4 >> labyrinth1500.txt
	echo "" >> labyrinth1500.txt
done

/home/stefano/scripts/./set_2000.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 2000 MHz"
	./labyrinth -i inputs/random-x512-y512-z7-n512.txt -t4 >> labyrinth2000.txt
	echo "" >> labyrinth2000.txt
done

/home/stefano/scripts/./set_2600.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 2600 MHz"
	./labyrinth -i inputs/random-x512-y512-z7-n512.txt -t4 >> labyrinth2600.txt
	echo "" >> labyrinth2600.txt
done

/home/stefano/scripts/./set_3300.sh
for i in $(seq 1 20)
do	
	echo "Iteration $i at 3300 MHz"
	./labyrinth -i inputs/random-x512-y512-z7-n512.txt -t4 >> labyrinth3300.txt
	echo "" >> labyrinth3300.txt
done

echo "Test completed. Check files for results." 
