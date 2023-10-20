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

#include <math.h>
//-----------------------------------------------------------------------------

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		spatialPose->angles.x = a3trigValid_sind(rx_degrees);
		spatialPose->angles.y = a3trigValid_sind(ry_degrees);
		spatialPose->angles.z = a3trigValid_sind(rz_degrees);
		return 1;
	}
	return -1;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale.x = sx;
		spatialPose->scale.y = sy;
		spatialPose->scale.z = sz;
		return 1;
	}
	return -1;
}

// translation
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->translation.x = tx;
		spatialPose->translation.y = ty;
		spatialPose->translation.z = tz;
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		spatialPose->transform = a3mat4_identity;
		spatialPose->orientation = a3vec4_w;
		spatialPose->angles = a3vec4_zero;
		spatialPose->scale = a3vec4_one;
		spatialPose->translation = a3vec4_w;
		return 1;
	}
	return -1;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3_SpatialPose* spatialPose, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (spatialPose)
	{
		a3mat4 rx, ry, rz, r;
		a3real4x4SetRotateX(rx.m, spatialPose->angles.x);
		a3real4x4SetRotateY(ry.m, spatialPose->angles.y);
		a3real4x4SetRotateZ(rz.m, spatialPose->angles.z);
		switch (order)
		{
		case a3poseEulerOrder_xyz:
			a3real4x4Product(r.m, rx.m, ry.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rz.m);
			break;
		case a3poseEulerOrder_yzx:
			a3real4x4Product(r.m, ry.m, rz.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rx.m);
			break;
		case a3poseEulerOrder_zxy:
			a3real4x4Product(r.m, rz.m, rx.m);
			a3real4x4Product(spatialPose->transform.m, r.m, ry.m);
			break;
		case a3poseEulerOrder_yxz:
			a3real4x4Product(r.m, ry.m, rx.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rz.m);
			break;
		case a3poseEulerOrder_xzy:
			a3real4x4Product(r.m, rx.m, rz.m);
			a3real4x4Product(spatialPose->transform.m, r.m, ry.m);
			break;
		case a3poseEulerOrder_zyx:
			a3real4x4Product(r.m, rz.m, ry.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rx.m);
			break;
		}
		a3real3MulS(spatialPose->transform.v0.v, spatialPose->scale.x);
		a3real3MulS(spatialPose->transform.v1.v, spatialPose->scale.y);
		a3real3MulS(spatialPose->transform.v2.v, spatialPose->scale.z);
		spatialPose->transform.v3 = spatialPose->translation;
		return 1;
	}
	return -1;
}

// restore single node pose from matrix
inline a3i32 a3spatialPoseRestore(a3_SpatialPose* spatialPose, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (spatialPose)
	{

	}
	return -1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		*spatialPose_out = *spatialPose_in;
		return 1;
	}
	return -1;
}

// concat
inline a3i32 a3spatialPoseConcat(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_lhs, const a3_SpatialPose* spatialPose_rhs)
{
	if (spatialPose_out && spatialPose_lhs && spatialPose_rhs)
	{
		spatialPose_out->angles.x = a3trigValid_sind(spatialPose_lhs->angles.x + spatialPose_rhs->angles.x);
		spatialPose_out->angles.y = a3trigValid_sind(spatialPose_lhs->angles.y + spatialPose_rhs->angles.y);
		spatialPose_out->angles.z = a3trigValid_sind(spatialPose_lhs->angles.z + spatialPose_rhs->angles.z);

		spatialPose_out->scale.x = spatialPose_lhs->scale.x * spatialPose_rhs->scale.x;
		spatialPose_out->scale.y = spatialPose_lhs->scale.y * spatialPose_rhs->scale.y;
		spatialPose_out->scale.z = spatialPose_lhs->scale.z * spatialPose_rhs->scale.z;

		spatialPose_out->translation.x = spatialPose_lhs->translation.x + spatialPose_rhs->translation.x;
		spatialPose_out->translation.y = spatialPose_lhs->translation.y + spatialPose_rhs->translation.y;
		spatialPose_out->translation.z = spatialPose_lhs->translation.z + spatialPose_rhs->translation.z;

		return 1;
	}
	return -1;
}

// lerp
inline a3i32 a3spatialPoseLerp(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_0, const a3_SpatialPose* spatialPose_1, const a3real u)
{
	if (spatialPose_out && spatialPose_0 && spatialPose_1)
	{
		a3real3Lerp(spatialPose_out->angles.v, spatialPose_0->angles.v, spatialPose_1->angles.v, u);

		a3real3Lerp(spatialPose_out->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, u);

		a3real3Lerp(spatialPose_out->translation.v, spatialPose_0->translation.v, spatialPose_1->translation.v, u);

		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------
//identity
a3_SpatialPose* spatialPoseIdentity(a3_SpatialPose* pose_out)
{
	return pose_out;
}

//Construct
//creates spatial/hierarchical poses with the given transform
a3_SpatialPose* spatialPoseConstruct(a3_SpatialPose* pose_out, a3vec4 translation, a3vec4 rotation, a3vec4 scale)
{
	pose_out->translation = translation;
	pose_out->angles = rotation;
	pose_out->scale = scale;
	return pose_out;
}

//Buffer/Copy
//returns pose_out + 0 / * 1
a3_SpatialPose* spatialPoseCopy(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in)
{
	//use a3spatialPoseCopy
	return pose_out;
}

//Negate/Invert
//returns pose_out * -1 / ^-1
a3_SpatialPose* spatialPoseInvert(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in)
{
	//translation - additive:
	pose_out->translation.x = pose_in->translation.x * -1;
	pose_out->translation.y = pose_in->translation.y * -1;
	pose_out->translation.z = pose_in->translation.z * -1;

	//rotation - additive:
	pose_out->angles.x = pose_in->angles.x * -1;
	pose_out->angles.y = pose_in->angles.y * -1;
	pose_out->angles.z = pose_in->angles.z * -1;

	//scale - multiplicative:
	pose_out->scale.x = pow(pose_in->scale.x, -1);
	pose_out->scale.y = pow(pose_in->scale.y, -1);
	pose_out->scale.z = pow(pose_in->scale.z, -1);


	return pose_out;
}

//Concatenate / Add
a3_SpatialPose* spatialPoseAdd(a3_SpatialPose* pose_out, a3_SpatialPose* leftHand, a3_SpatialPose* rightHand)
{
	//use a3spatialPoseConcat
	return pose_out;
}

//Scale / Mul
a3_SpatialPose* spatialPoseMul(a3_SpatialPose* pose_out, a3_SpatialPose* leftHand, a3_SpatialPose* rightHand)
{
	//translation - additive:
	pose_out->translation.x = leftHand->translation.x * rightHand->translation.x;
	pose_out->translation.y = leftHand->translation.y * rightHand->translation.y;
	pose_out->translation.z = leftHand->translation.z * rightHand->translation.z;

	//rotation - additive:
	pose_out->angles.x = a3trigValid_sind(leftHand->angles.x * rightHand->angles.x);
	pose_out->angles.y = a3trigValid_sind(leftHand->angles.y * rightHand->angles.y);
	pose_out->angles.z = a3trigValid_sind(leftHand->angles.z * rightHand->angles.z);

	//scale - multiplicative:
	pose_out->scale.x = pow(leftHand->scale.x, rightHand->scale.x);
	pose_out->scale.y = pow(leftHand->scale.y, rightHand->scale.y);
	pose_out->scale.z = pow(leftHand->scale.z, rightHand->scale.z);

	
	return pose_out;
}

//Deconcat / Sub
//
a3_SpatialPose* spatialPoseSub(a3_SpatialPose* pose_out, a3_SpatialPose* leftHand, a3_SpatialPose* rightHand)
{
	a3_SpatialPose* temp;
	pose_out = spatialPoseAdd(pose_out, leftHand, spatialPoseInvert(temp, rightHand));
	return pose_out;
}

//De-scale / Div
a3_SpatialPose* spatialPoseDiv(a3_SpatialPose* pose_out, a3_SpatialPose* leftHand, a3_SpatialPose* rightHand)
{
	a3_SpatialPose* temp;
	a3spatialPoseCopy(temp, rightHand);

	temp->translation.x = pow(temp->translation.x, -1);
	temp->translation.y = pow(temp->translation.y, -1);
	temp->translation.z = pow(temp->translation.z, -1);


	temp->scale.x = pow(temp->scale.x, -1);
	temp->scale.y = pow(temp->scale.y, -1);
	temp->scale.z = pow(temp->scale.z, -1);


	temp->angles.x = pow(temp->angles.x, -1);
	temp->angles.y = pow(temp->angles.y, -1);
	temp->angles.z = pow(temp->angles.z, -1);
	
	pose_out = spatialPoseMul(pose_out, leftHand, temp);
	return pose_out;
}

//Interpolate
a3_SpatialPose* spatialPoseLerp(a3_SpatialPose* pose_out, a3_SpatialPose* pose_0, a3_SpatialPose* pose_1, a3real u)
{
	//use a3spatialPoseLerp

	return pose_out;
}

//Un-interpolate
a3_SpatialPose* spatialPoseUnLerp(a3_SpatialPose* pose_out, a3_SpatialPose* leftHand, a3_SpatialPose* rightHand, a3real u)
{
	
	return pose_out;
}

//Integrate
a3_SpatialPose* spatialPoseIntegrate(a3_SpatialPose* pose_out)
{

}

//Differentiate
a3_SpatialPose* spatialPoseDifferentiate(a3_SpatialPose* pose_out)
{

}








//-------------------------------------------------------------------------------

#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H