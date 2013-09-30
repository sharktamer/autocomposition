#ifndef MIDIFILE_H
#define MIDIFILE_H

//Constants defining how many tracks in the midi file.
const int MIDIFILE_SINGLETRACK           = 0;
const int MIDIFILE_MULTIPLETRACKS_SYNCH  = 1;
const int MIDIFILE_MULTIPLETRACKS_ASYNCH = 2;

//Constants used to set the note added to the midi track.
const int MIDIFILE_NOTE_C  =  0;
const int MIDIFILE_NOTE_C_ =  1;
const int MIDIFILE_NOTE_D  =  2;
const int MIDIFILE_NOTE_D_ =  3;
const int MIDIFILE_NOTE_E  =  4;
const int MIDIFILE_NOTE_F  =  5;
const int MIDIFILE_NOTE_F_ =  6;
const int MIDIFILE_NOTE_G  =  7;
const int MIDIFILE_NOTE_G_ =  8;
const int MIDIFILE_NOTE_A  =  9;
const int MIDIFILE_NOTE_A_ = 10;
const int MIDIFILE_NOTE_B  = 11;

#include <iostream>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

//MIDI file class. This object will have MIDI tracks added to it.
class MidiFile
{
	//MidiFileHeader class definition.
	class MidiFileHeader
	{
		//The MIDI file signature. This is always set to 0x4D546864 (MThd in ASCII).
		unsigned long midiFileSignature;
		//The MIDI file header length. This is always set to 0x0000006, since the MIDI file header length will always be 6 bytes.
		unsigned long midiFileHeaderLength;
		//The file format (Single track or multiple tracks synchronous/asynchronous).
		unsigned short  midiFileFormat;
		//The number of delta ticks per quarter note.
		unsigned short  midiFileDeltaTimeTicks;

		public:
			//Constructor method.
			MidiFileHeader(unsigned short fileformat = MIDIFILE_MULTIPLETRACKS_SYNCH, unsigned short deltaTimeticks = 128);
			//Write the header to the file.
			virtual void writeToFile(std::ostream& os, unsigned short trackcount);
	};
	
	//Declare the MidiTrack object, which will be defined later.
	class MidiTrack;
	
	//The MIDI file header.
	MidiFileHeader header;
	//The vector storing the tracks within the MIDI file.
	std::vector<MidiTrack*> tracks;
	
	public:
		MidiFile(unsigned short deltaTimeticks = 128, unsigned short fileformat = MIDIFILE_SINGLETRACK); //Class contructor.
		//Write the MIDI to file. Takes a string argument.
		virtual void writeToFile(const char* filename);
		//Write the MIDI to file. Takes an ostream argument.
		virtual void writeToFile(std::ostream& os);
		//Add a track to the MIDI file object.
		void addTrack();
		// Add a note to the MIDI file object. Will add both a note on and note off to the MIDI object.
		void addNote(int track, long deltaTime, unsigned char octave, unsigned char noteNumber,
			unsigned char velocity = 96, unsigned char vel_off = 64, unsigned char channel = 0);
		//Add a note on, which will note be terminated until the note off is added.
		void addNoteOn(int track, long deltaTime, unsigned char octave, unsigned char noteNumber,
			unsigned char velocity = 96, unsigned char channel = 0);
		//Add a note off.
		void addNoteOff(int track, long deltaTime, unsigned char octave, unsigned char noteNumber,
			unsigned char velocity = 64, unsigned char channel = 0);
		//Add a chord to the MIDI object.
		void addChord(int firsttrack, long deltatime, unsigned char octave, const int chordNote, bool minor = false,
			unsigned char velocity = 96, unsigned char vel_off = 64, unsigned char channel = 0);
};

class MidiFile::MidiTrack
{
		//The MIDI track header.
		class MidiTrackHeader
		{
			unsigned long midiTrackSignature; //The MIDI track signature. This is always set to 0x4D54726B (MTrk in ASCII).
			unsigned long midiTrackLength; //The MIDI track length. This is the length of the track (not including the header).

			public:
				MidiTrackHeader(unsigned long tracklength = 0); //Constructor method.
				virtual void writeToFile(std::ostream& os, int length); //Write the track header to the file.
		};
	
		//The MIDI track header.
		MidiTrackHeader header;
		
		//Midi command declarations.
		class MidiCommand;
		class MidiCommandNoteOn;
		class MidiCommandNoteOff;
		
		//The vector storing the added MIDI commands.
		std::vector<MidiCommand*> commands;

		public:
			MidiTrack(); //Class constructor.
			virtual void writeToFile(std::ostream& os); //Write the MIDI track to the file.
	
			//Functions that will add the certain command to the MIDI track.
			virtual void noteOn(unsigned char channel, long deltaTime, unsigned char octave, unsigned char noteNumber,
				unsigned char velocity = 96);
			virtual void noteOff(unsigned char channel, long deltaTime, unsigned char octave, unsigned char noteNumber,
				unsigned char velocity = 64);
			virtual void note(unsigned char channel, long deltaTime, unsigned char octave, unsigned char noteNumber,
				unsigned char velocity = 96, unsigned char vel_off = 64);
};

class MidiFile::MidiTrack::MidiCommand
{
	public:
		MidiCommand(unsigned char channel = 0, long deltaTime = 0); //Class constructor.
		virtual void writeToFile(std::ostream& os); //Write the MIDI command to file.
		virtual unsigned long getLength() //Get the length of the MIDI event. Each MIDI event will have a different length, overiding this function.
		{
			return 0;
		}
		unsigned char channel; //The channel to use for the MIDI command.
		long deltaTime; //The deltatime before the MIDI command is executed.
};

//The note on MIDI command.
class MidiFile::MidiTrack::MidiCommandNoteOn : public MidiFile::MidiTrack::MidiCommand
{
	//Variables which will store the information of the note on command.
	unsigned char octave;
	unsigned char noteNumber;
	unsigned char velocity;

	public:
		//The class constructor.
		MidiCommandNoteOn(unsigned char channel, long deltaTime, unsigned char octave, unsigned char noteNumber, unsigned char velocity);
		//Write the command to file.
		virtual void writeToFile(std::ostream& os);
		//Get the length of the midi command, which needs to be written to the file.
		virtual unsigned long getLength()
		{
			return 3;
		}
};

//The note off MIDI command.
class MidiFile::MidiTrack::MidiCommandNoteOff : public MidiFile::MidiTrack::MidiCommand
{
	//Variables which store the information of the note off.
	unsigned char octave;
	unsigned char noteNumber;
	unsigned char velocity;

	public:
		//The class constructor.
		MidiCommandNoteOff(unsigned char channel, long deltaTime, unsigned char octave, unsigned char noteNumber, unsigned char velocity);
		//Write the command to file.
		virtual void writeToFile(std::ostream& os);
		//Get the length of the command, which needs to be written to the file.
		virtual unsigned long getLength()
		{
			return 3;
		}
};

#endif //MIDIFILE_H
