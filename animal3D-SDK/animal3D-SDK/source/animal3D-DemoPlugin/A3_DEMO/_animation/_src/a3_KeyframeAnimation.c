/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_KeyframeAnimation.c
	Implementation of keyframe animation interfaces.
*/

/*
=========================================
File: a3_KeyframeAnimation.c
Purpose: Establish the data structures of Keyframes, Keyframe Pools, Clips, and Clip Pools, and have constructors so they can be used elsewhere
Author(s) and Contribution(s):
-Tommy Wagner: Filled out the constructors, initializers, and destructors for the data structures
-Aster Nie: Bug fixing
Date:   9/14/2023
=========================================
*/


//#include "../a3_KeyframeAnimation.h"
#include "A3_DEMO/_animation/a3_KeyframeAnimation.h"

#include <stdlib.h>
#include <string.h>

// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
//	if (keyframePool_out == 0)
	//	return -1;

	//keyframePool_out->keyframe = (...)malloc(...);

	//keyframePool_out = (a3_KeyframePool*)malloc(sizeof(a3_KeyframePool));
	if (!keyframePool_out) return -1;

	keyframePool_out->keyframe = malloc(count * sizeof(a3_Keyframe));
	if (!keyframePool_out->keyframe) return -1;

	keyframePool_out->count = count;
	return 0;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	free(keyframePool);

	return 0;
}

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x[4])
{
	if (!keyframe_out) return -1;

	keyframe_out->keyframeDuration = duration;
	keyframe_out->invDuration = 1.0f / duration;
	keyframe_out->keyData;

	return 0;
}


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	if (!clipPool_out) return -1;

	clipPool_out->clip = malloc(count * sizeof(a3_Clip));

	if (!clipPool_out->clip) return -1;

	clipPool_out->count = count;
	return 0;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	free(clipPool);
	return 0;
}


// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 clipDuration, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	
	memcpy(clip_out->name, clipName, sizeof(clip_out->name));

	clip_out->first = firstKeyframeIndex;
	clip_out->last = finalKeyframeIndex;

	clip_out->pool = keyframePool; 


	//goes through each keyframe in the pool and finds the total duration
	/*
	a3real sum = 0;
	
	for(a3ui32 i = clip_out->first; i <= clip_out->last; i++)
	{
		sum += clip_out->pool->keyframe[i].keyframeDuration;
	}
	clip_out->duration = sum;
	*/
	clip_out->duration = clipDuration;
	clip_out->invDuration = 1 / clip_out->duration;

	return 0;
}

// get clip index from pool
//will search through each clip in the clip pool and return the index of one with a matching name
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	for (a3ui32 i = 0; i < clipPool->count; i++)
	{
		if (clipPool->clip[i].name == clipName)
		{
			return i;
		}
	}
	return -1;
}

/*
will read through the file, fill out keyframe pool with each cell (length * width or col * row)
Will go through each line, creating a clip with the given duration, start, and end, from that keypool

for now, ignore transitions and just get that part down

file name: resource/tex/sprite/spriteTest8x8.png

sprite sheet is 512x512
Each sprite is 64x64
So do position (64*clip, 64*keyframe), with width and height 64,64

*/
a3i32 a3clipPoolFileInit(a3_ClipPool* clipPool, a3byte clipFile[256], a3_KeyframePool* keyPool)
{
	//creating the keyframe pool to hold each sprite
	a3keyframePoolCreate(keyPool, 64); //64 keyframes, for the 8x8 sprite sheet

	//going through each row and column, adding the sprite's location and dimensions
	for (int col = 0; col < 8; col++)
	{
		for (int rows = 0; rows < 8; rows++)
		{
			a3ui32 spriteData[4] = {64*rows, 64*col, 64, 64};
			a3keyframeInit(&keyPool->keyframe[col*8+rows], 1, spriteData); //will go left to right, top to bottom on sprite sheet
		}
	}

	//the file that contains the clip information
	FILE* myFile = fopen(clipFile, "r");

	//the first character of the line, will be @ if it is one that should be read
	a3byte check[1];

	//the number of valid lines, or the number of clips in the pool
	int clipCount = 0;

	while (!feof(myFile))
	{
		fgets(check, 1, myFile); //read the first char into the buffer
		if (check == '@')
		{
			clipCount++;
		}
	}

	//resetting the file back to the start for the next pass
	rewind(myFile);

	//creation of the clip pool, with the clipCount we just found
	a3clipPoolCreate(clipPool, clipCount);
		
	//the buffer that holds each line of the text file
	a3byte buffer[256];

	//the variables that will store clip data being read
	a3byte clipName[a3keyframeAnimation_nameLenMax];
	a3ui32 clipDuration;
	a3ui32 firstIndex;
	a3ui32 lastIndex;
	a3byte ignore;

	//keeping track of which clip is being added
	a3ui32 counter = 0;
	//going through each line of the file
	while (!feof(myFile))
	{
		fgets(buffer, 256, myFile); //read a line into the buffer
		if (buffer[0] != '@') 
			//making sure it is a valid line
		{
			continue;
		}

		//reading values into the variables (ignore is just to catch the @ symbol at the start of the line)
		if (!sscanf(buffer, "%c %s %d %d %d", &ignore, &clipName, &clipDuration, &firstIndex, &lastIndex))
		{
			//failed to read line
		}

		//initiating clip with the read values
		a3clipInit(&clipPool->clip[counter], clipName, keyPool, clipDuration, firstIndex, lastIndex);

		//keeping track of how many clips have been put in the clip pool already
		counter++;
	}

	fclose(myFile);
}





//-----------------------------------------------------------------------------
