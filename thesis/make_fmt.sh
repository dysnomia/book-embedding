#!/bin/sh
pdflatex -ini -jobname="thesis_full" "&pdflatex thesis"
source ./clean.sh