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
#include <stdio.h>
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
	if (poseGroup_out && hierarchy_out && resourceFilePath)
	{
		// indices of joints, their parents and branching joints
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;
		a3i32 rootJointIndex;// upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;

		// load the file
		FILE* fptr;
		fptr = fopen(resourceFilePath, "r");
		if (fptr == NULL) return -1;

		a3byte line[256]; // each line read in gets put here
		a3byte commentChar = '#'; // used to ignore lines
		a3byte headerChar = '['; // denotes headers
		a3byte attribute[256]; // used to hold headers
		a3byte value[256]; // used to hold header values
		a3ui32 numVal; // used to hold numerical values

		a3byte objName[256]; // used to hold node name
		a3f32 tx, ty, tz, rx, ry, rz, boneLength; // position & rotation values
		a3ui32 poseIndex; // which pose are we on?

		while (!feof(fptr))
		{
			// keep reading lines
			fgets(line, 256, fptr);
			// skip comments
			if (line[0] == commentChar) continue;

			// check for headers
			if (line[0] == headerChar)
			{
				sscanf(line, " %s", attribute);
				if (attribute[0] == headerChar)
				{
					continue; // go to the next line to start processing
				}
			}

			// handle each section based on their header

			if (strcmp(attribute, "[Header]") == 0)
			{
				sscanf(line, " %s", value);

				if (strcmp(value, "NumSegments") == 0)
				{
					sscanf(line, "%s %d", value, &numVal);
					a3hierarchyCreate(hierarchy_out, hierarchy_out->numNodes, 0); // init hierarchy
				}
				if (strcmp(value, "NumFrames") == 0)
				{
					sscanf(line, "%s %d", value, &numVal);
					a3hierarchyPoseGroupCreate(poseGroup_out, hierarchy_out, numVal); // init poses
				}
			}

			else if (strcmp(attribute, "[SegmentNames&Hierarchy]") == 0)
			{
				sscanf(line, " %s", value);

				if (strcmp(value, "main") == 0) continue;

				a3byte currNode[256];
				a3byte parentNode[256];

				// first value is the node
				// second value is the parent
				sscanf(line, "%s %s", currNode, parentNode);

				// set the values
				if(strcmp(parentNode, "main") == 0)
					jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy_out, jointIndex++, jointParentIndex, currNode);
				else
					jointParentIndex = a3hierarchySetNode(hierarchy_out, jointIndex++, jointParentIndex, currNode);
				
				// not sure what to do with these tbh

				//jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
			//	jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
			}

			else if (strcmp(attribute, "[BasePosition]") == 0)
			{
				//# ObjectName<tab>Tx<tab>Ty<tab>Tz<tab>Rx<tab>Ry<tab>Rz<tab>BoneLength<CR>

				// read vals
				sscanf(line, "%s %e %e %e %e %e %e %e", objName, &tx, &ty, &tz, &rx, &ry, &rz, &boneLength);

				a3i32 nodeIndex = a3hierarchyGetNodeIndex(hierarchy_out, objName);

				// set vals
				//hPose[0] is the BASE pose
				poseGroup_out->hPose[0].spatialPose[nodeIndex].position.x = tx;
				poseGroup_out->hPose[0].spatialPose[nodeIndex].position.y = ty;
				poseGroup_out->hPose[0].spatialPose[nodeIndex].position.z = tz;

				poseGroup_out->hPose[0].spatialPose[nodeIndex].rotation.x = rx;
				poseGroup_out->hPose[0].spatialPose[nodeIndex].rotation.y = ry;
				poseGroup_out->hPose[0].spatialPose[nodeIndex].rotation.z = rz;

				// what do we do with bone length???
			}

			else
			{
				// the header must be a joint
				// this will contain the translation/rotation of the node at every pose

				// remove the brackets
				int i, j;
				i = 0;
				while (i < strlen(attribute))
				{
					if (attribute[i] == '[' || attribute[i] == ']')
					{
						for (j = i; j < strlen(attribute); j++)
							attribute[j] = attribute[j + 1];
					}
					else i++;
				}

				// read vals
				sscanf(line, "%u %e %e %e %e %e %e %e", &poseIndex, &tx, &ty, &tz, &rx, &ry, &rz, &boneLength);

				a3i32 nodeIndex = a3hierarchyGetNodeIndex(hierarchy_out, attribute);

				a3_SpatialPose* spatialPose = poseGroup_out->hPose[poseIndex].spatialPose + nodeIndex;
				a3spatialPoseSetTranslation(spatialPose, tx, ty, tz);
				a3spatialPoseSetRotation(spatialPose, rx, ry, rz);

				// what do we do with bone length???
			}
		}

		fclose(fptr);
		return 1;
	}
	return -1;
}

// convenience function from https://stackoverflow.com/questions/7821997/c-remove-chars-from-string
void removeChar(char str[], char t)
{
	int i, j;
	i = 0;
	while (i < strlen(str))
	{
		if (str[i] == t)
		{
			for (j = i; j < strlen(str); j++)
				str[j] = str[j + 1];
		}
		else i++;
	}
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
