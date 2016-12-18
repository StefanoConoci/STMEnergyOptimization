#!/bin/bash
rm profiler
gcc profiler.c -o profiler -lpthread
gcc energy_monitor.c -o energy_monitor
gcc utils/stamp_parser_energy_desktop.c -o utils/parser_desktop
cd rapl-power
make clean
make
cd ..
cd tinySTM
make clean
make
cd ..
cd stamp/vacation
make -f Makefile.stm clean
make -f Makefile.stm



