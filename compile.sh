#!/bin/bash
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



