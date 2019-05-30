#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
constexpr auto PI = 3.14159265358979323846f;

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/intersect.hpp"
#include "FreeImage/FreeImage.h"
using namespace glm;
using namespace std;

#include "classes/COutput.h"
#include "classes/CCamera.h"
#include "classes/CRay.h"
#include "classes/CImage.h"
#include "classes/CObject.h"
#include "classes/CScene.h"
#include "classes/CRayTrace.h"

int main()
{
	CRayTrace *RayTrace = new CRayTrace;
	int err = RayTrace->scene->parse("../scene3.txt");
	if (!err) {
		RayTrace->init();
		RayTrace->scene->image->save("../result.png");
	}

	return 0;
}