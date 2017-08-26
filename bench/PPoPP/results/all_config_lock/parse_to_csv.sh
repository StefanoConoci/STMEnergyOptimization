echo "Parsing bechmark $1"

touch $1.csv 

echo "P-state,Threads,Steps, Runtime, Commits, Aborts , Throughput, EnergyPerTx, Power" >> $1.csv

for p in $(seq 0 11)
do
	for t in $(seq 1 20)
	do
		./csv_server_parser $1-$p-$t.txt >> $1.csv
	done
done