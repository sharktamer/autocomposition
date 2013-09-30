#include <string.h>
#include <vector>
using namespace std;
//#include <iostream>
#include "include/MersenneTwister.h"
#include "midifile.h"

//Random number generator object.
MTRand mtrand;

//Constants used to set the chord numbers used in the transition tables.
const int CHORD_C  = MIDIFILE_NOTE_C*2;
const int CHORD_Dm = MIDIFILE_NOTE_D*2+1;
const int CHORD_F  = MIDIFILE_NOTE_F*2;
const int CHORD_G  = MIDIFILE_NOTE_G*2;
const int CHORD_Am = MIDIFILE_NOTE_A*2+1;

//Choose next chord using the chord transition table given.
int chooseNextChord(float chordTransitionTable[24])
{
	//Get a random number between 0 and 1.
	float randomProb = mtrand.rand();
	//Float used to store the sum of the probabilities in the transition table.
	float cumulativeProb = 0;
	
	//For each of the probabilities in the transition table.
	for(int i = 0; i < 24; i++)
	{
		//Add the probability to the cumulativeProb variable.
		cumulativeProb += chordTransitionTable[i];
		
		/* If the cumulative probability is more than the random number chosen
		   Return the element number, which doubles as the chord number */
		if(cumulativeProb > randomProb)
			return i;
	}
}

/* The function used to generate the MIDI file.
   PARAMETERS:
   midiName - the name of the MIDI file that will be written.
   noOfBars - the number of bars the MIDI file will have.
   transitionTable - the transition table used to generate the MIDI file. */
void generateMidi(const char* midiName, int noOfBars, float transitionTable[24][24])
{
	//Melody notes for each chord used.
	//TODO: Try and put these outside the function again.
	int cMelodyNotes[3]	 = { MIDIFILE_NOTE_C, MIDIFILE_NOTE_E, MIDIFILE_NOTE_G };
	int dMinorMelodyNotes[3] = { MIDIFILE_NOTE_D, MIDIFILE_NOTE_F, MIDIFILE_NOTE_A };
	int fMelodyNotes[3]	 = { MIDIFILE_NOTE_F, MIDIFILE_NOTE_A, MIDIFILE_NOTE_C };
	int gMelodyNotes[3]	 = { MIDIFILE_NOTE_G, MIDIFILE_NOTE_B, MIDIFILE_NOTE_D };
	int aMinorMelodyNotes[3] = { MIDIFILE_NOTE_A, MIDIFILE_NOTE_C, MIDIFILE_NOTE_E };
	 
	//The note durations that can be chosen.
	int noteDurations[] = {64, 128, 256};
	
	//Create the midifile object.
	MidiFile withchordaccompaniment;
	
	//Add four tracks to the midi file. Three for chords and one for melody.
	for(int i = 0; i < 4; i++)
		withchordaccompaniment.addTrack();
			
	//Holds the number for the chord. First chord should be C.
	int chordNumber = CHORD_C;
	
	for(int i = 0; i < noOfBars; i++)
	{
		//Length of a bar.
		int lengthLeft = 512;
			
		//Array used to store the melody notes for the chord chosen.
		int melodyNotes[3];
		//Array used to store the transition table for the chord chosen.
		float chordTransitionTable[24];
			
		//Add the chosen chord and set the melody notes.
		switch(chordNumber)
		{
			//If the chord chosen was C...
			case CHORD_C:
				//Add the C chord to the midi file.
				withchordaccompaniment.addChord(0, lengthLeft, 4, MIDIFILE_NOTE_C);
				//Copy the C chords melody notes to the melodyNotes array.
				memcpy(melodyNotes, cMelodyNotes, sizeof(melodyNotes));
				//Copy the C chord transition table to the transitionTable array.
				memcpy(chordTransitionTable, transitionTable[MIDIFILE_NOTE_C*2], sizeof(transitionTable[MIDIFILE_NOTE_C*2]));
				break;
				
				case CHORD_Dm:
				//Add the Dm chord to the midi file.
				withchordaccompaniment.addChord(0, lengthLeft, 4, MIDIFILE_NOTE_D, true);
				//Copy the Dm chords melody notes to the melodyNotes array.
				memcpy(melodyNotes, dMinorMelodyNotes, sizeof(melodyNotes));
				//Copy the Dm chord transition table to the transitionTable array.
				memcpy(chordTransitionTable, transitionTable[MIDIFILE_NOTE_D*2+1], sizeof(transitionTable[MIDIFILE_NOTE_C*2+1]));
				break;
				
			case CHORD_F:
				//Add the F chord to the midi file.
				withchordaccompaniment.addChord(0, lengthLeft, 4, MIDIFILE_NOTE_F);
				//Copy the F chords melody notes to the melodyNotes array.
				memcpy(melodyNotes, fMelodyNotes, sizeof(melodyNotes));
				//Copy the F chord transition table to the transitionTable array.
				memcpy(chordTransitionTable, transitionTable[MIDIFILE_NOTE_F*2], sizeof(transitionTable[MIDIFILE_NOTE_F*2]));
				break;
				
			case CHORD_G:
				//Add the G chord to the midi file.
				withchordaccompaniment.addChord(0, lengthLeft, 4, MIDIFILE_NOTE_G);
				//Copy the G chords melody notes to the melodyNotes array.
				memcpy(melodyNotes, gMelodyNotes, sizeof(melodyNotes));
				//Copy the G chord transition table to the transitionTable array.
				memcpy(chordTransitionTable, transitionTable[MIDIFILE_NOTE_G*2], sizeof(transitionTable[MIDIFILE_NOTE_G*2]));
				break;
				
			case CHORD_Am:
				//Add the Am chord to the midi file.
				withchordaccompaniment.addChord(0, lengthLeft, 4, MIDIFILE_NOTE_A, true);
				//Copy the Am chords melody notes to the melodyNotes array.
				memcpy(melodyNotes, aMinorMelodyNotes, sizeof(melodyNotes));
				//Copy the Am chord transition table to the transitionTable array.
				memcpy(chordTransitionTable, transitionTable[MIDIFILE_NOTE_A*2+1], sizeof(transitionTable[MIDIFILE_NOTE_A*2+1]));
				break;
			}
			
		//Vector for the note durations to be stored in.
			std::vector<int> noteDurationsChosen;
			
		//Add notes until the bar is full.
	   	while(lengthLeft > 0)
		{
			//Store the length of the current note.
	  		int currentNoteLength;
			
			//Repeat...
			do
			{
  				//Get a random note length from the array.
				currentNoteLength = noteDurations[mtrand.randInt(2)];
	  		}
			//...while the note length chosen is longer than length left in the bar.
			while(currentNoteLength > lengthLeft);
		
			//Push the note duration chosen onto the noteDurationsChosen vector.
			noteDurationsChosen.push_back(currentNoteLength);
		
			//Subtract the length of the current note from the current time left in the bar.
			lengthLeft -= currentNoteLength;
		}
			
		//Add the melody notes to the midi file based on the chord that has been chosen.
		for(int i = 0; i < noteDurationsChosen.size(); i++)
		{
			withchordaccompaniment.addNote(3, noteDurationsChosen[i], 6, melodyNotes[mtrand.randInt(3)]);
		}
		
		//Choose the next chord.
		chordNumber = chooseNextChord(chordTransitionTable);
	}
	
	//Write the midi object to file.
	withchordaccompaniment.writeToFile(midiName);
}

int main()
{
	/* First chord transition table array.
	   Table is implemented with even numbers as major chords and odd numbers as minor chords.
	   E.g. MIDIFILE_NOTE_F (which is 5) * 2 = 10, so it is F major.
	        MIDIFILE_NOTE_A (9) * 2 + 1 = 19, so it is A minor.
       This transition table has all notes at the same probability. 
       There is the same chance of any allowed note occuring.*/
	float transitionTable1[24][24];
	
	/* Second chord transition table array
	   This transition table favours the transition "Dm -> G -> Am -> F"
	   If any of these notes are chosen, it will always follow the path.
	   The only notes that do not have one chord they will always move onto are F and C.*/
	float transitionTable2[24][24];
	
	/* Third chord transition table array
	   This transiton table favours the two minor chords, Dm and Am.
	   Whenever a chord can move on to a minor chord, there is about twice the chance of it happening. */
	float transitionTable3[24][24];
	
	//Initialise all elements of the transition tables to 0.
	for(int i = 0; i < 24; i++)
	{
		for(int j = 0;j < 24; j++)
		{
			transitionTable1[i][j] = 0;
			transitionTable2[i][j] = 0;
			transitionTable3[i][j] = 0;
		}
	}
	
	//Transition table 1 definition.
	
	//Transition table values for C major.
	transitionTable1[CHORD_C] [CHORD_Dm] = 0.25;
	transitionTable1[CHORD_C] [CHORD_F]  = 0.25;
	transitionTable1[CHORD_C] [CHORD_G]  = 0.25;
	transitionTable1[CHORD_C] [CHORD_Am] = 0.25;
	//Values for D minor.
	transitionTable1[CHORD_Dm][CHORD_G]  = 0.5;
	transitionTable1[CHORD_Dm][CHORD_Am] = 0.5;
	//Values for F major.
	transitionTable1[CHORD_F] [CHORD_F]  = 1.0/3;
	transitionTable1[CHORD_F] [CHORD_Dm] = 1.0/3;
	transitionTable1[CHORD_F] [CHORD_C]  = 1.0/3;
	//Values for G major.
	transitionTable1[CHORD_G] [CHORD_C]  = 0.5;
	transitionTable1[CHORD_G] [CHORD_Am] = 0.5;
	//Values for A minor.
	transitionTable1[CHORD_Am][CHORD_Dm] = 0.5;
	transitionTable1[CHORD_Am][CHORD_F]  = 0.5;
	
	//Run the generateMidi function with the first transition table.
	generateMidi("transitiontable1.mid", 8, transitionTable1);
	
	//Transition table 2 definition.
	
	//Transition table values for C major.
	transitionTable2[CHORD_C] [CHORD_Dm] = 0.25;
	transitionTable2[CHORD_C] [CHORD_F]  = 0.25;
	transitionTable2[CHORD_C] [CHORD_G]  = 0.25;
	transitionTable2[CHORD_C] [CHORD_Am] = 0.25;
	//Values for D minor.
	transitionTable2[CHORD_Dm][CHORD_G]  = 1.0;
	transitionTable2[CHORD_Dm][CHORD_Am] = 0.0;
	//Values for F major.
	transitionTable2[CHORD_F] [CHORD_F]  = 0.1;
	transitionTable2[CHORD_F] [CHORD_Dm] = 0.4;
	transitionTable2[CHORD_F] [CHORD_C]  = 0.5;
	//Values for G major.
	transitionTable2[CHORD_G] [CHORD_C]  = 0.0;
	transitionTable2[CHORD_G] [CHORD_Am] = 1.0;
	//Values for A minor.
	transitionTable2[CHORD_Am][CHORD_Dm] = 0.0;
	transitionTable2[CHORD_Am][CHORD_F]  = 1.0;
	
	//Generate a MIDI file based on transitionTable2.
	generateMidi("transitiontable2.mid", 8, transitionTable2);
	
	//Transition table 3 definition.
	
	//Transition table values for C major.
	transitionTable3[CHORD_C] [CHORD_Dm] = 0.4;
	transitionTable3[CHORD_C] [CHORD_F]  = 0.1;
	transitionTable3[CHORD_C] [CHORD_G]  = 0.1;
	transitionTable3[CHORD_C] [CHORD_Am] = 0.4;
	//Values for D minor.
	transitionTable3[CHORD_Dm][CHORD_G]  = 0.3;
	transitionTable3[CHORD_Dm][CHORD_Am] = 0.7;
	//Values for F major.
	transitionTable3[CHORD_F] [CHORD_F]  = 0.1;
	transitionTable3[CHORD_F] [CHORD_Dm] = 0.6;
	transitionTable3[CHORD_F] [CHORD_C]  = 0.3;
	//Values for G major.
	transitionTable3[CHORD_G] [CHORD_C]  = 0.3;
	transitionTable3[CHORD_G] [CHORD_Am] = 0.7;
	//Values for A minor.
	transitionTable3[CHORD_Am][CHORD_Dm] = 0.7;
	transitionTable3[CHORD_Am][CHORD_F]  = 0.3;
	
	//Generate a MIDI file based on transitionTable3.
	generateMidi("transitiontable3.mid", 8, transitionTable3);
	
}


/*

--Melody notes--
If the chord is C then you could use C E or G
If the chord is D minor then you can use D F or A
If the chord is F  then you can use F A or C
If the chord is G  then you can use G B or D
If the chord is A minor then you can use A C or E

--Chord notes--
current chord  -> next chord 
C -> D F G A 
D -> G A 
F -> F D C
G -> C A
A -> D F 
 
Problem ChordProgressions
C -> G
Dm -> Am

TODO: Look into the add note methods, see if they check if the track is available properly.
TODO: Add checks to the midiFileFormat attribute, so it can only be 0, 1 or 2 (add to the writeToFile method).

*/
