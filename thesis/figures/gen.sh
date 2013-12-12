#!/bin/sh
#rm t_*.pdf
pdflatex -shell-escape gen
rm *.log *.aux *.auxlock *.dpth *.idx gen.pdf