#pragma once
#pragma comment(lib,"WJMath.lib")
#include "WJMath.h"

typedef __int32 COLOR;

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
	COLOR Ambient;
	COLOR Diffuse;
	COLOR Specular;
	COLOR Emissive;
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
	COLOR color;
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
	COLOR color;		//assume each point is the same color.
	Material mtrl;
	virtual void CalcRayIntersectionInfo(Ray& ray,IntersectionInfo** pInf);
};

class Plane:public BaseGraphics
{
public:
	Vector4 normal;		//plane normal vector
	float distance;		//origin to the plane.
	COLOR color;
	Vector4 PlanePoint;
	Material mtrl;
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
	COLOR SampleTextureMap(float x, float z);
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
	COLOR Diffuse;
	COLOR Specular;
	COLOR Ambient;
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

COLOR RayTrace(Ray& ray,Scene& scene,unsigned int Depth, float refractiveness);

void Render(COLOR* pData, int width, int height);

//x,y=>(0,1) assume camera.up(0.0f,1.0f,0.0f) right(1.0f,0.0f,0.0f) eye(0.0f,0.0f,-1.0f)
Ray GenerateRay(float x, float y,float fovAngle, float aspect);

COLOR ApplyLight(PointLight& light, IntersectionInfo& intersectionInf, Ray& ray);

void InitScene(Scene& scene);

void InitCamera(Camera& camera);

COLOR ColorModulate(COLOR color1, COLOR color2);

COLOR ColorAdd(COLOR color1, COLOR color2);

COLOR ColorMutiply(COLOR color1, float k);

COLOR ColorMerge(COLOR sourceColor, COLOR mergeColor, float ratio);

COLOR GammaCorrection(COLOR sourceColor);