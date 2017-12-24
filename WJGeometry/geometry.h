#pragma once

#include "WJMath.h"
#include "Color.h"

#define MAXSPHERECNT				8
#define MAXFLOORCNT					6
#define MAXLIGHTCNT					4
#define COLOR_RED					0x00FF0000
#define COLOR_GREEN					0x0000FF00
#define COLOR_BLUE					0x000000FF
#define COLOR_WHITE					0x00FFFFFF
#define COLOR_BLACK					0x00000000
#define DEFAULT_BACKCOLOR			0x0
#define DEFAULT_REFRACTIONRATIO		1.0f
#define MAX_DEPTH					10
#define WIDTH						1920
#define HEIGHT						1080
#define POINT_DEVIATION				1e-4f
#define TRACE_COUNT					5
#define REFLECT_DEVIATION			1e-6f
#define GLASS_REFRACTION			1.66666f

//object material 
enum RenderMode {
	SPEC,	//specular material
	DIFF,	//diffuse material
	BOTH	//specular & diffuse material
};

class Material 
{
public:
	float		fReflectiveness		= 0.0f;
	float		fRefractiveness		= 0.0f;
	float		fRefractionRatio	= 0.0f;
	
	color_t		Ambient;
	color_t		Diffuse;
	color_t		Specular;
	color_t		Emissive;

	Material();
};

class Ray
{
public:
	Vector4		position;
	Vector4		direction;
};

class IntersectionInfo
{
public:
	float		fDistance		= 0.0f;
	bool		bIsHit			= false;
	bool		bIsInner		= false;

	Vector4		position;
	Vector4		normal;
	color_t		color;
	Material	material;
	
	IntersectionInfo();
};


class BaseGraphics
{
public:
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf) = 0;
};

class Sphere:public BaseGraphics
{
public:
	float		fRadius				= 0.0f;

	Vector4		vectorPosition;
	color_t		sphereColor;		//assume each point is the same color.
	Material	mtrlSphere;
	
	Sphere();
	virtual void CalcRayIntersectionInfo(Ray& ray,IntersectionInfo** pInf);
};

class Plane:public BaseGraphics
{
public:
	float		fDistance				= 0.0f;	//origin to the plane.

	Vector4		vectorNormal;		//plane normal vector
	Vector4		vectorPlanePoint;
	color_t		planeColor;
	Material	planeMaterial;
	
	Plane();
	color_t SampleTextureMap(float x, float z);
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
};

class Camera 
{
public:
	float		fVisibleDistance	= 0.0f;
	float		fFovy				= 0.0f;
	float		fAspect				= 0.0f;

	Vector4		vectorEye;
	Vector4		vectorUp;
	Vector4		vectorRight;
	Vector4		vectorPosition;
	
	Matrix4 GenerateLookAtMatrix();
};

class PointLight
{
public:
	float		fPower			= 0.0f;

	Vector4		position;
	color_t		Diffuse;
	color_t		Specular;
	color_t		Ambient;

	PointLight();
};

class Scene 
{
public:
	int				nSphereCnt				= 0;
	int				nFloorCnt				= 0;
	int				nLightCnt				= 0;
	float			fDefaultRefractiveness	= 1.0f;

	Camera			mainCamera;
	Sphere			sphereList[MAXSPHERECNT];
	PointLight		lightList[MAXLIGHTCNT];
	Plane			floors[MAXFLOORCNT];

	Scene();
	void ApplyMatrix(Matrix4 mat);
};

IntersectionInfo GetNearestIntersectionInfo(Scene& scene, Ray& ray);

bool IsSamePosition(Vector4 position1, Vector4 position2,float distance);

color_t RayTrace(Ray& ray,Scene& scene,unsigned int Depth, float refractiveness);

void Render(__int32* pData, int width, int height);

//x,y=>(0,1) assume camera.up(0.0f,1.0f,0.0f) right(1.0f,0.0f,0.0f) eye(0.0f,0.0f,-1.0f)
Ray GenerateRay(float x, float y,float fovAngle, float aspect);

color_t ApplyLight(PointLight& light, IntersectionInfo& intersectionInf, Ray& ray);

void InitScene(Scene& scene);

void InitCamera(Camera& camera);