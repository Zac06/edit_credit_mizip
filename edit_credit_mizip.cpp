#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<cstdint>
#include<vector>
#include<cstring>
using namespace std;

#include "credit_mizip.hpp"


int main(){
    string block_0, block_1, block_2;

    cout<<"Insert [sector 2][block 0]: ";
    getline(cin, block_0);

    cout<<"Insert [sector 2][block 1]: ";
    getline(cin, block_1);

    cout<<"Insert [sector 2][block 2]: ";
    getline(cin, block_2);

    credit_mizip cm(block_0, block_1, block_2);
    float tmp;
    do{
        cout<<"Insert a new credit (max. 655.35): ";
        cin>>tmp;

        if(tmp<0||tmp>655.35){
            cout<<"Credit must be positive and less than the 16-bit integer overflow limit.\n";
        }
    }while(tmp<0||tmp>655.35);
    cout<<"\n";

    cm.update(tmp);

    cout<<"Here is the new modified [sector 2].\nUse a tool such as MCT to write the sector onto the MiZip key.\n\n";
    cm.print_hex_contents();
    cout<<"\nGoodbye!\n";

    return 0;
}