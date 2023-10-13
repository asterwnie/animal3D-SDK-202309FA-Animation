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
	
	a3_Kinematics.c
	Implementation of kinematics solvers.
*/

#include "../a3_Kinematics.h"


//-----------------------------------------------------------------------------

// partial FK solver
a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy && 
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: finish implementing forward kinematics algorithm
		
		//	- for all nodes starting at first index
		a3i32 i, n, j, p; // j is joint index we are currently evaluating; p is the parent index
		for (i = firstIndex, n = firstIndex + nodeCount;
			i < n; ++i) // iterate through all the nodes we're asking for in the range
		{
			const a3_HierarchyNode* node = hierarchyState->hierarchy->nodes + i;
			j = node->index;
			p = node->parentIndex;
			//		- if node is not root (has parent node)
			if (p >= 0)
			{
				//	- object matrix = parent object matrix * local matrix
				a3real4x4Product(hierarchyState->objectSpace.spatialPose[j].transform.m, //object matrix = 
					hierarchyState->objectSpace.spatialPose[p].transform.m, //parent object matrix *
					hierarchyState->localSpace.spatialPose[j].transform.m); //local matrix
			}
			//	- else
			else
			{
				//	- copy local matrix to object matrix
				a3real4x4SetReal4x4(hierarchyState->objectSpace.spatialPose[j].transform.m, //object matrix
					hierarchyState->localSpace.spatialPose[j].transform.m); //= local matrix

			}
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------

// partial IK solver
a3i32 a3kinematicsSolveInversePartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy &&
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement inverse kinematics algorithm
		//	- for all nodes starting at first index
		//		- if node is not root (has parent node)
		//			- local matrix = inverse parent object matrix * object matrix
		//		- else
		//			- copy object matrix to local matrix
	}
	return -1;
}


//-----------------------------------------------------------------------------
