#include <fstream>
#include <iostream>
#include <cstring>
#include "bitmap.h"
using namespace std;

//Bitmap Constructor.
Bitmap::Bitmap()
{
	tag[0] = 'A';
	tag[1] = 'A';
	size = 0;
	dataOffset = 0;
	secondHeaderSize = 0;
	pixelWidth = 0;
	pixelHeight = 0;
	colorPlanes = 0;
	colorDepth = 0;
	compression = 0;
	rawSize = 0;
	hres = 0;
	vres = 0;
	colorPalate = 0;
	importantColors = 0;
	redMask = 0;
	greenMask = 0;
	blueMask = 0;
	alphaMask = 0;
}

//bitmap destructor.
Bitmap::~Bitmap()
{
	tag[0] = 'A';
	tag[1] = 'A';
	size = 0;
	dataOffset = 0;
	secondHeaderSize = 0;
	pixelWidth = 0;
	pixelHeight = 0;
	colorPlanes = 0;
	colorDepth = 0;
	compression = 0;
	rawSize = 0;
	hres = 0;
	vres = 0;
	colorPalate = 0;
	importantColors = 0;
	redMask = 0;
	greenMask = 0;
	blueMask = 0;
	alphaMask = 0;
}

//updates the header when width and height are swapped.
void Bitmap::widthHeightUpdate(const int width, const int height)
{
	pixelHeight = height;
	pixelWidth = width;
}

//update the header size information after a grow or shrink operation.
void Bitmap::headerSizeUpdates(const int width, const int height)
{
	int area = width * height;
	rawSize = area * 4; //rawSize = num pixel bytes. (bytes)
	size = rawSize + dataOffset;
	pixelHeight = height;
	pixelWidth = width;
}

//This function overloads the insertion operator to work for reading in a
//bitmap file. For the headers of the bitmap file we go field by field to read
//in the header information. For reading in the pixel data, we loop through a
//2d vector and set the pixel data for each element in the vector.
istream& operator>>(std::istream& in , Bitmap& b)
{

	pixel p;
	uint16_t temp = 0;

	//Header 1
	in >> b.tag[0] >> b.tag[1]; //Type of bitmap
	if(strncmp(b.tag, "BM", 2) !=  0) //if the tag is not BM error.
	{
		cout << b.tag << endl;
		throw(BitmapException("Invalid bitmap type (not BM)", 0));
	}
	in.read((char*)&b.size, 4); //size of the BMP file in bytes
	in.ignore(4);
	in.read((char*)&b.dataOffset, 4); //offset to the start of the data

	//Header 2
	in.read((char*)&b.secondHeaderSize, 4); //Size of the second header.
	//Size is always 40 or 124. If its not throw error.
	if(b.secondHeaderSize != 40 && b.secondHeaderSize != 124) 
	{
		throw(BitmapException("Invalid second header size", 14));
	}
	in.read((char*)&b.pixelWidth, 4); //Width of image (in pixels).
	in.read((char*)&b.pixelHeight, 4); //Height of image (in pixels).
	in.read((char*)&b.colorPlanes, 2); //Num of color planes. 
	if(b.colorPlanes != 1) //num color planes MUST be 1. error if not.
	{
		throw(BitmapException("Number of color planes is not 1", 22));
	}
	in.read((char*)&b.colorDepth, 2); //Color depth. Either 24 or 32.
	if(b.colorDepth != 24 && b.colorDepth != 32) //Color depth must be 24 or 32
	{
		throw(BitmapException("Invalid color depth (must be either 24 or 32)", 24));
	}
	in.read((char*)&b.compression, 4); //The compression method used (0 or 3).
	if(b.compression != 0 && b.compression != 3) //if not 0 or 3 the wrong compression is used.
	{
		throw(BitmapException("Invalid compression method (must be 0 or 3)", 26));
	}
	in.read((char*)&b.rawSize, 4); //The size of the raw bitmap data.
	in.read((char*)&b.hres, 4); //The horizontal resolution of the image.
	if(b.hres != 2835) //horizontal res must be 2835
	{
		throw(BitmapException("Invalid horizontal resolution (must be 2835)", 30));
	}	
	in.read((char*)&b.vres, 4); //The verticle resolution of the image.
	if(b.vres != 2835) //verticle res must be 2835
	{
		throw(BitmapException("Invalid verticle resolution (must be 2835)", 34));
	}
	in.read((char*)&b.colorPalate, 4); //Colors in the color palate. (0)
	if(b.colorPalate != 0) //we arent using a color palate so it should be 0.
	{
		throw(BitmapException("Invalid number of colors in the color palate. (must be 0)", 38));
	}
	in.read((char*)&b.importantColors, 4); //Number of important colors. (0)
	if(b.importantColors != 0) //we arent using a color palate so it should be 0.
	{
		throw(BitmapException("Invalid number of important colors. (must be -1)", 42));
	}
	
	//resize the 2d vector now that we have the width and height (in pixels).
	//of the image.
	b._ps.resize(b.pixelHeight, vector<pixel> (b.pixelWidth, p));

	//If the compression field is 3 than this information is present.
	if(b.compression == 3)
	{
		in.read((char*)&b.redMask,4);
		in.read((char*)&b.greenMask,4);
		in.read((char*)&b.blueMask,4);
		in.read((char*)&b.alphaMask,4);
		in.ignore(68);
		for(int i = 0; i < b.pixelHeight; ++i)
			for(int j = 0; j < b.pixelWidth; ++j)
			{
				in.read((char*)&temp,1);
				b._ps[i][j].setAlpha(temp);
				in.read((char*)&temp,1);
				b._ps[i][j].setBlue(temp);
				in.read((char*)&temp,1);
				b._ps[i][j].setGreen(temp);
				in.read((char*)&temp,1);
				b._ps[i][j].setRed(temp);	
			}
	}

	else
	{	//the padding can be 0-3 bytes depending on the size of the row.
		//int padding = (3*b.pixelWidth)%4;
		int padding = b.pixelWidth % 4;
		for(int i = 0; i < b.pixelHeight; ++i)
		{
			for(int j = 0; j < b.pixelWidth; ++j)
			{
				in.read((char*)&temp,1);
				b._ps[i][j].setBlue(temp);
				in.read((char*)&temp,1);
				b._ps[i][j].setGreen(temp);
				in.read((char*)&temp,1);
				b._ps[i][j].setRed(temp);
			}
			in.ignore(padding); //padding at the end of the row, ignore it.
		}
	}
	
	//debugging :)
	//b.printBitmap();
	return in;
}

//write a bitmap header and following pixel data to a file. The way the pixel
//array is written is slightly different when the image is 24 bit vs 32 bit.
ostream& operator<<(std::ostream& out, const Bitmap& b)
{
	//useed for the garbage bytes in the headers.
	uint32_t temp = 0;
	int tempCol = 0; //used to assist in writing the pixel array to file. 

	//Header 1
	out.write((char*)&b.tag[0], 1);
	out.write((char*)&b.tag[1], 1);
	out.write((char*)&b.size, 4);
	out.write((char*)&temp, 4);
	out.write((char*)&b.dataOffset, 4);

	//Header 2
	out.write((char*)&b.secondHeaderSize, 4);
	out.write((char*)&b.pixelWidth, 4);
	out.write((char*)&b.pixelHeight, 4);
	out.write((char*)&b.colorPlanes, 2);
	out.write((char*)&b.colorDepth, 2);
	out.write((char*)&b.compression, 4);
	out.write((char*)&b.rawSize, 4);
	out.write((char*)&b.hres, 4);
	out.write((char*)&b.vres, 4);
	out.write((char*)&b.colorPalate, 4);
	out.write((char*)&b.importantColors, 4);

	//Header 3 (only write for 32 bit images)
	if(b.colorDepth == 32)
	{	
		out.write((char*)&b.redMask, 4);
		out.write((char*)&b.greenMask, 4);
		out.write((char*)&b.blueMask, 4);
		out.write((char*)&b.alphaMask, 4);
		//we have to write 68 bytes of color space information that we are
		//ignoring. since temp is a 4 byte variable, 68/4 = 17 so we write
		//temp to the file 17 times.
		for(int i = 0; i < 17; ++i)
			out.write((char*)&temp, 4);

		//write the pixel vector for a 32 bit image
		for(int i = 0; i < b.pixelHeight; ++i)
			for(int j = 0; j < b.pixelWidth; ++j)
			{
				tempCol = b._ps[i][j].getAlpha();
				out.write((char*)&tempCol, 1);
				tempCol = b._ps[i][j].getBlue();
				out.write((char*)&tempCol, 1);
				tempCol = b._ps[i][j].getGreen();
				out.write((char*)&tempCol, 1);
				tempCol = b._ps[i][j].getRed();
				out.write((char*)&tempCol, 1);
			}
	}

	else //color depth == 24. (write the pixel array for 24bit images.
	{
		//int padding = (3*b.pixelWidth)%4;
		int padding = b.pixelWidth % 4;
		int padval = 0; //make zero so our padding is zero and ez to read.
		for(int i = 0; i < b.pixelHeight; ++i)
		{
			for(int j = 0; j < b.pixelWidth; ++j)
			{
				tempCol = b._ps[i][j].getBlue();
				out.write((char*)&tempCol, 1);
				tempCol = b._ps[i][j].getGreen();
				out.write((char*)&tempCol, 1);
				tempCol = b._ps[i][j].getRed();
				out.write((char*)&tempCol, 1);
			}
			//write the padding bytes. padval is zero and padding
			//is a number between 0 and 3.
			out.write((char*)&padval,padding); //write padding bytes.
		}	

	}
	//b.printBitmap();
	return out;
}

//Print out all fields in a bitmap. Only really useful for debug purposes.
void Bitmap::printBitmap() const
{
	cout << tag[0] << tag[1] << "\n"
	     << "size: " << size << "\n"
	     << "Offset to the start of the data: " << dataOffset << "\n" 
	     << "2nd header size: " << secondHeaderSize << "\n"
	     << "Pixel Width: " << pixelWidth << "\n"
	     << "Pixel Height: " << pixelHeight << "\n"
	     << "Color planes: " << colorPlanes << "\n"
	     << "Color depth: " << colorDepth << "\n"
	     << "Compression used: " << compression << "\n"
	     << "raw size: " << rawSize << "\n"
	     << "horizontal resolution: " << hres << "\n"
	     << "verticle resolution: " << vres << "\n"
	     << "color palate: " << colorPalate << "\n"
	     << "importantColors: " << importantColors << "\n"
	     << "red mask: " << redMask << "\n"
	     << "green mask: " << greenMask << "\n"
	     << "blue mask: " << blueMask << "\n"
	     << "alpha mask: " << alphaMask << endl;
}

//getter function for pixel vector.
vector<vector<pixel>>Bitmap::getVector() const
{
	return _ps;
}

//setter function for the pixel vector.
void Bitmap::setVector(const vector<vector<pixel>> &ps)
{
	_ps = ps;
}

//Given a color value, see if it is in a particular range. If it is, then
//return 0,128, or 255 depending on where the color value lies.
int cellShadeRounding(int color)
{
	if(0 <= color && color <= 64) //0 <= color <= 64
		return 0;
	if(64 < color && color < 192) // 64 < color < 192
		return 128;
	if(192 <= color && color <= 255) //192 <= color <= 255
		return 255;
}

//limit RGB values to 0,128,255 instead of 0-255
void cellShade(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	int red = 0;
	int blue = 0;
	int green = 0;
	
	for(int i = 0; i < height; ++i)
			for(int j = 0; j < width; ++j)
			{
				red = ps[i][j].getRed();
				red = cellShadeRounding(red);
				ps[i][j].setRed(red);

				green = ps[i][j].getGreen();
				green = cellShadeRounding(green);
				ps[i][j].setGreen(green);

				blue = ps[i][j].getBlue();
				blue = cellShadeRounding(blue);
				ps[i][j].setBlue(blue);
			}
	b.setVector(ps);

}

//Turn an image into grayscale by making every pixel have the same red,
//green, and blue pixel values. To do this simply add rgb together and divide
//by 3. The average is now the value for rgb for that pixel.
void grayscale(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	int red = 0;
	int blue = 0;
	int green = 0;
	int average = 0;

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
		{
			red = ps[i][j].getRed();
			green = ps[i][j].getGreen();
			blue = ps[i][j].getBlue();
			average = (red + green + blue) / 3;
			ps[i][j].setRed(average);
			ps[i][j].setGreen(average);
			ps[i][j].setBlue(average);
		}
	b.setVector(ps);
}

//Pixelate an image by dividing the pixel vector into 16x16 chunks, and take
//the average color of the pixels in that chunk.
void pixelate(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	vector<pixel> pix;
	int width = ps[0].size();
	int height = ps.size();
	int red = 0;
	int blue = 0;
	int green = 0;
	int ychunks = (height-1) / 16;
	int xchunks = (width - 1) / 16;

	//number of chunks on the y axis of the image.
	for(int y = 1; y <= ychunks; ++y)
	{
		//number of chunks on the x axis of the image
		for(int x = 1; x <= xchunks; ++x)
		{
			//height of one 16x16 chunk
			for(int i = (y*16); i > ((y*16)-16); --i)
			{
				//width of one 16x16 chunk
				for(int j = (x*16); j > ((x*16)-16); --j)
				{
					pix.push_back(ps[i][j]);
				}
			}

			//sum the red, blue, and green values for a chunk
			for(pixel x:pix)
			{
				red += x.getRed();
				green += x.getGreen();
				blue += x.getBlue();
			}

			//divide each of the sums by 256 (16x16)
			red = red / 256;
			green = green / 256;
			blue = blue / 256;

			//height of one 16x16 chunk
			for(int i = (y*16); i > ((y*16)-16); --i)
			{
				//width of one 16x16 chunk
				for(int j = (x*16); j > ((x*16)-16); --j)
				{
					ps[i][j].setRed(red);
					ps[i][j].setGreen(green);
					ps[i][j].setBlue(blue);
				}
			}

			//set the sums back to zero for the next loop through.
			red = 0;
			green = 0;
			blue = 0;
			
			//get rid of the old pixel info for the chunk.
			for(pixel x:pix)
			{	
				pix.pop_back();
			}
		}
	}
	b.setVector(ps);
}

//This function handels the 5x5 matrix for guassian blurring. Depending on
//the row and column the function knows what to multiply that pixel by.
//(and divide by 256, which is done in blur pixel. Once the matrix is complete
//all of those pixel values are added up and the pixel is returned.
pixel guassianMatrixSum(vector<vector<pixel>> &pix)
{
	pixel p;
	int red = 0;
	int green = 0;
	int blue = 0;

	for(int y = 0; y < 5; ++y)
	{
		for(int x = 0; x < 5; ++x)
		{
			//if first or last row.
			if(y == 0 || y == 4)
			{
				//if first or last column
				if(x == 0 || x == 4)
					pix[y][x].blurPixel(1);

				//if 2nd or 4th column
				if(x == 1 || x == 3)
					pix[y][x].blurPixel(4);
				//if 3rd column (middle).
				if(x == 2)
					pix[y][x].blurPixel(6);

			}

			//if 2nd or 4th row.
			if(y == 1 || y == 3)
			{
				if(x == 0 || x == 4)
					pix[y][x].blurPixel(4);

				if(x == 1 || x == 3)
					pix[y][x].blurPixel(16);

				if(x == 2)
					pix[y][x].blurPixel(24);
			}

			//if mid row.
			if(y == 2)
			{
				if(x == 0 || x == 4)
					pix[y][x].blurPixel(6);

				if(x == 1 || x == 3)
					pix[y][x].blurPixel(24);

				if(x == 2)
					pix[y][x].blurPixel(36);
			}
		}
	}

	//sum the rgb values for each pixel in the 5x5 matrix.
	for(int y = 0; y < 5; ++y)
	{
		for(int x = 0; x < 5; ++x)
		{
			red += pix[y][x].getRed();	
			green += pix[y][x].getGreen();
			blue += pix[y][x].getBlue();
		}
	}
	//set those sums to be the new pixel value.
	p.setRed(red);
	p.setGreen(green);
	p.setBlue(blue);
	return p;
}

//guassian blurring to blur an image.
void blur(Bitmap& b)
{
	//all pixel data.
	vector<vector<pixel>> ps = b.getVector();
	pixel p;
	//current 5x5 blur matrix.
	vector<vector<pixel>> pix(5, vector<pixel>(5,p));
	int width = ps[0].size();
	int height = ps.size();
	int countx = 0;
	int county = 0;

	//loop through every row of the pixel vector
	for(int i = 0; i < height; ++i)
	{
		//loop through every column of the pixel vector
		for(int j = 0; j < width; ++j)
		{
			county = 0;
			//loop through rows of the 5x5 pixel matrix
			for(int y = i-2; y < i+3; ++y)
			{
				countx = 0;
				//loop through columns of the 5x5 pix matrix
				for(int x = j-2; x < j+3; ++x)
				{
				//when the 5x5 matrix is out of bounds with respect to height,
				//all 5 of that row will be zero.

					//if we are not out of bounds on y-axis.
					if((0 < y) && (y < height) && (0 < x) && (x < width))
					{
						//cout << "x: " << x << " y: " << y << endl;
						pix[county][countx] = ps[y][x];
					}	
					++countx;
				}
				++county;
			}
			p = guassianMatrixSum(pix);
			ps[i][j] = p;
		}
	}
	b.setVector(ps);
}

//rotates the image 90 degrees. To do this we simply swap the width and the
//height AND reverse the order of the original columns.
void rot90(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> rotate;
	vector<pixel> temp;
	for(int i = width; i > 0; --i)
	{
		for(int j = 0; j < height; ++j)
		{
			temp.push_back(ps[j][i]);
		}
		rotate.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(rotate);
	//width and height are now reveresed so update in the header
	b.widthHeightUpdate(rotate[0].size(), rotate.size());
}

//rotates the image by 180 degrees. To do this the rows and columns are both
//reversed.
void rot180(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> rotate;
	vector<pixel> temp;
	//loop through every row of the pixel data
	for(int i = height-1; i > 0; --i)
	{
		//loop through every column of the pixel data.
		for(int j = width-1; j > 0; --j)
		{
			temp.push_back(ps[i][j]);
		}
		rotate.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(rotate);
}

//rotates the image by 270 degrees. To do this the width and height are swapped.
//AND the order of the rows are reversed.
void rot270(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> rotate;
	vector<pixel> temp;
	for(int i = 0; i < width; ++i)
	{
		for(int j = height-1; j > 0; --j)
		{
			temp.push_back(ps[j][i]);
		}
		rotate.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(rotate);
	//width and height are now reveresed so update in the header
	b.widthHeightUpdate(rotate[0].size(), rotate.size());
}

//flips the image vertically by reversing the order of the rows.
void flipv(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> flip;
	vector<pixel> temp;
	//loop through every row of the pixel data
	for(int i = height-1; i > 0; --i)
	{
		//loop through every column of the pixel data.
		for(int j = 0; j < width; ++j)
		{
			temp.push_back(ps[i][j]);
		}
		flip.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(flip);
}

//flips the image horizontal by reversing the order of the columsn.
void fliph(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> flip;
	vector<pixel> temp;
	//loop through every row of the pixel data
	for(int i = 0; i < height; ++i)
	{
		//loop through every column of the pixel data.
		for(int j = width-1; j > 0; --j)
		{
			temp.push_back(ps[i][j]);
		}
		flip.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(flip);
}

//Flip the image over diag1. To do this we swap the width and the height AND
//reverse both the order of the rows and columns.
void flipd1(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> rotate;
	vector<pixel> temp;
	for(int i = width-1; i > 0; --i)
	{
		for(int j = height-1; j > 0; --j)
		{
			temp.push_back(ps[j][i]);
		}
		rotate.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(rotate);
	//width and height are now reveresed so update in the header
	b.widthHeightUpdate(rotate[0].size(), rotate.size());
}

//Flip the image over diag2. To do this we only swap the width and height.
void flipd2(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> rotate;
	vector<pixel> temp;
	for(int i = 0; i < width; ++i)
	{
		for(int j = 0; j < height; ++j)
		{
			temp.push_back(ps[j][i]);
		}
		rotate.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(rotate);
	//width and height are now reveresed so update in the header
	b.widthHeightUpdate(rotate[0].size(), rotate.size());
}

//scale the image up 2x. Simply loop through the vector and push every row
//and column twice.
void scaleUp(Bitmap& b)
{
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	vector<vector<pixel>> grow;
	vector<pixel> temp;
	//loop through every row of the pixel data
	for(int i = 0; i < height; ++i)
	{
		//loop through every column of the pixel data.
		for(int j = 0; j < width; ++j)
		{
			temp.push_back(ps[i][j]);
			temp.push_back(ps[i][j]);
		}
		grow.push_back(temp);
		grow.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(grow);
	//adjust bitmap header info.
	b.headerSizeUpdates(grow[0].size(), grow.size());

}

//scale the image down by 1/2
void scaleDown(Bitmap& b)
{
	//vector holding all original pixel data.
	vector<vector<pixel>> ps = b.getVector();
	int width = ps[0].size();
	int height = ps.size();
	//vector holding the pixel data after shrink operation.
	vector<vector<pixel>> half;
	//temp vector to hold a single row that we then push back on the 2d vector.
	vector<pixel> temp;

	//loop through every other row of the pixel data
	for(int i = 0; i < height; i+=2)
	{
		//loop through every other column of the pixel data.
		for(int j = 0; j < width; j+=2)
		{
			temp.push_back(ps[i][j]);
		}
		half.push_back(temp);
		for(pixel p : temp)
			temp.pop_back();
	}
	b.setVector(half);
	//adjust bitmap header info.
	b.headerSizeUpdates(half[0].size(), half.size());
}

//BITMAP EXCEPTION FUNCTIONS

//Prints the exception error message.
void BitmapException::print_exception()
{
	cout << "Error in bitmap at position 0x" 
		<< _position << ": " << _message << "." << endl;
}

//constructor for a bitmapexception, note, this class has no default constructor.
BitmapException::BitmapException(std::string&& message, uint32_t position)
{
	_message = message;
	_position = position;
}

BitmapException::BitmapException(const std::string& message, uint32_t position)
{
	_message = message;
	_position = position;
}

//PIXEL FUNCTIONS

//Pixel default constructor. set each rgba val to zero.
pixel::pixel()
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
}

//copy constructor for pixel. Invoked on pixel p1 = p2.
pixel::pixel(const pixel &p2)
{
	red = p2.red;
	blue = p2.blue;
	green = p2.green;
	alpha = p2.alpha;
}

//multiplies rgb pixel values by a given scaler. Mostly used for blurring
void pixel::blurPixel(int val)
{
	red = (red * val) / 256;
	green = (green * val) / 256;
	blue = (blue * val) / 256;
}

//getter function for red
int pixel::getRed() const
{
	return red;
}

//getter function for red
int pixel::getGreen() const
{
	return green;
}

//getter function for red
int pixel::getBlue() const
{
	return blue;
}

//getter function for red
int pixel::getAlpha() const
{
	return alpha;
}

//prints the rgba values for an indivdual pixel. Main purpose is for debug.
void pixel::printPixel() const
{
	cout << "red: " << red << "    "
	     << "green: " << green << "    "
	     << "blue: " << blue << "    "
	     << "alpha: " << alpha << "\n" << endl;
}

//setter function for the red value in a pixel
void pixel::setRed(int val)
{
	red = val;
}

//setter function for the green value in a pixel
void pixel::setGreen(int val)
{
	green = val;
}

//setter function for the green value in a pixel
void pixel::setBlue(int val)
{
	blue = val;
}

//setter function for the alpha value in a pixel
void pixel::setAlpha(int val)
{
	alpha = val;
}

//pixel destructor.
pixel::~pixel()
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
}
