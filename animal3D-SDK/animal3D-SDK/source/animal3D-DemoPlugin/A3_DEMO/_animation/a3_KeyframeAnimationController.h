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
	
	a3_KeyframeAnimationController.h
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.
*/

/*
=========================================
File: a3_KeyframeAnimationController.h
Purpose: Set up the Clip Controller data structure
Author(s) and Contribution(s):
-Tommy Wagner: Added member variables that will be used in its functions
Date:   9/14/2023
=========================================
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ClipController			a3_ClipController;
#endif	// __cplusplus

#pragma warning( disable : 4090)
//just testing without this warning, will remove later
//-----------------------------------------------------------------------------

// clip controller
// metaphor: playhead
struct a3_ClipController
{
	a3byte name[a3keyframeAnimation_nameLenMax];

	//index of current clip
	a3ui32 clip; 

	//clip time, relative to start of clip (non-normalized)
	a3real clipTime;

	//normalized time, should always be between 0 and 1
	a3real clipParameter;

	//index of current keyframe
	a3ui32 keyframe;

	//clip time, relative to start of keyframe (non-normalized)
	a3real keyframeTime;

	//normalized time, should always be between 0 and 1
	a3real keyframeParameter;

	//playback direction, should be 1 (forward), 0 (paused), or -1 (reversed)
	a3ui32 playback;

	//clip pool being controlled
	a3_ClipPool* _clipPool;
};

//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt);

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);



//function that handles terminus actions
a3i32 a3HandleTerminus(a3_ClipController* clipCtrl, a3_Clip* actionClip, a3byte terminus[a3keyframeAnimation_nameLenMax]);
//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimationController.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H