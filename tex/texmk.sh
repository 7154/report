#!/bin/sh
cd ~/Desktop/zikken/tex
ls
echo -n "tex file name:"
read file
mkdir $file
mv $file.tex $file
cd $file
platex $file
dvipdfmx $file
mv $file.tex ..
evince $file.pdf
