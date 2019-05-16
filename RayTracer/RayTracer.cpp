#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "FreeImage/FreeImage.h"
using namespace glm;
using namespace std;

#include "classes/COutput.h"
#include "classes/CCamera.h"
#include "classes/CRay.h"
#include "classes/CObject.h"
#include "classes/CImage.h"
#include "classes/CScene.h"
#include "classes/CRayTrace.h"

int main()
{
	CRayTrace *RayTrace = new CRayTrace;
	int err = RayTrace->scene->parse("../scene_trian.txt");
	if (!err) {
		RayTrace->rayTrace();
		RayTrace->scene->image->save("../result.png");
	}

	return 0;
}