touch $1.csv 

echo "P-state,Threads,Runtime,Commits,Aborts,Throughput,Package0_energy,Dram0_energy,Package1_energy,Dram1_energy,Package0_power,Dram0_power,Package1_power,Dram1_power" >> $1.csv

for p in $(seq 0 11)
do
	printf "$p," >> $1.csv
	./csv_server_parser $1-$p.txt >> $1.csv
done
