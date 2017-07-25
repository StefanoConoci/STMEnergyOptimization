#!/bin/bash
rm profiler
gcc profiler.c -o profiler -lpthread
gcc set_profile.c -o set_profile
cd rapl-power
make clean
make
cd ..
cd tinySTM
make clean
make
cd ..
cd stamp/bayes
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd genome
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd intruder
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd kmeans
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd labyrinth
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd vacation
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd ssca2
make -f Makefile.stm clean
make -f Makefile.stm
cd ..
cd yada
make -f Makefile.stm clean
make -f Makefile.stm


