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
					internalNote->volume*=0.9998;
					if(internalNote->volume<=0.00001){
						internalNote->on=0;
					}
				}
				if(internalNote->on){
					InternalNote *internalNote=&(data->internalNotes[j]);
					internalNote->phase+=internalNote->phaseDelta
					* (1+sineFunction(internalNote->lfoPhase)*0.005);
					internalNote->lfoPhase+=INV_SAMPLE_RATE*5;
					//while(internalNote->phase>=1.) internalNote->phase-=1.;

					// Nice kinda like piano sound
					float a = (sawFunction(
							internalNote->phase+
							
								sineFunction(internalNote->phase)
								
								 //*internalNote->ring
								)
							)
							*sawFunction(internalNote->phase);//*internalNote->ring;
					a *= 0.3*internalNote->volume;
 		
					internalNote->ring*=0.9999;
					//internalNote->volume*=0.99995;

					

					// Bass sound
					/*
					float r = internalNote->ring;
					float p = internalNote->phase;
					float a = (0
							
							+sineFunction(
						p
						+sineFunction(p)*r
						//sineFunction(sineFunction(p))
						//sineFunction(sineFunction(sineFunction(p)))*r*r

							)
							+sineFunction(sineFunction(p))*r
							)
							*0.2*internalNote->volume ;
					internalNote->ring*=0.9999;
					internalNote->volume*=0.99995;
					*/

					
					// Bass sound
					/*
					float r = internalNote->ring;
					float p = internalNote->phase;
					float a = (0
							
							+sineFunction(
						sineFunction(p)*r
						+p

						)
							)

							*0.4*internalNote->volume ;
					//internalNote->ring*=0.9999;
					internalNote->volume*=0.99995;
					*/
/*
						double p = internalNote->phase;
					//float a = triangleFunction(p+(1+0.001*sineFunction(internalNote->framesPassed*INV_SAMPLE_RATE)))*0.3*internalNote->volume;

					float a = triangleFunction(p)
						+sineFunction(p*.5);
					
					a*=0.15*internalNote->volume;
					//float a = sineFunction(sawFunction(p*.5))*0.3*internalNote->volume;
*/




					internalNote->framesPassed++;
					


					sumL+=a;
					sumR+=a;
				}
			}

			out[i*2]=sumL;
			out[i*2+1]=sumR;
    }
    return 0;
}

/*******************************************************************/
static paTestData data;
int main(void);
int main(void)
{
    PaStream *stream;
    PaError err;
	SDL_Event e; 
	bool exit = false;
	
	SDL_Init( SDL_INIT_EVERYTHING );
	//Set up screen

	SDL_Window *screen = SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          320, 240,
                          0);
	(void) screen;

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

	
		data.unusedNotePointer=0;
    
    float octave = 1;
		float fifth = 
		//7./12.   //12edo
		//log2(3./2.) //pythagorean
		//log2(5.)/4. //quarter-comma meantone
		13./22.
		//10./17.
		//9./15.
		//11./19.
		//10./16.
		//15./26.
		//4./7.
		//18./31.
		//0.6 //5edo
		//31./53.
	;
	float fourth = octave-fifth;
	float wholetone = 2.*fifth - octave;
	float chromaticSemitone = fourth - 2.*wholetone;
    //float leftStep = 2./13.;//wholetone; //
    //float upStep =-1./13.;//(fourth - 3.*wholetone);
    //float leftStep = 2./10.;//wholetone; //
    //float upStep =-1./10.;//(fourth - 3.*wholetone);
    float leftStep = wholetone; //
    float upStep =(fourth - 3.*wholetone);
		//float leftStep=log2(4./3)/3.;
		//float upStep=1-8*leftStep;
    
    /* Initialize library before making any other calls. */
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
	
		//Handle events on queue
		SDL_WaitEvent(&e);
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
									+(note->keyPressed.y-1)*upStep)*440;
							note->volume=1;
							internalNote->lfoPhase=0;
							internalNote->phase=0;
							internalNote->framesPassed=0;	
							internalNote->ring=1;
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


	
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    Pa_Terminate();
    
    
  
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
