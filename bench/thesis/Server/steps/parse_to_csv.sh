touch $1.csv 

echo "Heuristic,Pstate,Threads,Steps,Runtime,Commits,Aborts,Throughput,Energy_per_tx,Package_power" >> $1.csv

printf "0," >> $1.csv
./parser $1-0.txt >> $1.csv

printf "1," >> $1.csv
./parser $1-1.txt >> $1.csv

printf "2," >> $1.csv
./parser $1-2.txt >> $1.csv

printf "3," >> $1.csv
./parser $1-3.txt >> $1.csv

printf "7," >> $1.csv
./parser $1-7.txt >> $1.csv

echo "File created"
