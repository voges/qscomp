#!/bin/bash

###############################################################################
#                               Command line                                  #
###############################################################################

if [ "$#" -ne 1 ]; then
    printf "Usage: $0 input_sam\n"
    exit -1
fi

input_sam=$1
printf "Input SAM file: $input_sam\n"

printf "Checking input SAM file $input_sam ... "
if [ ! -f $input_sam ]; then printf "did not find input SAM file: $input_sam\n"; exit -1; fi
printf "OK\n"

###############################################################################
#                                Executables                                  #
###############################################################################

# Binaries
bzip2="/usr/bin/bzip2"
gzip="/usr/bin/gzip"
python="/usr/bin/python"
QScomp="/home/voges/git/QScomp/build/QScomp"
time="/usr/bin/time"

# Python scripts
replace_qual_sam_py="/home/voges/git/ngstools/replace_qual_sam.py"
xtract_qual_sam_py="/home/voges/git/ngstools/xtract_qual_sam.py"

printf "Checking executables ... "
if [ ! -x $bzip2 ]; then printf "did not find $bzip2\n"; exit -1; fi
if [ ! -x $gzip ]; then printf "did not find $gzip\n"; exit -1; fi
if [ ! -x $python ]; then printf "did not find $python\n"; exit -1; fi
if [ ! -x $QScomp ]; then printf "did not find $QScomp\n"; exit -1; fi
if [ ! -x $time ]; then printf "did not find $time\n"; exit -1; fi
if [ ! -e $replace_qual_sam_py ]; then printf "did not find $replace_qual_sam_py\n"; exit -1; fi
if [ ! -e $xtract_qual_sam_py ]; then printf "did not find $xtract_qual_sam_py\n"; exit -1; fi
printf "OK\n"

###############################################################################
#                                  Compress                                   #
###############################################################################

printf "Extracting quality values from SAM file ... "
$python $xtract_qual_sam_py $input_sam 2> $input_sam.qual
printf "OK\n"

printf "Running QScomp ... "
cmd="$QScomp $input_sam.qual"
$time -v -o $input_sam.QScomp.time $cmd &> $input_sam.QScomp.log
printf "OK\n"

printf "Constructing new SAM file with QScomp'd quality values ... "
$python $replace_qual_sam_py $input_sam $input_sam.qual.dim1_rc
mv $input_sam.new_qual.sam $input_sam.QScomp.sam
printf "OK\n"

###############################################################################
#                                 Statistics                                  #
###############################################################################

printf "Compressing QScomp output with bzip2 and generating statistics ... "
$bzip2 -9 -c $input_sam.qual.dim1 > $input_sam.qual.dim1.bz2
wc -c $input_sam.qual.dim1.bz2 > $input_sam.QScomp.stats

for f in $input_sam.qual.dim2.*; do
    $bzip2 -9 -c $f > $f.bz2
    wc -c $f.bz2 >> $input_sam.QScomp.stats;
done

$bzip2 -9 -c $input_sam.qual.dim2_a > $input_sam.qual.dim2_a.bz2
wc -c $input_sam.qual.dim2_a.bz2 >> $input_sam.QScomp.stats
printf "OK\n"

printf "Compressing quality scores with gzip and bzip2 for reference ... "
$gzip -c $input_sam.qual > $input_sam.qual.gz
wc -c $input_sam.qual.gz > $input_sam.gzip.stats
$bzip2 -9 -c $input_sam.qual > $input_sam.qual.bz2
wc -c $input_sam.qual.bz2 > $input_sam.bzip2.stats
printf "OK\n"

###############################################################################
#                                   Cleanup                                   #
###############################################################################

printf "Cleanup ... "
rm -f $input_sam.qual
printf "OK\n";

