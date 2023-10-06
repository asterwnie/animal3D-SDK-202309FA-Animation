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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		const size_t sz =
			sizeof(a3_HierarchyPose) * poseCount
			+ sizeof(a3_SpatialPose) * poseCount * hierarchy->numNodes
			+ sizeof(a3_SpatialPoseChannel) * poseCount
			+ sizeof(a3_SpatialPoseEulerOrder);

		void* memory = malloc(sz);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		//starting address for hierarchical poses
		a3_HierarchyPose* hPoseBase = (a3_HierarchyPose*)memory;
		//starting address for spatial poses
		a3_SpatialPose* poseBase = (a3_SpatialPose*)(hPoseBase + poseCount);
		//starting address for channels
		poseGroup_out->channel = (a3_SpatialPoseChannel*)(poseBase + poseCount * hierarchy->numNodes); // this might be wrong
		// starting address for order
		poseGroup_out->order = (a3_SpatialPoseEulerOrder*)(poseGroup_out->channel + poseCount);

		//link them together; each hPose should get 'numNodes' spatial poses
		poseGroup_out->hPose = hPoseBase;
		for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		{
			if (poseGroup_out->hPose)
			{
				//this might be wrong
				poseGroup_out->hPose[i].spatialPose = (a3_SpatialPose*)(poseBase + i);
			}
		}

		// reset all data
		// ....
		poseGroup_out->poseCount = poseCount;

		// done
		return 1;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		free(poseGroup->hPose);

		// reset pointers
		poseGroup->hierarchy = 0;
		poseGroup->channel = 0;
		poseGroup->hPose = 0;
		poseGroup->order = 0;
		poseGroup->poseCount = 0;
		poseGroup.spatialPoseCount = 0;
		poseGroup.spatialPoses = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		const size_t sz =
			sizeof(a3_HierarchyPose) * hierarchy->numNodes * 3;

		void* memory = malloc(sz);

		// starting addresses
		// ** ADJUST THESE now that they're not pointers!! the spatial poses ARE the pointers tho
		//state_out = (a3_HierarchyState*)memory;

		//state_out->objectSpace = state_out;
		//state_out->localSpace = (a3_HierarchyPose*)(&(state_out->objectSpace) + hierarchy->numNodes);
		//state_out->objectSpaceBindToCurrent = (a3_HierarchyPose*)(&(state_out->localSpace) + hierarchy->numNodes);
		
		// initialize hierarchy poses
		state_out->objectSpace = (a3_HierarchyPose){ state_out->objectSpace.spatialPose = (a3_HierarchyPose*)memory };
		state_out->localSpace = (a3_HierarchyPose){ state_out->objectSpace.spatialPose = &(state_out->objectSpace) + hierarchy->numNodes };
		state_out->objectSpaceBindToCurrent = (a3_HierarchyPose){ state_out->objectSpace.spatialPose = &(state_out->localSpace) + hierarchy->numNodes };

		// set pointers
		state_out->hierarchy = hierarchy;

		// go through all of the objects in object/local/etc. and figure out the address of the array
		// and add it to each one
		// ^ do the same thing for the pose group

		// object space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
			// calc address of spatial pose array
			//state_out->objectSpace.spatialPose[i] = (a3_SpatialPose*)(&(state_out->objectSpace) + i);
			a3spatialPoseCreate(&(state_out->objectSpace.spatialPose[i]));
			// assign to pointer in hierarchy pose
		}

		// localSpace space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
			// calc address of spatial pose array
			//state_out->localSpace.spatialPose[i] = (a3_SpatialPose*)(&(state_out->localSpace) + i);
			a3spatialPoseCreate(&(state_out->localSpace.spatialPose[i]));
			// assign to pointer in hierarchy pose
		}

		// objectSpaceBindToCurrent space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
			// calc address of spatial pose array
			//state_out->objectSpaceBindToCurrent.spatialPose[i] = (a3_SpatialPose*)(&(state_out->objectSpaceBindToCurrent) + i);
			a3spatialPoseCreate(&(state_out->objectSpaceBindToCurrent.spatialPose[i]));
			// assign to pointer in hierarchy pose
		}

		// done
		return 1;
	}

	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		free(state->objectSpace);

		// reset pointers
		state->hierarchy = 0;
		state->localSpace = 0;
		state->objectSpace = 0;
		state->objectSpaceBindToCurrent = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------
