

#include <iostream>
#include "direct_mapped_dict.hh"
#include <string>
#include <cassert>
#include <sstream>

void test_code_of(){
	
	LZW_Encode_Dictionary f;

	f.add_string("A", 1);
	f.add_string("AT", 2);
	f.add_string("ACT", 3);


	assert(f.code_of("A", 1) == 1);
	assert(f.code_of("AT", 2) == 2);
	assert(f.code_of("ACT", 3) == 3);


}


void test_find_longest_in_dict(){
	LZW_Encode_Dictionary f;

	f.add_string("A", 1);
	f.add_string("AC", 2);
	f.add_string("ACT", 3);


	std::string input = "ACTG";


	int ret ;
	ret = f.find_longest_in_dict(input.c_str(), 0, 4);
	std::string output = input.substr(0,ret);
	assert(output == "ACT");
	assert(f.code_of(output, ret) == 3);

}
void test_add_string_encode(){
	LZW_Encode_Dictionary f;

	f.add_string("ACTGC", 1);

	assert(f.code_of("ACTGC", 5) == 1);
}

void test_encode(){
	test_add_string_encode();
	test_code_of();
	test_find_longest_in_dict();
}


void test_add_string_decode(){
	LZW_Decode_Dictionary f;
	f.add_string("ACTGC", 1);
	assert(f.str_of(1) == "ACTGC");
}

void test_str_of(){
	LZW_Decode_Dictionary f;

	f.add_string("A", 1);
	f.add_string("AT", 2);
	f.add_string("ACT", 3);


	assert(f.str_of(1) == "A");
	assert(f.str_of(2) == "AT");
	assert(f.str_of(3) == "ACT");



}

void test_decode(){
	test_add_string_decode();
	test_str_of();
}


// test for codeword_helper

void test_codeword_helper(){

	Codeword_Helper codeword_helper;

	// check that it is set up correctly
	assert(codeword_helper.EOF_CODEWORD == 5);
	assert(codeword_helper.current_codeword = 6);

	// check that helper increments correctly
	for(int i = codeword_helper.current_codeword; i < (1<<codeword_helper.bits_per_codeword); i++){
		assert(i == codeword_helper.get_next_codeword());
	}
	// make sure we didn't go over max
	assert(codeword_helper.get_next_codeword() == (1<<codeword_helper.bits_per_codeword) -1);
}

int main(){
	test_encode();
	test_decode();
	test_codeword_helper();
	return 0;
}
