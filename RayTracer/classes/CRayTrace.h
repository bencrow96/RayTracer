/** Raytracer main class
 */
class CRayTrace {
private:
	CRay *primaryRay;
public:
	COutput *output;
	CScene *scene;

	/** Main loop
	 */
	CRayTrace() {
		scene = new CScene;
		output = new COutput;
		primaryRay = new CRay;
	}

	int init() {
		vec3 look, u, v, o;
		float radian = scene->cam.fov * 3.1416 / 180;
		look = scene->cam.target - scene->cam.pos;
		u = normalize(cross(scene->cam.up, look));
		v = normalize(cross(look, u));
		o = normalize(look) * (scene->cam.width / (2 * tan(radian / 2))) - (float)(scene->cam.width / 2) * u - (float)(scene->cam.height / 2) * v;

		for (int y = 0; y < scene->cam.height; y++) {
			for (int x = 0; x < scene->cam.width; x++) {
				primaryRay->pos = scene->cam.pos;
				primaryRay->dir = normalize(mat3(u, v, o) * vec3(x, y, 1));
				primaryRay->t = -1;

				rayTrace(primaryRay, output);

				scene->image->set(x, y, output->color * 255.0f);
			}
		}

		return 0;
	}

	int closest(CRay *ray) { // choose an intersected object by minimal positive t
		float minT = INFINITY;
		int index = 0;
		for (int i = 0; i < scene->objCount; i++) {
			if (scene->obj[i]->intersect(ray)) {
				if (ray->t > 0.01 && ray->t < minT) {
					minT = ray->t;
					index = i;
				}
			}
		}
		if (minT < INFINITY) {
			ray->t = minT;
			return index;
		}
		else return -1;
	}
	bool shadow(CRay *ray) { // check if ray intersects with anything
		for (int i = 0; i < scene->objCount; i++) {
			if (scene->obj[i]->intersect(ray) && ray->t > 0.001)  return true; //ray->t > 0.001 lets avoid hit surface that was just collided
		}
		return false;
	}

	/** Sending ray
	*/
	int rayTrace(CRay *ray, COutput *output) {
		CRay *shadowRay = new CRay;

		int index = closest(primaryRay);
		if (index > -1) { // when intersected
			// blinn - phong model
			shadowRay->pos = primaryRay->pos + primaryRay->t * primaryRay->dir;
			shadowRay->t = -1;
			vec3 a = vec3(0, 0, 0);
			vec3 d = vec3(0, 0, 0);
			vec3 s = vec3(0, 0, 0);
			vec3 n = scene->obj[index]->normal(primaryRay);
			vec3 v = normalize(scene->cam.pos - shadowRay->pos);
			for (int i = 0; i < scene->lightCount; i++) {
				vec3 l = normalize(scene->lights[i]->pos - shadowRay->pos);
				shadowRay->dir = l;

				vec3 la, ld, ls;

				la = scene->lights[i]->ambient;
				if (shadow(shadowRay)) { // when covered by shadow
					ld = vec3(0, 0, 0);
					ls = vec3(0, 0, 0);
				}
				else {
					ld = scene->lights[i]->diffuse;
					ls = scene->lights[i]->specular;
				}

				// ambient
				a += scene->obj[index]->ambient * la;
				// diffuse
				float asd = dot(l, n);
				if (asd < 0) asd = 0;
				d += scene->obj[index]->diffuse * ld * asd;
				// specular
				vec3 h = normalize(l + v);
				float qwe = pow(dot(n, h), scene->obj[index]->shine);
				if (qwe < 0) qwe = 0;
				s += scene->obj[index]->specular * ls * qwe;
			}

			output->color = a + d + s;
			// >1 color cutting
			if (output->color.x > 1) output->color.x = 1;
			if (output->color.y > 1) output->color.y = 1;
			if (output->color.z > 1) output->color.z = 1;
			// gamma correction
			output->color.x = pow(output->color.x, 1 / 2.2);
			output->color.y = pow(output->color.y, 1 / 2.2);
			output->color.z = pow(output->color.z, 1 / 2.2);
		}
		else output->color = vec3(0, 0, 0);

		return 0;
	}
};
