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
	
	a3_HierarchyState.h
	Hierarchy transformation state.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATE_H
#define __ANIMAL3D_HIERARCHYSTATE_H


// A3 hierarchy
#include "a3_Hierarchy.h"

// A3 spatial pose
#include "a3_SpatialPose.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_HierarchyPose			a3_HierarchyPose;
typedef struct a3_HierarchyPoseGroup	a3_HierarchyPoseGroup;
typedef struct a3_HierarchyState		a3_HierarchyState;
#endif	// __cplusplus
	

//-----------------------------------------------------------------------------

// single pose for a collection of nodes
// makes algorithms easier to keep this as a separate data type
struct a3_HierarchyPose
{
	a3_SpatialPose* spatialPose;

};


// pose group
struct a3_HierarchyPoseGroup
{
	// A pointer or reference to the hierarchy associated with this spatial data.  The pose pool describes the spatial properties of the hierarchy (where are they in space), while the hierarchy describes the overall organization of the poses (how are the poses related).
	const a3_Hierarchy* hierarchy;

	// The actual array of individual node poses.  This is comparable to the 'keyframe pool' for all poses associated with a hierarchy and its nodes.  To clarify: this is the set of all poses for all nodes.
	a3_SpatialPose* spatialPoses; 	// spatial pose pool 

	// Total number of spatial poses: hierarchical pose count times hierarchy node count.
	a3ui32 spatialPoseCount;

	// An array of hierarchical poses (referencing the spatial poses).  This is comparable to the 'keyframe pool' for a whole hierarchy.  This is what organizes the above individual node poses.
	a3_HierarchyPose* hPose;

	// number of hierarchical poses
	a3ui32 poseCount;

	// An array of transformation channels for each node in the hierarchy; describes which individual pose transformation components are used by each node (e.g. rotation x, translation xyz, etc.); this is useful for optimization later.
	a3_SpatialPoseChannel* channel;

	// Some global flag for the pool that describes the concatenation order of orientation channels.
	a3_SpatialPoseEulerOrder* order;
};


// hierarchy state structure
struct a3_HierarchyState
{
	// A pointer or reference to the hierarchy associated with this spatial state.
	const a3_Hierarchy* hierarchy;

	//need to fill out

	//sample pose
	//A hierarchical pose representing each node's animated pose at the current time.

	//A hierarchical pose representing each node's transformation relative to the root's parent space (the actual object that the hierarchy represents).
	a3_HierarchyPose objectSpace;

	//A hierarchical pose representing each node's transformation relative to its parent's space.
	a3_HierarchyPose localSpace;

	// is this the sample pose...?
	a3_HierarchyPose objectSpaceBindToCurrent;

};
	

//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount);

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup);

// get offset to hierarchy pose in contiguous set
a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex);

// get offset to single node pose in contiguous set
a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex);


//-----------------------------------------------------------------------------

// reset full hierarchy pose
a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount);

// convert full hierarchy pose to hierarchy transforms
a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order);

// copy full hierarchy pose
a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount);

// For this assignment, implement the 'step' interpolation algorithm, which is effectively just duplication of a single input key pose.  This phase takes key poses to be interpolated and stores the result in the target state's sample pose (for the 'step' function, this just means directly copying the pose from the pool into the state).
a3i32 a3hierarchyPoseLerp(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_from, const a3_HierarchyPose* pose_to, const a3real param, const a3ui32 nodeCount);

// This phase takes the base pose from the source pool and the sample pose from the state as inputs, and concatenates them into the state's local pose.
a3i32 a3hierarchyPoseConcat(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* base_pose, const a3_HierarchyPose* sample_pose, const a3ui32 nodeCount);


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy);

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state);

// update inverse object-space matrices
a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state);


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyState.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATE_H