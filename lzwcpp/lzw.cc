#include "lzw.hh"

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <climits>
#include "./dictionary/direct_mapped_dict.hh"

namespace fs = std::filesystem;

void LZW::encode(const char* input_file, int file_size, std::ostream& output){
   
    // initialize starter dictionary
	Direct_Mapped_Encode_Dictionary<> dictionary;
    dictionary.add_string("A", 1);
    
    dictionary.add_string("T", 2);
    dictionary.add_string("C", 3);
    dictionary.add_string("G", 4);

	

    BitOutput bit_output(output);

    // the current codeword we are using, and the size of the codewords
    // each time we use a codeword we will have to increment so all codewords are unique
    // TODO: need to make sure our codewords don't go over the max size of codeword_type
    codeword_type codeword = STARTING_CODEWORD;
	int codeword_size = STARTING_CODE_SIZE; 
	int biggest_possible_codeword = (1<<STARTING_CODE_SIZE)-1;

    // the pieces of the file we are reading
    // current string seen is a string that we've seen before (it is in the dictionary), next_character is the following character that we are looking at
	

	char next_character;
	std::string new_string_seen;
	std::string current_string_seen;
	int index = 0;
	Direct_Mapped_Encode_Dictionary<>::Dict_Entry entry; 
	while(true)
	{

		entry = dictionary.find_longest_in_dict(input_file, index, file_size);
		current_string_seen = entry.str;
		index += current_string_seen.length();
		if(index >= file_size){
			break;


		}
		next_character = input_file[index];
		
			
		bit_output.output_n_bits(entry.codeword, codeword_size);
		bit_output.output_n_bits(static_cast<uint8_t>(next_character), CHAR_BIT);
		new_string_seen = current_string_seen + next_character;
		dictionary.add_string(new_string_seen, codeword);
		codeword+=1;
	   // increment the codword size if the current codeword becomes too large
        if (codeword >= biggest_possible_codeword){
			return;
            // codeword_size += 1;
            // biggest_possible_codeword<<= 1;
        }


		current_string_seen = "";
		index++;


	}
    // output special eof character
    bit_output.output_n_bits(EOF_CODEWORD, codeword_size);

    // after we've encoded, we either have 
    // no current block (case 0)
    // we have a current block that is a single character (case 1)
    // otherwise we have a current block > 1 byte (default)
    switch (current_string_seen.length()){
    case 0:
        bit_output.output_bit(false);
        bit_output.output_bit(false);
        break;
    case 1:
        bit_output.output_bit(false);
        bit_output.output_bit(true);
        bit_output.output_n_bits(static_cast<uint8_t>(current_string_seen[0]), CHAR_BIT);
        break;
    default:
        bit_output.output_bit(true);
        bit_output.output_bit(true);

        int code = entry.codeword;
        bit_output.output_n_bits(code, codeword_size);
        break;
    }
}



void LZW::decode(const char* input, std::ostream& output){

    // starting dictionary
    Direct_Mapped_Decode_Dictionary<> dictionary;
    
    dictionary.add_string("A", 1);
    
    dictionary.add_string("T", 2);
    dictionary.add_string("C", 3);
    dictionary.add_string("G", 4);


    int code_size = STARTING_CODE_SIZE;
    codeword_type codeword = STARTING_CODEWORD;
    int codeword_found;
    int biggest_possible_codeword = (1<<STARTING_CODE_SIZE) -1;
    char next_byte;
    BitInput bit_input(input);

    // assume the file isn't empty
    codeword_found = bit_input.read_n_bits(code_size);
    while(codeword_found!=EOF_CODEWORD){

        next_byte = char(bit_input.read_n_bits(CHAR_BIT));
		assert(next_byte == 'A' ||next_byte == 'C'||next_byte == 'T'||next_byte == 'G');

        // look up the codeword in the dictionary
		std::string decodedCodeword = dictionary.str_of(codeword_found);
		std::string new_string = decodedCodeword+ next_byte;
        
		
        // output what we had in the dictionary and the byte following
        output << new_string; 

        // add this new sequence to our dictionary   
        dictionary.add_string(new_string, codeword);
        codeword+=1;

        // increment the codeword size if needed
        if (codeword == biggest_possible_codeword){
			return;
            // code_size += 1;
            // biggest_possible_codeword <<= 1;
        }
        
        codeword_found = bit_input.read_n_bits(code_size);
    }
    
    // the two bits after eof are a code for the last portion of the encoded file
    // either there is an extra codeword(1), extra byte (3), or nothing left (0)
    int whats_left = bit_input.read_n_bits(2);
    switch(whats_left){
    case 1:
        output << char(bit_input.read_n_bits(CHAR_BIT));
        break;
    case 3:
        int last_codeword = bit_input.read_n_bits(code_size);
        auto decoded_codeword = dictionary.str_of(last_codeword);
        output << decoded_codeword; 
        break;
    }

}
