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
#define GLASS_REFRACTION			1.666f
#define GAMMA_RATIO					2.2f

#define CMID(x) {((x) < 0 ) ? 0 : (((x) > 255) ? 255 : (x))}

//object material 
enum RenderMode {
	SPEC,	//specular material
	DIFF,	//diffuse material
	BOTH	//specular & diffuse material
};

class Material 
{
public:
	color_t Ambient;
	color_t Diffuse;
	color_t Specular;
	color_t Emissive;
	float reflectiveness;
	float refractiveness;
	float refractionRatio;
};

class Ray
{
public:
	Vector4 position;
	Vector4 direction;
};

class IntersectionInfo
{
public:
	Vector4 position;
	Vector4 normal;
	color_t color;
	float t;
	Material material;
	bool isHit = false;
	bool isInner = false;
};


class BaseGraphics
{
public:
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf) = 0;
};

class Sphere:public BaseGraphics
{
public:
	Vector4 position;
	float radius;
	color_t color;		//assume each point is the same color.
	Material mtrl;
	virtual void CalcRayIntersectionInfo(Ray& ray,IntersectionInfo** pInf);
};

class Plane:public BaseGraphics
{
public:
	Vector4 normal;		//plane normal vector
	float distance;		//origin to the plane.
	color_t color;
	Vector4 PlanePoint;
	Material mtrl;
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
	color_t SampleTextureMap(float x, float z);
};

class Camera 
{
public:
	Vector4 eye;
	Vector4 up;
	Vector4 right;
	Vector4 position;
	float visibleDistance;
	float fovy;
	float aspect;
	Matrix4 generateLookAtMatrix();
};

class PointLight
{
public:
	Vector4 position;
	color_t Diffuse;
	color_t Specular;
	color_t Ambient;
	float power;
};

class Scene 
{
public:
	Camera mainCamera;
	Sphere sphereList[MAXSPHERECNT];
	int sphereCnt = 0;
	Plane floors[MAXFLOORCNT];
	int floorCnt = 0;
	PointLight lightList[MAXLIGHTCNT];
	int lightCnt = 0;
	float defaultRefractiveness = 1.0f;

	void ApplyMatrix(Matrix4 mat);
};

IntersectionInfo CalcNearestNonTransparentIntersectionInf(Scene& scene, Ray& ray);

bool IsSamePosition(Vector4 position1, Vector4 position2);

color_t RayTrace(Ray& ray,Scene& scene,unsigned int Depth, float refractiveness);

void Render(color_t* pData, int width, int height);

//x,y=>(0,1) assume camera.up(0.0f,1.0f,0.0f) right(1.0f,0.0f,0.0f) eye(0.0f,0.0f,-1.0f)
Ray GenerateRay(float x, float y,float fovAngle, float aspect);

color_t ApplyLight(PointLight& light, IntersectionInfo& intersectionInf, Ray& ray);

void InitScene(Scene& scene);

void InitCamera(Camera& camera);