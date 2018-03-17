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

enum RenderMode {
	SPEC,	//specular material
	DIFF,	//diffuse  material
	BOTH	//specular & diffuse material
};

typedef Color_t LightReflectRatio;
struct MaterialReflectRatio
{
	LightReflectRatio m_Ambient;
	LightReflectRatio m_Diffuse;
	LightReflectRatio m_Specular;
	LightReflectRatio m_Emissive;

	MaterialReflectRatio(
		LightReflectRatio Ambient,
		LightReflectRatio Diffuse,
		LightReflectRatio Specular,
		LightReflectRatio Emissive) 
		:m_Ambient(Ambient), m_Diffuse(Diffuse), m_Specular(Specular), m_Emissive(Emissive)
	{};
};

//object material 
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

<<<<<<< HEAD
	MaterialReflectRatio m_materialReflectRatio;
	float reflectiveness;
	float refractiveness;
	float refractionRatio;
=======
	Material();
>>>>>>> 7f61243d7facfe4edbfb93d0dfb98b249d55312d
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
<<<<<<< HEAD

	Vector4		m_position;
	Vector4		m_normalRay;
	Color_t		m_color;
	Material	m_material;
	
	float		t;
	bool		isHit = false;
	bool		isInner = false;
=======
>>>>>>> 7f61243d7facfe4edbfb93d0dfb98b249d55312d
};


class BaseGraphics
{
public:
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf) = 0;
};

class Sphere: public BaseGraphics
{
public:
	float		fRadius				= 0.0f;

<<<<<<< HEAD
	Vector4		m_position;
	float		m_fRadius;
	Color_t		m_color;		//assume each point is the same color.
	Material	m_material;

=======
	Vector4		vectorPosition;
	color_t		sphereColor;		//assume each point is the same color.
	Material	mtrlSphere;
	
	Sphere();
>>>>>>> 7f61243d7facfe4edbfb93d0dfb98b249d55312d
	virtual void CalcRayIntersectionInfo(Ray& ray,IntersectionInfo** pInf);
};

class Plane: public BaseGraphics
{
public:
	float		fDistance				= 0.0f;	//origin to the plane.

<<<<<<< HEAD
	Vector4 normal;		//plane normal vector
	float distance;		//origin to the plane.
	Color_t color;
	Vector4 PlanePoint;
	Material mtrl;

	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
	Color_t SampleTextureMap(float x, float z);
=======
	Vector4		vectorNormal;		//plane normal vector
	Vector4		vectorPlanePoint;
	color_t		planeColor;
	Material	planeMaterial;
	
	Plane();
	color_t SampleTextureMap(float x, float z);
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
>>>>>>> 7f61243d7facfe4edbfb93d0dfb98b249d55312d
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

<<<<<<< HEAD
	Vector4 position;
	Color_t Diffuse;
	Color_t Specular;
	Color_t Ambient;
	float power;
=======
	Vector4		position;
	color_t		Diffuse;
	color_t		Specular;
	color_t		Ambient;

	PointLight();
>>>>>>> 7f61243d7facfe4edbfb93d0dfb98b249d55312d
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

Color_t RayTrace(Ray& ray,Scene& scene,unsigned int Depth, float refractiveness);

void Render(__int32* pData, int width, int height);

//x,y=>(0,1) assume camera.up(0.0f,1.0f,0.0f) right(1.0f,0.0f,0.0f) eye(0.0f,0.0f,-1.0f)
Ray GenerateRay(float x, float y,float fovAngle, float aspect);

Color_t ApplyLight(PointLight& light, IntersectionInfo& intersectionInf, Ray& ray);

void InitScene(Scene& scene);

void InitCamera(Camera& camera);