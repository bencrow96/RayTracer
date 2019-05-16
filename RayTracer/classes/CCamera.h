/** Klasa opisujaca parametry kamery
 */
class CCamera {
public:
	vec3 pos; // pozycja poczatkowa
	vec3 target; // kierunek patrzenia
	vec3 up; // gora obrazu
	int width; // szerokosc
	int height; // wysokosc
	int fov; // FOV
};