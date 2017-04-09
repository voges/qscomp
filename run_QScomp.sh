#!/bin/bash

###############################################################################
#                               Command line                                  #
###############################################################################

if [ "$#" -ne 2 ]; then
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
pgrep="/usr/bin/pgrep"
python="/usr/bin/python"
QScomp="/home/voges/git/QScomp/build/QScomp"
time="/usr/bin/time"

# Python scripts
ps_mem_py="/home/voges/git/QScomp/ps_mem.py"
replace_qual_sam_py="/home/voges/git/ngstools/replace_qual_sam.py"
xtract_qual_sam_py="/home/voges/git/ngstools/xtract_qual_sam.py"

printf "Checking executables ... "
if [ ! -x $pgrep ]; then printf "did not find $pgrep\n"; exit -1; fi
if [ ! -x $python ]; then printf "did not find $python\n"; exit -1; fi
if [ ! -x $QScomp ]; then printf "did not find $QScomp\n"; exit -1; fi
if [ ! -x $time ]; then printf "did not find $time\n"; exit -1; fi
if [ ! -e $ps_mem_py ]; then printf "did not find $ps_mem_py\n"; exit -1; fi
if [ ! -e $replace_qual_sam_py ]; then printf "did not find $replace_qual_sam_py\n"; exit -1; fi
if [ ! -e $xtract_qual_sam_py ]; then printf "did not find $xtract_qual_sam_py\n"; exit -1; fi
printf "OK\n"

###############################################################################
#                                  Compress                                   #
###############################################################################

printf "Extracting quality values from SAM file ... "
$python $xtract_qual_sam_py $input_sam 2> $input_sam.qual
printf "OK\n"

printf "Running QScomp ..."
cmd="$QScomp $input_sam.qual"
$time -v -o $input_sam.QScomp.time $cmd &> $input_sam.QScomp.log
time_pid=$!
cmd_pid=$($pgrep -P $time_pid)
printf "Command being traced: \"$cmd\"\n" > $input_sam.QScomp.mem
$python $ps_mem_py -t -w 1 --swap -p $cmd_pid >> $input_sam.QScomp.mem
printf "OK\n"

printf "Constructing new SAM file with QScomp'd quality values ... "
$python $replace_qual_sam_py $input_sam $input_sam.QScomp.dim1_rc
mv $input_sam.new_qual.sam $input_sam.QScomp.sam
printf "OK\n"

###############################################################################
#                                 Statistics                                  #
###############################################################################

$bzip2 -9 -c $input_sam.qual.dim1 > $input_sam.qual.dim1.bz2
printf "$input_sam.qual.dim.bz2 size: " > $input_sam.QScomp.stats
wc -c $input_sam.qual.dim1.bz2 >> $input_sam.QScomp.stats

for f in $input_sam.qual.dim2.*; do
    $bzip2 -9 -c $f > $f.bz2
    printf "$f.bz2 size: " >> $input_sam.QScomp.stats
    wc -c $f.bz2 >> $input_sam.QScomp.stats;
done

###############################################################################
#                                   Cleanup                                   #
###############################################################################

printf "Cleanup ... "
#
printf "OK\n";

