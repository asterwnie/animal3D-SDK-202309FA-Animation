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
	
	a3_KeyframeAnimationController.c
	Implementation of keyframe animation controller.
*/

#include "../a3_KeyframeAnimationController.h"

#include <string.h>


/*
=========================================
File: a3_KeyframeAnimationController.c
Purpose: Add functionality to the Clip Controller
Author(s) and Contribution(s):
-Tommy Wagner: Gave functionality to the Clip Controller Initializer
-Aster Nie: Bug fixing
Date:   9/14/2023
=========================================
*/

//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	if (!clipCtrl_out) {
		return -1;
	}
	if (!clipPool) {
		return -1;
	}
	//setting the controller's name
	memcpy(clipCtrl_out->name, ctrlName, 10);
	


	//setting the clip pool being controlled
#pragma warning( disable : 4090)
//just testing without this warning, will remove later
	clipCtrl_out->_clipPool = clipPool;


	//setting the index of the clip currently accessed
	clipCtrl_out->clip = clipIndex_pool;
	//should be starting with the first keyframe
	clipCtrl_out->keyframe = 0;
	//should be starting forward
	clipCtrl_out->playback = 1;

	return 0;
}


//-----------------------------------------------------------------------------
