#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cout << "usage:\n"
             << "bitmap option inputfile.bmp outputfile.bmp\n"
             << "options:\n"
             << "  -c cell shade\n"
             << "  -g gray scale\n"
             << "  -p pixelate\n"
             << "  -b blur\n"
             << "  -r90 rotate 90\n"
             << "  -r180 rotate 180\n"
             << "  -r270 rotate 270\n"
             << "  -v flip vertically\n"
             << "  -h flip horizontally\n"
             << "  -d1 flip diagonally 1\n"
             << "  -d2 flip diagonally 2\n"
             << "  -grow scale the image by 2\n"
             << "  -shrink scale the image by .5" << endl;

        return 0;
    }

    string flag(argv[1]);
    string infile(argv[2]);
    string outfile(argv[3]);

    ifstream in;
    Bitmap image;
    ofstream out;

    try
    {
        in.open(infile, ios::binary);
        in >> image;
        in.close();
    }
    catch(BitmapException& e)
    {
        e.print_exception();
        return 0;
    }

    if(flag == "-c"s)
    {
        cellShade(image);
    }
    if(flag == "-g"s)
    {
        grayscale(image);
    }
    if(flag == "-p"s)
    {
        pixelate(image);
    }
    if(flag == "-b"s)
    {
        blur(image);
    }
    if(flag == "-r90"s)
    {
        rot90(image);
    }
    if(flag == "-r180"s)
    {
        rot180(image);
    }
    if(flag == "-r270"s)
    {
        rot270(image);
    }
    if(flag == "-v"s)
    {
        flipv(image);
    }
    if(flag == "-h"s)
    {
        fliph(image);
    }
    if(flag == "-d1"s)
    {
        flipd1(image);
    }
    if(flag == "-d2"s)
    {
        flipd2(image);
    }
    if(flag == "-grow"s)
    {
        scaleUp(image);
    }
    if(flag == "-shrink"s)
    {
        scaleDown(image);
    }

    out.open(outfile, ios::binary);
    out << image;
    out.close();

    return 0;
}

