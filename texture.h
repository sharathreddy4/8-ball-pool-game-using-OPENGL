
#include <iostream>
#include <fstream>

// read 4 bytes as an integer
static int readInt(std::ifstream &input)
{
	char buffer[4];
	input.read(buffer, 4);
	return (int)(((unsigned char)buffer[3] << 24) |
				((unsigned char)buffer[2] << 16) |
				((unsigned char)buffer[1] << 8) |
				(unsigned char)buffer[0]);
}

// load BMP image and return width, height and pixel data
bool loadBmp(const char* filename, int &width, int &height, char* &data)
{
	// open file
	std::ifstream input;
	input.open(filename, std::ifstream::binary);
	if(!input)
	{
		std::cout << "Error loading file: " << filename << std::endl;
		return false;
	}

	// read header
	char buffer[2];
	input.read(buffer, 2);
	input.ignore(8);
	int dataOffset = readInt(input);
	readInt(input);
	width = readInt(input);
	height = readInt(input);
	input.ignore(2);

	// read data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	char * pixels = new char[size];
	input.seekg(dataOffset, std::ios_base::beg);
	input.read(pixels, size);
	
	// convert data into appropriate format
	data = new char[width * height * 3];
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			for(int c = 0; c < 3; c++)
			{
				data[3 * (width * y + x) + c] = pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	
	// clean up
	delete[] pixels;
	input.close();
	std::cout << "Loaded " << filename << " (" << width << "x" << height << ")" << std::endl;
	return true;
}

// texture creation
GLuint createTexture(const char* filename)
{
	// create new texture
	GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

	// set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// load image
    int width, height;
    char* data;
    if(!loadBmp(filename, width, height, data)) return id;

	// set texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
	return id;
}
