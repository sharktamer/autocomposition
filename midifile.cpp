#include "midifile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

//File writing functions
//This function is used to write an unsigned short to a file.
void writeShortToFile(ostream& os, unsigned short value)
{
	const char* ptr = (const char*)(&value); //Make a cast to a const char* from the unsigned short input.
	//Write ptr to the file.
	os.write(ptr+1, 1);
	os.write(ptr, 1);
}

//Writes an unsigned long to a file.
void writeLongToFile(ostream& os, unsigned long value)
{
	const char* ptr = (const char*)(&value); //Make a cast to a const char*.
	//Write the ptr to the file.
	os.write(ptr+3, 1);
	os.write(ptr+2, 1);
	os.write(ptr+1, 1);
	os.write(ptr, 1); //ptr+0
}

//Write value which is encoded using a variable number of bytes.
void writeVarLen(ostream& os, long value)
{
	if (value < 0)
		return; // not supported. Value can not be negative.
	long buffer;
	buffer = value & 0x7F; //Perform bitwise AND on the value inputted and the hexadecimal value 7F (1111111).
	while ((value >>= 7) > 0)
	{
		buffer <<= 8;
		buffer |= 0x80; //Perform bitwise OR on the buffer and the hexadecimal value 80 (10000000).
				//Converts the buffer to its negative value and adds it to the buffer.
		buffer += (value & 0x7F);
	}
	while (true)
	{
		char c = buffer & 0xFF;
		os.put(c);
		if (buffer & 0x80)
			buffer >>= 8;
		else
			break;
	}
}

//Returns the length of a variable length value.
long varLenLen(long value)
{
	if (value < 0)
		return 0; // not supported
	long buffer;
	long ret = 0;
	buffer = value & 0x7F;
	while ((value >>= 7) > 0)
	{
		buffer <<= 8;
		buffer |= 0x80;
		buffer += (value & 0x7F);
	}
	while (true)
	{
		char c = buffer & 0xFF;
		ret++;
		if (buffer & 0x80)
			buffer >>= 8;
		else
			break;
	}
	return ret;
}

//Midi file header class functions
MidiFile::MidiFileHeader::MidiFileHeader(unsigned short fileformat, unsigned short deltatimeticks)
{
	midiFileSignature = 0x4D546864;
	midiFileHeaderLength = 0x0000006;
	midiFileFormat = fileformat;
	midiFileDeltaTimeTicks = deltatimeticks;
}

void MidiFile::MidiFileHeader::writeToFile(ostream& os, unsigned short trackcount)
{
	if (trackcount > 1 && midiFileFormat == MIDIFILE_SINGLETRACK)
	{
		midiFileFormat = MIDIFILE_MULTIPLETRACKS_SYNCH;
	}

	writeLongToFile(os, midiFileSignature);
	writeLongToFile(os, midiFileHeaderLength);
	writeShortToFile(os, midiFileFormat);
	writeShortToFile(os, trackcount);
	writeShortToFile(os, midiFileDeltaTimeTicks);
}

//Midi file functions
MidiFile::MidiFile(unsigned short deltatimeticks, unsigned short fileformat)
	: header(fileformat, deltatimeticks)
{
	tracks.push_back(new MidiTrack);
}

void MidiFile::writeToFile(const char* filename)
{
	ofstream os(filename);
	writeToFile(os);
}

void MidiFile::writeToFile(ostream& os)
{
	header.writeToFile(os, tracks.size());
	
	for (int i=0; i < tracks.size(); i++)
	{
		tracks[i]->writeToFile(os);
	}
}

void MidiFile::addTrack()
{
	tracks.push_back(new MidiTrack);
}

void MidiFile::addNote(int track, long deltaTime, unsigned char octave, unsigned char noteNumber, unsigned char velocity, 
			unsigned char vel_off, unsigned char channel)
{
	if(track <= tracks.size())
		tracks[track]->note(channel, deltaTime, octave, noteNumber, velocity, vel_off);
	else
		std::cout << "ERROR: Invalid track number." << std::endl;
}

void MidiFile::addNoteOn(int track, long deltaTime, unsigned char octave, unsigned char noteNumber, unsigned char velocity, unsigned char channel)
{
	if(track <= tracks.size())
		tracks[track]->noteOn(channel, deltaTime, octave, noteNumber, velocity);
	else
		std::cout << "ERROR: Invalid track number." << std::endl;
}

void MidiFile::addNoteOff(int track, long deltaTime, unsigned char octave, unsigned char noteNumber, unsigned char velocity, unsigned char channel)
{
	if(track <= tracks.size())
		tracks[track]->noteOff(channel, deltaTime, octave, noteNumber, velocity);
	else
		std::cout << "ERROR: Invalid track number." << std::endl;
}

void MidiFile::addChord(int firsttrack, long deltatime, unsigned char octave, const int chordNote, bool minor, unsigned char velocity, 
     unsigned char vel_off, unsigned char channel)
{
     if(firsttrack + 2 >= tracks.size())
     {
                   std::cout << "ERROR: not enough tracks available to add chord." << std::endl;
                   return;
     }
     
     switch (chordNote)
     {
            case MIDIFILE_NOTE_C:
                 if(minor)
                 {
                     addNote(firsttrack, deltatime, octave, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_G, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_E, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_G, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_C_:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_C_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_E, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_F, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_D:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_F, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_A, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_A, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_D_:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_G, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_E:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_E, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_G, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_B, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave, MIDIFILE_NOTE_E,  velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave, MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave, MIDIFILE_NOTE_B,  velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_F:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_F,  velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_C,  velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_F, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_A, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_F_:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_A,  velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_C_, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_C_, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_G:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_G, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_G, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave    , MIDIFILE_NOTE_B, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_G_:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_B, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_G_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_A:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_A, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_C, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_E, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_A, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_C_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_E, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_A_:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_C_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_F, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_A_, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_F, velocity, vel_off, channel);
                 }
                 break;
                 
            case MIDIFILE_NOTE_B:
                 if(minor)
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_B, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_D, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                 }
                 else
                 {
                     addNote(firsttrack    , deltatime, octave    , MIDIFILE_NOTE_B, velocity, vel_off, channel);
                     addNote(firsttrack + 1, deltatime, octave + 1, MIDIFILE_NOTE_D_, velocity, vel_off, channel);
                     addNote(firsttrack + 2, deltatime, octave + 1, MIDIFILE_NOTE_F_, velocity, vel_off, channel);
                 }
                 break;
                 
            default:
                    std::cout << "ERROR: Invalid chord name." << std::endl;
     }
}

//Midi track functions
MidiFile::MidiTrack::MidiTrack()
{
}

//Midi track class functions
MidiFile::MidiTrack::MidiTrackHeader::MidiTrackHeader(unsigned long tracklength/* = 0*/)
{
	midiTrackSignature = 0x4D54726B;
	midiTrackLength = tracklength;
}

void MidiFile::MidiTrack::MidiTrackHeader::writeToFile(ostream& os, int length)
{
	midiTrackLength = length;

	writeLongToFile(os, midiTrackSignature);
	writeLongToFile(os, midiTrackLength);
}

void MidiFile::MidiTrack::writeToFile(ostream& os)
{
	int midiTrackLength = 0;
	for(int i = 0; i < commands.size(); i++)
	{
		midiTrackLength += commands[i]->getLength();
		midiTrackLength += varLenLen(commands[i]->deltaTime);
	}
	header.writeToFile(os, midiTrackLength);
	
	for (int i = 0; i < commands.size(); i++)
	{
		commands[i]->writeToFile(os);
	}
}

void MidiFile::MidiTrack::noteOn(unsigned char channel, long deltatime, unsigned char octave, unsigned char notenumber, unsigned char velocity)
{
	commands.push_back(new MidiCommandNoteOn (channel, deltatime, octave, notenumber, velocity));
}

void MidiFile::MidiTrack::noteOff(unsigned char channel, long deltatime, unsigned char octave, unsigned char notenumber, unsigned char velocity)
{
	commands.push_back(new MidiCommandNoteOff(channel, deltatime, octave, notenumber, velocity));
}

void MidiFile::MidiTrack::note(unsigned char channel, long deltatime, unsigned char octave, unsigned char notenumber, unsigned char velocity, unsigned char vel_off)
{
	commands.push_back(new MidiCommandNoteOn(channel, 0, octave, notenumber, velocity));
	commands.push_back(new MidiCommandNoteOff(channel, deltatime, octave, notenumber, vel_off));
}

//MidiCommand functions
MidiFile::MidiTrack::MidiCommand::MidiCommand(unsigned char channel, long deltatime) : channel(channel), deltaTime(deltatime)
{
}

void MidiFile::MidiTrack::MidiCommand::writeToFile(ostream& os)
{
	if (deltaTime >= 0)
		writeVarLen(os, deltaTime);
}

MidiFile::MidiTrack::MidiCommandNoteOn::MidiCommandNoteOn(unsigned char channel, long deltatime, unsigned char octave, unsigned char notenumber, unsigned char velocity) 
	: MidiCommand(channel, deltatime), octave(octave), noteNumber(notenumber), velocity(velocity)
{
}

void MidiFile::MidiTrack::MidiCommandNoteOn::writeToFile(ostream& os)
{
	MidiFile::MidiTrack::MidiCommand::writeToFile(os);
	unsigned char w = 0x90;
	w |= channel;
	os.put(w);
	w = 12*octave+noteNumber;
	os.put(w);
	w = velocity;
	os.put(w);
}

MidiFile::MidiTrack::MidiCommandNoteOff::MidiCommandNoteOff(unsigned char channel, long deltatime, unsigned char octave, unsigned char notenumber, unsigned char velocity) 
	: MidiCommand(channel, deltatime), octave(octave), noteNumber(notenumber), velocity(velocity)
{
}

void MidiFile::MidiTrack::MidiCommandNoteOff::writeToFile(ostream& os)
{
	MidiFile::MidiTrack::MidiCommand::writeToFile(os);
	unsigned char w = 0x80;
	w |= channel;
	os.put(w);
	w = 12*octave+noteNumber;
	os.put(w);
	w = velocity;
	os.put(w);
}
