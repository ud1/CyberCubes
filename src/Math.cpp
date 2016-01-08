#include "Math.hpp"

RotationMatrices::RotationMatrices()
{
	matrices[0] = math::mat3(1.0f);

	math::mat3 rotateZ90 = math::mat3(0, 1, 0,  -1, 0, 0,  0, 0, 1);

	for (int i = 1; i < 4; ++i)
		matrices[i] = matrices[i - 1] * rotateZ90;

	math::mat3 rotateXP90 = math::mat3(1, 0, 0,  0, 0, 1,  0, -1, 0);

	for (int i = 0; i < 4; ++i)
	{
		matrices[4 + i] = rotateXP90 * matrices[i];
	}

	math::mat3 rotateYP90 = math::mat3(0, 0, -1,  0, 1, 0,  1, 0, 0);

	for (int i = 0; i < 4; ++i)
	{
		matrices[8 + i] = rotateYP90 * matrices[i];
	}

	math::mat3 rotateXN90 = math::mat3(1, 0, 0,  0, 0, -1,  0, 1, 0);

	for (int i = 0; i < 4; ++i)
	{
		matrices[12 + i] = rotateXN90 * matrices[i];
	}

	math::mat3 rotateYN90 = math::mat3(0, 0, 1,  0, 1, 0,  -1, 0, 0);

	for (int i = 0; i < 4; ++i)
	{
		matrices[16 + i] = rotateYN90 * matrices[i];
	}

	math::mat3 rotateXP180 = rotateXP90 * rotateXP90;

	for (int i = 0; i < 4; ++i)
	{
		matrices[20 + i] = rotateXP180 * matrices[i];
	}

	math::mat3 neg = math::mat3(1.0) * (-1.0f);

	for (int i = 0; i < 24; ++i)
	{
		matrices[24 + i] = neg * matrices[i];
	}

	for (int i = 0; i < 48; ++i)
	{
		invMatrices[i] = math::inverse(matrices[i]);

		for (int dir = 0; dir < 6; ++dir)
		{
			math::vec3 v = dirToVec((Dir) dir);
			v = invMatrices[i] * v;
			invDirs[i * 6 + dir] = vecToDir(v);
		}
	}

	quadRotations[(int) Dir::XN] = 4;
	quadRotations[(int) Dir::XP] = 0;
	quadRotations[(int) Dir::YN] = 0;
	quadRotations[(int) Dir::YP] = 4;
	quadRotations[(int) Dir::ZN] = 4;
	quadRotations[(int) Dir::ZP] = 0;

	const math::mat2 texMat[8] = {math::mat2(1, 0, 0, 1), math::mat2(0, -1, 1, 0), math::mat2(-1, 0, 0, -1), math::mat2(0, 1, -1, 0),  math::mat2(-1, 0, 0, 1), math::mat2(0, -1, -1, 0), math::mat2(1, 0, 0, -1), math::mat2(0, 1, 1, 0)};

	math::vec3 unitX(1.0f, 0.0f, 0.0f);
	math::vec3 unitY(0.0f, 1.0f, 0.0f);
	math::vec3 unitZ(0.0f, 0.0f, 1.0f);

	for (int i = 1; i < 48; ++i)
	{
		for (int dir = 0; dir < 6; ++dir)
		{
			quadRotations[i * 6 + dir] = 0;

			Dir origDir = invDirs[i * 6 + dir];
			math::mat3 origMat = textureMatrixToMat3(math::inverse(texMat[quadRotations[(int) origDir]]), origDir);
			math::vec3 destUnitX = matrices[i] * origMat * unitX;
			math::vec3 destUnitY = matrices[i] * origMat * unitY;
			math::vec3 destUnitZ = matrices[i] * origMat * unitZ;
			math::vec3 destUnit[2];
			int j = 0;

			if (math::length(destUnitX) > 0.9)
				destUnit[j++] = destUnitX;

			if (math::length(destUnitY) > 0.9)
				destUnit[j++] = destUnitY;

			if (math::length(destUnitZ) > 0.9)
				destUnit[j++] = destUnitZ;

			assert(j == 2);

			int k = 0;

			for (; k < 8; ++k)
			{
				math::vec3 testUnitX = textureMatrixToMat3(math::inverse(texMat[k]), (Dir) dir) * unitX;
				math::vec3 testUnitY = textureMatrixToMat3(math::inverse(texMat[k]), (Dir) dir) * unitY;
				math::vec3 testUnitZ = textureMatrixToMat3(math::inverse(texMat[k]), (Dir) dir) * unitZ;
				math::vec3 testUnit[2];
				j = 0;

				if (math::length(testUnitX) > 0.9)
					testUnit[j++] = testUnitX;

				if (math::length(testUnitY) > 0.9)
					testUnit[j++] = testUnitY;

				if (math::length(testUnitZ) > 0.9)
					testUnit[j++] = testUnitZ;

				assert(j == 2);

				if (testUnit[0] == destUnit[0] && testUnit[1] == destUnit[1])
				{
					quadRotations[i * 6 + dir] = k;
					break;
				}
			}

			assert(k < 8);
		}
	}
}

const RotationMatrices &RotationMatrices::getInstance()
{
	static RotationMatrices instance;
	return instance;
}

static math::vec3 truncateVec3XY(const math::vec3 &v)
{
	float absX = std::abs(v.x);
	float absY = std::abs(v.y);
		
	math::vec3 result;
	if (absX > absY)
		result = (v.x > 0.0f) ? math::vec3(1.0f, 0.0f, 0.0f) : math::vec3(-1.0f, 0.0f, 0.0f);
	else
		result = (v.y > 0.0f) ? math::vec3(0.0f, 1.0f, 0.0f) : math::vec3(0.0f, -1.0f, 0.0f);
		
	return result;
}

static math::vec3 truncateVec3XYZ(const math::vec3 &v)
{
	float absX = std::abs(v.x);
	float absY = std::abs(v.y);
	float absZ = std::abs(v.z);
	
	math::vec3 result;
	if (absX >= absY && absX >= absZ)
		result = (v.x > 0.0f) ? math::vec3(1.0f, 0.0f, 0.0f) : math::vec3(-1.0f, 0.0f, 0.0f);
	else if (absY >= absX && absY >= absZ)
		result = (v.y > 0.0f) ? math::vec3(0.0f, 1.0f, 0.0f) : math::vec3(0.0f, -1.0f, 0.0f);
	else
		result = (v.z > 0.0f) ? math::vec3(0.0f, 0.0f, 1.0f) : math::vec3(0.0f, 0.0f, -1.0f);
	
	return result;
}

int getRotation(const math::vec3 &cameraFwDir, const math::vec3 &cameraUpDir, int rotationGroup)
{
	if (rotationGroup == 1)
		return 0;
	
	math::vec3 fw = math::vec3(0.0f, 1.0f, 0.0f);
	math::vec3 r = math::vec3(1.0f, 0.0f, 0.0f);
	
	if (rotationGroup == 4)
	{
		math::vec3 targetDir = truncateVec3XY(cameraFwDir);
		for (int i = 0; i < 4; ++i)
		{
			math::vec3 testDir = RotationMatrices::getInstance().matrices[i] * fw;
			if (testDir == targetDir)
				return i;
		}
	}
	
	if (rotationGroup > 4)
	{
		math::vec3 targetFwDir = truncateVec3XYZ(cameraFwDir);
		math::vec3 targetRDir = truncateVec3XYZ(math::cross(targetFwDir, cameraUpDir));
		
		for (int i = 0; i < 24; ++i)
		{
			math::vec3 testFwDir = RotationMatrices::getInstance().matrices[i] * fw;
			math::vec3 testRDir = RotationMatrices::getInstance().matrices[i] * r;
			if (testFwDir == targetFwDir && testRDir == targetRDir)
				return i;
		}
	}
	
	return 0;
}