touch $1.csv 

echo "Power_limit,Pstate,Threads,Steps,Runtime,Commits,Aborts,Throughput,Energy_per_tx,Package_power" >> $1.csv

printf "40," >> $1.csv
./parser $1-40.txt >> $1.csv

printf "50," >> $1.csv
./parser $1-50.txt >> $1.csv

printf "60," >> $1.csv
./parser $1-60.txt >> $1.csv

printf "70," >> $1.csv
./parser $1-70.txt >> $1.csv

printf "80," >> $1.csv
./parser $1-80.txt >> $1.csv

printf "90," >> $1.csv
./parser $1-90.txt >> $1.csv

printf "100," >> $1.csv
./parser $1-100.txt >> $1.csv

printf "110," >> $1.csv
./parser $1-110.txt >> $1.csv

echo "File created"
