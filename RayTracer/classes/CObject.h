/** Typy obiektow
 */
#define OBJ_SPHERE 0
#define OBJ_TRIANGLE 1
#define OBJ_PLANE 2

/** Wirtualna klasa definuj¹ca obiekt
 */
class CObject {
public:
	int type; /**< rodzaj obiektu (OBJ_SPHERE, OBJ_TRIANGLE) */
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shine;
	float reflecting;

	CObject() {
	}

	/** Obliczenie przeciêcia promienia z obiektem */
	virtual float intersect(CRay* ray) {
		return -1;
	}

	virtual vec3 normal(CRay *ray) {
		return vec3(NULL);
	}

};

/** Klasa opisujaca obiekt - kula
 */
class CSphere : public CObject {
public:
	float r; /**< promien */
	vec3 o; /**< polozenie srodka */

	CSphere(float sR, vec3 sO, float sReflecting, vec3 sAmbient, vec3 sDiffuse, vec3 sSpecular, float sShine) {
		type = OBJ_SPHERE;
		r = sR;
		o = sO;
		reflecting = sReflecting;
		ambient = sAmbient;
		diffuse = sDiffuse;
		specular = sSpecular;
		shine = sShine;
	}

	/** Obliczenie przeciêcia promienia z kula */
	float intersect(CRay* ray) {
		float a, b, c, delta, x0, x1, x2;
		vec3 v = ray->pos - o;
		a = ray->dir.x * ray->dir.x + ray->dir.y * ray->dir.y + ray->dir.z * ray->dir.z;
		b = 2 * (v.x * ray->dir.x + v.y * ray->dir.y + v.z * ray->dir.z);
		c = v.x * v.x + v.y * v.y + v.z * v.z - r * r;
		delta = b * b - 4 * a * c;
		if (delta > 0) {
			x1 = (-b - sqrt(delta)) / (float)(2 * a);
			x2 = (-b + sqrt(delta)) / (float)(2 * a);
			if (x1 < x2 && x1 > 0) ray->t = x1;
			else ray->t = x2;
			return 2;
		}
		else if (delta == 0) {
			x0 = -b / (float)(2 * a);
			if (x0 > 0) {
				ray->t = x0;
				return 1;
			}
			return 0;
		}
		else if (delta < 0) {
			ray->t = INFINITY;
			return 0;
		}
	}

	vec3 normal(CRay *ray) { // returns normal vec
		return normalize((ray->pos + ray->t * ray->dir) - o);
	}
};

/** Klasa opisujaca obiekt - triangle
 */
class CTriangle : public CObject {
public:
	vec3 p0;
	vec3 p1;
	vec3 p2;

	CTriangle(vec3 tP0, vec3 tP1, vec3 tP2, float tReflecting, vec3 tAmbient, vec3 tDiffuse, vec3 tSpecular, float tShine) {
		type = OBJ_TRIANGLE;
		p0 = tP0;
		p1 = tP1;
		p2 = tP2;
		reflecting = tReflecting;
		ambient = tAmbient;
		diffuse = tDiffuse;
		specular = tSpecular;
		shine = tShine;
	}

	/** Obliczenie przeciêcia promienia z trojkatem */
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

		float invDenom = 1 / (dot00*dot11 - dot01*dot01);
		float bu = (dot11*dot02 - dot01*dot12) * invDenom;
		float bv = (dot00*dot12 - dot01*dot02) * invDenom;

		if (bu >= 0 && bv >= 0 && bu + bv < 1) {
			ray->t = x0;
			return 1;
		}
		return 0;
	}
	vec3 normal(CRay *ray) {
		vec3 u = p1 - p0;
		vec3 v = p2 - p0;
		vec3 n;
		n.x = u.y*v.z - u.z*v.y;
		n.y = u.z*v.x - u.x*v.z;
		n.z = u.x*v.y - u.y*v.x;
		return normalize(n);
	}
};

/** Definicja zrodla swiatla
 */
class CLight {
public:
	vec3 pos; // pozycja
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