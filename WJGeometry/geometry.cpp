#include "geometry.h"
#include <iostream>

Matrix4 Camera::GenerateLookAtMatrix()
{
	float moveArray[4][4] = {
		1,					0,					0,					0,
		0,					1,					0,					0,
		0,					0,					1,					0,
		-vectorPosition.fX,	-vectorPosition.fY,	-vectorPosition.fZ,	1
	};
	Matrix4 moveMatrix = moveArray;
	
	float RotateArray[4][4] =
	{
		vectorRight.fX,		vectorUp.fX,		vectorEye.fX,		0,
		vectorRight.fY,		vectorUp.fY,		vectorEye.fY,		0,
		vectorRight.fZ,		vectorUp.fZ,		vectorEye.fZ,		0,
		0,					0,					0,					1
	};
	Matrix4 rotateMatrix = RotateArray;
	return moveMatrix * rotateMatrix;
}

IntersectionInfo* GetNearestIntersectionInfo(Scene & scene, Ray & ray)
{
	IntersectionInfo*		inf = 0;
	IntersectionInfo*		nearestInf = nullptr;
	float					t = -1.0f; // t维护一个距离交点最近的物体的距离

	auto itGraphics = scene.m_graphicsObjectVec.begin();
	while (itGraphics != scene.m_graphicsObjectVec.end())
	{
		inf = nullptr;
		(*(itGraphics++))->CalcRayIntersectionInfo(ray, &inf);
		if (inf == nullptr)
		{
			continue;
		}

		if (t < 0.0f)
		{
			t = inf->fDistance;
			nearestInf = inf;
		}

		if (t > inf->fDistance)
		{
			t = inf->fDistance;
			delete (nearestInf);
			nearestInf = inf;
		}
	}

	return nearestInf;
}

bool IsSamePosition(Vector4 position1, Vector4 position2, float distance)
{
	float	fDeviation	= distance * POINT_DEVIATION;
	bool	result		= false;

	result = 
		((fabs(position1.fX - position2.fX)) < fDeviation) &&
		((fabs(position1.fY - position2.fY)) < fDeviation) &&
		((fabs(position1.fZ - position2.fZ)) < fDeviation);

	return result;
}

Color_t RayTrace(Ray& eyeRay, Scene& scene, unsigned int maxRayTraceDepth, float refractionRatio)
{
	int					VisibleLightCount	= 0;
	
	Color_t				resultColor			= 0x0;
	Color_t				reflectColor		= 0x0;
	IntersectionInfo*	nearestInf			= GetNearestIntersectionInfo(scene, eyeRay);

	Ray								lightRay;
	IntersectionInfo*				lightIntersectionInf;
	std::vector<BaseLight*>			VisibleLightArray;

	if (nearestInf == nullptr)
	{
		return DEFAULT_BACKCOLOR;
	}
	//if (nearestInf.position.length() > scene.mainCamera.visibleDistance)
	//{
	//	return DEFAULT_BACKCOLOR;
	//}
	//PointLight* VisibleLightArray[8] = { 0 };
	

	auto itLightObject = scene.m_lightObjectVec.begin();
	while (itLightObject != scene.m_lightObjectVec.end())
	{
		Vector4 lightDir = nearestInf->m_position - (*itLightObject)->m_position;
		lightDir.ResetUnitVector();
		lightRay.direction = lightDir;
		lightRay.position = (*itLightObject)->m_position;
		lightIntersectionInf = GetNearestIntersectionInfo(scene, lightRay);

		if (lightIntersectionInf != nullptr &&
			IsSamePosition(lightIntersectionInf->m_position, nearestInf->m_position, lightIntersectionInf->fDistance))
		{
			VisibleLightArray.push_back(*itLightObject);
		}

		itLightObject ++;
	}
	
	for (auto itLight : VisibleLightArray)
	{
		resultColor.ColorAdd(itLight->ApplyLightShading(*nearestInf, eyeRay));
	}

	if (maxRayTraceDepth > 0 && nearestInf->m_material.fReflectiveness > 0.0f)
	{
		Ray reflectRay;
		reflectRay.direction = eyeRay.direction - nearestInf->m_normalRay *(nearestInf->m_normalRay * eyeRay.direction) * 2;
		reflectRay.position = nearestInf->m_position - eyeRay.direction * REFLECT_DEVIATION;
		reflectColor = RayTrace(reflectRay, scene, maxRayTraceDepth - 1, nearestInf->m_material.fRefractionRatio);
	}
	else
	{
		reflectColor = 0x0;
	}

	Color_t refractionColor = 0x0;
	if (maxRayTraceDepth > 0 && nearestInf->m_material.fRefractiveness > 0.0f)
	{
		Ray refractionRay;
		if (nearestInf->bIsInner)
		{
			nearestInf->m_material.fRefractionRatio = DEFAULT_REFRACTIONRATIO;
		}
		float LdivideT = refractionRatio / nearestInf->m_material.fRefractionRatio;
		float NdotL = -nearestInf->m_normalRay * eyeRay.direction;
		float refractionTemp = 1 - ((1 - NdotL * NdotL) * LdivideT * LdivideT);
		if (refractionTemp > .0f)
		{
			refractionRay.direction = nearestInf->m_normalRay * (LdivideT * NdotL - sqrtf(refractionTemp)) +
				eyeRay.direction * LdivideT;
			refractionRay.position = nearestInf->m_position + eyeRay.direction * REFLECT_DEVIATION;
			refractionRay.direction.ResetUnitVector();

			refractionColor = RayTrace(refractionRay, scene, maxRayTraceDepth - 1, nearestInf->m_material.fRefractionRatio);
		}
	}

	resultColor.ColorMutiply(1 - nearestInf->m_material.fReflectiveness - nearestInf->m_material.fRefractiveness);
	reflectColor.ColorMutiply(nearestInf->m_material.fReflectiveness);
	resultColor.ColorAdd(reflectColor);
	refractionColor.ColorMutiply(nearestInf->m_material.fRefractiveness);
	resultColor.ColorAdd(refractionColor);

	delete(nearestInf);

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
/*
Color_t ApplyLight(PointLight & light, IntersectionInfo & intersectionInf, Ray& ray)
{
	Color_t result = 0x0;
	Color_t Diffuse = 0x0;
	Color_t Specular = 0x0;
	Color_t Ambient = 0x0;

	Vector4 lightDir = intersectionInf.m_position - light.m_position;
	lightDir.ResetUnitVector();
	Vector4 reflectDir = lightDir - intersectionInf.m_normalRay *(intersectionInf.m_normalRay * lightDir) * 2;
	reflectDir.ResetUnitVector();
	Vector4 eyeDir = -ray.direction;
	eyeDir.ResetUnitVector();

	Ambient = light.Ambient;
	Ambient.ColorModulate(intersectionInf.m_material.m_materialReflectRatio.m_Ambient);
	Ambient.ColorModulate(intersectionInf.m_color);

	Diffuse = light.Diffuse;
	Diffuse.ColorModulate(intersectionInf.m_material.m_materialReflectRatio.m_Diffuse);
	float k = fmax(0.0f, (-lightDir) * intersectionInf.m_normalRay);
	Diffuse.ColorModulate(intersectionInf.m_color);
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
*/

void Sphere::CalcRayIntersectionInfo(Ray& ray, IntersectionInfo ** pInf)
{
	Vector4 distance = ray.position - m_position;
	distance.fW = 1.0f;
	float length = distance.length();
	bool isInsideSphere = (length < m_fRadius + POINT_DEVIATION) ? true : false;
	if (*pInf)
	{
		delete (*pInf);
		*pInf = nullptr;
	}
	float k = ray.direction * distance;
	float temp = k*k - (distance*distance - m_fRadius*m_fRadius);

	if (temp < 0)
		return;
	else
	{
		float t;
		if (!isInsideSphere)
			t = -k - sqrt(temp);
		else
			t = -k + sqrt(temp);
		
		if (t < POINT_DEVIATION)
			return;
		
		Vector4 intersectionPos = ray.position + ray.direction*t;
		intersectionPos.fW = 1.0f;

		*pInf = new IntersectionInfo();
		(*pInf)->fDistance = t;
		(*pInf)->m_position = intersectionPos;
		(*pInf)->m_color = m_color;
		(*pInf)->bIsHit = true;
		(*pInf)->m_material = m_material;
		(*pInf)->m_normalRay = (intersectionPos - m_position);

		if (isInsideSphere)
		{
			(*pInf)->m_normalRay = -(*pInf)->m_normalRay;
			(*pInf)->bIsInner = true;
		}
		(*pInf)->m_normalRay.ResetUnitVector();
		return;
	}
}

void Sphere::ApplyMatrixTransform(const Matrix4 & viewMatrix)
{
	m_position *= viewMatrix;
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
		(*pInf)->m_position = ray.position + ray.direction * t;
		(*pInf)->m_position.fW = 1.0f;
		(*pInf)->m_color = SampleTextureMap((*pInf)->m_position.fX, (*pInf)->m_position.fZ);
		//(*pInf)->color = COLOR_WHITE;
		(*pInf)->bIsHit = true;
		(*pInf)->m_material = planeMaterial;
		(*pInf)->m_normalRay = vectorNormal;
	}
}

void Plane::ApplyMatrixTransform(const Matrix4 & viewMatrix)
{
	vectorNormal *= viewMatrix;
	vectorNormal.ResetUnitVector();
	vectorPlanePoint *= viewMatrix;
	fDistance = fabs(vectorPlanePoint * vectorNormal);
}

Color_t Plane::SampleTextureMap(float x, float z)
{
	if (x > 1e5 || x < -1e5 || z > 1e5 || z < -1e5)
		return Color_t(COLOR_TYPE::BLACK);

	int nX = (x > 0) ? static_cast<int>(x) : static_cast<int>(x) - 1;
	int nZ = (z > 0) ? static_cast<int>(z) : static_cast<int>(z) - 1;

	Color_t result = Color_t(((nX + nZ) & 0x1) ? COLOR_TYPE::WHITE : COLOR_TYPE::BLACK);
	return result;
}

void PointLight::ApplyMatrixTransform(const Matrix4 & transformMatrix)
{
	m_position *= transformMatrix;
}

Color_t PointLight::ApplyLightShading(IntersectionInfo & intersectionInf, Ray & ray)
{
	Color_t result			= COLOR_TYPE::BLACK;
	Color_t diffuseTemp		= COLOR_TYPE::BLACK;
	Color_t specularTemp	= COLOR_TYPE::BLACK;
	Color_t ambientTemp		= COLOR_TYPE::BLACK;

	Vector4 lightDir = intersectionInf.m_position - m_position;
	lightDir.ResetUnitVector();
	Vector4 reflectDir = lightDir - intersectionInf.m_normalRay *(intersectionInf.m_normalRay * lightDir) * 2;
	reflectDir.ResetUnitVector();
	Vector4 eyeDir = -ray.direction;
	eyeDir.ResetUnitVector();

	ambientTemp = this->Ambient;
	ambientTemp.ColorModulate(intersectionInf.m_material.m_materialReflectRatio.m_Ambient);
	ambientTemp.ColorModulate(intersectionInf.m_color);

	diffuseTemp = this->Diffuse;
	diffuseTemp.ColorModulate(intersectionInf.m_material.m_materialReflectRatio.m_Diffuse);
	float k = fmax(0.0f, (-lightDir) * intersectionInf.m_normalRay);

	diffuseTemp.ColorModulate(intersectionInf.m_color);
	diffuseTemp.ColorMutiply(k);

	float SpecTemp = eyeDir * reflectDir;
	SpecTemp = (SpecTemp > 0.0f) ? SpecTemp : 0.0f;
	SpecTemp = pow(SpecTemp, fPower);
	specularTemp = this->Specular;
	specularTemp.ColorModulate(0xffffff);
	specularTemp.ColorMutiply(SpecTemp);

	result.ColorAdd(diffuseTemp);
	result.ColorAdd(ambientTemp);
	result.ColorAdd(specularTemp);

	return result;
}

bool Scene::AddGraphics(BaseGraphics* graphics)
{
	bool bResult = false;
	m_graphicsObjectVec.push_back(graphics);
	bResult = true;

	return bResult;
}

bool Scene::AddLight(BaseLight* light)
{
	bool bResult = false;
	m_lightObjectVec.push_back(light);
	bResult = true;

	return bResult;
}

void Scene::ApplyMatrix(Matrix4& viewMatrix)
{
	for (auto gameObject : m_graphicsObjectVec)
	{
		gameObject->ApplyMatrixTransform(viewMatrix);
	}

	for (auto gameObject : m_lightObjectVec)
	{
		gameObject->ApplyMatrixTransform(viewMatrix);
	}

	/*for (int i = 0; i < nSphereCnt; ++i)
	{
	sphereList[i].m_position *= viewMatrix;
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
	lightList[i].m_position *= viewMatrix;
	}*/
}

void InitScene(Scene& scene)
{
	// Scene init
	scene.nSphereCnt = 0;
	scene.nFloorCnt = 0;
	Sphere* ball0 = new Sphere();

#pragma region InitSphere
	ball0->m_color = COLOR_TYPE::RED;
	ball0->m_material.m_materialReflectRatio.m_Ambient = COLOR_TYPE::WHITE;
	ball0->m_material.m_materialReflectRatio.m_Diffuse = COLOR_TYPE::WHITE;
	ball0->m_material.m_materialReflectRatio.m_Specular = COLOR_TYPE::WHITE;
	ball0->m_material.m_materialReflectRatio.m_Emissive = COLOR_TYPE::BLACK;

	ball0->m_material.fReflectiveness = 0.3f;
	ball0->m_material.fRefractionRatio = DEFAULT_REFRACTIONRATIO;
	ball0->m_material.fRefractiveness = 0.0f;

	ball0->m_position.fX = -3.0f;
	ball0->m_position.fY = -1.0f;
	ball0->m_position.fZ = 3.0f;
	ball0->m_position.fW = 1.0f;

	ball0->m_fRadius = 1.0f;
	scene.AddGraphics(ball0);
	//scene.sphereList[scene.nSphereCnt++] = ball0;

	Sphere* ball1 = new Sphere();
	ball1->m_color = COLOR_TYPE::RED;

	ball1->m_material.m_materialReflectRatio.m_Ambient = COLOR_TYPE::WHITE;
	ball1->m_material.m_materialReflectRatio.m_Diffuse = COLOR_TYPE::WHITE;
	ball1->m_material.m_materialReflectRatio.m_Specular = COLOR_TYPE::WHITE;
	ball1->m_material.m_materialReflectRatio.m_Emissive = COLOR_TYPE::BLACK;

	ball1->m_material.fReflectiveness = 0.0f;
	ball1->m_material.fRefractionRatio = GLASS_REFRACTION;
	ball1->m_material.fRefractiveness = 0.9f;

	ball1->m_position.fX = .0f;
	ball1->m_position.fY = .0f;
	ball1->m_position.fZ = 5.0f;
	ball1->m_position.fW = 1.0f;

	ball1->m_fRadius = 2.0f;
	scene.AddGraphics(ball1);

	Sphere* ball2 = new Sphere();
	ball2->m_color = COLOR_TYPE::GREEN;

	ball2->m_material.m_materialReflectRatio.m_Ambient = COLOR_TYPE::WHITE;
	ball2->m_material.m_materialReflectRatio.m_Diffuse = COLOR_TYPE::WHITE;
	ball2->m_material.m_materialReflectRatio.m_Specular = COLOR_TYPE::WHITE;
	ball2->m_material.m_materialReflectRatio.m_Emissive = COLOR_TYPE::BLACK;

	ball2->m_material.fReflectiveness = 0.8f;
	ball2->m_material.fRefractionRatio = GLASS_REFRACTION;
	ball2->m_material.fRefractiveness = 0.2f;

	ball2->m_position.fX = 4.0f;
	ball2->m_position.fY = -1.0f;
	ball2->m_position.fZ = 3.0f;
	ball2->m_position.fW = 1.0f;

	ball2->m_fRadius = 1.0f;
	scene.AddGraphics(ball2);
#pragma endregion

#pragma region InitPlane
	Plane* plane = new Plane();
	plane->planeColor = COLOR_TYPE::BLUE;

	plane->vectorPlanePoint.fY = -2.0f;
	plane->vectorPlanePoint.fW = 1.0f;

	plane->vectorNormal.fY = 1.0f;
	plane->vectorNormal.fX = 0.0f;
	plane->vectorNormal.fZ = 0.0f;
	plane->vectorNormal.fW = 0.0f;

	plane->fDistance = 5.0f;
	plane->planeMaterial.m_materialReflectRatio.m_Ambient = COLOR_TYPE::WHITE;
	plane->planeMaterial.m_materialReflectRatio.m_Diffuse = COLOR_TYPE::WHITE;
	plane->planeMaterial.m_materialReflectRatio.m_Emissive = COLOR_TYPE::WHITE;
	plane->planeMaterial.m_materialReflectRatio.m_Specular = COLOR_TYPE::WHITE;

	plane->planeMaterial.fReflectiveness = .3f;
	plane->planeMaterial.fRefractionRatio = .0f;
	plane->planeMaterial.fRefractiveness = .0f;

	scene.AddGraphics(plane);
#pragma endregion

#pragma region InitLight
	PointLight* light = new PointLight();
	light->Ambient = 0x00404040;
	light->Diffuse = 0x00808080;
	light->Specular = 0x00ffffff;

	light->fPower = 8.0f;

	light->m_position.fX = -1.0f;
	light->m_position.fY = 5.0f;
	light->m_position.fZ = 2.0f;
	light->m_position.fW = 1.0f;

	scene.AddLight(light);
#pragma endregion

	//scene.lightList[scene.nLightCnt++] = light;

	/*PointLight light2 = PointLight();
	light2.Ambient = 0x202020;
	light2.Diffuse = 0x808080;
	light2.Specular = 0xffffff;
	light2.fPower = 32.0f;

	light2.position.fX = 4.0f;
	light2.position.fY = 3.0f;
	light2.position.fZ = -1.0f;
	light2.position.fW = 1.0f;

	scene.lightList[scene.nLightCnt++] = light2;*/

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
			Color_t pixelColor = RayTrace(pixelRay, scene, TRACE_COUNT, scene.fDefaultRefractiveness);

			pixelColor.GammaCorrection();
			*(pData + j * width + i) = pixelColor.m_colorValue;
		}
		float completePercent = 100 * i / (float)(width);

		//Output current rendering progress rate 
		std::cout << "Render complete:" << completePercent << "%!" << std::endl;
	}
}


