#include <vector>
class pixel
{
public:
	pixel();
	~pixel();
	pixel(const pixel &p2);
	void blurPixel(int val);
	void setRed(int val);
	void setBlue(int val);
	void setGreen(int val);
	void setAlpha(int val);
	int getRed() const;
	int getBlue() const;
	int getGreen() const;
	int getAlpha() const;
	void printPixel() const;
private:
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t alpha;
};

class Bitmap
{
private:
	//Header 1
	char tag[2];
	uint32_t size;
	uint32_t dataOffset;

	//Header 2
	uint32_t secondHeaderSize;
	uint32_t pixelWidth;
	uint32_t pixelHeight;
	uint16_t colorPlanes;
	uint16_t colorDepth;
	uint32_t compression;
	uint32_t rawSize;
	uint32_t hres;
	uint32_t vres;
	uint32_t colorPalate;
	uint32_t importantColors;

	//These fields only exist if compression == 3.
	uint32_t redMask;
	uint32_t greenMask;
	uint32_t blueMask;
	uint32_t alphaMask;

	//pixel data (2d vector of pixels)
	std::vector<std::vector<pixel>> _ps;
    /**
     * Read in an image.
     * reads a bitmap in from the stream
     *
     * @param in the stream to read from.
     * @param b the bitmap that we are creating.
     *
     * @return the stream after we've read in the image.
     *
     * @throws BitmapException if it's an invalid bitmap.
     * @throws bad_alloc exception if we failed to allocate memory.
     */
    friend std::istream& operator>>(std::istream& in, Bitmap& b);

    /**
     * Write the binary representation of the image to the stream.
     *
     * @param out the stream to write to.
     * @param b the bitmap that we are writing.
     *
     * @return the stream after we've finished writting.
     *
     * @throws failure if we failed to write.
     */
    friend std::ostream& operator<<(std::ostream& in, const Bitmap& b);


public:
    Bitmap();
    ~Bitmap();
    void printBitmap() const; //prints all relevant values for a particular bitmap.
    std::vector<std::vector<pixel>> getVector() const;
    void setVector(const std::vector<std::vector<pixel>> &ps);
    void headerSizeUpdates(const int width, const int height);
    void widthHeightUpdate(const int width, const int height);

};

/**
 * cell shade an image.
 * for each component of each pixel we round to 
 * the nearest number of 0, 180, 255
 *
 * This has the effect of making the image look like.
 * it was colored.
 */
void cellShade(Bitmap& b);
int cellShadeRounding(int color);

/**
 * Grayscales an image by averaging all of the components.
 */
void grayscale(Bitmap& b);

/**
 * Pixelats an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b);

/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b);
pixel guassianMatrixSum(std::vector<std::vector<pixel>>&);

/** * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b);

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b);

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b);

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b);

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b);

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b);

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b);

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b);

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b);



/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
class BitmapException : public std::exception
{
    // the message to print out
    std::string _message;

    // position in the bitmap file (in bytes) where the error occured.
    uint32_t _position;

public:
    BitmapException() = delete;

    BitmapException(const std::string& message, uint32_t position);
    BitmapException(std::string&& message, uint32_t position);

    /**
     * prints out the exception in the form:
     *
     * "Error in bitmap at position 0xposition :
     * message"
     */
    void print_exception();
};
