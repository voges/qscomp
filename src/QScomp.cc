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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define DIM2_MIN 0
#define DIM2_MAX 11
#define DIM2_RANGE (DIM2_MAX - DIM2_MIN + 1)

// Maximum quality score line length
#define LINE_LEN_MAX 16384

// Quality scores are printable chars and thus in the range [33,126]
#define QS_MIN 33
#define QS_MAX 126
#define QS_RANGE (QS_MAX - QS_MIN + 1)

bool file_exists(const std::string &path)
{
    if (path.empty()) return false;
    std::ifstream ifs(path);
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

std::streampos file_size(const std::string &path)
{
    std::ifstream ifs(path, std::ios::binary);
    std::streampos beg = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    return ifs.tellg() - beg;
}

int main(int argc, const char * argv[])
{
    try {
        if (argc != 2) {
            throw std::exception("Usage: QScomp input.qual");
        }

        // in file
        std::string inputFileName(argv[1]);
        std::cout << "Opening input file " << inputFileName << std::endl;
        if (!file_exists(inputFileName)) {
            throw std::exception("Input file does not exist");
        }
        std::streampos in_size = file_size(inputFileName);
        std::cout << "Input file size: " << in_size << std::endl;
        std::ifstream in(inputFileName);

        // dim1 file
        std::string dim1FileName(inputFileName + ".dim1");
        std::cout << "Creating dim1 file " << dim1FileName << std::endl;
        if (file_exists(dim1FileName)) {
            throw std::exception("dim1 file already exists");
        }
        std::ofstream dim1(dim1FileName, std::ofstream::binary);

        // dim1_rc file
        std::string dim1_rcFileName(inputFileName + ".dim1_rc");
        std::cout << "Creating dim1_rc file " << dim1_rcFileName << std::endl;
        if (file_exists(dim1_rcFileName)) {
            throw std::exception("dim1_rc file already exists");
        }
        std::ofstream dim1_rc(dim1_rcFileName, std::ofstream::binary);

        // dim2 files
        std::vector<std::string> dim2FileNames;
        std::vector<std::ofstream> dim2;
        // The quality scores are printable chars smaller than 128. Thus largest a is actually 11 as 128 = 11*11 +7
        for (int i = DIM2_MIN; i < DIM2_MAX; i++) {
            std::string dim2FileName(inputFileName + ".dim2." + std::to_string(i));
            std::cout << "Creating dim2 file " << dim2FileName << std::endl;
            if (file_exists(dim2FileName)) {
                throw std::exception("dim2 file already exists");
            }
            dim2FileNames.push_back(dim2FileName);
            dim2.emplace_back(std::ofstream{ dim2FileName, std::ofstream::binary });
        }

        // Any QS between [a^2-a+1, a^2+a] is represented by qsDim1 = a and qsDim2 = QS - (a^2-a+1);
        // Thus, QS = qsDim1^2 - qsDim1 + 1 + qsDim2
        char qsDim1[QS_RANGE];
        char qsDim2[QS_RANGE];

        qsDim1[0] = 0;
        qsDim2[0] = 0;
        //std::cout << 0 << '\t' << (int)qsDim1[0] << '\t' << (int)qsDim2[0] << std::endl;

        for (int i = 1; i < QS_RANGE; i++) {
            char nearestSqrt = (char)round(sqrt((double)i));
            qsDim1[i] = nearestSqrt;

            // +1 is to make everything positive integer since sdsl/sca_wt construction does not accept 0 values in the sequence
            qsDim2[i] = i - (qsDim1[i] * qsDim1[i] - qsDim1[i] + 1) + 1;
            //std::cout << i << '\t'<< (int)qsDim1[i] << '\t' << (int)qsDim2[i] << std::endl;
        }

        char line[LINE_LEN_MAX];
        while (in.getline(line, LINE_LEN_MAX)) {
            size_t lineLen = strlen(line);

            for (int i = 0; i < lineLen; i++) {
                char qs = line[i];
                //std::cout << "QS=" << qs << "=" << (int)qs << '\t';

                char d1 = qsDim1[qs];
                dim1.write((const char *)&d1, 1);
                //std::cout << "dim1=" << (int)d1 << '\t';
                
                dim1_rc << (char)(d1 * d1);
                //std::cout << "dim1_rc=" << (char)(d1 * d1) << "=" << (int)(d1 * d1) << '\t';
                
                char d2 = qsDim2[qs];
                dim2[d1].write((const char *)&d2, 1);
                //std::cout << "dim2[" << (int)d1 << "]=" << (int)d2 << std::endl;
            }

            dim1_rc << '\n';

            std::cout << "Processed " << (100 * (double)in.tellg() / (double)in_size) << "%" << std::endl;
        }

        // Remove empty dim2 files
        for (auto const &dim2FileName : dim2FileNames) {
            if (file_isempty(dim2FileName)) {
                std::cout << "Removing empty dim2 file " << dim2FileName << std::endl;
                if (remove(dim2FileName.c_str()) != 0) {
                    perror("Could not delete file");
                }
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
