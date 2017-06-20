# QScomp

**Q**uality **S**core **comp**ression

---

This is the official repository for the development of the QScomp software. It is hosted at GitHub (https://github.com/voges/QScomp) and Bitbucket (https://bitbucket.org/voges/qscomp).

## Build instructions

QScomp has been tested on the following systems:

* openSUSE Leap 42.1 with GCC 4.8.5
* openSUSE Tumbleweed 20170308 with GCC 6.3.1
* macOS Sierra (version 10.12.3) with Apple LLVM (i.e., Clang) version 8.0.0.

Clone the QScomp repository with either

    git clone https://github.com/voges/QScomp.git

or

    git clone https://bitbucket.org/voges/qscomp.git

Build the executable from the command line with the following commands; alternatively use the CMake GUI.

    cd QScomp
    mkdir build
    cd build
    cmake ..
    make

This generates a QScomp executable named ``QScomp`` in the ``build`` folder.

## Usage examples

QScomp compresses quality scores extracted from e.g. a FASTQ, SAM, or BAM file.

The quality scores can e.g. be extracted from a SAM file with the Python script ``xtract_qual_sam.py`` from the **ngstools** repository (see https://github.com/voges/ngstools or https://bitbucket.org/voges/ngstools).

    python xtract_qual_sam.py file.sam 2> file.qual

Compression of the quality scores can then be performed with the following commands.

    QScomp file.qual
    bzip2 -9 -c file.qual.dim1 > file.qual.dim1.bz2
    for f in file.qual.dim2.*; do
        bzip2 -9 -c $f > $f.bz2
    done

QScomp produces the file ``file.qual.dim1`` which contains the lossy representation of the quality scores. The file ``file.qual.dim1_rc`` contains the reconstructed quality scores. The files ``file.qual.dim2.*`` contain the necessary information for the lossless reconstruction of the quality scores. The file ``file.qual.dim2_a`` contains all quality scores residues (i.e., all data from the files ``file.qual.dim2.*``).

Finally, a SAM file containing the reconstructed quality scores can be produced with the Python script ``replace_qual_sam.py`` from the **ngstools** repository (see https://github.com/voges/ngstools or https://bitbucket.org/voges/ngstools).

    python replace_qual_sam.py file.sam file.qual.dim1_rc

This produces a new SAM file ``file.sam.new_qual.sam`` which contains the reconstructed quality scores.

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

