//
//  QScomp.cc
//  QScomp
//
//  Created by Muhammed Oguzhan Kulekci on 2017-03-10.
//  Copyright © 2017 Muhammed Oguzhan Kulekci. All rights reserved.
//

// Algorithm description:
// ----------------------
// orig: original quality values
// dim1: nearest sqrt base (e.g. qv=34 -> nearest_qv=36 -> dim1=6)
// dim1_rc: reconstructed sqrt bases (e.g. qv=34 -> nearest_qv=reconstructed_qv=36)
// dim2.x: position of original QV in the list of ordered QVs belonging to dim1=x
//
// dim1^2 = nearest_qv
//  0   ^2    =   0
//  1   ^2    =   1
//  2   ^2    =   4
//  4   ^2    =   8
//  5   ^2    =  16
//  6   ^2    =  36
//  7   ^2    =  49
//  8   ^2    =  64
//  9   ^2    =  81
// 10   ^2    = 100
// 11   ^2    = 121
// 12   ^2    = 144
//
// We recommend compression of the *dim1 and *dim2.x files with bzip2 -9.
//
// Lossy compression ratio: CR_lossy = 8 * size(original) / size(dim1.bz2)
// Lossless compression ratio: CR_lossless = CR_lossy + size(dim2.x.bz2)
//
// Build on the Windows Developer Command Line with:
//   cl /EHsc /W4 main2.cpp /link /out:qscomp2
//

//#include <math.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//#define MAXDIM2FILE 13
//#define MAXFILENAMELENGTH 1024
//#define MAXQS 128 //QS values are printable chars
//#define MAXDIM1 13 // maximum value of dim1 is 11
//#define DIM2BFRLENGTH 65536 // buffering for better file I/O
//#define LINEBFRLENGTH 16384

#define DIM2_MIN 0
#define DIM2_MAX 13
#define DIM2_RANGE (DIM2_MAX - DIM2_MIN + 1)
#define LINE_LENGTH_MAX 16384
#define QS_MIN 1
#define QS_MAX 128
#define QS_RANGE (QS_MAX - QS_MIN + 1)

bool file_exists(const std::string &path)
{
    if (path.empty()) return false;
    std::ifstream ifs(path.c_str());
    return ifs.good();
}

bool file_isempty(const std::string &path)
{
    if (path.empty()) return false;
    std::ifstream ifs(path);
    if (ifs.peek() == std::ifstream::traits_type::eof())
        return true;
    return false;
}

int main(int argc, const char * argv[])
{
    try {
        if (argc != 2) {
            throw std::exception("Usage: QScomp input.qual");
        }

        // Input file
        std::string inputFileName(argv[1]);
        std::cout << "Opening input file " << inputFileName << std::endl;
        if (!file_exists(inputFileName)) {
            throw std::exception("Input file does not exist");
        }
        std::ifstream in(inputFileName);

        // dim1 file
        std::string dim1FileName(inputFileName + ".dim1");
        std::cout << "Opening dim1 file " << dim1FileName << std::endl;
        if (file_exists(dim1FileName)) {
            throw std::exception("dim1 file already exists");
        }
        std::ofstream dim1(dim1FileName);

        // dim1_rc file
        std::string dim1_rcFileName(inputFileName + ".dim1_rc");
        std::cout << "Opening dim1_rc file " << dim1_rcFileName << std::endl;
        if (file_exists(dim1_rcFileName)) {
            throw std::exception("dim1_rc file already exists");
        }
        std::ofstream dim1_rc(dim1_rcFileName);

        // dim2 files
        std::vector<std::string> dim2FileNames;
        std::vector<std::ofstream> dim2;
        // The quality scores are printable chars smaller than 128. Thus largest a is actually 11 as 128 = 11*11 +7
        for (int i = DIM2_MIN; i < DIM2_MAX; i++) {
            std::string dim2FileName(inputFileName + ".dim2." + std::to_string(i));
            std::cout << "Opening dim2 file " << dim2FileName << std::endl;
            if (file_exists(dim2FileName)) {
                throw std::exception("dim2 file already exists");
            }
            dim2FileNames.push_back(dim2FileName);
            dim2.emplace_back(std::ofstream{ dim2FileName });
        }

        //char dim2bfr[MAXDIM2FILE][DIM2BFRLENGTH];
        //unsigned int dim2bfrptr[MAXDIM2FILE];
        //for (int i = 0; i<MAXDIM2FILE; i++) dim2bfrptr[i] = 0;

        //char qsDim1[MAXQS];// any QS value between <a^2-a+1, a^2+a> is represented by qsDim1=a and qsDim2 = QS - (a^2-a+1);
        //char qsDim2[MAXQS];// thus QS = qsDim1^2 - qsDim1 +1 + qsDim2;


        char qsDim1[QS_RANGE];

        for (int i = QS_MIN; i < QS_MAX; i++) {
            char nearestSqrt = (char)round(sqrt((double)i));
            qsDim1[i] = nearestSqrt;
        }

        //qsDim1[0] = 0;
        //qsDim2[0] = 0;
        //for (int i = 1; i<MAXQS; i++) {
        //    char nearest_sqrt = (char)round(sqrt((double)i));
        //    qsDim1[i] = nearest_sqrt;
        //    qsDim2[i] = i - (qsDim1[i] * qsDim1[i] - qsDim1[i] + 1) + 1;// +1 is to make everything positive integer since sdsl/sca_wt construction does not accept 0 values in the sequence
        //                                                                //cout << i << '\t'<< (int) qsDim1[i] << '\t' << (int) qsDim2[i] << endl;
        //}


        //unsigned long int qsDim1_stat[MAXDIM1];
        //for (int i = 0; i<MAXDIM1; i++) qsDim1_stat[i] = 0;

        //unsigned long int* qsDim2_stat[MAXDIM1];
        //qsDim2_stat[0] = new unsigned long[1];
        //qsDim2_stat[0][0] = 0;
        //for (int i = 1; i<MAXDIM1; i++) {
        //    qsDim2_stat[i] = new unsigned long int[2 * i];
        //    for (int j = 0; j<2 * i; j++) qsDim2_stat[i][j] = 0;
        //}


        //char str[LINEBFRLENGTH];
        //char strdim2[LINEBFRLENGTH];
        //unsigned long linenumber = 0;


        //char str[LINEBFRLENGTH];
        //char strdim2[LINEBFRLENGTH];
        //unsigned long linenumber = 0;


        char line[LINE_LENGTH_MAX];

        while (in.getline(line, LINE_LENGTH_MAX)) {
            if (strlen(line) != 0) {
                std::cout << line << std::endl;
                size_t lineLen = strlen(line);

                for (int i = 0; i < lineLen; i++) {
                    //char dim1 = qsDim1[str[i]];
                    //char dim1 = qsDim2[str[i]];
                }
            }
        }

        //for (int i = 0; i<MAXDIM2FILE; i++) {
        //    outQSdim2[i].write(dim2bfr[i], dim2bfrptr[i]);
        //    outQSdim2[i].close();
        //}

        // Remove empty dim2 files
        for (auto const &dim2FileName : dim2FileNames) {
            if (file_isempty(dim2FileName)) {
                std::cout << "Removing empty dim2 file " << dim2FileName << std::endl;
                remove(dim2FileName.c_str());
            }
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unkown error occured" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
