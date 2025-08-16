#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<cstdint>
#include<vector>
#include<cstring>
using namespace std;

union price {
    uint16_t whole;

    struct {
        uint8_t lsbyte;
        uint8_t hsbyte;
    } __attribute__((packed)) parts;
    
};

union price_block {
    struct {
        uint8_t reserved_1;
        price price_val;
        uint8_t price_xor;
        uint8_t reserved_2[11];
        uint8_t operation_no;
    } __attribute__((packed)) parts;
    
    uint8_t whole[16];
};

struct currents_block {
    uint8_t curr_byte;
    uint8_t curr_operation_no;
    uint8_t reserved[14];
}__attribute__((packed));

struct mizip_sec_2 {
    price_block block_0;
    price_block block_1;
    currents_block block_2;
} __attribute__((packed));

vector<uint8_t> hex_to_bytes(const string& hex) {
    if (hex.size()%2!=0)
        throw runtime_error("Invalid hex string length");

    vector<uint8_t> bytes;
    bytes.reserve(hex.size()/2);

    for (int i=0; i<hex.size();i+=2) {
        string byteString=hex.substr(i,2);
        uint8_t byte=static_cast<uint8_t>(stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

void print_hex(const uint8_t* data, size_t len) {
    for (int i=0;i<len;i++) {
        cout<<hex<<uppercase<<setw(2)<<setfill('0')<<(int)data[i];
    }
    cout<<dec<<"\n"; // reset back to decimal output
}

int main(){
    cout<<"price_block: "<<sizeof(price_block)<<"\n";
    cout<<"price: "<<sizeof(price)<<"\n";
    cout<<"currents_block: "<<sizeof(currents_block)<<"\n";

    string block_0, block_1, block_2;

    cout<<"Insert [sector 2][block 0]: ";
    getline(cin, block_0);

    cout<<"Insert [sector 2][block 1]: ";
    getline(cin, block_1);

    cout<<"Insert [sector 2][block 2]: ";
    getline(cin, block_2);

    
    mizip_sec_2 price_sec;

    memcpy(&price_sec.block_0, hex_to_bytes(block_0).data(), sizeof(price_sec.block_0));
    memcpy(&price_sec.block_1, hex_to_bytes(block_1).data(), sizeof(price_sec.block_1));
    memcpy(&price_sec.block_2, hex_to_bytes(block_2).data(), sizeof(price_sec.block_2));

    cout<<"\n";
    cout<<"Credit\tOperation\tNext to edit\n";
    cout<<(float)price_sec.block_0.parts.price_val.whole/100<<"\t"<<(int)price_sec.block_0.parts.operation_no<<"\t\t"<<(price_sec.block_2.curr_byte==0x55)<<"\n";
    cout<<(float)price_sec.block_1.parts.price_val.whole/100<<"\t"<<(int)price_sec.block_1.parts.operation_no<<"\t\t"<<(price_sec.block_2.curr_byte==0xAA)<<"\n";
    cout<<"Last.op."<<(int)price_sec.block_2.curr_operation_no<<"\n";
    cout<<"New op.\t"<<(int)price_sec.block_2.curr_operation_no+1<<"\n\n";

    price new_credit;
    float tmp;
    do{
        cout<<"Insert a new credit (max. 655.35): ";
        cin>>tmp;

        if(tmp<0||tmp>655.35){
            cout<<"Credit must be positive and less than the 16-bit integer overflow limit.\n";
        }
    }while(tmp<0||tmp>655.35);
    cout<<"\n";

    new_credit.whole=(uint16_t)(tmp*100);
    
    price_block* to_edit;
    if(price_sec.block_2.curr_byte==0x55){
        to_edit=&price_sec.block_0;
    }else{
        to_edit=&price_sec.block_1;
    }
    
    price_sec.block_2.curr_byte=~price_sec.block_2.curr_byte;
    price_sec.block_2.curr_operation_no++;

    to_edit->parts.operation_no=price_sec.block_2.curr_operation_no;
    to_edit->parts.price_val.whole=new_credit.whole;
    to_edit->parts.price_xor=new_credit.parts.hsbyte^new_credit.parts.lsbyte;

    cout<<"Here is the new modified [sector 2].\nUse a tool such as MCT to write the sector onto the MiZip key.\n\n";

    print_hex((uint8_t*)&price_sec.block_0, 16);
    print_hex((uint8_t*)&price_sec.block_1, 16);
    print_hex((uint8_t*)&price_sec.block_2, 16);

    cout<<"\nGoodbye!\n";

    return 0;
}