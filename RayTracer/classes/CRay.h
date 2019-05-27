/** Ray class
 */
class CRay {
public:
	vec3 pos; // start position
	vec3 dir; // direction
	float t = -1.0f; // distance from start to intersection
};