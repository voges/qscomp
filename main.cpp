//
//  main.cpp
//  QScompress
//
//  Created by Muhammed Oguzhan Kulekci on 10/03/2017.
//  Copyright © 2017 Muhammed Oguzhan Kulekci. All rights reserved.
//

#include <math.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
    std::string inputFileName(argv[1]);
    std::string outputFileName(inputFileName + ".k");
    std::ifstream ifs(inputFileName.c_str(), std::ifstream::in);
    std::ofstream ofs(outputFileName.c_str(), std::ofstream::trunc);

    char str[10000];
    char alteredScores[256];

    for (int i = 0; i < 256; i++) {
        alteredScores[i] = (char)pow(round(sqrt((double)i)), 2.0);
    }

    size_t linenumber = 0;

    while (ifs.getline(str, 10000)) {
        size_t a = linenumber % 4;

        switch (a) {
        case 0:
            ofs << str << std::endl;
            break;
        case 1:
            ofs << str << std::endl;
            break;
        case 2:
            ofs << str << std::endl;
            break;
        case 3:
            size_t l = strlen(str);
            for (size_t i = 0; i < l; i++) {
                str[i] = alteredScores[str[i]];
            }
            ofs << str << std::endl;
            break;
        }

        linenumber++;
    }

    std::cout << linenumber/4 << " reads from " << inputFileName;
    std::cout << " processed and written to " << std::string(argv[1]) + ".k";
    std::cout << std::endl;

    return 0;
}

