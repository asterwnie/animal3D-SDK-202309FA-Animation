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
	
	passTangentBasis_morph5_transform_vs4x.glsl
	Calculate and pass tangent basis with morphing.
*/

#version 450

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aNormal;		// usually 2
layout (location = 2) in vec4 aTangent;		// usually 10
//layout (location = 2) in vec4 aBitangent;	// usually 11
layout (location = 15) in vec4 aTexcoord;	// usually 8

// these read from a buffer (made elsewhere) that has the
// 5 different teapot models
// these allow the data to live on the GPU!
// remember we can only have 16 attributes
layout (location = 3) in vec4 aPosition1;
layout (location = 4) in vec4 aNormal1;		
layout (location = 5) in vec4 aTangent1;
layout (location = 6) in vec4 aPosition2;
layout (location = 7) in vec4 aNormal2;		
layout (location = 8) in vec4 aTangent2;
layout (location = 9) in vec4 aPosition3;
layout (location = 10) in vec4 aNormal3;		
layout (location = 11) in vec4 aTangent3;
layout (location = 12) in vec4 aPosition4;
layout (location = 13) in vec4 aNormal4;		
layout (location = 14) in vec4 aTangent4;

uniform mat4 uP;
uniform mat4 uMV, uMV_nrm;
uniform mat4 uAtlas;
uniform double uTime; // for simplicity, we're doing 1 second = 1 keyframe

out vbVertexData {
	mat4 vTangentBasis_view;
	vec4 vTexcoord_atlas;
};

flat out int vVertexID;
flat out int vInstanceID;

vec4 lerp(in vec4 v0, in vec4 v1, in float u);
vec4 nlerp(in vec4 v0, in vec4 v1, in float u);
vec4 CatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);
vec4 nCatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
	//	gl_Position = aPosition;

	// build our arrays in GLSL
	// these are effectively keyframe "pools"
	vec4[5] positionArr = vec4[5](
		aPosition, aPosition1, aPosition2, aPosition3, aPosition4
	);
	vec4[5] normalArr = vec4[5](
		aNormal, aNormal1, aNormal2, aNormal3, aNormal4
	);
	vec4[5] tangentArr = vec4[5](
		aTangent, aTangent1, aTangent2, aTangent3, aTangent4
	);

	// FAKE clip controller data: 1fps (k for current keyframe index
	// that you would read in from your clip controller)
	int k0 = int(uTime) % 5; // cast time to an int (mod 5 to loop)
	int k1 = (k0 + 1) % 5; // the next keyframe (so we can lerp between keyframes)
	int kN = (k1 + 1) % 5; // next for Catmull-Rom Interpolation
	int kP = (k0 - 2 + 5) % 5; // prev for Catmull-Rom Interpolation

	//[0.1) result of clip controller update (post-resolve)
	float param = float(uTime) - float(int(uTime)); //fract(float(uTime));

	// catmullrom interpolate from keyframe0 and keyframe1
	vec4 position = CatmullRom(positionArr[kP], positionArr[k0], 
					positionArr[k1], positionArr[kN], param);
	vec4 normal = nCatmullRom(normalArr[kP], normalArr[k0], 
					normalArr[k1], normalArr[kN], param); // for norms, use normalized lerp
	vec4 tangent = nCatmullRom(tangentArr[kP], tangentArr[k0], 
					tangentArr[k1], tangentArr[kN], param); // for tangent, use normalized lerp
	vec4 bitangent = vec4(cross(normal.xyz, tangent.xyz), 0.0);
	
	vTangentBasis_view = uMV_nrm * mat4(tangent, bitangent, normal, vec4(0.0));
	vTangentBasis_view[3] = uMV * position;
	gl_Position = uP * vTangentBasis_view[3];
	
	vTexcoord_atlas = uAtlas * aTexcoord;

	vVertexID = gl_VertexID;
	vInstanceID = gl_InstanceID;
}
