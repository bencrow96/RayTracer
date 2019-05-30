/** Raytracer main class
 */
class CRayTrace {
private:
	CRay *primaryRay;
public:
	COutput *output;
	CScene *scene;

	
	CRayTrace() {
		scene = new CScene;
		output = new COutput;
		primaryRay = new CRay;
	}

	/** Main loop
	 */
	int init() {
		vec3 look, u, v, o;
		float radian = scene->cam.fov * PI / 180.0f;
		look = scene->cam.dir - scene->cam.pos;
		u = normalize(cross(scene->cam.up, look));
		v = normalize(cross(look, u));
		o = normalize(look) * (scene->cam.width / (2 * tan(radian / 2.0f))) - (scene->cam.width / 2.0f) * u - (scene->cam.height / 2.0f) * v;

		for (int y = 0; y < scene->cam.height; y++) {
			for (int x = 0; x < scene->cam.width; x++) {
				primaryRay->pos = scene->cam.pos;
				primaryRay->dir = normalize(mat3(u, v, o) * vec3(x, y, 1));
				// Supersampling
				float amount = 0.0005f;
				if (scene->superSampling) {
					rayTrace(primaryRay, output);
					vec3 color = output->color;
					for (int i = 1; i < 9; i++) {
						switch (i) {
						case 1: 
							primaryRay->dir.x -= amount;
							primaryRay->dir.y += amount;
							break;
						case 2:
							primaryRay->dir.y += amount;
							break;
						case 3:
							primaryRay->dir.x += amount;
							primaryRay->dir.y += amount;
							break;
						case 4:
							primaryRay->dir.x -= amount;
							break;
						case 5:
							primaryRay->dir.x += amount;
							break;
						case 6:
							primaryRay->dir.x -= amount;
							primaryRay->dir.y -= amount;
							break;
						case 7:
							primaryRay->dir.y -= amount;
							break;
						case 8:
							primaryRay->dir.x += amount;
							primaryRay->dir.y -= amount;
							break;
						}
						rayTrace(primaryRay, output);
						color += output->color;
						// reset
						output->energy = 1.0f;
						output->tree = 0;
						output->color = vec3(0, 0, 0);
						primaryRay->dir = normalize(mat3(u, v, o) * vec3(x, y, 1));
					}
					output->color = color / 9.0f;
					
				}
				else rayTrace(primaryRay, output); 

				// >1 color cutting
				if (output->color.x > 1) output->color.x = 1;
				if (output->color.y > 1) output->color.y = 1;
				if (output->color.z > 1) output->color.z = 1;
				// gamma correction
				if (output->color.x < 0.00304f) output->color.x *= 12.92f;
				else output->color.x = 1.055f * pow(output->color.x, 1.0f / 2.2f) - 0.055f;
				if (output->color.y < 0.00304f) output->color.y *= 12.92f;
				else output->color.y = 1.055f * pow(output->color.y, 1.0f / 2.2f) - 0.055f;
				if (output->color.z < 0.00304f) output->color.z *= 12.92f;
				else output->color.z = 1.055f * pow(output->color.z, 1.0f / 2.2f) - 0.055f;

				scene->image->set(x, y, output->color * 255.0f);

				// reset output
				output->energy = 1.0f;
				output->tree = 0;
				output->color = vec3(0, 0, 0);

			}
			cout << (y+1) * 100 / (float)(scene->cam.height) << "%" << endl;
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
			if (scene->obj[i]->intersect(ray) && ray->t > 1.5 && !scene->obj[i]->transparent) {
				return true;
			}
		}
		return false;
	}

	/** Sending ray
	*/
	int rayTrace(CRay *ray, COutput *output) {
		vec3 hitPos;
		int index = closest(ray);
		if (index > -1) { // when intersected
			hitPos = ray->pos + ray->t * ray->dir;
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
				// diffuse / texture
				float asd = dot(shadowRay->dir, n);
				if (asd < 0) asd = 0;
				if (scene->obj[index]->texture.length() > 0) {
					d += scene->obj[index]->texColor(hitPos) * ld * asd;
				}
				else {
					d += scene->obj[index]->diffuse * ld * asd;
				}
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

			// transparency
			if (scene->obj[index]->transparent) {
				float factor = 1.1f; // negative flips
				bool reverse = false;
				CRay *transparentRay = scene->obj[index]->transRay(hitPos, ray, factor, reverse);

				output->energy *= scene->obj[index]->transparent;
				rayTrace(transparentRay, output);
			}

			// clean
			delete shadowRay;
		}
		else output->color += vec3(0, 0, 0);

		return 0;
	}
};
