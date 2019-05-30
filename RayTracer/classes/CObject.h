/** Types of objects
 */
#define OBJ_SPHERE 0
#define OBJ_TRIANGLE 1
#define OBJ_PLANE 2

/** Virtual class defines object
 */
class CObject {
public:
	int type; /**< object type (OBJ_SPHERE, OBJ_TRIANGLE) */
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shine;
	float reflecting;
	CImage *tex;
	string texture = "";
	float transparent = 0.0f;

	CObject() {
	}

	/** intersect computing */
	virtual float intersect(CRay* ray) {
		return -1;
	}
	/** normal vector computing */
	virtual vec3 normal(CRay *ray) {
		return vec3(NULL);
	}
	// texture color computing
	virtual vec3 texColor(vec3 hitPos) {
		return vec3(NULL);
	}
	virtual CRay* transRay(vec3 hitPos, CRay *ray, float factor, bool reverse) {
		return NULL;
	}
};

/** Sphere class
 */
class CSphere : public CObject {
public:
	float r; /**< radius */
	vec3 o; /**< center point position */

	CSphere(float sR, vec3 sO, float sReflecting, vec3 sAmbient, vec3 sDiffuse, vec3 sSpecular, float sShine, float sTransparent, string sTexture) {
		type = OBJ_SPHERE;
		r = sR;
		o = sO;
		reflecting = sReflecting;
		ambient = sAmbient;
		diffuse = sDiffuse;
		specular = sSpecular;
		shine = sShine;
		transparent = sTransparent;
		if (sTexture.length() > 0) {
			tex = new CImage;
			texture = sTexture;
			FREE_IMAGE_FORMAT ftype = FreeImage_GetFileType(texture.c_str(), 0);
			tex->bitmap = FreeImage_Load(ftype, texture.c_str());
			tex->width = FreeImage_GetWidth(tex->bitmap);
			tex->height = FreeImage_GetHeight(tex->bitmap);
		}
	}

	/** Sphere intersection computing */
	float intersect(CRay* ray) {
		float a, b, c, delta, x0, x1, x2;
		vec3 v = ray->pos - o;
		a = ray->dir.x * ray->dir.x + ray->dir.y * ray->dir.y + ray->dir.z * ray->dir.z;
		b = 2.0f * (v.x * ray->dir.x + v.y * ray->dir.y + v.z * ray->dir.z);
		c = v.x * v.x + v.y * v.y + v.z * v.z - r * r;
		delta = b * b - 4.0f * a * c;
		if (delta > 0) {
			x1 = (-b - sqrt(delta)) / (2.0f * a);
			x2 = (-b + sqrt(delta)) / (2.0f * a);
			if (x1 <= x2 && x1 > 0) ray->t = x1;
			else if(x2 > 0) ray->t = x2;
			else {
				ray->t = INFINITY;
				return 0;
			}
			return 2;
		}
		else if (delta == 0) {
			x0 = -b / (2.0f * a);
			if (x0 > 0) {
				ray->t = x0;
				return 1;
			}
			else {
				ray->t = INFINITY;
				return 0;
			}
		}
		else if (delta < 0) {
			ray->t = INFINITY;
			return 0;
		}
	}
	// returns normal vector of Sphere
	vec3 normal(CRay *ray) { 
		return normalize((ray->pos + ray->t * ray->dir) - o);
	}
	// returns color of texture texel
	vec3 texColor(vec3 hitPos) {
		if (texture.length() > 0) {
			hitPos -= o;
			float s = acos(hitPos.z / (float)r) / PI;
			float t = acos(hitPos.x / ((float)r * sin(PI * s))) / PI;
			int i = floor(abs(1.0f - s) * tex->width);
			int j = floor(t * tex->height);
			return tex->get(i, j)/255.0f;
		}
		return vec3(0, 0, 0);
	}
	// returns transparent Ray
	CRay* transRay(vec3 hitPos, CRay *ray, float factor, bool reverse) {
		CRay *transparentRay = new CRay;
		vec3 centerDir = normalize(hitPos - o);

		if (reverse) {
			factor *= 0.3f;
			transparentRay->pos = hitPos + r * 2.0f * centerDir * -1.0f;
			transparentRay->pos += centerDir * -1.0f * 0.01f;
		}
		else {
			transparentRay->pos = hitPos + r * 2.0f * ray->dir;
			transparentRay->pos += ray->dir * 0.01f;
		}

		transparentRay->dir.x = (ray->dir.x*factor - centerDir.x*(1.0f - factor)) / 2.0f;
		transparentRay->dir.y = (ray->dir.y*factor - centerDir.y*(1.0f - factor)) / 2.0f;
		transparentRay->dir.z = (ray->dir.z*factor - centerDir.z*(1.0f - factor)) / 2.0f;

		return transparentRay;
	}
};

/** Triangle class
 */
class CTriangle : public CObject {
public:
	vec3 p0;
	vec3 p1;
	vec3 p2;

	CTriangle(vec3 tP0, vec3 tP1, vec3 tP2, float tReflecting, vec3 tAmbient, vec3 tDiffuse, vec3 tSpecular, float tShine, float tTransparent) {
		type = OBJ_TRIANGLE;
		p0 = tP0;
		p1 = tP1;
		p2 = tP2;
		reflecting = tReflecting;
		ambient = tAmbient;
		diffuse = tDiffuse;
		specular = tSpecular;
		shine = tShine;
		transparent = tTransparent;
	}

	/** Triangle intersection computing */
	float intersect(CRay* ray) {
		vec3 n = normal(ray);
		float d = -dot(n, p0);
		float x0 = -(dot(n, ray->pos) + d) / dot(n, ray->dir);
		vec3 p = ray->pos + x0 * ray->dir;

		vec3 v0 = p2 - p0;
		vec3 v1 = p1 - p0;
		vec3 v2 = p - p0;

		float dot00 = dot(v0, v0);
		float dot01 = dot(v0, v1);
		float dot02 = dot(v0, v2);
		float dot11 = dot(v1, v1);
		float dot12 = dot(v1, v2);

		float invDenom = 1.0f / (dot00*dot11 - dot01*dot01);
		float bu = (dot11*dot02 - dot01*dot12) * invDenom;
		float bv = (dot00*dot12 - dot01*dot02) * invDenom;

		if (bu >= 0 && bv >= 0 && bu + bv < 1) {
			ray->t = x0;
			return 1;
		}
		return 0;
	}
	// returns normal vector of triangle
	vec3 normal(CRay *ray) {
		vec3 u = p1 - p0;
		vec3 v = p2 - p0;
		vec3 n;
		n.x = u.y*v.z - u.z*v.y;
		n.y = u.z*v.x - u.x*v.z;
		n.z = u.x*v.y - u.y*v.x;
		return normalize(n);
	}

	CRay* transRay(vec3 hitPos, CRay *ray, float factor, bool reverse) {
		CRay *transparentRay = new CRay;
		transparentRay->pos = hitPos + ray->dir * 0.01f;
		transparentRay->dir = ray->dir;

		return transparentRay;
	}
};

/** Light source definition
 */
class CLight {
public:
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	CLight(vec3 lPos, vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular) {
		pos = lPos;
		ambient = lAmbient;
		diffuse = lDiffuse;
		specular = lSpecular;
	}
};