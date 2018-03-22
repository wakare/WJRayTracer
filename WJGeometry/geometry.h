#pragma once

#include "WJMath.h"
#include "Color.h"
#include <vector>

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

class Camera
{
public:
	float		fVisibleDistance = 0.0f;
	float		fFovy = 0.0f;
	float		fAspect = 0.0f;

	Vector4		vectorEye;
	Vector4		vectorUp;
	Vector4		vectorRight;
	Vector4		vectorPosition;

	Matrix4 GenerateLookAtMatrix();
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
	float		fReflectiveness = 0.0f;
	float		fRefractiveness = 0.0f;
	float		fRefractionRatio = 0.0f;

	MaterialReflectRatio m_materialReflectRatio;

	Material() :m_materialReflectRatio(0x0, 0x0, 0x0, 0x0) {};
};

class IntersectionInfo
{
public:
	float		fDistance = 0.0f;
	bool		bIsHit = false;
	bool		bIsInner = false;

	Vector4		m_position;
	Vector4		m_normalRay;
	Color_t		m_color;
	Material	m_material;

	IntersectionInfo() :m_color(0x0) {};
};

class Ray
{
public:
	Vector4		position;
	Vector4		direction;
};

class BaseGraphics
{
public:
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf) = 0;
	virtual void ApplyMatrixTransform(const Matrix4& transformMatrix) = 0;
};

class BaseLight
{
public:
	Vector4				m_position;

	virtual void		ApplyMatrixTransform(const Matrix4& transformMatrix) = 0;
	virtual Color_t		ApplyLightShading(IntersectionInfo& intersectionInf, Ray& ray) = 0;
};

class Sphere: public BaseGraphics
{
public:
	Vector4		m_position;
	float		m_fRadius;
	Color_t		m_color;		//assume each point is the same color.
	Material	m_material;

	Sphere():m_color(0x0) {};
	virtual void CalcRayIntersectionInfo(Ray& ray,IntersectionInfo** pInf);
	virtual void ApplyMatrixTransform(const Matrix4&);
};

class Plane: public BaseGraphics
{
public:
	float		fDistance = 0.0f;	//origin to the plane->

	Vector4		vectorNormal;		//plane normal vector
	Vector4		vectorPlanePoint;
	Color_t		planeColor;
	Material	planeMaterial;
	
	Plane():planeColor(0x0) {};
	Color_t SampleTextureMap(float x, float z);
	virtual void CalcRayIntersectionInfo(Ray& ray, IntersectionInfo** pInf);
	virtual void ApplyMatrixTransform(const Matrix4&);
};



class PointLight: public BaseLight
{
public:
	float		fPower			= 0.0f;

	Color_t		Diffuse;
	Color_t		Specular;
	Color_t		Ambient;

	PointLight():Diffuse(0x0),Specular(0x0),Ambient(0x0) {};
	virtual void		ApplyMatrixTransform(const Matrix4& transformMatrix);
	virtual Color_t		ApplyLightShading(IntersectionInfo& intersectionInf, Ray& ray);
};

class Scene 
{
public:
	int				nSphereCnt				= 0;
	int				nFloorCnt				= 0;
	int				nLightCnt				= 0;
	float			fDefaultRefractiveness	= 1.0f;

	Camera			mainCamera;

	std::vector<BaseGraphics*>	m_graphicsObjectVec;
	std::vector<BaseLight*>		m_lightObjectVec;

	Scene() {};
	bool AddGraphics(BaseGraphics* graphicsObject);
	bool AddLight(BaseLight* lightObject);
	void ApplyMatrix(Matrix4& transformMatrix);
};

IntersectionInfo* GetNearestIntersectionInfo(Scene& scene, Ray& ray);

bool IsSamePosition(Vector4 position1, Vector4 position2,float distance);

Color_t RayTrace(Ray& ray,Scene& scene,unsigned int Depth, float refractiveness);

void Render(__int32* pData, int width, int height);

//x,y=>(0,1) assume camera.up(0.0f,1.0f,0.0f) right(1.0f,0.0f,0.0f) eye(0.0f,0.0f,-1.0f)
Ray GenerateRay(float x, float y,float fovAngle, float aspect);

void InitScene(Scene& scene);

void InitCamera(Camera& camera);