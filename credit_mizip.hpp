#pragma once

#include<cstdint>
#include<string>
#include<stdexcept>
#include<vector>
#include<cstring>
#include<iostream>
#include<iomanip>
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

#define BLOCK_CHAR_LEN 32

class credit_mizip {
    private:
        mizip_sec_2 price_sec;

        string block_0, block_1, block_2;

        /// @brief Converts a string of hex characters into a vector of bytes.
        /// @param hex The HEX string to convert
        /// @return The newly created vector
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

        /// @brief Prints the contents of the data buffer to stdout, in HEX format
        /// @param data Buffer containing the bytes to be printed
        /// @param len Number of bytes to print
        void print_hex(const uint8_t* data, size_t len) {
            for (int i=0;i<len;i++) {
                cout<<hex<<uppercase<<setw(2)<<setfill('0')<<(int)data[i];
            }
            cout<<dec<<"\n"; // reset back to decimal output
        }

    public:
        /// @brief Class constructor
        /// @param p_block_0 Hex bytes [0-15] of the MiZip key sector no. 2
        /// @param p_block_1 Hex bytes [16-31] of the MiZip key sector no. 2
        /// @param p_block_2 Hex bytes [32-47] of the MiZip key sector no. 2
        credit_mizip(const string& p_block_0, const string& p_block_1, const string& p_block_2)
            :block_0(p_block_0),block_1(p_block_1),block_2(p_block_2) 
        {
            if(block_0.length()!=BLOCK_CHAR_LEN||block_1.length()!=BLOCK_CHAR_LEN||block_2.length()!=BLOCK_CHAR_LEN){
                throw runtime_error("Blocks must contain 32-character hex strings (16 bytes).");
            }

            memcpy(&price_sec.block_0, hex_to_bytes(block_0).data(), sizeof(price_sec.block_0));
            memcpy(&price_sec.block_1, hex_to_bytes(block_1).data(), sizeof(price_sec.block_1));
            memcpy(&price_sec.block_2, hex_to_bytes(block_2).data(), sizeof(price_sec.block_2));
        }

        /// @brief Prints information (credit, last transactions, next transaction, transaction number, ecc.)
        void print_info(){
            cout<<"\n";
            cout<<"Credit\tOperation\tNext to edit\n";
            cout<<(float)price_sec.block_0.parts.price_val.whole/100<<"\t"<<(int)price_sec.block_0.parts.operation_no<<"\t\t"<<(price_sec.block_2.curr_byte==0x55)<<"\n";
            cout<<(float)price_sec.block_1.parts.price_val.whole/100<<"\t"<<(int)price_sec.block_1.parts.operation_no<<"\t\t"<<(price_sec.block_2.curr_byte==0xAA)<<"\n";
            cout<<"Last.op."<<(int)price_sec.block_2.curr_operation_no<<"\n";
            cout<<"New op.\t"<<(int)price_sec.block_2.curr_operation_no+1<<"\n\n";
        }

        /// @brief Prints the contents of the whole sector, in HEX, divided into three blocks of 16 bytes each
        void print_hex_contents(){
            print_hex((uint8_t*)&price_sec.block_0, 16);
            print_hex((uint8_t*)&price_sec.block_1, 16);
            print_hex((uint8_t*)&price_sec.block_2, 16);
        }

        /// @brief Updates the credit with the value provided, together with all the related fields in the sector. Should be between 0 and 655.35 to work properly.
        /// @param new_credit The new credit value to insert into the MiZip sector no. 2.
        /// @warning This method does not check for overflow.
        void update(float new_credit){
            price tmp;
            tmp.whole=(uint16_t)(new_credit*100);

            price_block* to_edit;
            if(price_sec.block_2.curr_byte==0x55){
                to_edit=&price_sec.block_0;
            }else{
                to_edit=&price_sec.block_1;
            }
            
            price_sec.block_2.curr_byte=~price_sec.block_2.curr_byte;
            price_sec.block_2.curr_operation_no++;

            to_edit->parts.operation_no=price_sec.block_2.curr_operation_no;
            to_edit->parts.price_val.whole=tmp.whole;
            to_edit->parts.price_xor=tmp.parts.hsbyte^tmp.parts.lsbyte;
        }

};