#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
 
using namespace std;
 
#pragma pack(1) 
  
typedef struct BitMapFileHeader{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved_1, bfReserved_2;
    DWORD bfOffBits;                
};
 
typedef struct BitMapInfoHeader{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount; 
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
 
typedef struct RGB_Struct{
    char rgbBlue;
	char rgbGreen;
	char rgbRed;
   // char rgbReserved;
};
 
 class image
{
	private: 
		BitMapFileHeader file_header; 
        BitMapInfoHeader info_header;
        RGB_Struct rgb_l;
		char** r;
		char** g;
		char** b;
		char** rr;
		FILE *f1;
	public: 
		image();
		image(char *name);
		void create();
};

 /*image::image()
 {
	 memset (&file_header, 0, sizeof(file_header));
	file_header.bfType=0x4D42;
	file_header.bfOffBits = sizeof(file_header) + sizeof(info_header) + 1024;
	file_header.bfSize = file_header.bfOffBits + 
         sizeof(0x1f02)*100*100+ 
         100*((sizeof(0x1f02)*100) % 4);
	memset (&info_header, 0, sizeof(info_header));
	info_header.biSize = sizeof(info_header);
	info_header.biBitCount = 24;
	info_header.biClrUsed=16777216;
	info_header.biCompression = BI_RGB;
	info_header.biHeight=100;
	info_header.biWidth=100;
	info_header.biPlanes= 1;
 }*/

 image::image(char *name)
 {
	int i;
	if ((f1=fopen(name, "rb"))==NULL)
	{
			printf("file not found \n");
			exit(1);
	 }
	fread(&file_header, sizeof(file_header),1 ,f1);
	 fread(&info_header, sizeof(info_header), 1, f1);
	  fread(&rgb_l, sizeof(rgb_l), 1, f1);

		r=new char*[info_header.biHeight]; 
		for(int i=0; i<info_header.biHeight;i++) 
		{ 
			r[i]=new char[info_header.biWidth]; 
		} 

		g=new char*[info_header.biHeight]; 
		for(i=0; i<info_header.biHeight;i++) 
		{ 
			g[i]=new char[info_header.biWidth]; 
		} 

		b=new char*[info_header.biHeight]; 
		for(i=0; i<info_header.biHeight;i++) 
		{ 
			b[i]=new char[info_header.biWidth]; 
		} 

	//	rr=new char*[info_header.biHeight]; 
	//	for( i=0; i<info_header.biHeight;i++) 
	//	{ 
	//		rr[i]=new char[info_header.biWidth]; 
	//	} 

		fseek(f1, file_header.bfOffBits, SEEK_SET);
		fread(&rgb_l, sizeof(rgb_l), 1, f1);
		for(i=0; i<info_header.biHeight;i++) 
		{
			for(int j=0; j<info_header.biWidth;j++) 
			{
				fread(&rgb_l, sizeof(rgb_l), 1, f1);
				b[i][j]=rgb_l.rgbBlue;
				g[i][j]=rgb_l.rgbGreen;
				r[i][j]=rgb_l.rgbRed;
			//	rr[i][j]=rgb_l.rgbReserved;
			}
		}
		fclose(f1);
 }

 void image::create()
 {
	FILE *f2;
	if ((f2=fopen("D:\\temp.bmp", "wb"))==NULL)
	{
		printf("file not created \n");
	}
	fwrite(&file_header, sizeof(file_header), 1, f2);
    fwrite(&info_header, sizeof(info_header), 1, f2);
	fseek(f2, file_header.bfOffBits, SEEK_SET);
	size_t padding = 0;
    if ((info_header.biWidth * 3) % 4) padding = 4 - (info_header.biWidth * 3) % 4;
 
        for(int i=0;i< info_header.biHeight;i++)
        {
                for (int j = 0; j < info_header.biWidth; j++)
				{
					rgb_l.rgbBlue=b[i][j];
					rgb_l.rgbGreen=g[i][j];
					rgb_l.rgbRed=r[i][j];
				   // rgb_l.rgbReserved=rr[i][j];
                    fwrite(&rgb_l, sizeof(rgb_l), 1, f2);
					//fwrite(&rgb_l.rgbBlue, sizeof(rgb_l.rgbBlue), 1, f3);
                }
			 if(padding != 0) 
			 {
				fread(&rgb_l, padding,1, f1);
				fwrite(&rgb_l, padding, 1, f2);
			 }   
        }
		fclose(f2);
 }

int main()
{
	image obj1((char*)"3.bmp");
	obj1.create();
   // getch();
    return 0;
}