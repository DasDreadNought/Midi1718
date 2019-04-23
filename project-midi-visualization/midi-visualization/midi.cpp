#include "midi.h"
#include "read.h"
#include "vli.h"
#include "functions.h"
#include "endianness.h"
#include "bitmap.h"
#include <vector>
#include <iostream>
#include <algorithm>


bool read_header(std::istream& in, CHUNK_HEADER* head) {
	read(in, head);
	switch_endianness(&(head->size));

	return !in.eof();
}


std::string header_id(const CHUNK_HEADER& header) {
	std::string s(header.id, 4);
	return s;
}

bool read_mthd(std::istream& in, MThd* m) {
	

	if (!read_header(in, &(m->header))) {
		std::cout
			<< std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!  ERROR  !!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!  WITH READING  !!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< std::endl;
		return false;
	}

	if (header_id(m->header) != "MThd") {
		std::cout
			<< std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!  ERROR  !!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!  ID DOES NOT = MThD  !!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< std::endl;
		return false;
	}
	if (m->header.size != 6) {
		std::cout
			<< std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!  ERROR  !!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!! SIZE IS NOT 6 !!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< std::endl;
		return false;
	}

	read(in, &(m->type));
	switch_endianness(&(m->type));
	read(in, &(m->ntracks));
	switch_endianness(&(m->ntracks));
	read(in, &(m->division));
	switch_endianness(&(m->division));
	

	return !in.eof();
}

EventReceiver::EventReceiver() {};


NoteFilter::NoteFilter(int channel, std::vector<NOTE>* notes) : channel(channel), notes(std::move(notes)) {}

void NoteFilter::note_on(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) {

	if (velocity == 0) note_off(dt, channel, note, velocity);
	else {
		this->time += dt;
		if (channel == this->channel) noteArray[note] = time;
	}
};
void NoteFilter::note_off(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) {
	this->time += dt;
	if (channel == this->channel) notes->push_back(NOTE{ channel,note,noteArray[note],this->time - noteArray[note] });


};
void NoteFilter::polyphonic_key_pressure(uint32_t dt, uint8_t channel, uint8_t note, uint8_t pressure) { this->time += dt; };
void NoteFilter::control_change(uint32_t dt, uint8_t channel, uint8_t controller, uint8_t value) { this->time += dt; };
void NoteFilter::program_change(uint32_t dt, uint8_t channel, uint8_t program) { this->time += dt; };
void NoteFilter::channel_pressure(uint32_t dt, uint8_t channel, uint8_t pressure) { this->time += dt; };
void NoteFilter::pitch_wheel_change(uint32_t dt, uint8_t channel, uint16_t value) { this->time += dt; };
void NoteFilter::meta(uint32_t dt, uint8_t type, const char* data, int data_size) { this->time += dt; };
void NoteFilter::sysex(uint32_t dt, const char* data, int data_size) { this->time += dt; };

EventMulticaster::EventMulticaster(std::vector<std::shared_ptr<EventReceiver>>& note_filters) : note_filters(note_filters) {}

void EventMulticaster::note_on(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->note_on(dt, channel, note, velocity);
	}

};
void EventMulticaster::note_off(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->note_off(dt, channel, note, velocity);
	}
};
void EventMulticaster::polyphonic_key_pressure(uint32_t dt, uint8_t channel, uint8_t note, uint8_t pressure) {

	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->polyphonic_key_pressure(dt, channel, note, pressure);
	}
};
void EventMulticaster::control_change(uint32_t dt, uint8_t channel, uint8_t controller, uint8_t value) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->control_change(dt, channel, controller, value);
	}
};
void EventMulticaster::program_change(uint32_t dt, uint8_t channel, uint8_t program) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->program_change(dt, channel, program);
	}
};
void EventMulticaster::channel_pressure(uint32_t dt, uint8_t channel, uint8_t pressure) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->channel_pressure(dt, channel, pressure);
	}
};
void EventMulticaster::pitch_wheel_change(uint32_t dt, uint8_t channel, uint16_t value) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->pitch_wheel_change(dt, channel, value);
	}
};
void EventMulticaster::meta(uint32_t dt, uint8_t type, const char* data, int data_size) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->meta(dt, type, data, data_size);
	}
};
void EventMulticaster::sysex(uint32_t dt, const char* data, int data_size) {
	for (int i = 0; i < note_filters.size(); ++i) {
		note_filters.at(i)->sysex(dt, data, data_size);
	}
};

bool read_mtrk(std::istream& in, EventReceiver& e) {

	CHUNK_HEADER h;
	read_header(in, &h);


	bool end = false;

	//if (header_id(h) != "MTrk") return false;
	if (header_id(h) != "MTrk") {
		std::cout
			<< std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!  ERROR  !!!!!!!!!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!  ID DOES NOT = MTrk  !!!!!!!!!!!!!" << std::endl
			<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl
			<< std::endl;
		return false;
	}
	std::cout
		<< "Size: "
		<< h.size
		<< std::endl;

	
	//if (in.seekg(h.size - 2, std::ios::cur).peek() != 0x2F) return false;
	

	uint8_t prevId;
	int z = 0;
	while (!end) {
		uint32_t td = read_variable_length_integer(in);
		uint8_t id = ((in.peek() > 0x7F)) ? read_byte(in) : prevId;
		prevId = id;

		if (id == 0xFF) {
			uint8_t type = read_byte(in);
			uint8_t data_size = read_variable_length_integer(in);
			char *data = (char*)malloc(data_size); in.read(data, data_size);
			e.meta(td, type, data, data_size);
			if (type == 0x2F) {
				std::cout
					<< "END OF FILE"
					<< std::endl;
				end = true;
			}
		}
		else if (id == 0xF0) {
			uint8_t data_size = read_variable_length_integer(in);
			char *data = (char*)malloc(data_size); in.read(data, data_size);
			e.sysex(td, data, data_size);
		}
		else if ((id & 0xF0) == 0x80) {
			//std::cout << "#< NOTE OFF >#" << std::endl;
			uint8_t note = read_byte(in);
			uint8_t velocity = read_byte(in);
			e.note_off(td, (id & 0x0F), note, velocity);
		}
		else if ((id & 0xF0) == 0x90) {
			//std::cout << "#< NOTE ON >#" << std::endl;
			uint8_t note = read_byte(in);
			uint8_t velocity = read_byte(in);
			e.note_on(td, (id & 0x0F), note, velocity);
		}
		else if ((id & 0xF0) == 0xA0) {
			//std::cout << "#< Polyphonic >#" << std::endl;
			uint8_t note = read_byte(in);
			uint8_t pressure = read_byte(in);
			e.polyphonic_key_pressure(td, (id & 0x0F), note, pressure);
		}
		else if ((id & 0xF0) == 0xB0) {
			//std::cout << "#< CONTROL >#" << std::endl;
			uint8_t control = read_byte(in);
			uint8_t value = read_byte(in);
			e.control_change(td, (id & 0x0F), control, value);
		}
		else if ((id & 0xF0) == 0xC0) {
			//std::cout << "#< PROGRAM >#" << std::endl;
			uint8_t program = read_byte(in);
			e.program_change(td, (id & 0x0F), program);
		}
		else if ((id & 0xF0) == 0xD0) {
			//std::cout << "#< CHANNEL >#" << std::endl;
			uint8_t pressure = read_byte(in);
			e.channel_pressure(td, (id & 0x0F), pressure);
		}
		else if ((id & 0xF0) == 0xE0) {
			//std::cout << "#< PITCH >#" << std::endl;
			uint8_t value1 = read_byte(in);
			uint8_t value2 = read_byte(in);
			uint16_t value = (((value2 | 0x0000) << 8) | value1);
			e.pitch_wheel_change(td, (id & 0x0F), value);
		}

	}

	return true;


}

bool read_notes(std::istream& in, std::vector<NOTE>* notes) {
	printf("READING NOTES\n");

	MThd mthd;

	if (read_mthd(in, &mthd)) {

		std::cout << "header: (id: " << mthd.header.id << " - size: " << mthd.header.size <<
			") - type " << mthd.type <<
			" - ntracks " << mthd.ntracks <<
			" - division " << mthd.division << std::endl;



		for (int i = 0; i < mthd.ntracks; i++) {

			std::vector<std::shared_ptr<EventReceiver>> filters;
			for (int j = 0; j < 16; j++) {
				filters.push_back(std::make_shared<NoteFilter>(j, notes));
			}
			std::cout << std::endl << "=== MULICASTING FILTERS ===" << std::endl;

			//std::cout << "nTracks: " << int(mthd.ntracks) << std::endl;
			EventMulticaster e(filters);
			read_mtrk(in, e);



			std::cout
				<< "---- END " << i << std::endl
				<< "===##########################===" << std::endl;



		}
	}

	std::cout << "END: " << in.eof() << std::endl;
	return !in.eof();
}

void drawNote(Bitmap& bitmap, const NOTE& note) {
	int start = note.start;
	int end = note.duration;
	for (int i = start; i < (start+end); i++) {
		for (int j = (note.channel)*200; j < 200+200*note.channel; j++) {
			Position2D a(i, j);
			bitmap[a] = colors::green();
		}
	}
}

void draw_rectangle(Bitmap& bitmap, int left, int top, int width, int height)
{
	bitmap.clear(colors::red());
	NOTE a{ 1,1,50,50 };
	drawNote(bitmap, a);
}



Bitmap visualize(const std::vector<NOTE>& notes) {

	std::cout << "Size: " << notes.size() << std::endl;
	int end = 0, channel = 0;
	for (int i = 0; i < notes.size(); i++) {
		if (end < notes[i].end()) end = notes[i].end();
		if (channel < notes[i].channel) channel = notes[i].channel;
		std::cout
			<< "NOTE [ start: " << notes[i].start
			<< " - end: " << (notes[i].start + notes[i].duration)
			<< " - channel: " << notes[i].channel
			<< " - index: " << notes[i].note_index
			<< " ]" << std::endl;
	}

	Bitmap bmap(end, channel*200);

	for (int i = 0; i < notes.size(); i++) {
		drawNote(bmap, notes.at(i));
	}
	

	return bmap;
}




