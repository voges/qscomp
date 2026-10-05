#!/usr/bin/env bash

###############################################################################
#                               Command line                                  #
###############################################################################

if [ "$#" -ne 1 ]; then printf "Usage: $0 input_sam\n"; exit -1; fi

input_sam=$1
printf "Input SAM file: $input_sam\n"

if [ ! -f $input_sam ]; then printf "Error: Input SAM file $input_sam is not a regular file.\n"; exit -1; fi


###############################################################################
#                                Executables                                  #
###############################################################################

# Binaries
bzip2="/usr/bin/bzip2"
gzip="/usr/bin/gzip"
python="/usr/bin/python3"
QScomp="/Users/janvoges/Code/qscomp/build/QScomp"
time="/usr/bin/time"
wc="/usr/bin/wc"

# Python scripts
replace_qual_sam_py="/Users/janvoges/Code/qscomp/scripts/replace_qual_sam.py"
xtract_field_sam_py="/Users/janvoges/Code/qscomp/scripts/xtract_field_sam.py"

if [ ! -x $bzip2 ]; then printf "Error: Binary file $bzip2 is not executable.\n"; exit -1; fi
if [ ! -x $gzip ]; then printf "Error: Binary file $gzip is not executable.\n"; exit -1; fi
if [ ! -x $python ]; then printf "Error: Binary file $python is not executable.\n"; exit -1; fi
if [ ! -x $QScomp ]; then printf "Error: Binary file $QScomp is not executable.\n"; exit -1; fi
if [ ! -x $time ]; then printf "Error: Binary file $time is not executable.\n"; exit -1; fi
if [ ! -x $wc ]; then printf "Error: Binary file $wc is not executable.\n"; exit -1; fi
if [ ! -f $replace_qual_sam_py ]; then printf "Error: Python script $replace_qual_sam_py is not a regular file.\n"; exit -1; fi
if [ ! -f $xtract_field_sam_py ]; then printf "Error: Python script $xtract_field_sam_py is not a regular file.\n"; exit -1; fi


###############################################################################
#                                  Compress                                   #
###############################################################################

printf "Extracting quality values from SAM file\n"
$python $xtract_field_sam_py $input_sam 10 1> $input_sam.qual

printf "Running QScomp\n"
$time -l $QScomp $input_sam.qual &> $input_sam.QScomp.log

printf "Constructing new SAM file with QScomp'd quality values\n"
$python $replace_qual_sam_py $input_sam $input_sam.qual.dim1_rc 1> $input_sam.QScomp.sam


###############################################################################
#                                 Statistics                                  #
###############################################################################

printf "Compressing QScomp output with bzip2 and generating statistics\n"
$bzip2 -9 -c $input_sam.qual.dim1 > $input_sam.qual.dim1.bz2
wc -c $input_sam.qual.dim1.bz2 > $input_sam.QScomp.stats

for f in $input_sam.qual.dim2.*; do
    $bzip2 -9 -c $f > $f.bz2
    wc -c $f.bz2 >> $input_sam.QScomp.stats;
done

$bzip2 -9 -c $input_sam.qual.dim2_a > $input_sam.qual.dim2_a.bz2
wc -c $input_sam.qual.dim2_a.bz2 >> $input_sam.QScomp.stats

printf "Compressing quality scores with gzip and bzip2 for reference\n"
$gzip -c $input_sam.qual > $input_sam.qual.gz
wc -c $input_sam.qual.gz > $input_sam.gzip.stats
$bzip2 -9 -c $input_sam.qual > $input_sam.qual.bz2
wc -c $input_sam.qual.bz2 > $input_sam.bzip2.stats
