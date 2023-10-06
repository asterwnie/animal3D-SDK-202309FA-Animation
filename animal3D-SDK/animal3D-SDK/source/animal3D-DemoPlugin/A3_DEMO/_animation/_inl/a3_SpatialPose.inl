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
	
	a3_SpatialPose.inl
	Implementation of inline spatial pose operations.
*/


#ifdef __ANIMAL3D_SPATIALPOSE_H
#ifndef __ANIMAL3D_SPATIALPOSE_INL
#define __ANIMAL3D_SPATIALPOSE_INL


//-----------------------------------------------------------------------------

// initialize a spacial pose with default values
inline a3i32 a3spatialPoseCreate(a3_SpatialPose* spatialPose)
{
	// defaults: transform = identity, rotation channels = 0, scale components = +1, translation channels = 0
	if (spatialPose)
	{
		spatialPose->transform = a3mat4_identity;
		spatialPose->rotation = (a3vec3){ 0, 0, 0 };
		spatialPose->scale = (a3vec3){ 1, 1, 1 };
		spatialPose->position = (a3vec3){ 0, 0, 0 };

		return 1;
	}

	return -1;
}

// set rotation values for a single node pose
//example of its use
//a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		//rotation may need to be in Euler angles, I am not fully sure so just doing this for now
		spatialPose->rotation = (a3vec3){ rx_degrees, ry_degrees, rz_degrees };
	}
	return -1;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale = (a3vec3){ sx, sy, sz };
	}
	return -1;
}

// translation
// 
// example of its use
//a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->position = (a3vec3){ tx,ty,tz };
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		// defaults: transform = identity, rotation channels = 0, scale components = +1, translation channels = 0
		spatialPose->transform = a3mat4_identity;
		spatialPose->rotation = (a3vec3){ 0, 0, 0 };
		spatialPose->scale = (a3vec3){ 1, 1, 1 };
		spatialPose->position = (a3vec3){ 0, 0, 0 };
	}
	return -1;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{

	}
	return -1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		spatialPose_out->position = spatialPose_in->position;
		spatialPose_out->rotation = spatialPose_in->rotation;
		spatialPose_out->scale = spatialPose_in->scale;
		spatialPose_out->transform = spatialPose_in->transform;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H