#version 330

layout(location = 0) out vec4 outputData1;
layout(location = 1) out vec4 outputData2;
//layout(location = 2) out vec4 outputData3;
//layout(location = 3) out int outputMaterial;
in vec4 viewSpace;
in vec2 relCoord;
in vec2 texCoord;
in vec4 shadowCoord1;
in vec4 shadowCoord2;
in vec3 fcoord;
flat in vec2 fcolor;
flat in int ftextureId;
flat in mat2 fglightMat;
flat in mat2 fslightMat;
flat in float fdotNormalSunLight;
flat in vec3 boxPos;

uniform vec3 sunLightDirInv;
uniform float fogFar;
uniform sampler2DArray blockSampler;
uniform sampler2D detailSampler;
uniform sampler2DShadow shadowSampler1;
uniform sampler2DShadow shadowSampler2;
uniform sampler2D shadowBoxPosSampler1;
uniform sampler2D shadowBoxPosSampler2;
uniform int mode;

bool inside01(vec2 v)
{
    return v.x > 0.01 && v.y > 0.01 && v.x < 0.99 && v.y < 0.99;   
}

bool intersection(vec3 boxCenter, vec3 r0)
{
    vec3 relPos = boxCenter - r0;
    vec3 t1 = ((relPos - vec3(0.5, 0.5, 0.5))) * sunLightDirInv;
    vec3 t2 = ((relPos + vec3(0.5, 0.5, 0.5))) * sunLightDirInv;
    vec3 mint1t2 = min(t1, t2);
    vec3 maxt1t2 = max(t1, t2);
    float tmin = max(mint1t2.x, max(mint1t2.y, mint1t2.z));
    float tmax = min(maxt1t2.x, min(maxt1t2.y, maxt1t2.z));
    return tmax >= tmin && tmax > 0.001;
    //return dot(boxCenter - r0, ray) > 0;
}

vec2 dirs[4] = vec2[](
	vec2(1, 0),
	vec2(-1, 0),
	vec2(0, 1),
	vec2(0, -1)
);

bool isGeomShadow(sampler2D shadowBoxPosSampler, vec4 shadowCoord)
{
	vec3 shadowBoxPos = texture(shadowBoxPosSampler, shadowCoord.xy).xyz;
	if (boxPos != shadowBoxPos && intersection(shadowBoxPos, fcoord))
	{
		return true;
	}
	
	vec2 mapSizeInv = 1.0/textureSize(shadowBoxPosSampler, 0).xy;
	
	for (int d = 0; d < 4; ++d)
	{
		vec2 shTexCoord = shadowCoord.xy + dirs[d]*mapSizeInv;
		vec3 shadowBoxPos2 = texture(shadowBoxPosSampler, shTexCoord).xyz;
		if (boxPos != shadowBoxPos2 && shadowBoxPos != shadowBoxPos2 && intersection(shadowBoxPos2, fcoord))
		{
			return true;
		}
	}
	
	return false;
}

void main()
{
	float dist = length(viewSpace);
	float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	
	vec2 r1 = vec2(1 - relCoord.x, relCoord.x);
	vec2 r2 = vec2(1 - relCoord.y, relCoord.y);
	float slight = dot(r1, fslightMat * r2);
	float glight = dot(r1, fglightMat * r2);
	
	//outputData1 = vec4(fogFactor, texCoord, 0);
	
	vec3 detailColor = texture(detailSampler, vec2(texCoord.x, texCoord.y)*5).rgb - vec3(0.5, 0.5, 0.5);
	vec2 shift = detailColor.rg * 0.03;
	vec3 detailColor2 = texture(detailSampler, vec2(texCoord.x, texCoord.y) + shift).rgb - vec3(0.5, 0.5, 0.5);
	//vec3 tex_color = textureGrad(blockSampler, vec3(texCoord.x + shift.x, texCoord.y + shift.y, materialId - 1), ddxy.xy, ddxy.zw).rgb * (vec3(1.0, 1.0, 1.0) + detailColor2 * 0.4);
	
	
	float visibility = 1.0;
	
	if (mode != 4)
	{
		if (fdotNormalSunLight < 0)
		{
			visibility = 0.0f;
		}
		else
		{
			bool innerMap = inside01(shadowCoord1.xy);
			if (mode == 0 || mode == 2 || mode == 3)
			{
				if (innerMap)
					visibility = texture( shadowSampler1, vec3(shadowCoord1.xy, (shadowCoord1.z)/shadowCoord1.w) );
				else
					visibility = texture( shadowSampler2, vec3(shadowCoord2.xy, (shadowCoord2.z)/shadowCoord2.w) );
			}
			
			if ((mode == 1 || mode == 2 || mode == 3) && visibility > 0)
			{
				if (mode == 3)
					visibility = 1;
				
				if (innerMap && isGeomShadow(shadowBoxPosSampler1, shadowCoord1))
					visibility = 0.0;
					
				if (!innerMap && isGeomShadow(shadowBoxPosSampler2, shadowCoord2))
					visibility = 0.0;
			}
			visibility = 2*visibility*fdotNormalSunLight;
			//visibility = visibility*fdotNormalSunLight;
			
			//vec2 tcoord = floor(shadowCoord2.xy*textureSize(shadowBoxPosSampler1, 0).xy);
			//visibility = 0.7*visibility + 0.3*fract((tcoord.x + tcoord.y)/2);
		}
		
		visibility = visibility + 1;
	}

	outputData1 = vec4(fogFactor, texture(blockSampler, vec3(texCoord + shift, ftextureId - 1)).rgb * (vec3(1.0, 1.0, 1.0) + detailColor2 * 0.4) * visibility);
	
	outputData2 = vec4(glight, slight, fcolor);
	//outputData3 = vec4(dFdx(texCoord), dFdy(texCoord));
	//outputMaterial = ftextureId;
}
