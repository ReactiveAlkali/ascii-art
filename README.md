# ASCII Art Converter
Converts images to ASCII art in your console

## Description
This is a simple console application which converts a given image file into ASCII art and prints it to your console.  The application depends on ImageMagick 
through use of the Magick++ API.  ImageMagick is used in order to load and process the image.

## Installation
First make sure ImageMagick is installed on your system, afterwards you can download the source file and compile the program.  If you use gcc you can compile it
with the following:

    g++ ascii-art.cpp \`Magick++-config --cppflags --cxxflags --ldflags --libs\` -o ascii-art

## Usage
    ascii-art <filename> <options>

The program requires you to provide the path to the image file you wish to convert.  Following this you may provide any of the following options:
  -  `--invert` Inverts the given image
  -  `--brightness-mapping <arg>`  Sets the function used to map a pixel's RGB values into a single brightness value.  The available options are `min_max`, `luminosity`, and `average`.  Note that average is the dafault mapping function.
  -  `--output-colour <arg>` Sets the colour which the ASCII image will be printed as. Requires one of the following arguments: `matrix_green` or `colour`.

## Credits
This project was guided by the following blog post: https://robertheaton.com/2018/06/12/programming-projects-for-advanced-beginners-ascii-art/

## License
[MIT](https://mit-license.org/)
