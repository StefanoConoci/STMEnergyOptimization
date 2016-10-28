#!/bin/bash
rm profiler
cd rapl-power
make clean
cd ..
cd tinySTM
make clean
cd ..
cd stamp/bayes
make -f Makefile.stm clean
cd ..
cd genome
make -f Makefile.stm clean
cd ..
cd intruder
make -f Makefile.stm clean
cd ..
cd kmeans
make -f Makefile.stm clean
cd ..
cd labyrinth
make -f Makefile.stm clean
cd ..
cd vacation
make -f Makefile.stm clean
cd ..
cd ssca2
make -f Makefile.stm clean
cd ..
cd yada
make -f Makefile.stm clean



