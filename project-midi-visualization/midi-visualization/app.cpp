#include "settings.h"

/*
    If you want your main()-function to be run, go to settings.h.
*/

#ifndef TEST_BUILD

#include <iostream>
#include <fstream>
#include "midi.h"
#include "bitmap.h"
#include <vector>


void show_help()
{
	printf("Please add a file input as second argument\n");
	printf("	--->  Midi.cpp [INPUT] ([OUTPUT])\n");
	printf("	--->  [INPUT] : input a midi stream file\n");
	printf("	--->  [OUTPUT] : expected output bitmap file. not required. debug mode if absent\n");
}

void process_midi_file(const std::string& path)
{
	std::cerr << "Opening file " << path << std::endl;
	std::ifstream in(path, std::ios::binary);
	
	if (in) {
		std::vector<NOTE> notes;
		read_notes(in, &notes);
		printf("VISUALIZING NOTES\n");
		Bitmap bit = visualize(notes);
		bit.save(R"(c:/temp/output.bmp)");
	}
	else {
		printf("DID NOT OPEN FILE\n");
	}
}

int main(int argc, char** argv)
{
    std::cout << "Hello world!" << std::endl;

	if (argc <= 1 || argc > 3) {
		process_midi_file(R"(C:\Users\Oli\Desktop\UCLL 18-19\PVM\Midi 2017-18\project-midi-visualization\midi-files\harmonies.mid)");
	}
	else if (argc == 2) {
		process_midi_file(argv[1]);
	}
	else if (argc == 3) {
		if (*argv[2] == 'T') {
			std::ifstream in(argv[1], std::ios_base::binary);
			uint8_t x =0xff;
			while (!in.eof()) {
				std::cout << (int)in.get();
			}
			std::cout << std::endl <<(int)x<< std::endl;
		}
		printf("Starting Midi translations\n");

	}


}

#endif
