
#include <iostream>

#include <iostream>
#include <fstream>

#include "main.h"

using namespace std;



int main(int argc, char* argv[])
{

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " file_name" << std::endl;
		return 0;
	}

	char* fileName = argv[1];

	//char* fileName = (char*)"Kartinka.bmp";

	// открываем файл
	std::ifstream fileStream(fileName, std::ifstream::binary);
	if (!fileStream) {
		std::cout << "Error opening file '" << fileName << "'." << std::endl;
		return 0;
	}

	// заголовк изображения
	BITMAPFILEHEADER fileHeader;
	read(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
	read(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
	read(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	read(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	read(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));// номер начального байта значений цветов

	if (fileHeader.bfType != 0x4D42) {
		std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
		return 0;
	}

	// информация изображения
	BITMAPINFOHEADER fileInfoHeader;
	read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

	// bmp core
	if (fileInfoHeader.biSize >= 12) {
		read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
		read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
		read(fileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
		read(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));//битов на пикесель
	}

	// получаем информацию о битности
	int colorsCount = fileInfoHeader.biBitCount >> 3; // colorscount = 4
	if (colorsCount < 3) {
		colorsCount = 3;
	}

	int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;// bitsoncolor= 8
	int maskValue = (1 << bitsOnColor) - 1; // maskValue = 255

	// bmp v1
	if (fileInfoHeader.biSize >= 40) {
		read(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
		read(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
		read(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));// resolution_X
		read(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));//resolution_Y
		read(fileStream, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
		read(fileStream, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
	}

	// bmp v2
	fileInfoHeader.biRedMask = 0;
	fileInfoHeader.biGreenMask = 0;
	fileInfoHeader.biBlueMask = 0;

	if (fileInfoHeader.biSize >= 52) {
		read(fileStream, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
		read(fileStream, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
		read(fileStream, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
	}

	// если маска не задана, то ставим маску по умолчанию
	if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
		fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
		fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
		fileInfoHeader.biBlueMask = maskValue;
	}

	// bmp v3
	if (fileInfoHeader.biSize >= 56) {
		read(fileStream, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
	}
	else {
		fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
	}

	// bmp v4
	if (fileInfoHeader.biSize >= 108) {
		read(fileStream, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
		read(fileStream, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
		read(fileStream, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
		read(fileStream, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
		read(fileStream, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
	}

	// bmp v5
	if (fileInfoHeader.biSize >= 124) {
		read(fileStream, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
		read(fileStream, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
		read(fileStream, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
		read(fileStream, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
	}

	// проверка на поддерку этой версии формата
	if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
		fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124) {
		std::cout << "Error: Unsupported BMP format." << std::endl;
		return 0;
	}

	if (fileInfoHeader.biBitCount != 16 && fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32) {
		std::cout << "Error: Unsupported BMP bit count." << std::endl;
		return 0;
	}

	if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3) {
		std::cout << "Error: Unsupported BMP compression." << std::endl;
		return 0;
	}

	// rgb info
	RGBQUAD** rgbInfo = new RGBQUAD * [fileInfoHeader.biHeight];

	for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
		rgbInfo[i] = new RGBQUAD[fileInfoHeader.biWidth];
	}

	// определение размера отступа в конце каждой строки
	//пока будет для 32 и 24 битов с остальными может не работать
	int linePadding;
	if (fileInfoHeader.biBitCount == 24)
	{
		linePadding = 1;// 00 байт разграничение между строками
	}
	else if (fileInfoHeader.biBitCount == 32)
	{
		linePadding = 0;// 00 байта как в 24 б. в. нет ибо альфа канал
	}


	// чтение
	unsigned int bufer;

	fileStream.seekg(fileHeader.bfOffBits);// файловый указатель на начало цветов

	for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
		for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
			read(fileStream, bufer, fileInfoHeader.biBitCount / 8);

			rgbInfo[i][j].rgbRed = bitextract(bufer, fileInfoHeader.biRedMask);
			rgbInfo[i][j].rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
			rgbInfo[i][j].rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
			rgbInfo[i][j].rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);
		}
		fileStream.seekg(linePadding, std::ios_base::cur);
	}



	// вывод
	/*for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
		for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
			std::cout << std::hex
				<< +rgbInfo[i][j].rgbRed << " "
				<< +rgbInfo[i][j].rgbGreen << " "
				<< +rgbInfo[i][j].rgbBlue << " "
				<< +rgbInfo[i][j].rgbReserved
				<< std::endl;
		}
		std::cout << std::endl;
	}*/
	fileStream.close();

	fileName = (char*)"CreatedBMPc++.bmp";

	std::ofstream fileStreamo(fileName, std::ifstream::binary);

	write(fileStreamo, fileHeader.bfType, sizeof(fileHeader.bfType));
	write(fileStreamo, fileHeader.bfSize, sizeof(fileHeader.bfSize));
	write(fileStreamo, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	write(fileStreamo, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	write(fileStreamo, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));// номер начального байта значений цветов

	write(fileStreamo, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

	if (fileInfoHeader.biSize >= 12) {
		write(fileStreamo, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
		write(fileStreamo, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
		write(fileStreamo, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
		write(fileStreamo, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));//битов на пикесель
	}

	if (fileInfoHeader.biSize >= 40) {
		write(fileStreamo, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
		write(fileStreamo, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
		write(fileStreamo, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));// resolution_X
		write(fileStreamo, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));//resolution_Y
		write(fileStreamo, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
		write(fileStreamo, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
	}

	if (fileInfoHeader.biSize >= 52) {
		write(fileStreamo, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
		write(fileStreamo, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
		write(fileStreamo, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
	}

	if (fileInfoHeader.biSize >= 56) {
		write(fileStreamo, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
	}

	if (fileInfoHeader.biSize >= 108) {
		write(fileStreamo, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
		write(fileStreamo, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
		write(fileStreamo, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
		write(fileStreamo, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
		write(fileStreamo, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
	}

	if (fileInfoHeader.biSize >= 124) {
		write(fileStreamo, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
		write(fileStreamo, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
		write(fileStreamo, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
		write(fileStreamo, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
		/* дописать еще 3 группы по 4 байта paintnet сохраняет 32 битные картинки странно*/
	}

	int Red, Green, Blue;;;;;;;;;;;;;;;;
	char Byte1;// чар это целый байт
	int EndLine24bit = 0;

	if (fileInfoHeader.biBitCount == 24) // цикл записи в 24
	{
		for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
			for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {


				Red = rgbInfo[i][j].rgbRed + 100;//= bitextract(bufer, fileInfoHeader.biRedMask);
				Green = rgbInfo[i][j].rgbGreen;// = bitextract(bufer, fileInfoHeader.biGreenMask);
				Blue = rgbInfo[i][j].rgbBlue;// = bitextract(bufer, fileInfoHeader.biBlueMask)

				Red = Red << 16;// сдвиг до нужного разряда
				Green = Green << 8;// сдвиг до нужного разряда

				bufer = Red | Green | Blue;

				write(fileStreamo, bufer, fileInfoHeader.biBitCount / 8);
			}
			write(fileStreamo, EndLine24bit, sizeof(Byte1));// запись нелевого байта
		}
	}
	else if (fileInfoHeader.biBitCount == 32)// цикл записи в 32
	{
		// Если вермя будет пока 24 бита
	}

	fileStreamo.close();

	//CreateBMP((char*)"NewBMP.bmp", fileHeader,
	//	fileInfoHeader, rgbInfo, fileInfoHeader.biHeight, fileInfoHeader.biWidth);


	return 1;
}



unsigned char bitextract(const unsigned int byte, const unsigned int mask) {
	if (mask == 0) {
		return 0;
	}

	// определение количества нулевых бит справа от маски
	int
		maskBufer = mask,
		maskPadding = 0;

	while (!(maskBufer & 1)) {
		maskBufer >>= 1;
		maskPadding++;
	}

	// применение маски и смещение
	return (byte & mask) >> maskPadding;
}

/*void CreateBMP(char* fileName, BITMAPFILEHEADER& fileHeader,
	BITMAPINFOHEADER fileInfoHeader, RGBQUAD** rgbInfo,
	unsigned int n, unsigned int m)
{
	std::ofstream fileStream(fileName, std::ifstream::binary);

	//write(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
	//write(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
	//write(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	//write(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	//write(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));


	fileStream.close();
}*/

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
