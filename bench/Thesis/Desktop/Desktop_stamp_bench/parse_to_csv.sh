touch $1/$1.csv 

echo "P-state;Threads;Runtime;Commits;Aborts;Throughput;Package_energy;Core_energy;Dram_energy;Package_power;Core_power;Dram_power" >> $1/$1.csv


for p in $(seq 0 15)
do
	for t in $(seq 1 4)
	do
		printf "$p;" >> $1/$1.csv
		./csv_parser $1/$1-P$p-TH$t.txt >> $1/$1.csv
	done
done