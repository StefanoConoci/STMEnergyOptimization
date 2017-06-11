echo "Parsing bechmark $1"

touch $1_plot.csv

printf "#thread\t#p0\t#p1\t#p2\t#p3\t#p4\t#p5\t#p6\t#p7\t#p8\t#p9\t#p10\t#p11\n" >> $1_plot.csv

for t in $(seq 1 20)
do
	printf "$t\t" >> $1_plot.csv
	for p in $(seq 0 11)
	do
		./gnuplot_tp $1-$p-$t.txt >> $1_plot.csv
		printf "\t" >> $1_plot.csv
	done
printf "\n" >> $1_plot.csv
done