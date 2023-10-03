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
	
	a3_HierarchyState.inl
	Implementation of inline transform hierarchy state operations.
*/


#ifdef __ANIMAL3D_HIERARCHYSTATE_H
#ifndef __ANIMAL3D_HIERARCHYSTATE_INL
#define __ANIMAL3D_HIERARCHYSTATE_INL


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

// get offset to hierarchy pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes);
	return -1;
}

// get offset to single node pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes + nodeIndex);
	return -1;
}

//-----------------------------------------------------------------------------

// reset full hierarchy pose
inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount)
{
	if (pose_inout && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; ++i)
		{
			//pose_inout->spatialPose = ???;
		}
	}
	return -1;
}

// convert full hierarchy pose to hierarchy transforms
inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	a3mat4 identity = { 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };

	//this is the important one
	if (pose_inout && nodeCount)
	{
		//Take all of the poses (rotation, transformation, scale) and encode them into a matrix (local space)
		//Before we can convert, we need to know the final state of the pose
		
		//for the sake of this, will just use the identity matrix
		for (a3ui32 i = 0; i < nodeCount; i++)
		{

			//getting the TRS matrices to multiply, probably not the best method but i cant find anything build already
			a3mat4 transMat = { 0, 0, 0, 1,
								0, 0, 1, pose_inout->spatialPose[i].position.z,
								0, 1, 0, pose_inout->spatialPose[i].position.y,
								1, 0, 0, pose_inout->spatialPose[i].position.x };

			a3mat4 rotXMat = { 1, 0, 0, 0,
								0, a3cosd(pose_inout->spatialPose[i].rotation.x), -a3sind(pose_inout->spatialPose[i].rotation.x), 0,
								0, a3sind(pose_inout->spatialPose[i].rotation.x), a3cosd(pose_inout->spatialPose[i].rotation.x), 0,
								0, 0, 0, 1 };
			a3mat4 rotYMat = { a3cosd(pose_inout->spatialPose[i].rotation.y), 0, a3sind(pose_inout->spatialPose[i].rotation.y), 0,
								0, 1, 0, 0,
								-a3sind(pose_inout->spatialPose[i].rotation.y), 0, a3cosd(pose_inout->spatialPose[i].rotation.y), 0,
								0, 0, 0, 1 };
			a3mat4 rotZMat = { a3cosd(pose_inout->spatialPose[i].rotation.z), -a3sind(pose_inout->spatialPose[i].rotation.z), 0, 0,
								a3sind(pose_inout->spatialPose[i].rotation.z), a3cosd(pose_inout->spatialPose[i].rotation.z), 1, 0,
								0, 0, 1, 0,
								0, 0, 0, 1 };

			a3mat4 rotXYMat; //can only multiply 2 matrices at a time
			a3real4x4Product(rotXYMat.m, rotXMat.m, rotYMat.m);
			a3mat4 rotMat;
			a3real4x4Product(rotMat.m, rotXYMat.m, rotZMat.m);


			a3mat4 scaleMat = { pose_inout->spatialPose[i].scale.x, 0, 0, 0,
									0, pose_inout->spatialPose[i].scale.y, 0, 0,
									0, 0, pose_inout->spatialPose[i].scale.z, 0,
									0, 0, 0, 1 };


			a3mat4 posRotMat; //can only multiply 2 matrices at a time, doing (pos * rot) * scale
			a3real4x4Product(posRotMat.m, transMat.m, rotMat.m);
			//final result of the transform matrix
			a3real4x4Product(pose_inout->spatialPose[i].transform.m, posRotMat.m, scaleMat.m);
			

			//multiplying the transform by the "change" (no change because we just want base pose, so just identity matrix)
			a3real4x4Product(pose_inout->spatialPose[i].transform.m,
				pose_inout->spatialPose[i].transform.m,
				identity.m);
		}
		return 1;
	}
	return -1;
}

// copy full hierarchy pose
inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount)
{
	if (pose_out && pose_in && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			pose_out->spatialPose[i] = pose_in->spatialPose[i];
		}
		return 1;
	}
	return -1;
}

// For this assignment, implement the 'step' interpolation algorithm, which is effectively just duplication of a single input key pose.  This phase takes key poses to be interpolated and stores the result in the target state's sample pose (for the 'step' function, this just means directly copying the pose from the pool into the state).
inline a3i32 a3hierarchyPoseLerp(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_from, const a3_HierarchyPose* pose_to, const a3real param, const a3ui32 nodeCount)
{
	return -1;
}

// This phase takes the base pose from the source pool and the sample pose from the state as inputs, and concatenates them into the state's local pose.
inline a3i32 a3hierarchyPoseConcat(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* base_pose, const a3_HierarchyPose* sample_pose, const a3ui32 nodeCount)
{
	return -1;
}


//-----------------------------------------------------------------------------

// update inverse object-space matrices
inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state)
{
	return -1;
}


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATE_INL
#endif	// __ANIMAL3D_HIERARCHYSTATE_H