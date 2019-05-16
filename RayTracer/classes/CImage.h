/** Klasa reprezentujaca obraz wyjsciowy
 */
class CImage {
public:
	FIBITMAP* bitmap;
	RGBQUAD data; /**< dane obrazu */
	int width; /**< liczba pikseli w poziomie */
	int height; /**< liczba pikseli w pionie */

	CImage(int fwidth = 800, int fheight = 600) {
		width = fwidth;
		height = fheight;
		FreeImage_Initialise();
		bitmap = FreeImage_Allocate(width, height, 24);
		if (!bitmap) exit(1);
	}

	void set(int i, int j, vec3 color) {
		data.rgbRed = color[0];
		data.rgbGreen = color[1];
		data.rgbBlue = color[2];

		FreeImage_SetPixelColor(bitmap, i, j, &data);
	}

	/** Zapisanie obrazu w pliku dyskowym
	 *	@param fname - nazwa pliku
	 */
	void save(const char* fname) {
		if (FreeImage_Save(FIF_PNG, bitmap, fname, 0))
			cout << "Image successfully saved!" << endl;
	};
};