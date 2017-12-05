#include "geometry.h"
#include <iostream>

Matrix4 Camera::generateLookAtMatrix()
{
	float moveArray[4][4] = {
		1			,0			,0			,0,
		0			,1			,0			,0,
		0			,0			,1			,0,
		-position.x	,-position.y,-position.z,1
	};
	Matrix4 moveMatrix = moveArray;
	
	float RotateArray[4][4] =
	{
		right.x,	up.x,		eye.x,		0,
		right.y,	up.y,		eye.y,		0,
		right.z,	up.z,		eye.z,		0,
		0,			0,			0,			1
	};
	Matrix4 rotateMatrix = RotateArray;
	return moveMatrix * rotateMatrix;
}

IntersectionInfo CalcNearestNonTransparentIntersectionInf(Scene & scene, Ray & ray)
{
	IntersectionInfo* inf = 0;
	IntersectionInfo nearestInf;
	nearestInf.isHit = false;
	nearestInf.t = -1.0f;
	float t = -1.0f;
	for (int i = 0; i < scene.sphereCnt; i++)
	{
		scene.sphereList[i].CalcRayIntersectionInfo(ray, &inf);
		if (inf == NULL)
		{
			continue;
		}
		if (t < 0.0f)
		{
			t = inf->t;
			if (nearestInf.isHit == true)
			{
				if (nearestInf.t > inf->t)
				{
					nearestInf = *inf;
				}
			}
			else
			{
				nearestInf = *inf;
			}
		}
		else if (t > inf->t)
		{
			t = inf->t;
			nearestInf = *inf;
		}
	}
	for (int i = 0; i < scene.floorCnt; ++i)
	{
		scene.floors[i].CalcRayIntersectionInfo(ray, &inf);
		if (inf == NULL)
		{
			continue;
		}
		if (t < 0.0f)
		{
			t = inf->t;
			if (nearestInf.isHit == true)
			{
				if (nearestInf.t > inf->t)
				{
					nearestInf = *inf;
				}
			}
			else
			{
				nearestInf = *inf;
			}
		}
		else if (t > inf->t)
		{
			t = inf->t;
			nearestInf = *inf;
		}
	}
	if (nearestInf.t < .0f)
	{
		nearestInf.isHit = false;
	}
	return nearestInf;
}

bool IsSamePosition(Vector4 position1, Vector4 position2)
{
	bool result = ((fabs(position1.x - position2.x)) < POINT_DEVIATION) &&
		((fabs(position1.y - position2.y)) < POINT_DEVIATION) &&
		((fabs(position1.z - position2.z)) < POINT_DEVIATION);
	return result;
}

color_t RayTrace(Ray& eyeRay, Scene& scene, unsigned int maxRayTraceDepth, float refractionRatio)
{
	IntersectionInfo nearestInf = CalcNearestNonTransparentIntersectionInf(scene, eyeRay);
	if (nearestInf.isHit == false)
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
	for (int i = 0; i < scene.lightCnt; ++i)
	{
		Vector4 lightDir = nearestInf.position - scene.lightList[i].position;
		lightDir.ResetUnitVector();
		lightRay.direction = lightDir;
		lightRay.position = scene.lightList[i].position;
		lightIntersectionInf = CalcNearestNonTransparentIntersectionInf(scene, lightRay);
		if (IsSamePosition(lightIntersectionInf.position, nearestInf.position))
		{
			VisibleLightArray[VisibleLightCount++] = &scene.lightList[i];
		}
	}
	color_t resultColor = 0x0;

	color_t reflectColor = 0x0;
	for (int i = 0; i < VisibleLightCount; ++i)
	{
		resultColor = ColorAdd(resultColor, ApplyLight(*VisibleLightArray[i], nearestInf, eyeRay));
	}

	if (maxRayTraceDepth > 0 && nearestInf.material.reflectiveness > 0.0f)
	{
		Ray reflectRay;
		reflectRay.direction = eyeRay.direction - nearestInf.normal *(nearestInf.normal * eyeRay.direction) * 2;
		reflectRay.position = nearestInf.position - eyeRay.direction * REFLECT_DEVIATION;
		reflectColor = RayTrace(reflectRay, scene, maxRayTraceDepth - 1, nearestInf.material.refractionRatio);
	}
	else
	{
		reflectColor = 0x0;
	}

	color_t refractionColor = 0x0;
	if (maxRayTraceDepth > 0 && nearestInf.material.refractiveness > 0.0f)
	{
		Ray refractionRay;
		if (nearestInf.isInner)
		{
			nearestInf.material.refractionRatio = DEFAULT_REFRACTIONRATIO;
		}
		float LdivideT = refractionRatio / nearestInf.material.refractionRatio;
		float NdotL = -nearestInf.normal * eyeRay.direction;
		float refractionTemp = 1 - ((1 - NdotL * NdotL) * LdivideT * LdivideT);
		if (refractionTemp > .0f)
		{
			refractionRay.direction = nearestInf.normal * (LdivideT * NdotL - sqrtf(refractionTemp)) +
				eyeRay.direction * LdivideT;
			refractionRay.position = nearestInf.position + eyeRay.direction * REFLECT_DEVIATION;
			refractionRay.direction.ResetUnitVector();
			refractionColor = RayTrace(refractionRay, scene, maxRayTraceDepth - 1, nearestInf.material.refractionRatio);
		}
	}

	resultColor = ColorAdd(ColorMutiply(resultColor, 1 - nearestInf.material.reflectiveness - nearestInf.material.refractiveness), 
		ColorMutiply(reflectColor, nearestInf.material.reflectiveness));
	resultColor = ColorAdd(resultColor, ColorMutiply(refractionColor, nearestInf.material.refractiveness));

	return resultColor;

}

Ray GenerateRay(float x, float y, float fovAngle, float aspect)
{
	Ray result;
	float fovScale = tanf(fovAngle * PI / 360.0f);
	float sx = (x - 0.5f) * fovScale * 2;
	float sy = (0.5f - y) * fovScale * 2 * aspect;
	result.position.x = .0f;
	result.position.y = .0f;
	result.position.z = .0f;
	result.position.w = 1.0f;
	result.direction.x = sx;
	result.direction.y = sy;
	result.direction.z = 1.0f;
	result.direction.w = .0f;
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

	Ambient = ColorModulate(light.Ambient, intersectionInf.material.Ambient);
	Ambient = ColorModulate(Ambient, intersectionInf.color);

	Diffuse = ColorModulate(light.Diffuse, intersectionInf.material.Diffuse);
	float k = fmax(0.0f, (-lightDir) * intersectionInf.normal);
	Diffuse = ColorModulate(Diffuse, intersectionInf.color);
	Diffuse = ColorMutiply(Diffuse, k);

	float SpecTemp = eyeDir * reflectDir;
	SpecTemp = (SpecTemp > 0.0f) ? SpecTemp : 0.0f;
	SpecTemp = pow(SpecTemp, light.power);
	Specular = ColorModulate(light.Specular, 0xffffff);
	Specular = ColorMutiply(Specular, SpecTemp);
	result = ColorAdd(Diffuse, Ambient);
	result = ColorAdd(result, Specular);
	return result;
}

Sphere::Sphere():color(0.0f)
{
}

void Sphere::CalcRayIntersectionInfo(Ray& ray, IntersectionInfo ** pInf)
{
	Vector4 distance = ray.position - position;
	distance.w = 1.0f;
	float length = distance.length();
	bool isInsideSphere = (length < radius + POINT_DEVIATION) ? true : false;
	if (*pInf)
	{
		delete (*pInf);
		*pInf = 0;
	}
	float k = ray.direction * distance;
	float temp = k*k - (distance*distance - radius*radius);
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
		intersectionPos.w = 1.0f;
		*pInf = new IntersectionInfo();
		(*pInf)->t = t;
		(*pInf)->position = intersectionPos;
		(*pInf)->color = color;
		(*pInf)->isHit = true;
		(*pInf)->material = mtrl;
		(*pInf)->normal = (intersectionPos - position);
		if (isInsideSphere)
		{
			(*pInf)->normal = -(*pInf)->normal;
			(*pInf)->isInner = true;
		}
		(*pInf)->normal.ResetUnitVector();
		return;
	}
}

Plane::Plane():color(0.0f)
{
}

void Plane::CalcRayIntersectionInfo(Ray& ray, IntersectionInfo ** pInf)
{
	if (*pInf)
	{
		delete *pInf;
		*pInf = 0;
	}
	float temp = normal * ray.direction;
	if (temp < FLOAT_DEVIATION && temp > -FLOAT_DEVIATION)
	{
		return;
	}
	float t = (-distance - normal * ray.position) / temp;
	if (t > .0f)
	{
		*pInf = new IntersectionInfo();
		(*pInf)->t = t;
		(*pInf)->position = ray.position + ray.direction * t;
		(*pInf)->position.w = 1.0f;
		(*pInf)->color = SampleTextureMap((*pInf)->position.x, (*pInf)->position.z);
		//(*pInf)->color = COLOR_WHITE;
		(*pInf)->isHit = true;
		(*pInf)->material = mtrl;
		(*pInf)->normal = normal;
	}
}

color_t Plane::SampleTextureMap(float x, float z)
{
	int _x = 0;
	int _z = 0;
	if (x > 0)
	{
		_x = x;
	}
	else
	{
		_x = x - 1;
	}
	if (z > 0)
	{
		_z = z;
	}
	else
	{
		_z = z - 1;
	}
	int temp = _x + _z;
	color result = ((temp & 0x1) ? COLOR_WHITE : COLOR_BLACK);
	return result;
}


void InitScene(Scene& scene)
{
	// Scene init
	scene.sphereCnt = 0;
	scene.floorCnt = 0;
	Sphere ball0 = Sphere();
	ball0.color = 0x0000ff00;
	ball0.mtrl.Ambient = 0x00ffffff;
	ball0.mtrl.Diffuse = 0x00ffffff;
	ball0.mtrl.Specular = 0x00ffffff;
	ball0.mtrl.Emissive = 0x0;
	ball0.mtrl.reflectiveness = .3f;
	ball0.mtrl.refractionRatio = DEFAULT_REFRACTIONRATIO;
	ball0.mtrl.refractiveness = .0f;
	ball0.position.x = -3.0f;
	ball0.position.y = -1.0f;
	ball0.position.z = 3.0f;
	ball0.position.w = 1.0f;
	ball0.radius = 1.0f;
	scene.sphereList[scene.sphereCnt++] = ball0;

	Sphere ball1 = Sphere();
	ball1.color = 0x00ff0000;
	ball1.mtrl.Ambient = 0x00ffffff;
	ball1.mtrl.Diffuse = 0x00ffffff;
	ball1.mtrl.Specular = 0x00ffffff;
	ball1.mtrl.Emissive = 0x0;
	ball1.mtrl.reflectiveness = .0f;
	ball1.mtrl.refractionRatio = GLASS_REFRACTION;
	ball1.mtrl.refractiveness = .5f;
	ball1.position.x = .0f;
	ball1.position.y = .0f;
	ball1.position.z = 5.0f;
	ball1.position.w = 1.0f;
	ball1.radius = 2.0f;
	scene.sphereList[scene.sphereCnt++] = ball1;

	Sphere ball2 = Sphere();
	ball2.color = 0x0000ff00;
	ball2.mtrl.Ambient = 0x00ffffff;
	ball2.mtrl.Diffuse = 0x00ffffff;
	ball2.mtrl.Specular = 0x00ffffff;
	ball2.mtrl.Emissive = 0x0;
	ball2.mtrl.reflectiveness = .0f;
	ball2.mtrl.refractionRatio = GLASS_REFRACTION;
	ball2.mtrl.refractiveness = 2.0f;
	ball2.position.x = 4.0f;
	ball2.position.y = -1.0f;
	ball2.position.z = 3.0f;
	ball2.position.w = 1.0f;
	ball2.radius = 1.0f;
	scene.sphereList[scene.sphereCnt++] = ball2;

	Plane plane = Plane();
	plane.color = 0x000000ff;
	plane.PlanePoint.y = -2.0f;
	plane.PlanePoint.w = 1.0f;
	plane.normal.y = 1.0f;
	plane.distance = 5.0f;
	plane.mtrl.Ambient = 0x00ffffff;
	plane.mtrl.Diffuse = 0x00ffffff;
	plane.mtrl.Emissive = 0x00ffffff;
	plane.mtrl.Specular = 0x00ffffff;
	plane.mtrl.reflectiveness = .3f;
	plane.mtrl.refractionRatio = .0f;
	plane.mtrl.refractiveness = .0f;
	scene.floors[scene.floorCnt++] = plane;

	PointLight light = PointLight();
	light.Ambient = 0x202020;
	light.Diffuse = 0x808080;
	light.Specular = 0xffffff;
	light.power = 2.0f;
	light.position.x = -1.0f;
	light.position.y = 5.0f;
	light.position.z = 2.0f;
	light.position.w = 1.0f;
	scene.lightList[scene.lightCnt++] = light;

	PointLight light2 = PointLight();
	light2.Ambient = 0x202020;
	light2.Diffuse = 0x808080;
	light2.Specular = 0xffffff;
	light2.power = 32.0f;
	light2.position.x = 4.0f;
	light2.position.y = 3.0f;
	light2.position.z = -1.0f;
	light2.position.w = 1.0f;
	scene.lightList[scene.lightCnt++] = light2;

	InitCamera(scene.mainCamera);
}

void InitCamera(Camera & mainCamera)
{
	mainCamera.position.x = .0f;
	mainCamera.position.y = 0.0f;
	mainCamera.position.z = -3.0f;
	mainCamera.position.w = .0f;
	mainCamera.aspect = WIDTH / (float)HEIGHT;
	mainCamera.fovy = 90.0f;
	mainCamera.eye.x = .0f;
	mainCamera.eye.y = .0f;
	mainCamera.eye.z = 1.0f;
	mainCamera.eye.w = .0f;
	mainCamera.right.x = 1.0f;
	mainCamera.right.y = .0f;
	mainCamera.right.z = .0f;
	mainCamera.right.w = .0f;
	mainCamera.up.x = .0f;
	mainCamera.up.y = 1.0f;
	mainCamera.up.z = .0f;
	mainCamera.up.w = .0f;
	mainCamera.eye.ResetUnitVector();
	mainCamera.right.ResetUnitVector();
	mainCamera.up.ResetUnitVector();
	mainCamera.visibleDistance = 1000.0f;
}

Scene::Scene()
{
}

void Scene::ApplyMatrix(Matrix4 viewMatrix)
{
	for (int i = 0; i < sphereCnt; ++i)
	{
		sphereList[i].position *= viewMatrix;
	}
	for (int i = 0; i < floorCnt; ++i)
	{
		floors[i].normal *= viewMatrix;
		floors[i].normal.ResetUnitVector();
		floors[i].PlanePoint *= viewMatrix;
		floors[i].distance = fabs(floors[i].PlanePoint * floors[i].normal);
	}
	for (int i = 0; i < lightCnt; ++i)
	{
		lightList[i].position *= viewMatrix;
	}
}

void Render(color_t* pData, int width, int height)
{
	Scene scene = Scene();
	InitScene(scene);

	//Calculate CameraViewMatrix
	Matrix4 viewMatrix = scene.mainCamera.generateLookAtMatrix();

	//Apply ViewMatrix to Scene inner Object
	scene.ApplyMatrix(viewMatrix);

	//Calculate every pixel's color in frameBuffer
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Ray pixelRay = GenerateRay(i / (float)width, j / (float)height, scene.mainCamera.fovy, height / (float)width);
			color_t pixelColor = RayTrace(pixelRay, scene, TRACE_COUNT, scene.defaultRefractiveness);
			*(pData + j * width + i) = pixelColor.GammaCorrection();
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
