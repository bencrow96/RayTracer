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

				rayTrace(primaryRay, output); 

				// >1 color cutting
				if (output->color.x > 1) output->color.x = 1;
				if (output->color.y > 1) output->color.y = 1;
				if (output->color.z > 1) output->color.z = 1;
				// gamma correction
				output->color.x = pow(output->color.x, 1 / 2.2);
				output->color.y = pow(output->color.y, 1 / 2.2);
				output->color.z = pow(output->color.z, 1 / 2.2);

				scene->image->set(x, y, output->color * 255.0f);

				// reset output
				output->energy = 1.0f;
				output->tree = 0;
				output->color = vec3(0, 0, 0);
			}
		}

		return 0;
	}

	int closest(CRay *ray) { // choose an intersected object by minimal positive t
		float minT = INFINITY;
		int index = 0;
		for (int i = 0; i < scene->objCount; i++) {
			if (scene->obj[i]->intersect(ray) && ray->t < minT && ray->t > 0.001) {
				minT = ray->t;
				index = i;
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
			if (scene->obj[i]->intersect(ray) && ray->t > 1.5)  return true;
		}
		return false;
	}

	/** Sending ray
	*/
	int rayTrace(CRay *ray, COutput *output) {
		int index = closest(ray);
		if (index > -1) { // when intersected
			vec3 hitPos = ray->pos + ray->t * ray->dir;
			vec3 n = scene->obj[index]->normal(ray); // normal vector
			// shadow
			CRay *shadowRay = new CRay;
			shadowRay->pos = hitPos;
			// blinn - phong model
			vec3 a = vec3(0, 0, 0);
			vec3 d = vec3(0, 0, 0);
			vec3 s = vec3(0, 0, 0);
			//vec3 v = normalize(ray->pos - shadowRay->pos); // v = -ray->dir
			for (int i = 0; i < scene->lightCount; i++) {
				shadowRay->dir = normalize(scene->lights[i]->pos - shadowRay->pos);
				shadowRay->pos += shadowRay->dir * 0.01f; // move ray from the surface to avoid hitting it

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
				float asd = dot(shadowRay->dir, n);
				if (asd < 0) asd = 0;
				d += scene->obj[index]->diffuse * ld * asd;
				// specular
				vec3 h = normalize(shadowRay->dir - ray->dir);
				float qwe = pow(dot(n, h), scene->obj[index]->shine);
				if (qwe < 0) qwe = 0;
				s += scene->obj[index]->specular * ls * qwe;
			}

			output->color += (a + d + s) * output->energy;

			// reflections
			if (scene->obj[index]->reflecting && output->tree < 1) {
				output->energy *= 0.8f;
				CRay *reflectRay = new CRay;
				reflectRay->pos = hitPos;
				reflectRay->dir = normalize(ray->dir - (2.0f * ray->dir * n) * n);
				reflectRay->pos += reflectRay->dir * 0.01f;

				output->tree++;
				rayTrace(reflectRay, output);

				//clean
				delete reflectRay;
			}

			// clean
			delete shadowRay;
		}
		else output->color += vec3(0, 0, 0);

		return 0;
	}
};
