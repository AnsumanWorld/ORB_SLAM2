splot "07.txt" using 4:12:8 title 'GROUND-TRUTH' with lines, "oxts-in-local.txt" using 1:2:3 title 'OxtsInLocal' with lines


splot "data/07.txt" using 4:12:8 title 'GROUND-TRUTH' with lines, "data/07-oxts-in-local.txt" using ($2*-1):($1):3 title 'OxtsInLocal' with lines
