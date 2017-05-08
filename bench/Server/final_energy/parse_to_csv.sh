touch $1.csv 

echo "Energy_limit,Pstate,Threads,Steps,Runtime,Commits,Aborts,Throughput,Energy_per_tx,Package_power" >> $1.csv

printf "1," >> $1.csv
./parser $1-1.txt >> $1.csv

printf "2," >> $1.csv
./parser $1-2.txt >> $1.csv

printf "3," >> $1.csv
./parser $1-3.txt >> $1.csv


echo "File created"
