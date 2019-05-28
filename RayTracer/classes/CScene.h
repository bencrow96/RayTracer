/** Scene class
 */
class CScene {
public:
	CImage *image;
	CCamera cam;
	bool superSampling = false;

	CObject* obj[200]; /**< list of objects */
	int objCount = 0;

	CLight* lights[10]; /**< list of lights */
	int lightCount = 0;

	/** Parsing text file into scene
	 *	@param - file name
	 */
	vector<string> split(string str, string token) {
		vector<string>result;
		while (str.size()) {
			int index = str.find(token);
			if (index != string::npos) {
				result.push_back(str.substr(0, index));
				str = str.substr(index + token.size());
				if (str.size() == 0)result.push_back(str);
			}
			else {
				result.push_back(str);
				str = "";
			}
		}
		return result;
	}
	string removeSpaces(string &str)
	{
		int n = str.length();
		int i = 0, j = -1;
		bool spaceFound = false;
		while (++j < n && str[j] == ' ');
		while (j < n)
		{
			if (str[j] != ' ')
			{
				str[i++] = str[j++];
				spaceFound = false;
			}
			else if (str[j++] == ' ')
			{
				if (!spaceFound)
				{
					str[i++] = ' ';
					spaceFound = true;
				}
			}
		}
		str.erase(str.begin() + i, str.end());

		return str;
	}
	
	int parse(const char* fname) {
		ifstream in(fname);
		string line;
		char commentChar = '%';
		vector<string> arguments;

		if (!in) {
			cout << "File cannot be opened" << endl;
			return 1;
		}
		while (getline(in, line)) {
			if (line[0] == commentChar || line[0] == '\0') continue;
			line = removeSpaces(line);
			cout << line << endl;
			arguments = split(line, " ");
			if (arguments.at(0) == "cam_width") {
				cam.width = atof(arguments.at(1).c_str());
			}
			else if (arguments.at(0) == "cam_height") {
				cam.height = atof(arguments.at(1).c_str());
			}
			else if (arguments.at(0) == "cam_FOV") {
				cam.fov = atof(arguments.at(1).c_str());
			}
			else if (arguments.at(0) == "cam_eyep") {
				float x = atof(arguments.at(1).c_str());
				float y = atof(arguments.at(2).c_str());
				float z = atof(arguments.at(3).c_str());
				cam.pos = vec3(x, y, z);
			}
			else if (arguments.at(0) == "cam_lookp") {
				float x = atof(arguments.at(1).c_str());
				float y = atof(arguments.at(2).c_str());
				float z = atof(arguments.at(3).c_str());
				cam.dir = vec3(x, y, z);
			}
			else if (arguments.at(0) == "cam_up") {
				float x = atof(arguments.at(1).c_str());
				float y = atof(arguments.at(2).c_str());
				float z = atof(arguments.at(3).c_str());
				cam.up = vec3(x, y, z);
			}
			else if (arguments.at(0) == "sphere") {
				float r = atof(arguments.at(1).c_str());
				float ox = atof(arguments.at(2).c_str());
				float oy = atof(arguments.at(3).c_str());
				float oz = atof(arguments.at(4).c_str());
				float ref = atof(arguments.at(5).c_str());
				float ax = atof(arguments.at(6).c_str());
				float ay = atof(arguments.at(7).c_str());
				float az = atof(arguments.at(8).c_str());
				float dx = atof(arguments.at(9).c_str());
				float dy = atof(arguments.at(10).c_str());
				float dz = atof(arguments.at(11).c_str());
				float sx = atof(arguments.at(12).c_str());
				float sy = atof(arguments.at(13).c_str());
				float sz = atof(arguments.at(14).c_str());
				float sh = atof(arguments.at(15).c_str());
				float tr = atof(arguments.at(16).c_str());
				string tex = "";
				if (arguments.size() > 17) {
					if (arguments.at(17).length() > 0) tex = arguments.at(17);
				}
				CObject *sphere = new CSphere(r, vec3(ox, oy, oz), ref, vec3(ax, ay, az), vec3(dx, dy, dz), vec3(sx, sy, sz), sh, tr, tex);
				obj[objCount++] = sphere;
			}
			else if (arguments.at(0) == "triangle") {
				float p0x = atof(arguments.at(1).c_str());
				float p0y = atof(arguments.at(2).c_str());
				float p0z = atof(arguments.at(3).c_str());
				float p1x = atof(arguments.at(4).c_str());
				float p1y = atof(arguments.at(5).c_str());
				float p1z = atof(arguments.at(6).c_str());
				float p2x = atof(arguments.at(7).c_str());
				float p2y = atof(arguments.at(8).c_str());
				float p2z = atof(arguments.at(9).c_str());
				float ref = atof(arguments.at(10).c_str());
				float ax = atof(arguments.at(11).c_str());
				float ay = atof(arguments.at(12).c_str());
				float az = atof(arguments.at(13).c_str());
				float dx = atof(arguments.at(14).c_str());
				float dy = atof(arguments.at(15).c_str());
				float dz = atof(arguments.at(16).c_str());
				float sx = atof(arguments.at(17).c_str());
				float sy = atof(arguments.at(18).c_str());
				float sz = atof(arguments.at(19).c_str());
				float sh = atof(arguments.at(20).c_str());
				CObject *triangle = new CTriangle(vec3(p0x,p0y,p0z), vec3(p1x,p1y,p1z), vec3(p2x,p2y,p2z), ref, vec3(ax, ay, az), vec3(dx, dy, dz), vec3(sx, sy, sz), sh);
				obj[objCount++] = triangle;
			}
			else if (arguments.at(0) == "light") {
				float ox = atof(arguments.at(1).c_str());
				float oy = atof(arguments.at(2).c_str());
				float oz = atof(arguments.at(3).c_str());
				float ax = atof(arguments.at(4).c_str());
				float ay = atof(arguments.at(5).c_str());
				float az = atof(arguments.at(6).c_str());
				float dx = atof(arguments.at(7).c_str());
				float dy = atof(arguments.at(8).c_str());
				float dz = atof(arguments.at(9).c_str());
				float sx = atof(arguments.at(10).c_str());
				float sy = atof(arguments.at(11).c_str());
				float sz = atof(arguments.at(12).c_str());
				CLight *light = new CLight(vec3(ox, oy, oz), vec3(ax, ay, az), vec3(dx, dy, dz), vec3(sx, sy, sz));
				lights[lightCount++] = light;
			}
			else if (arguments.at(0) == "superSampling") {
				if (arguments.at(1) == "true") superSampling = true;
			}
		}
		image = new CImage(cam.width, cam.height);
		return 0;
	}
};