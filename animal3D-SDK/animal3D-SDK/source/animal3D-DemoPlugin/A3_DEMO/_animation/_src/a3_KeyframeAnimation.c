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
	keyframePool_out = (a3_KeyframePool*)malloc(count * sizeof(a3_KeyframePool));
	if (keyframePool_out == NULL)
	{
		return -1;
	}
	keyframePool_out->count = count;
	return 1;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	free(keyframePool);

	return 1;
}

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x)
{
	keyframe_out->duration = duration;
	keyframe_out->invDuration = 1.0f / duration;
	keyframe_out->keyData = value_x;

	return 1;
}


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	clipPool_out = (a3_ClipPool*)malloc(count * sizeof(a3_ClipPool));
	if (clipPool_out == NULL)
	{
		return -1;
	}
	clipPool_out->count = count;
	return 1;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	free(clipPool);
	return 1;
}


// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	
	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax);
	clip_out->first = firstKeyframeIndex;
	clip_out->last = finalKeyframeIndex;
	clip_out->pool = keyframePool;


	//goes through each keyframe in the pool and finds the total duration
	a3real sum = 0;
	
	for(a3ui32 i = clip_out->first; i <= clip_out->last; i++)
	{
		sum += clip_out->pool->keyframe[i].duration;
	}
	clip_out->duration = sum;
	clip_out->invDuration = 1 / clip_out->duration;

	return 1;
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


//-----------------------------------------------------------------------------
