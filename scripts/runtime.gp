reset
set ylabel 'time(sec)'
set style fill solid
set title 'function comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'


plot [:][:2.7]'output.txt' using 2:xtic(1) with histogram title 'avx', \
'' using ($0-0.15):($2+0.04):2 with labels title ' ', \
''using 3:xtic(1) with histogram title 'sse', \
'' using ($0+0.01):($2-1.25):3 with labels title ' ', \
''using 4:xtic(1) with histogram title 'original', \
'' using ($0+0.17):($2-1.75):4 with labels title ' ', \
''using 5:xtic(1) with histogram title 'unrolling' , \
'' using ($0+0.34):($2-2.25):5 with labels title ' '
