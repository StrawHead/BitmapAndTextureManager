/***************************************************
file Texture.cpp
author Layne Duras
email: duras.l@digipen.edu
DigiPen login: duras.l
Course: CS200
Assignment #6
due date: 10/21/2022
***************************************************/

#include <stdexcept> // for exception, runtime_error, out_of_range
#include <fstream>
#include <cmath>     // std::pow, std::round
#include "Affine.h"
#include "Texture.h"


//creates a W X H bottom-up bitmap object.
cs200::Bitmap::Bitmap(unsigned W, unsigned H) :
	bmp_width(W),
	bmp_height(H),
	bmp_stride(computeStride(W))
{
	//resize vector
	bmp_data.resize(bmp_stride * bmp_height);

	//initialize image data
	glm::mat4 bmp2tex = bitmapToTextureTransform(*this);

	for (int j = 0; j < bmp_height; ++j)
	{
		for (int i = 0; i < bmp_width; ++i)
		{
			glm::vec4 uv = bmp2tex * cs200::point(i, j);
			float u = uv[0],
					v = uv[1];
			int index = offset(i, j);
			float r = std::pow(u - 0.5f, 2) + std::pow(v - 0.5, 2);

			if (r > 0.16f) 
			{
				bmp_data[index + 0] = 100 + 155 * u;
				bmp_data[index + 1] = 0;
				bmp_data[index + 2] = 100 + 155 * v;
			}
			else if (r < 0.09f)
			{
				bmp_data[index + 0] = 100 + 155 * v;
				bmp_data[index + 1] = 0;
				bmp_data[index + 2] = 100 + 155 * u;
			}
			else 
			{
				bmp_data[index + 0] = 0;
				bmp_data[index + 1] = 255;
				bmp_data[index + 2] = 0;
			}
		}
	}
}

//creates bitmap object from an image file (.BMP) with name 'bmp file'.
//The image file is assumed to be a 24 bit color uncompressed standard bitmap image.
//If the file is not a valid bitmap image file, a runtime error exception should be thrown.
cs200::Bitmap::Bitmap(const char* bmp_file)
{
	std::fstream in(bmp_file, std::ios_base::binary | std::ios_base::in);
	char header[54];
	in.read(header, 54);
	if (!in) {
		throw std::runtime_error("failed to read bitmap header");
	}

	//check to make sure file is a bitmap image
	if (header[0] != 'B' || header[1] != 'M') {
		throw std::runtime_error("file is not a valid bitmap image");
	}

	//check to make sure BitPlanes is set to 1
	unsigned short bit_planes = *reinterpret_cast<unsigned short*>(header + 26);
	if (bit_planes != 1) {
		throw std::runtime_error("bit planes not set to 1");
	}

	//check to make sure BitsPerPixel is set to 24
	unsigned short bits_per_pixel = *reinterpret_cast<unsigned short*>(header + 28);
	if (bits_per_pixel != 24) {
		throw std::runtime_error("not a 24 bit color image");
	}

	//check to see Compression is set to 0
	unsigned short Compression = *reinterpret_cast<unsigned int*>(header + 30);
	if (Compression != 0) {
		throw std::runtime_error("compression field not set to 0");
	}

	bmp_width = *reinterpret_cast<int*>(header + 18);
	bmp_height = std::abs(*reinterpret_cast<int*>(header + 22));
	unsigned data_size = *reinterpret_cast<unsigned*>(header + 34),
				data_offset = *reinterpret_cast<unsigned*>(header + 10);
	bmp_stride = data_size / bmp_height;

	bmp_data.resize(bmp_stride * bmp_height);
	in.seekg(data_offset, std::ios_base::beg);
	in.read(reinterpret_cast<char*>(&bmp_data[0]), data_size);
	if (!in) {
		throw std::runtime_error("failed to read bitmap data");
	}

	reverseRGB(*this);
}

//returns the offset into the bitmap data bmp data corresponding to bitmap coordinates(i, j).
//If the coordinates(i, j) fall outside of the image, a standard out of range exception should be thrown.
unsigned cs200::Bitmap::offset(int i, int j) const
{
	return bmp_stride * j + 3 * i;
}

//returns the stride (in bytes) for a bitmap of width W pixels that is aligned along 4-byte boundaries.
unsigned cs200::computeStride(unsigned W)
{
	unsigned stride = W * 3; 
	int r = stride % 4; 

	if (r == 0)
		return stride;

	stride += (4 - r);

	return stride;
}

//reverses the byte order (from RGB to BGR and vice versa) of the bitmap object b.
void cs200::reverseRGB(Bitmap& b)
{
	unsigned char* array = b.data();
	char a = array[0];
	array[0] = array[2];
	array[2] = a;
}

//returns the transformation that maps bitmap coordinates(U, V) to texture coordinates(u, v) for the bitmap object b.
glm::mat4 cs200::bitmapToTextureTransform(const Bitmap& b)
{
	glm::mat4 H = cs200::scale(1.0f / b.width(), 1.0f / b.height());
	glm::mat4 T = cs200::affine(cs200::vector(1.0f, 0.0f), cs200::vector(0.0f, 1.0f), cs200::point(0.5f,0.5f));
	return H*T;
}

//returns the transformation that maps texture coordinates(u, v) to bitmap coordinates(U, V) for the bitmap object b.
glm::mat4 cs200::textureToBitmapTransform(const Bitmap& b)
{
	glm::mat4 M = cs200::affine(cs200::vector(b.width(),0), cs200::vector(0, b.height()), cs200::point(-0.5f, -0.5f));
	return M;
}

//returns the color of the pixel in bitmap object b that is nearest to the texture coordinates(u, v).
//Texture coordinates wrapping should be used.
glm::vec3 cs200::getColor(const Bitmap& b, float u, float v)
{
	glm::mat4 tex2bmp = textureToBitmapTransform(b);
	
	while (u >= 1)
		u -= 1;

	while (u <= 0)
		u += 1;

	while (v >= 1)
		v -= 1;

	while (v <= 0)
		v += 1;

	glm::vec4 v4 = tex2bmp * cs200::point(u, v);

	u = std::round(v4[0]);
	v = std::round(v4[1]);

	unsigned ofst = b.Bitmap::offset(u, v);

	glm::vec3 v3(b.data()[ofst + 0], b.data()[ofst + 1], b.data()[ofst + 2]);

	return v3;
}



