set terminal postscript eps enhanced "Helvetica" 24

set ylabel "Speed-up"

set key reverse bmargin center horizontal Left
set key noinvert samplen 1 spacing 1 width 0 height 0 

set auto x
unset xtics
set grid y
set xtics nomirror #rotate by -90 scale 0 
set style data histogram
set style histogram gap 2

set yrange [0:2]

set output "parsed-50.eps"
plot    'RES-BSY_WR=0-LD_WR=1_LD_RD=0.dat' 	    u ($2):xticlabels(1) fs solid 0.90 lt -1 t "Baseline",\
		''						u ($3):xticlabels(1) fs solid 0.70 lt -1 t "H0",\
		''						u ($4):xticlabels(1) fs solid 0.50 lt -1 t "H1";