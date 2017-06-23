echo "Benchmark,Heuristic,Throughput,Error" >> heuristics-$1-stm.csv 

printf "intruder,baseline," >> heuristics-$1-stm.csv 
./net_stats_parser_csv intruder-$1-stm-baseline.txt >> heuristics-$1-stm.csv 

printf "intruder,h0," >> heuristics-$1-stm.csv 
./net_stats_parser_csv intruder-$1-stm-h0.txt >> heuristics-$1-stm.csv 

printf "intruder,h1," >> heuristics-$1-stm.csv 
./net_stats_parser_csv intruder-$1-stm-h1.txt >> heuristics-$1-stm.csv 

printf "vacation,baseline," >> heuristics-$1-stm.csv 
./net_stats_parser_csv vacation-$1-stm-baseline.txt >> heuristics-$1-stm.csv 

printf "vacation,h0," >> heuristics-$1-stm.csv 
./net_stats_parser_csv vacation-$1-stm-h0.txt >> heuristics-$1-stm.csv 

printf "vacation,h1," >> heuristics-$1-stm.csv 
./net_stats_parser_csv vacation-$1-stm-h1.txt >> heuristics-$1-stm.csv 

printf "genome,baseline," >> heuristics-$1-stm.csv 
./net_stats_parser_csv genome-$1-stm-baseline.txt >> heuristics-$1-stm.csv 

printf "genome,h0," >> heuristics-$1-stm.csv 
./net_stats_parser_csv genome-$1-stm-h0.txt >> heuristics-$1-stm.csv 

printf "genome,h1," >> heuristics-$1-stm.csv 
./net_stats_parser_csv genome-$1-stm-h1.txt >> heuristics-$1-stm.csv 




