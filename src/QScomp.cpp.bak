//
//  main.cpp
//  QScompress
//
//  Created by muhammed oguzhan kulekci on 10/03/2017.
//  Copyright © 2017 muhammed oguzhan kulekci. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>

#define MAXDIM2FILE 13
#define MAXFILENAMELENGTH 1024
#define MAXQS 128 //QS values are printable chars
#define MAXDIM1 13 // maximum value of dim1 is 11
#define DIM2BFRLENGTH 65536 // buffering for better file I/O
#define LINEBFRLENGTH 16384

using namespace std;

int main(int argc, const char * argv[]) {

    ifstream in(argv[1]);

    ofstream out;
    char outname[MAXFILENAMELENGTH]={0};
    strcat(outname, argv[1]);
    strcat(outname, ".lossy");
    out.open (outname,ios::trunc);
    
    ofstream outorigQS;
    char outnameorigQS[MAXFILENAMELENGTH]={0};
    strcat(outnameorigQS, argv[1]);
    strcat(outnameorigQS, ".origQS");
    outorigQS.open (outnameorigQS,ios::trunc);

    ofstream outQSdim1;
    char outname2[MAXFILENAMELENGTH]={0};
    sprintf(outname2,"%s.qs.dim1",argv[1]);
    outQSdim1.open (outname2,ios::trunc | ios::binary);

    ofstream outQSdim2[MAXDIM2FILE];// the quality scores are printable chars smaller than  128. Thus largest a is actually 11 as 128 = 11*11 +7
    for(int i=0;i<MAXDIM2FILE;i++){
        char outname3[MAXFILENAMELENGTH]={0};
        sprintf(outname3,"%s.qs.dim2.%d",argv[1],i);
        outQSdim2[i].open(outname3,ios::trunc | ios::binary);
    }
    
    char dim2bfr[MAXDIM2FILE][DIM2BFRLENGTH];
    unsigned int dim2bfrptr[MAXDIM2FILE];
    for(int i=0;i<MAXDIM2FILE;i++) dim2bfrptr[i] = 0;

    char qsDim1[MAXQS];// any QS value between <a^2-a+1, a^2+a> is represented by qsDim1=a and qsDim2 = QS - (a^2-a+1);
    char qsDim2[MAXQS];// thus QS = qsDim1^2 - qsDim1 +1 + qsDim2;

    qsDim1[0]=0;
    qsDim2[0]=0;
    for(int i=1;i<MAXQS;i++){
         char nearest_sqrt = (char)round(sqrt((double)i));
         qsDim1[i]= nearest_sqrt;
         qsDim2[i]= i - (qsDim1[i]*qsDim1[i]-qsDim1[i]+1) + 1;// +1 is to make everything positive integer since sdsl/sca_wt construction does not accept 0 values in the sequence
         //cout << i << '\t'<< (int) qsDim1[i] << '\t' << (int) qsDim2[i] << endl;
    }

    unsigned long int qsDim1_stat[MAXDIM1];
    for(int i=0;i<MAXDIM1;i++) qsDim1_stat[i]=0;

    unsigned long int* qsDim2_stat[MAXDIM1];
    qsDim2_stat[0]=new unsigned long[1];
    qsDim2_stat[0][0]=0;
    for(int i=1;i<MAXDIM1;i++){
        qsDim2_stat[i]= new unsigned long int[2*i];
        for (int j=0;j<2*i;j++) qsDim2_stat[i][j]=0;
    }


    char str[LINEBFRLENGTH];
    char strdim2[LINEBFRLENGTH];
    unsigned long linenumber=0;

    while(in) {
        in.getline(str, LINEBFRLENGTH);  // delim defaults to '\n'
        if(in){
            int a = linenumber % 4;
            switch (a){
                case 0: // label that should start with @ symbol
                    out << str << endl;
                    break;
                case 1: // ATCG sequence
                    out << str << endl;
                    break;
                case 2:// should be +
                    out << str << endl;
                    break;
                case 3: // quality scores
                    outorigQS << str; //store the original QS on a seperate file
                    unsigned long l = strlen(str);
                    for(unsigned i=0;i<l;i++){

                        char dim1 = qsDim1[str[i]];
                        char dim2 = qsDim2[str[i]];

                        qsDim1_stat[dim1]++;
                        qsDim2_stat[dim1][dim2-1]++; // remember we have made dim2  always 1 larger than original value due to sdsl/csa_wt compatibility before

                        str[i] = dim1*dim1;
                        strdim2[i]=dim2;

                        dim2bfr[dim1][dim2bfrptr[dim1]]=dim2;
                        dim2bfrptr[dim1]++;
                        if (dim2bfrptr[dim1]==65536){
                            outQSdim2[dim1].write(dim2bfr[dim1],65536);
                            dim2bfrptr[dim1]=0;
                        }

                    }
                    out << str << endl;
                    outQSdim1.write(str,l);
                    break;
            }
            linenumber++;
        }
    }

    out.close();
    outorigQS.close();
    in.close();
    outQSdim1.close();
    
    for(int i=0;i<MAXDIM2FILE;i++){
        outQSdim2[i].write(dim2bfr[i],dim2bfrptr[i]);
        outQSdim2[i].close();
    }

    cout << linenumber/4 << " reads from " << argv[1] << " processed and written into " << outname << endl;

    // print stats of the dim1 and dim2 values
    unsigned long dim2total[32];
    for(int i=0;i<32;i++) dim2total[i]=0;
    for(int i=0;i<16;i++){
        if (qsDim1_stat[i]>0){
            cout << i << '\t' << qsDim1_stat[i] << '\t';
            for(int j=0;j<2*i;j++){
                cout << qsDim2_stat[i][j] << '\t';
                dim2total[j] += qsDim2_stat[i][j];
            }
            cout << endl << endl;
        }
    }
    cout << "\t\t";
    for(int i=0;i<32;i++) cout << dim2total[i] << '\t';
    cout << endl;
    //**************************************************************
    

    //remove empty dim2.X files
    for(int i=0;i<MAXDIM2FILE;i++){
        char outname3[MAXFILENAMELENGTH]={0};
        sprintf(outname3,"%s.qs.dim2.%d",argv[1],i);
        ifstream file(outname3);
        if (file.peek() == std::ifstream::traits_type::eof())
            remove(outname3);
    }
    

    
    return 0;
}
