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
			+ sizeof(a3_SpatialPoseChannel) * hierarchy->numNodes
			+ sizeof(a3_SpatialPoseEulerOrder) * hierarchy->numNodes;

		void* memory = malloc(sz);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;

		//starting address for hierarchical poses
		a3_HierarchyPose* hPoseBase = (a3_HierarchyPose*)memory;
		//starting address for spatial poses
		a3_SpatialPose* poseBase = (a3_SpatialPose*)(hPoseBase + poseCount);

		//starting address for channels
		poseGroup_out->channel = (a3_SpatialPoseChannel*)(poseBase + poseCount * hierarchy->numNodes); 
		// starting address for order
		poseGroup_out->order = (a3_SpatialPoseEulerOrder*)(poseGroup_out->channel + poseCount);

		poseGroup_out->hPose = hPoseBase;
		poseGroup_out->spatialPoses = poseBase;
		for (a3ui32 i = 0; i < poseCount; i++)
		{
			//link sptialpose and hierarchypose together; each hPose should get 'numNodes' spatial poses
			poseGroup_out->hPose[i].spatialPose = (a3_SpatialPose*)(poseBase + i * hierarchy->numNodes);
			a3spatialPoseCreate(poseGroup_out->hPose[i].spatialPose); // init values
			memcpy(&(poseGroup_out->spatialPoses[i]), poseGroup_out->hPose[i].spatialPose, sizeof(a3_SpatialPose));
		}

		// reset all data
		poseGroup_out->spatialPoseCount = poseCount;
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
		poseGroup->spatialPoseCount = 0;
		poseGroup->spatialPoses = 0;

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
		
		// initialize hierarchy poses
		//state_out->objectSpace = (a3_HierarchyPose){ 0 };
		state_out->objectSpace = (a3_HierarchyPose){ state_out->objectSpace.spatialPose = (a3_SpatialPose*)memory };

		//state_out->localSpace = (a3_HierarchyPose){ 0 };
		state_out->localSpace = (a3_HierarchyPose){ state_out->localSpace.spatialPose = (a3_SpatialPose*)( & (state_out->objectSpace) + hierarchy->numNodes) };

		//state_out->objectSpaceBindToCurrent = (a3_HierarchyPose){ 0 };
		state_out->objectSpaceBindToCurrent = (a3_HierarchyPose){ state_out->objectSpaceBindToCurrent.spatialPose = (a3_SpatialPose*)( & (state_out->localSpace) + hierarchy->numNodes) };

		// set pointers
		state_out->hierarchy = hierarchy;

		// go through all of the objects in object/local/etc. and figure out the address of the array
		// and add it to each one
		// ^ do the same thing for the pose group

		// object space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
			a3spatialPoseCreate(&(state_out->objectSpace.spatialPose[i]));
			// assign to pointer in hierarchy pose
		}

		// localSpace space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
			a3spatialPoseCreate(&(state_out->localSpace.spatialPose[i]));
			// assign to pointer in hierarchy pose
		}

		// objectSpaceBindToCurrent space
		for (a3ui32 i = 0; i < hierarchy->numNodes; ++i)
		{
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
		free(state->objectSpace.spatialPose);

		// reset pointers
		state->hierarchy = 0;
		state->localSpace.spatialPose = 0;
		state->objectSpace.spatialPose = 0;
		state->objectSpaceBindToCurrent.spatialPose = 0;

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
		a3ui32 j, p;
		a3_Hierarchy* hierarchy = hierarchy_out;
		a3_HierarchyPoseGroup* hierarchyPoseGroup = poseGroup_out;
		a3_SpatialPose* spatialPose = 0;

		p = 0;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz | a3poseChannel_scale_xyz | a3poseChannel_translate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, -0.1f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, -0.1f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.2f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, -0.1f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.2f, 0.0f, 0.0f);


		// each remaining pose represents a "delta" from the base
		// initialize the changes where applicable
		// (note: the channels describe which pose components can change)
		p = 2;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetRotation(spatialPose, +45.0f, +60.0f, +90.0f);	// rotate whole figure about all axes


		p = 3;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetScale(spatialPose, 2.0f, 2.0f, 2.0f);	// uniformly scale whole figure up to 200%


		p = 4;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hPose[p].spatialPose + j;
		a3spatialPoseSetTranslation(spatialPose, +3.0f, +4.0f, +5.0f);	// shift whole figure by some vector


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
