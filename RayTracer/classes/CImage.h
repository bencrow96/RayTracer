/** Image class
 */
class CImage {
public:
	FIBITMAP* bitmap;
	RGBQUAD data;
	int width;
	int height;

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

	vec3 get(int i, int j) {
		RGBQUAD color;
		FreeImage_GetPixelColor(bitmap, i, j, &color);
		return vec3(color.rgbRed, color.rgbGreen, color.rgbBlue);
	}

	/** Save image
	 *	@param fname - file name
	 */
	void save(const char* fname) {
		if (FreeImage_Save(FIF_PNG, bitmap, fname, 0))
			cout << "Image successfully saved!" << endl;
	};
};