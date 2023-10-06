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
	inline definitions for keyframe animation controller.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


/*
=========================================
File: a3_KeyframeAnimationController.inl
Purpose: Give functionality to the Clip Controller's Update function
Author(s) and Contribution(s):
-Aster Nie: Gave functionalty to the clipControllerUpdate with all 7 options
Date:   9/14/2023
=========================================
*/

//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	// make sure clip ctrl is valid
	if (!clipCtrl) //|| !clipCtrl->init)
		return -1;

	// don't resolve anything if paused
	if (clipCtrl->playback == 0) return -1;

	// perform change in time in the playback direction
	// -1 = backward, 0 = paused, 1 = forward
	clipCtrl->clipTime += dt * clipCtrl->playback;
	clipCtrl->keyframeTime += dt * clipCtrl->playback;

	a3_Clip* currClip = &clipCtrl->_clipPool->clips[clipCtrl->clip];
	a3_Keyframe* currKeyframe = &currClip->pool->keyframes[clipCtrl->keyframe];

	//char arrays to store the terminus action and the clip to transition to (if any)
	//a3byte terminusAction[a3keyframeAnimation_nameLenMax];
	a3byte next[a3keyframeAnimation_nameLenMax];


	// IF PLAYHEAD MOVED FORWARD
	while (clipCtrl->keyframeTime >= currKeyframe->keyframeDuration) // update keyframe forward until we're in the right keyframe
	{
		clipCtrl->keyframe++; // go to the next keyframe
		clipCtrl->keyframeTime -= currKeyframe->keyframeDuration; // calculate the time in the next keyframe
		currKeyframe = &currClip->pool->keyframes[clipCtrl->keyframe]; // update our current keyframe pointer

		while (clipCtrl->clipTime >= currClip->duration) // update clips forward until we're in the right clip
		{
			clipCtrl->clip++; // go to the next clip

			// if playhead is past this clip, perform a terminus action
			if (clipCtrl->clip >= clipCtrl->_clipPool->count)
			{
				if (next == "") //no clip name given, perform terminus action on current clip
				{
					a3HandleTerminus(clipCtrl, currClip, currClip->terminus);
				}
				else //clip name given, find the associated clip and perform terminus action with it
				{
					a3_Clip* newClip;
					int counter = 0;
					while (&clipCtrl->_clipPool->clips[counter] != NULL) //going through the clip pool to find the clip with given name
					{
						if (clipCtrl->_clipPool->clips[counter].name == next)
						{
							newClip = &clipCtrl->_clipPool->clips[counter];
							a3HandleTerminus(clipCtrl, newClip, currClip->terminus);
						}
					}
				}
				return 1;
			}

			clipCtrl->clipTime -= currClip->duration; // calculate the time in the next clip
			currClip = &clipCtrl->_clipPool->clips[clipCtrl->clip]; //update our current clip pointer
		}
	}

	// IF PLAYHEAD MOVED BACKWARD
	while (clipCtrl->keyframeTime < 0) // update keyframe backwards until we're in the right keyframe
	{
		clipCtrl->keyframe--; // go to the previous keyframe
		clipCtrl->keyframeTime += currKeyframe->keyframeDuration; // calculate the time in the prev keyframe
		currKeyframe = &currClip->pool->keyframes[clipCtrl->keyframe]; // update our current keyframe pointer

		while (clipCtrl->clipTime < 0) // update clips backwards until we're in the right clip
		{
			clipCtrl->clip--; // go to the prev clip

			// if playhead is past this clip, perform a terminus action
			if (clipCtrl->clip >= clipCtrl->_clipPool->count)
			{
				clipCtrl->clip = clipCtrl->_clipPool->count; // no loop (stop)
				// loop...
				// ping-pong...
				return 1;
			}

			clipCtrl->clipTime += currClip->duration; // calculate the time in the prev clip
			currClip = &clipCtrl->_clipPool->clips[clipCtrl->clip]; //update our current clip pointer
		}
	}
	
	// if we are in a keyframe or a clip, do nothing (no resolution needed)

	//nothing to really do with this atm but for testing purposes
	printf("Sprite at: ( %d , %d ), with a width of %d and a height of %d\n", currKeyframe->keyData[0], currKeyframe->keyData[1],
		currKeyframe->keyData[2], currKeyframe->keyData[3]);


	// update the normalized clip value
	clipCtrl->clipParameter = clipCtrl->clipTime / currClip->duration;
	clipCtrl->keyframeParameter = clipCtrl->keyframeTime / currKeyframe->keyframeDuration;

	// extra (above and beyond: call some event when this happens, do terminus actions, etc.

	return 1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	if (!clipCtrl) return -1;
	if (!clipPool) return -1;
	if (clipIndex_pool > clipPool->count) return -1;

	clipCtrl->clip = clipIndex_pool;

	return 1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H