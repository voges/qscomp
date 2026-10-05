/** @file QScomp.cc
 *  @brief This file contains the QScomp source code.
 *  @author Jan Voges
 *  @author Muhammed Oguzhan Kulekci
 */

//
// Algorithm description:
// ----------------------
// dim1:      nearest sqrt base
//            (e.g. qs=34 -> nearest_qs=36 -> dim1=6)
// dim1_rc:   reconstructed sqrt bases
//            (e.g. qs=34 -> nearest_qs=reconstructed_qs=36)
// dim2.x:    positions of original QSs in the list of ordered QSs belonging
//            to dim1=x
// dim2_a:    all positions of original QSs in all lists of ordered QSs
//
// dim1^2 = nearest_qs
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
// We recommend compression of the *dim1 and the *dim2.x files with bzip2 -9.
//
// Lossy compression ratio:
//     CR_lossy = size(original) / size(dim1.bz2)
// Lossless compression ratio:
//     CR_lossless = size(original) / (size(dim1.bz2) + size(dim2.x.bz2))
//

#include <math.h>
#include <string.h>

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Maximum QS line length
#define LINE_LEN_MAX 16384

// QSs are printable chars and thus in the range [33,126]
#define QS_MIN 33
#define QS_MAX 126
#define QS_RANGE (QS_MAX - QS_MIN + 1)

// The largest dim1 value is 11 as 126 = 11*11 + 5
#define DIM2_MIN 0
#define DIM2_MAX 11
#define DIM2_RANGE (DIM2_MAX - DIM2_MIN + 1)

bool file_exists(const std::string &path) {
    std::ifstream ifs(path);
    return ifs.good();
}

std::streampos file_size(const std::string &path) {
    std::ifstream ifs(path, std::ios::binary);
    std::streampos beg = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    return ifs.tellg() - beg;
}

int main(int argc, const char * argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error("Usage: QScomp input.qual");
        }

        // in file
        std::string inputFileName(argv[1]);
        std::cout << "Opening input file " << inputFileName << std::endl;
        if (!file_exists(inputFileName)) {
            throw std::runtime_error("Input file does not exist");
        }
        double inputFileSize = static_cast<double>(file_size(inputFileName));
        std::cout << "Input file size: " << inputFileSize << std::endl;
        std::ifstream in(inputFileName);

        // dim1 file
        std::string dim1FileName(inputFileName + ".dim1");
        std::cout << "Creating dim1 file " << dim1FileName << std::endl;
        if (file_exists(dim1FileName)) {
            throw std::runtime_error("dim1 file already exists");
        }
        std::ofstream dim1(dim1FileName, std::ofstream::binary);

        // dim1_rc file
        std::string dim1_rcFileName(inputFileName + ".dim1_rc");
        std::cout << "Creating dim1_rc file " << dim1_rcFileName << std::endl;
        if (file_exists(dim1_rcFileName)) {
            throw std::runtime_error("dim1_rc file already exists");
        }
        std::ofstream dim1_rc(dim1_rcFileName, std::ofstream::binary);

        // dim2 files
        std::vector<std::string> dim2FileNames;
        std::ofstream dim2[DIM2_RANGE];
        for (int i = 0; i < DIM2_RANGE; i++) {
            std::string dim2FileName(inputFileName + ".dim2."
                                     + std::to_string(i));
            std::cout << "Creating dim2 file " << dim2FileName << std::endl;
            if (file_exists(dim2FileName)) {
                throw std::runtime_error("dim2 file already exists");
            }
            dim2[i].open(dim2FileName, std::ofstream::binary);
            dim2FileNames.push_back(dim2FileName);
        }

        // dim2_a file
        std::string dim2_aFileName(inputFileName + ".dim2_a");
        std::cout << "Creating dim2_a file " << dim2_aFileName << std::endl;
        if (file_exists(dim2_aFileName)) {
            throw std::runtime_error("dim2_a file already exists");
        }
        std::ofstream dim2_a(dim2_aFileName, std::ofstream::binary);

        // Any QS between [a^2-a+1, a^2+a] is represented by qsDim1 = a and
        // qsDim2 = QS - (a^2-a+1) .
        // Thus, QS = qsDim1^2 - qsDim1 + 1 + qsDim2
        char qsDim1[QS_RANGE];
        char qsDim2[QS_RANGE];

        qsDim1[0] = 0;
        qsDim2[0] = 0;

        for (int i = 1; i < QS_RANGE; i++) {
            double nearestSqrt = round(sqrt(static_cast<double>(i)));
            qsDim1[i] = static_cast<char>(nearestSqrt);

            // +1 is to make everything positive integer
            qsDim2[i] = i - (qsDim1[i] * qsDim1[i] - qsDim1[i] + 1) + 1;
        }

        char line[LINE_LEN_MAX];
        size_t lineCnt = 0;

        while (in.getline(line, LINE_LEN_MAX)) {
            size_t lineLen = strlen(line);

            for (size_t i = 0; i < lineLen; i++) {
                int qs = static_cast<int>(line[static_cast<int>(i)]);

                char d1 = qsDim1[qs];
                dim1.write((const char *)&d1, 1);

                dim1_rc << static_cast<char>(d1 * d1);

                char d2 = qsDim2[qs];
                dim2[static_cast<int>(d1)].write((const char *)&d2, 1);
                dim2_a.write((const char *)&d2, 1);
            }

            dim1_rc << '\n';

            lineCnt++;
            if (lineCnt % 100000 == 0) {
                double currFilePos = static_cast<double>(in.tellg());
                double processed = 100 * currFilePos / inputFileSize;
                std::cout << "Processed " << processed << "%" << std::endl;
            }
        }

        // Close all files
        in.close();
        dim1.close();
        dim1_rc.close();
        for (int i = DIM2_MIN; i <= DIM2_MAX; i++) {
            dim2[i].close();
        }
        dim2_a.close();

        // Remove empty dim2 files
        for (auto const &dim2FileName : dim2FileNames) {
            if ((size_t)file_size(dim2FileName) == 0) {
                std::cout << "Removing empty dim2 file ";
                std::cout  << dim2FileName << std::endl;
                if (remove(dim2FileName.c_str()) != 0) {
                    perror("Could not delete file");
                }
            }
        }
    }
    catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unkown error occured" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
