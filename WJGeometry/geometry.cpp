#include "geometry.h"
#include <iostream>

Matrix4 Camera::GenerateLookAtMatrix()
{
	float moveArray[4][4] = {
		1			,0			,0			,0,
		0			,1			,0			,0,
		0			,0			,1			,0,
		-vectorPosition.fX	,-vectorPosition.fY,-vectorPosition.fZ,1
	};
	Matrix4 moveMatrix = moveArray;
	
	float RotateArray[4][4] =
	{
		vectorRight.fX,	vectorUp.fX,		vectorEye.fX,		0,
		vectorRight.fY,	vectorUp.fY,		vectorEye.fY,		0,
		vectorRight.fZ,	vectorUp.fZ,		vectorEye.fZ,		0,
		0,			0,			0,			1
	};
	Matrix4 rotateMatrix = RotateArray;
	return moveMatrix * rotateMatrix;
}

IntersectionInfo GetNearestIntersectionInfo(Scene & scene, Ray & ray)
{
	IntersectionInfo*		inf = 0;
	IntersectionInfo		nearestInf;
	float					t = -1.0f;

	nearestInf.bIsHit = false;
	nearestInf.fDistance = -1.0f;

	for (int i = 0; i < scene.nSphereCnt; i++)
	{
		scene.sphereList[i].CalcRayIntersectionInfo(ray, &inf);
		if (inf == NULL)
			continue;
		
		if (t < 0.0f)
		{
			t = inf->fDistance;
			if (nearestInf.bIsHit == true && nearestInf.fDistance > inf->fDistance)
				nearestInf = *inf;
			else
				nearestInf = *inf;
		}
		else if (t > inf->fDistance)
		{
			t = inf->fDistance;
			nearestInf = *inf;
		}
	}

	for (int i = 0; i < scene.nFloorCnt; ++i)
	{
		scene.floors[i].CalcRayIntersectionInfo(ray, &inf);
		if (inf == NULL)
			continue;
		
		if (t < 0.0f)
		{
			t = inf->fDistance;
			if (nearestInf.bIsHit == true && nearestInf.fDistance > inf->fDistance)
				nearestInf = *inf;
			else
				nearestInf = *inf;
		}
		else if (t > inf->fDistance)
		{
			t = inf->fDistance;
			nearestInf = *inf;
		}
	}
	if (nearestInf.fDistance < .0f)
		nearestInf.bIsHit = false;
	
	return nearestInf;
}

bool IsSamePosition(Vector4 position1, Vector4 position2, float distance)
{
	float	fDeviation = distance * POINT_DEVIATION;
	bool	result = false;

	result = 
		((fabs(position1.fX - position2.fX)) < fDeviation) &&
		((fabs(position1.fY - position2.fY)) < fDeviation) &&
		((fabs(position1.fZ - position2.fZ)) < fDeviation);

	return result;
}

color_t RayTrace(Ray& eyeRay, Scene& scene, unsigned int maxRayTraceDepth, float refractionRatio)
{
	IntersectionInfo nearestInf = GetNearestIntersectionInfo(scene, eyeRay);
	if (nearestInf.bIsHit == false)
	{
		return DEFAULT_BACKCOLOR;
	}
	//if (nearestInf.position.length() > scene.mainCamera.visibleDistance)
	//{
	//	return DEFAULT_BACKCOLOR;
	//}
	PointLight* VisibleLightArray[8] = { 0 };
	int VisibleLightCount = 0;
	IntersectionInfo lightIntersectionInf;
	Ray lightRay;
	for (int i = 0; i < scene.nLightCnt; ++i)
	{
		Vector4 lightDir = nearestInf.position - scene.lightList[i].position;
		lightDir.ResetUnitVector();
		lightRay.direction = lightDir;
		lightRay.position = scene.lightList[i].position;
		lightIntersectionInf = GetNearestIntersectionInfo(scene, lightRay);
		if (IsSamePosition(lightIntersectionInf.position, nearestInf.position, lightIntersectionInf.fDistance))
		{
			VisibleLightArray[VisibleLightCount++] = &scene.lightList[i];
		}
	}
	color_t resultColor = 0x0;

	color_t reflectColor = 0x0;
	for (int i = 0; i < VisibleLightCount; ++i)
	{
		resultColor.ColorAdd(ApplyLight(*VisibleLightArray[i], nearestInf, eyeRay));
	}

	if (maxRayTraceDepth > 0 && nearestInf.material.fReflectiveness > 0.0f)
	{
		Ray reflectRay;
		reflectRay.direction = eyeRay.direction - nearestInf.normal *(nearestInf.normal * eyeRay.direction) * 2;
		reflectRay.position = nearestInf.position - eyeRay.direction * REFLECT_DEVIATION;
		reflectColor = RayTrace(reflectRay, scene, maxRayTraceDepth - 1, nearestInf.material.fRefractionRatio);
	}
	else
	{
		reflectColor = 0x0;
	}

	color_t refractionColor = 0x0;
	if (maxRayTraceDepth > 0 && nearestInf.material.fRefractiveness > 0.0f)
	{
		Ray refractionRay;
		if (nearestInf.bIsInner)
		{
			nearestInf.material.fRefractionRatio = DEFAULT_REFRACTIONRATIO;
		}
		float LdivideT = refractionRatio / nearestInf.material.fRefractionRatio;
		float NdotL = -nearestInf.normal * eyeRay.direction;
		float refractionTemp = 1 - ((1 - NdotL * NdotL) * LdivideT * LdivideT);
		if (refractionTemp > .0f)
		{
			refractionRay.direction = nearestInf.normal * (LdivideT * NdotL - sqrtf(refractionTemp)) +
				eyeRay.direction * LdivideT;
			refractionRay.position = nearestInf.position + eyeRay.direction * REFLECT_DEVIATION;
			refractionRay.direction.ResetUnitVector();
			refractionColor = RayTrace(refractionRay, scene, maxRayTraceDepth - 1, nearestInf.material.fRefractionRatio);
		}
	}

	resultColor.ColorMutiply(1 - nearestInf.material.fReflectiveness - nearestInf.material.fRefractiveness);
	reflectColor.ColorMutiply(nearestInf.material.fReflectiveness);
	resultColor.ColorAdd(reflectColor);
	refractionColor.ColorMutiply(nearestInf.material.fRefractiveness);
	resultColor.ColorAdd(refractionColor);

	return resultColor;

}

Ray GenerateRay(float x, float y, float fovAngle, float aspect)
{
	Ray result;
	float fovScale = tanf(fovAngle * PI / 360.0f);
	float sx = (x - 0.5f) * fovScale * 2;
	float sy = (0.5f - y) * fovScale * 2 * aspect;
	result.position.fX = .0f;
	result.position.fY = .0f;
	result.position.fZ = .0f;
	result.position.fW = 1.0f;
	result.direction.fX = sx;
	result.direction.fY = sy;
	result.direction.fZ = 1.0f;
	result.direction.fW = .0f;
	result.direction.ResetUnitVector();
	return result;
}

color_t ApplyLight(PointLight & light, IntersectionInfo & intersectionInf, Ray& ray)
{
	color_t result = 0x0;
	color_t Diffuse = 0x0;
	color_t Specular = 0x0;
	color_t Ambient = 0x0;

	Vector4 lightDir = intersectionInf.position - light.position;
	lightDir.ResetUnitVector();
	Vector4 reflectDir = lightDir - intersectionInf.normal *(intersectionInf.normal * lightDir) * 2;
	reflectDir.ResetUnitVector();
	Vector4 eyeDir = -ray.direction;
	eyeDir.ResetUnitVector();

	Ambient = light.Ambient;
	Ambient.ColorModulate(intersectionInf.material.Ambient);
	Ambient.ColorModulate(intersectionInf.color);

	Diffuse = light.Diffuse;
	Diffuse.ColorModulate(intersectionInf.material.Diffuse);
	float k = fmax(0.0f, (-lightDir) * intersectionInf.normal);
	Diffuse.ColorModulate(intersectionInf.color);
	Diffuse.ColorMutiply(k);

	float SpecTemp = eyeDir * reflectDir;
	SpecTemp = (SpecTemp > 0.0f) ? SpecTemp : 0.0f;
	SpecTemp = pow(SpecTemp, light.fPower);
	Specular = light.Specular;
	Specular.ColorModulate(0xffffff);
	Specular.ColorMutiply(SpecTemp);

	result = Diffuse;
	result.ColorAdd(Ambient);
	result.ColorAdd(Specular);

	return result;
}

Sphere::Sphere():sphereColor(0.0f)
{
}

void Sphere::CalcRayIntersectionInfo(Ray& ray, IntersectionInfo ** pInf)
{
	Vector4 distance = ray.position - vectorPosition;
	distance.fW = 1.0f;
	float length = distance.length();
	bool isInsideSphere = (length < fRadius + POINT_DEVIATION) ? true : false;
	if (*pInf)
	{
		delete (*pInf);
		*pInf = 0;
	}
	float k = ray.direction * distance;
	float temp = k*k - (distance*distance - fRadius*fRadius);
	if (temp < 0)
	{
		return;
	}
	else
	{
		float t;
		if (!isInsideSphere)
		{
			t = -k - sqrt(temp);
		}
		else
		{
			t = -k + sqrt(temp);
		}
		if (t < POINT_DEVIATION)
		{
			return;
		}
		Vector4 intersectionPos = ray.position + ray.direction*t;
		intersectionPos.fW = 1.0f;
		*pInf = new IntersectionInfo();
		(*pInf)->fDistance = t;
		(*pInf)->position = intersectionPos;
		(*pInf)->color = sphereColor;
		(*pInf)->bIsHit = true;
		(*pInf)->material = mtrlSphere;
		(*pInf)->normal = (intersectionPos - vectorPosition);
		if (isInsideSphere)
		{
			(*pInf)->normal = -(*pInf)->normal;
			(*pInf)->bIsInner = true;
		}
		(*pInf)->normal.ResetUnitVector();
		return;
	}
}

Plane::Plane():planeColor(0.0f)
{
}

void Plane::CalcRayIntersectionInfo(Ray& ray, IntersectionInfo ** pInf)
{
	if (*pInf)
	{
		delete *pInf;
		*pInf = nullptr;
	}

	float temp = vectorNormal * ray.direction;
	if (temp < FLOAT_DEVIATION && temp > -FLOAT_DEVIATION)
	{
		return;
	}
	float t = (-fDistance - vectorNormal * ray.position) / temp;
	if (t > .0f)
	{
		*pInf = new IntersectionInfo();
		(*pInf)->fDistance = t;
		(*pInf)->position = ray.position + ray.direction * t;
		(*pInf)->position.fW = 1.0f;
		(*pInf)->color = SampleTextureMap((*pInf)->position.fX, (*pInf)->position.fZ);
		//(*pInf)->color = COLOR_WHITE;
		(*pInf)->bIsHit = true;
		(*pInf)->material = planeMaterial;
		(*pInf)->normal = vectorNormal;
	}
}

color_t Plane::SampleTextureMap(float fX, float fZ)
{
	if (fX > 1e5 || fX < -1e5 || fZ > 1e5 || fZ < -1e5)
		return color_t(0);

	int nX = (fX > 0) ? static_cast<int>(fX) : static_cast<int>(fX) - 1;
	int nZ = (fZ > 0) ? static_cast<int>(fZ) : static_cast<int>(fZ) - 1;

	color_t result = color_t(((nX + nZ) & 0x1) ? COLOR_WHITE : COLOR_BLACK);
	return result;
}

void InitScene(Scene& scene)
{
	// Scene init
	scene.nSphereCnt = 0;
	scene.nFloorCnt = 0;
	Sphere ball0 = Sphere();
	ball0.sphereColor = 0x0000ff00;
	ball0.mtrlSphere.Ambient = 0x00ffffff;
	ball0.mtrlSphere.Diffuse = 0x00ffffff;
	ball0.mtrlSphere.Specular = 0x00ffffff;
	ball0.mtrlSphere.Emissive = 0x0;
	ball0.mtrlSphere.fReflectiveness = .3f;
	ball0.mtrlSphere.fRefractionRatio = DEFAULT_REFRACTIONRATIO;
	ball0.mtrlSphere.fRefractiveness = .0f;
	ball0.vectorPosition.fX = -3.0f;
	ball0.vectorPosition.fY = -1.0f;
	ball0.vectorPosition.fZ = 3.0f;
	ball0.vectorPosition.fW = 1.0f;
	ball0.fRadius = 1.0f;
	scene.sphereList[scene.nSphereCnt++] = ball0;

	Sphere ball1 = Sphere();
	ball1.sphereColor = 0x00ff0000;
	ball1.mtrlSphere.Ambient = 0x00ffffff;
	ball1.mtrlSphere.Diffuse = 0x00ffffff;
	ball1.mtrlSphere.Specular = 0x00ffffff;
	ball1.mtrlSphere.Emissive = 0x0;
	ball1.mtrlSphere.fReflectiveness = .0f;
	ball1.mtrlSphere.fRefractionRatio = GLASS_REFRACTION;
	ball1.mtrlSphere.fRefractiveness = .5f;
	ball1.vectorPosition.fX = .0f;
	ball1.vectorPosition.fY = .0f;
	ball1.vectorPosition.fZ = 5.0f;
	ball1.vectorPosition.fW = 1.0f;
	ball1.fRadius = 2.0f;
	scene.sphereList[scene.nSphereCnt++] = ball1;

	Sphere ball2 = Sphere();
	ball2.sphereColor = 0x0000ff00;
	ball2.mtrlSphere.Ambient = 0x00ffffff;
	ball2.mtrlSphere.Diffuse = 0x00ffffff;
	ball2.mtrlSphere.Specular = 0x00ffffff;
	ball2.mtrlSphere.Emissive = 0x0;
	ball2.mtrlSphere.fReflectiveness = .0f;
	ball2.mtrlSphere.fRefractionRatio = GLASS_REFRACTION;
	ball2.mtrlSphere.fRefractiveness = 2.0f;
	ball2.vectorPosition.fX = 4.0f;
	ball2.vectorPosition.fY = -1.0f;
	ball2.vectorPosition.fZ = 3.0f;
	ball2.vectorPosition.fW = 1.0f;
	ball2.fRadius = 1.0f;
	scene.sphereList[scene.nSphereCnt++] = ball2;

	Plane plane = Plane();
	plane.planeColor = 0x000000ff;
	plane.vectorPlanePoint.fY = -2.0f;
	plane.vectorPlanePoint.fW = 1.0f;
	plane.vectorNormal.fY = 1.0f;
	plane.vectorNormal.fX = 0.0f;
	plane.vectorNormal.fZ = 0.0f;
	plane.vectorNormal.fW = 0.0f;
	plane.fDistance = 5.0f;
	plane.planeMaterial.Ambient = 0x00ffffff;
	plane.planeMaterial.Diffuse = 0x00ffffff;
	plane.planeMaterial.Emissive = 0x00ffffff;
	plane.planeMaterial.Specular = 0x00ffffff;
	plane.planeMaterial.fReflectiveness = .3f;
	plane.planeMaterial.fRefractionRatio = .0f;
	plane.planeMaterial.fRefractiveness = .0f;
	scene.floors[scene.nFloorCnt++] = plane;

	PointLight light = PointLight();
	light.Ambient = 0x202020;
	light.Diffuse = 0x808080;
	light.Specular = 0xffffff;
	light.fPower = 2.0f;
	light.position.fX = -1.0f;
	light.position.fY = 5.0f;
	light.position.fZ = 2.0f;
	light.position.fW = 1.0f;
	scene.lightList[scene.nLightCnt++] = light;

	PointLight light2 = PointLight();
	light2.Ambient = 0x202020;
	light2.Diffuse = 0x808080;
	light2.Specular = 0xffffff;
	light2.fPower = 32.0f;
	light2.position.fX = 4.0f;
	light2.position.fY = 3.0f;
	light2.position.fZ = -1.0f;
	light2.position.fW = 1.0f;
	scene.lightList[scene.nLightCnt++] = light2;

	InitCamera(scene.mainCamera);
}

void InitCamera(Camera & mainCamera)
{
	mainCamera.vectorPosition.fX = .0f;
	mainCamera.vectorPosition.fY = 0.0f;
	mainCamera.vectorPosition.fZ = -3.0f;
	mainCamera.vectorPosition.fW = .0f;
	mainCamera.fAspect = WIDTH / (float)HEIGHT;
	mainCamera.fFovy = 90.0f;
	mainCamera.vectorEye.fX = .0f;
	mainCamera.vectorEye.fY = .0f;
	mainCamera.vectorEye.fZ = 1.0f;
	mainCamera.vectorEye.fW = .0f;
	mainCamera.vectorRight.fX = 1.0f;
	mainCamera.vectorRight.fY = .0f;
	mainCamera.vectorRight.fZ = .0f;
	mainCamera.vectorRight.fW = .0f;
	mainCamera.vectorUp.fX = .0f;
	mainCamera.vectorUp.fY = 1.0f;
	mainCamera.vectorUp.fZ = .0f;
	mainCamera.vectorUp.fW = .0f;
	mainCamera.vectorEye.ResetUnitVector();
	mainCamera.vectorRight.ResetUnitVector();
	mainCamera.vectorUp.ResetUnitVector();
	mainCamera.fVisibleDistance = 1000.0f;
}

Scene::Scene()
{
}

void Scene::ApplyMatrix(Matrix4 viewMatrix)
{
	for (int i = 0; i < nSphereCnt; ++i)
	{
		sphereList[i].vectorPosition *= viewMatrix;
	}
	for (int i = 0; i < nFloorCnt; ++i)
	{
		floors[i].vectorNormal *= viewMatrix;
		floors[i].vectorNormal.ResetUnitVector();
		floors[i].vectorPlanePoint *= viewMatrix;
		floors[i].fDistance = fabs(floors[i].vectorPlanePoint * floors[i].vectorNormal);
	}
	for (int i = 0; i < nLightCnt; ++i)
	{
		lightList[i].position *= viewMatrix;
	}
}

void Render(__int32* pData, int width, int height)
{
	Scene scene = Scene();
	InitScene(scene);

	//Calculate CameraViewMatrix
	Matrix4 viewMatrix = scene.mainCamera.GenerateLookAtMatrix();

	//Apply ViewMatrix to Scene inner Object
	scene.ApplyMatrix(viewMatrix);

	//Calculate every pixel's color in frameBuffer
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Ray pixelRay = GenerateRay(i / (float)width, j / (float)height, scene.mainCamera.fFovy, height / (float)width);
			color_t pixelColor = RayTrace(pixelRay, scene, TRACE_COUNT, scene.fDefaultRefractiveness);
			pixelColor.GammaCorrection();
			*(pData + j * width + i) = pixelColor.colorValue;
		}
		float completePercent = 100 * i / (float)(width);

		//Output current rendering progress rate 
		std::cout << "Render complete:" << completePercent << "%!" << std::endl;
	}
}

IntersectionInfo::IntersectionInfo() :color(0.0f),material()
{
}

Material::Material():Ambient(0.0f),Diffuse(0.0f),Specular(0.0f),Emissive(0.0f)
{
}

PointLight::PointLight():Diffuse(0.0f),Specular(0.0f),Ambient(0.0f)
{
}
