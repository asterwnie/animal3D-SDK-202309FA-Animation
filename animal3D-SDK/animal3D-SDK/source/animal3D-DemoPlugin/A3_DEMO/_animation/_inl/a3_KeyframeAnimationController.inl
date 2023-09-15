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

	a3_Clip* currClip = &clipCtrl->clipPool->clip[clipCtrl->clip];
	a3_Keyframe* currKeyframe = &currClip->pool->keyframe[clipCtrl->keyframe];
	
	// IF PLAYHEAD MOVED FORWARD
	while (clipCtrl->clipTime >= currClip->duration) // update clips forward until we're in the right clip
	{
		clipCtrl->clip++; // go to the next clip
		
		// if playhead is past this clip, perform a terminus action
		if (clipCtrl->clip >= clipCtrl->clipPool->count)
		{
			clipCtrl->clip = clipCtrl->clipPool->count; // no loop (stop)
			// loop...
			// ping-pong...
			return 0;
		}

		clipCtrl->clipTime -= currClip->duration; // calculate the time in the next clip
		currClip = &clipCtrl->clipPool->clip[clipCtrl->clip]; //update our current clip pointer

		while (clipCtrl->keyframeTime >= currKeyframe->duration) // update keyframe forward until we're in the right keyframe
		{
			clipCtrl->keyframe++; // go to the next keyframe
			clipCtrl->keyframeTime -= currKeyframe->duration; // calculate the time in the next keyframe
			currKeyframe = &currClip->pool->keyframe[clipCtrl->keyframe]; // update our current keyframe pointer
		}
	}

	// IF PLAYHEAD MOVED BACKWARD
	while (clipCtrl->clipTime < currClip->duration) // update clips backwards until we're in the right clip
	{
		clipCtrl->clip--; // go to the prev clip
		
		// if playhead is past this clip, perform a terminus action
		if (clipCtrl->clip >= clipCtrl->clipPool->count)
		{
			clipCtrl->clip = clipCtrl->clipPool->count; // no loop (stop)
			// loop...
			// ping-pong...
			return 0;
		}

		clipCtrl->clipTime += currClip->duration; // calculate the time in the prev clip
		currClip = &clipCtrl->clipPool->clip[clipCtrl->clip]; //update our current clip pointer

		while (clipCtrl->keyframeTime < currKeyframe->duration) // update keyframe backwards until we're in the right keyframe
		{
			clipCtrl->keyframe--; // go to the previous keyframe
			clipCtrl->keyframeTime += currKeyframe->duration; // calculate the time in the prev keyframe
			currKeyframe = &currClip->pool->keyframe[clipCtrl->keyframe]; // update our current keyframe pointer
		}
	}

	// if we are in a keyframe or a clip, do nothing (no resolution needed)

	// update the normalized clip value
	clipCtrl->clipParameter = clipCtrl->clipTime / currClip->duration;
	clipCtrl->keyframeParameter = clipCtrl->keyframeTime / currKeyframe->duration;

	// extra (above and beyond: call some event when this happens, do terminus actions, etc.
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H