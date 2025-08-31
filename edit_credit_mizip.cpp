#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<cstdint>
#include<vector>
#include<cstring>
using namespace std;

#include "credit_mizip.hpp"


int main(int argc, char** argv){
    string block_0, block_1, block_2;
    float tmp;
    credit_mizip cm;

    if(argc!=1&&argc==5){
        block_0 = argv[1];
        block_1 = argv[2];
        block_2 = argv[3];
        try{
            tmp = stof(argv[4]);
        }catch(...){
            cerr<<"Error: Credit must be a (float) number.\n";
            return 1;
        }

        if(tmp<0||tmp>655.35){
            cerr<<"Credit must be positive and less than the 16-bit integer overflow limit.\n";
            return 1;
        }

        cm=credit_mizip(block_0, block_1, block_2);
    }else{
        cout<<"MIZIP Credit Editor\n";
        cout<<"====================\n\n";

        cout<<"Insert [sector 2][block 0]: ";
        getline(cin, block_0);

        cout<<"Insert [sector 2][block 1]: ";
        getline(cin, block_1);

        cout<<"Insert [sector 2][block 2]: ";
        getline(cin, block_2);

        cm=credit_mizip(block_0, block_1, block_2);
        do{
            cout<<"Insert a new credit (max. 655.35): ";
            cin>>tmp;

            if(tmp<0||tmp>655.35){
                cerr<<"Credit must be positive and less than the 16-bit integer overflow limit.\n";
            }
        }while(tmp<0||tmp>655.35);
        cout<<"\n";

        cout<<"Here is the new modified [sector 2].\nUse a tool such as MCT to write the sector onto the MiZip key.\n\n";
    }

    cm.update(tmp);
    cm.print_hex_contents();

    return 0;
}