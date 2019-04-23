#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include "functions.h"
#include "endianness.h"
#include "bitmap.h"


struct CHUNK_HEADER
{
	char id[4];
	uint32_t size;
};

#pragma pack(push,1)
struct MThd
{
	CHUNK_HEADER header;
	uint16_t type;
	uint16_t ntracks;
	uint16_t division;
};
#pragma pack(pop)

struct NOTE {
	uint8_t channel;
	uint8_t note_index;
	uint32_t start;
	uint32_t duration;

	friend bool operator ==(const NOTE& one, const NOTE& other) {
		return (one.channel == other.channel && one.note_index == other.note_index && one.start == other.start && one.duration == other.duration);
	}
	friend bool operator !=(const NOTE& one, NOTE& other) {
		return !(one == other);
	}
	int end() const { return (int)(start + duration); }

};

bool read_header(std::istream& in, CHUNK_HEADER* head);

std::string header_id(const CHUNK_HEADER& header);

bool read_mthd(std::istream& in, MThd*);


class EventReceiver {
public:
	EventReceiver();
	virtual void note_on(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) = 0;
	virtual void note_off(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) = 0;
	virtual void polyphonic_key_pressure(uint32_t dt, uint8_t channel, uint8_t note, uint8_t pressure) = 0;
	virtual void control_change(uint32_t dt, uint8_t channel, uint8_t controller, uint8_t value) = 0;
	virtual void program_change(uint32_t dt, uint8_t channel, uint8_t program) = 0;
	virtual void channel_pressure(uint32_t dt, uint8_t channel, uint8_t pressure) = 0;
	virtual void pitch_wheel_change(uint32_t dt, uint8_t channel, uint16_t value) = 0;
	virtual void meta(uint32_t dt, uint8_t type, const char* data, int data_size) = 0;
	virtual void sysex(uint32_t dt, const char* data, int data_size) = 0;
};

bool read_mtrk(std::istream& in, EventReceiver&);

class NoteFilter : public EventReceiver {
public:
	uint32_t time = 0;
	uint8_t channel;
	std::vector<NOTE>* notes;
	uint32_t noteArray[128];
	NoteFilter(int channel, std::vector<NOTE>* notes);
	 void note_on(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) override;
	 void note_off(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) override;
	 void polyphonic_key_pressure(uint32_t dt, uint8_t channel, uint8_t note, uint8_t pressure) override;
	 void control_change(uint32_t dt, uint8_t channel, uint8_t controller, uint8_t value) override;
	 void program_change(uint32_t dt, uint8_t channel, uint8_t program) override;
	 void channel_pressure(uint32_t dt, uint8_t channel, uint8_t pressure) override;
	 void pitch_wheel_change(uint32_t dt, uint8_t channel, uint16_t value) override;
	 void meta(uint32_t dt, uint8_t type, const char* data, int data_size) override;
	 void sysex(uint32_t dt, const char* data, int data_size) override;
};

class EventMulticaster : public EventReceiver {
	std::vector<std::shared_ptr<EventReceiver>> note_filters;

public:
	EventMulticaster(std::vector<std::shared_ptr<EventReceiver>>& note_filters);
	void note_on(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) override;
	void note_off(uint32_t dt, uint8_t channel, uint8_t note, uint8_t velocity) override;
	void polyphonic_key_pressure(uint32_t dt, uint8_t channel, uint8_t note, uint8_t pressure) override;
	void control_change(uint32_t dt, uint8_t channel, uint8_t controller, uint8_t value) override;
	void program_change(uint32_t dt, uint8_t channel, uint8_t program) override;
	void channel_pressure(uint32_t dt, uint8_t channel, uint8_t pressure) override;
	void pitch_wheel_change(uint32_t dt, uint8_t channel, uint16_t value) override;
	void meta(uint32_t dt, uint8_t type, const char* data, int data_size) override;
	void sysex(uint32_t dt, const char* data, int data_size) override;
};


bool read_notes(std::istream& in, std::vector<NOTE>* notes);
#endif // !MIDI_H

Bitmap visualize(const std::vector<NOTE>& notes);
