# QScomp

**Q**uality **S**core **comp**ression

---

## Build instructions

Build the executable from the command line with the following commands.

    mkdir build
    cd build
    cmake ..
    make

This generates a QScomp executable named ``qscomp`` in the ``build`` folder.

## Usage examples

QScomp compresses quality scores extracted from e.g. a FASTQ, SAM, or BAM file.

The quality scores can e.g. be extracted from a SAM file with the Python script ``xtract_field_sam.py``. This and other supplementary scripts can be found in the folder ``scripts``.

    python xtract_field_sam.py file.sam 10 1> file.qual

Compression of the quality scores can then be performed with the following commands.

    qscomp file.qual
    bzip2 -9 -c file.qual.dim1 > file.qual.dim1.bz2
    for f in file.qual.dim2.*; do
        bzip2 -9 -c $f > $f.bz2
    done

QScomp produces the file ``file.qual.dim1`` which contains the lossy representation of the quality scores. The file ``file.qual.dim1_rc`` contains the reconstructed quality scores. The files ``file.qual.dim2.*`` contain the necessary information for the lossless reconstruction of the quality scores. The file ``file.qual.dim2_a`` contains all quality scores residues (i.e., all data from the files ``file.qual.dim2.*``).

Finally, a SAM file containing the reconstructed quality scores can be produced with the Python script ``replace_qual_sam.py``.

    python replace_qual_sam.py file.sam file.qual.dim1_rc 1> file.recon_qual.sam

This produces a new SAM file ``file.recon_qual.sam`` which contains the reconstructed quality scores.

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>
