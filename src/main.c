#include <stdio.h>
#include <SDL2/SDL.h>

#include <stdlib.h>
#include <math.h>
#include "portaudio.h"
#include "waveforms.h"
#include "keyboardMap.h"
#define NUM_SECONDS   (30)
#define SAMPLE_RATE   (44100)
#define INV_SAMPLE_RATE   (1./(SAMPLE_RATE))

#define DELAY_LENGTH 44100

float delay_line[DELAY_LENGTH];
int delay_line_read;
int delay_line_write;
double delay_line_lfo_phase;

float interpolate(float x, float y, float a){
	return a*y+(1-a)*x;
}

float unisonsPhases[20];
float unisonsMultipliers[20];

typedef struct Note{
	double frequency;
	double volume;
	char on;
	KeyPosition keyPressed;
} Note;

typedef struct InternalNote{
	double phase;
	double phaseDelta;
	double lfoPhase;
	//double lfoPhaseDelta;
	double framesPassed;
	double volume;
	char on;
	double ring;
} InternalNote;

void InternalNote_initialize(InternalNote* n){
	*n = (InternalNote){ 0., 0., 0., 0., 0., 0, 0};

}
float randomFloat()
{
      float r = (float)rand()/(float)RAND_MAX;
      return r;
}
#define noteNumber 16
typedef struct
{
	Note notes[noteNumber];
	InternalNote internalNotes[noteNumber];
	int unusedNotePointer;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */
		(void) timeInfo;
		(void) statusFlags;
    

		for (int j=0; j<noteNumber; j++){
			Note *note=&(data->notes[j]);
			InternalNote *internalNote=&(data->internalNotes[j]);
			if(note->on){
				internalNote->on=1;
			}
		}
	
    for( i=0; i<framesPerBuffer; i++ ){
			float sumL=0;
			float sumR=0;
			for (int j=0; j<noteNumber; j++){
				Note *note=&(data->notes[j]);
				InternalNote *internalNote=&(data->internalNotes[j]);
				if(!note->on){
					internalNote->volume*=0.9999;
					if(internalNote->volume<=0.00001){
						internalNote->on=0;
					}
				}
				if(internalNote->on){
					InternalNote *internalNote=&(data->internalNotes[j]);
					internalNote->phase+=internalNote->phaseDelta;
					//* (1+sineFunction(internalNote->lfoPhase)*0.005);
					internalNote->lfoPhase+=INV_SAMPLE_RATE;
					//while(internalNote->phase>=1.) internalNote->phase-=1.;

					//internalNote->volume*=0.99995;

					

					
					// Bass sound
					float r = internalNote->ring;
					float p = internalNote->phase;
					float a = sawFunction(p+sineFunction(p))

							*0.2*internalNote->volume ;
					
					//a*=0.3*internalNote->volume;
					internalNote->ring*=0.9999;

					internalNote->framesPassed++;

					sumL+=a;
					sumR+=a;
				}
			}



		delay_line_lfo_phase+=(0.1*INV_SAMPLE_RATE);
		if (delay_line_lfo_phase >= 1.) {
			delay_line_lfo_phase -= 1.;
		}


		double leftValue, rightValue; 
		{

		double fractionalPtr = delay_line_write -1000 - ((sineFunction(delay_line_lfo_phase)+1.)*(500.));

		double error = fractionalPtr - (int)(fractionalPtr);

		int prevPtr = ((int)fractionalPtr+DELAY_LENGTH) % DELAY_LENGTH;
		int nextPtr = (prevPtr+1) % DELAY_LENGTH;

		double prevValue = delay_line[prevPtr];
		double nextValue = delay_line[nextPtr];
		double interpolatedValue = nextValue * error + prevValue * (1-error);
		leftValue = interpolatedValue;
		}
		{

		double fractionalPtr = delay_line_write -1000 - ((sineFunction(delay_line_lfo_phase+0.5)+1.)*(500.));

		double error = fractionalPtr - (int)(fractionalPtr);

		int prevPtr = ((int)fractionalPtr+DELAY_LENGTH) % DELAY_LENGTH;
		int nextPtr = (prevPtr+1) % DELAY_LENGTH;

		double prevValue = delay_line[prevPtr];
		double nextValue = delay_line[nextPtr];
		double interpolatedValue = nextValue * error + prevValue * (1-error);
		rightValue = interpolatedValue;
		}

		delay_line[delay_line_write] = sumL;// + interpolatedValue*0.8;

		sumL += leftValue*0.5;
		sumR += rightValue*0.5;

		delay_line_write++;
		if (delay_line_write >= DELAY_LENGTH) {
			delay_line_write=0;
		}

		out[i*2]=sumL;
		out[i*2+1]=sumR;

    }

    return 0;
}

static paTestData data;

double scale[7];



void draw(SDL_Renderer* renderer) {

	double offset = 30;
	double octaveSize = 200;

	int screenWidth;
	int screenHeight;

	SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	SDL_RenderClear(renderer);


	SDL_Rect scaleLines[5*6];
	SDL_Rect octaveLines[5];

	for (int i=0; i<5; i++) {
		double octaveHeight = i*octaveSize + offset - octaveSize;
		octaveLines[i].x = 0;
		octaveLines[i].y = screenHeight - (int)(octaveHeight);
		octaveLines[i].w = screenWidth;
		octaveLines[i].h = 1;

		for (int j=0; j<6; j++) {
			double height = octaveHeight + scale[j]*octaveSize;
			scaleLines[i*6+j].x = 0;
			scaleLines[i*6+j].y = screenHeight - (int)(height);
			scaleLines[i*6+j].w = screenWidth;
			scaleLines[i*6+j].h = 1;
		}

	}

	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderFillRects(renderer, scaleLines, 5*6);

	SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
	SDL_RenderFillRects(renderer, octaveLines, 5);



	SDL_Rect noteLines[noteNumber];
	SDL_Rect noteLinesGlow[noteNumber];
	int noteCount = 0;

	for (int i=0; i<noteNumber; i++){
		Note *note=&(data.notes[i]);
		if (note->on) {
			double height = log2(note->frequency/110)*octaveSize + offset;
			noteLines[noteCount].x = 100;
			noteLines[noteCount].y = screenHeight - (int)(height);
			noteLines[noteCount].w = 100;
			noteLines[noteCount].h = 1;
			noteLinesGlow[noteCount].x = 99;
			noteLinesGlow[noteCount].y = screenHeight - (int)(height) - 1;
			noteLinesGlow[noteCount].w = 102;
			noteLinesGlow[noteCount].h = 3;
			noteCount++;
		}
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
	SDL_RenderFillRects(renderer, noteLinesGlow, noteCount);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(renderer, 255, 50, 0, 255);
	SDL_RenderFillRects(renderer, noteLines, noteCount);
	SDL_RenderPresent(renderer);
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStream *stream;
    PaError err;
	SDL_Event e; 
	bool exit = false;
	
	SDL_Init( SDL_INIT_EVERYTHING );

	SDL_Window *window = SDL_CreateWindow("Dank Memes",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          640, 480,
						  0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	for (int j=0; j<noteNumber; j++){
		Note *note=&(data.notes[j]);
		InternalNote *internalNote=&(data.internalNotes[j]);
		*note = (Note){ 0., 0., 0, {0,0} };
		InternalNote_initialize(internalNote);
	}


	for (int i=0; i<sizeof(unisonsPhases)/sizeof(unisonsPhases[0]); i++){
		unisonsPhases[i]=randomFloat();
		unisonsMultipliers[i]=1-0.01*(0.5-randomFloat());
	}

	for (int i=0; i<DELAY_LENGTH; i++) {
		delay_line[i]=0.;
	}

	delay_line_read=DELAY_LENGTH-1;
	delay_line_write=0;
	delay_line_lfo_phase=0.;


	data.unusedNotePointer=0;

	double octave = 1;
	double fifth = 
		//7./12.   //12edo

		//log2(3./2.) //pythagorean
		//31./53.   //practically the same thing


		log2(5.)/4. //quarter-comma meantone


		//log2(10./6.*2)/3. //minor third = 6/5 third-comma meantone
		//11./19.  //practically the same thing


		//13./22.
		//log2(12./7.*2)/3. //minor third = 7/5 , kinda like 22edo

		//710.291/1200.

		//log2(8.*7.)/10. //septimal meantone with Aug6 = 7/4
		//log2(8.*7./5.)/6. //septimal meantone with Aug4 = 7/5
		//log2(32.*7./6.)/9. //septimal meantone with dim2 = 7/6

		//18./31.  // 31 is pretty much in the same spirit (not all notes are covered)


		//4./7.
		//log2(5./3.*2)/3. //some kind of 7edo meantone temperament
		//log2(11./9.*4)/4. //some kind of 7edo meantone temperament
		//log2(11./6.*4)/5. //some kind of 7edo meantone temperament
		//log2(9./5.*4)/5. //some kind of 7edo meantone temperament


		//0.6 //5edo
		//log2(7./2.)/3. 
		//log2(9./5.*2)/3.
		//log2(7./3.)/2.




		//10./17.
		//log2(11./9. * 32.)/9. // some kind of 17edo meantone



		// weird,
		//10./16.
		//15./26.
		;
	double fourth = octave-fifth;
	double wholetone = 2.*fifth - octave;
	double diatonicSemitone = fourth - 2.*wholetone;
	//float leftStep = 2./13.;//wholetone; //
	//float upStep =-1./13.;//(fourth - 3.*wholetone);
	//float leftStep = 2./10.;//wholetone; //
	//float upStep =-1./10.;//(fourth - 3.*wholetone);
	double leftStep = wholetone; //
	double upStep =(fourth - 3.*wholetone);

	//float leftStep=log2(4./3)/3.;
	//float upStep=1-8*leftStep;

	//float leftStep=2./12.;
	//float upStep=-3./24.;


	// natural minor scale
	scale[0] = wholetone;
	scale[1] = wholetone+diatonicSemitone;
	scale[2] = fourth;
	scale[3] = fifth;
	scale[4] = fifth+diatonicSemitone;
	scale[5] = fifth+diatonicSemitone+wholetone;
	scale[6] = 1;


	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	/* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream( &stream,
			0,          /* no input channels */
			2,          /* stereo output */
			paFloat32,  /* 32 bit floating point output */
			SAMPLE_RATE,
			16,        /* frames per buffer */
			patestCallback,
			&data );
	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	while (!exit) {

		
		if (SDL_WaitEventTimeout(&e,50)) {
		//User requests quit
		if( e.type == SDL_QUIT ){
			exit = true;
		}

		//User presses a key
		else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 ){



			OptionalKeyPosition optKey= mapScancode(e.key.keysym.scancode);
			if(optKey.some){
				int notePointer=data.unusedNotePointer;
				do{
					Note *note=&(data.notes[notePointer]);
					InternalNote *internalNote=&(data.internalNotes[notePointer]);
					if(!note->on){
						note->keyPressed=optKey.position;
						note->on=1;
						note->frequency=exp2((note->keyPressed.x-5)*leftStep
								+(note->keyPressed.y-1)*upStep)*220;
						note->volume=1;
						internalNote->lfoPhase=0;
						internalNote->phase=0;
						internalNote->framesPassed=0;	
						internalNote->ring=0.99;
						internalNote->volume=note->volume;

						internalNote->phaseDelta=note->frequency/44100.;

						data.unusedNotePointer=(data.unusedNotePointer+1)%noteNumber;
						printf("(%d, %d) at position %d; ptr at %d\n",
								note->keyPressed.x, note->keyPressed.y,
								notePointer, data.unusedNotePointer);
						break;
					}
					notePointer=(notePointer+1)%noteNumber;
				}
				while(notePointer!=data.unusedNotePointer);
			}
		}
		else if ( e.type == SDL_KEYUP ) {

			OptionalKeyPosition optKey= mapScancode(e.key.keysym.scancode);
			if(optKey.some){
				printf("%d,%d depressed;\n",
						optKey.position.x, optKey.position.x);
				for (int j=0; j<noteNumber; j++){
					Note *note=&(data.notes[j]);
					InternalNote *internalNote=&(data.internalNotes[j]);
					if (note->keyPressed.x == optKey.position.x
							&& note->keyPressed.y == optKey.position.y){
						note->on=0;
					}
				}

			}
		}
		}

			draw(renderer);

	}



	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;
	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	Pa_Terminate();


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	//Quit SDL 
	SDL_Quit();


	return err;


error:
	Pa_Terminate();
	fprintf( stderr, "An error occured while using the portaudio stream\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}
