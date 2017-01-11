touch $1.csv 

echo "Constraint,Pstate,Threads,Steps,Runtime,Commits,Aborts,Throughput,Energy_per_tx,Package_power" >> $1.csv

printf "60," >> $1.csv
./parser $1-60.txt >> $1.csv

printf "90," >> $1.csv
./parser $1-90.txt >> $1.csv

printf "110," >> $1.csv
./parser $1-110.txt >> $1.csv

printf "TS," >> $1.csv
./parser $1-TS.txt >> $1.csv

printf "STATIC," >> $1.csv
./parser $1-STATIC.txt >> $1.csv

echo "File created"
