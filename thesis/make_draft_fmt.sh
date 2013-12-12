#!/bin/sh
pdflatex -ini -jobname="thesis_draft" "&pdflatex thesis"
source ./clean.sh