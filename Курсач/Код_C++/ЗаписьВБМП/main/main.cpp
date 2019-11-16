//#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int width, int paddingSize);
unsigned char* createBitmapInfoHeader(int height, int width);


int main() {
	const int height = 10;
	const int width = 10;
	unsigned char image[height][width][bytesPerPixel];
	char* imageFileName = (char*)"bitmapImage.bmp";

	int i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (j == 1 ) {
				image[i][j][2] = (unsigned char)(255.0); ///red
				image[i][j][1] = (unsigned char)(0.0); ///green
				image[i][j][0] = (unsigned char)(0.0); ///blue
			}
			else if (j == 2) {
				image[i][j][2] = (unsigned char)(0.0); ///red
				image[i][j][1] = (unsigned char)(255.0); ///green
				image[i][j][0] = (unsigned char)(0.0); ///blue
			}
			else if ( j == 3){
				image[i][j][2] = (unsigned char)(0.0); ///red
				image[i][j][1] = (unsigned char)(0.0); ///green
				image[i][j][0] = (unsigned char)(255.0); ///blue
			}
			else {

				image[i][j][2] = (unsigned char)(255.0); ///red
				image[i][j][1] = (unsigned char)(255.0); ///green
				image[i][j][0] = (unsigned char)(255.0); ///blue

			}
		}
	}

	generateBitmapImage((unsigned char*)image, height, width, imageFileName);
	//printf("Image generated!!");
}


void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName) {

	unsigned char padding[3] = { 0, 0, 0 };
	int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;

	unsigned char* fileHeader = createBitmapFileHeader(height, width, paddingSize);
	unsigned char* infoHeader = createBitmapInfoHeader(height, width);

	//FILE* imageFile = fopen(imageFileName, "wb");
	FILE* imageFile;
	fopen_s(&imageFile, imageFileName, "wb");

	fwrite(fileHeader, 1, fileHeaderSize, imageFile);
	fwrite(infoHeader, 1, infoHeaderSize, imageFile);

	int i;
	for (i = 0; i < height; i++) {
		fwrite(image + (i * width * bytesPerPixel), bytesPerPixel, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	fclose(imageFile);
}

unsigned char* createBitmapFileHeader(int height, int width, int paddingSize) {
	int fileSize = fileHeaderSize + infoHeaderSize + (bytesPerPixel * width + paddingSize) * height;

	static unsigned char fileHeader[] = {
		0,0, /// signature
		0,0,0,0, /// image file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0, /// start of pixel array
	};

	fileHeader[0] = (unsigned char)('B');
	fileHeader[1] = (unsigned char)('M');
	fileHeader[2] = (unsigned char)(fileSize);
	fileHeader[3] = (unsigned char)(fileSize >> 8);
	fileHeader[4] = (unsigned char)(fileSize >> 16);
	fileHeader[5] = (unsigned char)(fileSize >> 24);
	fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

	return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {
	static unsigned char infoHeader[] = {
		0,0,0,0, /// header size
		0,0,0,0, /// image width
		0,0,0,0, /// image height
		0,0, /// number of color planes
		0,0, /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// image size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0, /// important color count
	};

	infoHeader[0] = (unsigned char)(infoHeaderSize);
	infoHeader[4] = (unsigned char)(width);
	infoHeader[5] = (unsigned char)(width >> 8);
	infoHeader[6] = (unsigned char)(width >> 16);
	infoHeader[7] = (unsigned char)(width >> 24);
	infoHeader[8] = (unsigned char)(height);
	infoHeader[9] = (unsigned char)(height >> 8);
	infoHeader[10] = (unsigned char)(height >> 16);
	infoHeader[11] = (unsigned char)(height >> 24);
	infoHeader[12] = (unsigned char)(1);
	infoHeader[14] = (unsigned char)(bytesPerPixel * 8);

	return infoHeader;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
