echo "Parsing bechmark $1"

touch $1.csv 

echo "P-state,Threads,Runtime,Throughput,Commits" >> $1.csv

for p in $(seq 0 11)
do
	for t in $(seq 1 20)
	do
		printf "$p," >> $1.csv
		./barebone_parser $1-$p-$t.txt >> $1.csv
	done
	printf "\n" >>$1.csv
done