#include <Magick++.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

enum AsciiColour
{
    ascii_default,
    ascii_matrix_green,
    ascii_colour
};

enum BrightnessMapping
{
    brightness_average,
    brightness_min_max,
    brightness_luminosity
};

/////////////////////////////////////////////////
/// \brief Constructs a brightness matrix from an image
///
/// \param image const Magick::Image&           The image to construct a brightness matrix from
/// \param brightness_mapping BrightnessMapping What brightness mapping to use
/// \return std::vector<std::vector<int>>       Returns the constructed brightness matrix
///
/////////////////////////////////////////////////
std::vector<std::vector<int>> constructBrightnessMatrix(const Magick::Image& image, BrightnessMapping brightness_mapping)
{
    std::vector<std::vector<int>> brightness_matrix(image.rows(), std::vector<int>(image.columns()));
    std::function brightness_map{
        [](double r, double g, double b) {
            return 255 * (r + g + b) / 3;
        }
    };

    // Change the brightness mapping function if necessary
    if (brightness_mapping == brightness_min_max)
    {
        brightness_map = [](double r, double g, double b) {
            return 255 * (std::max({r, g, b}) + std::min({r, g, b})) / 2;
        };
    }
    else if (brightness_mapping == brightness_luminosity)
    {
        brightness_map = [](double r, double g, double b) {
            return 255 * (0.21 * r + 0.72 * g + 0.07 * b);
        };
    }

    // Construct the matrix
    for (std::size_t x{ 0 }; x < image.columns(); ++x)
    {
        for (std::size_t y{ 0 }; y < image.rows(); ++y)
        {
            Magick::ColorRGB pixel{ image.pixelColor(x, y) };

            brightness_matrix[y][x] = brightness_map(pixel.red(), pixel.green(), pixel.blue());
        }
    }

    return brightness_matrix;
}

/////////////////////////////////////////////////
/// \brief Converts a brightness matrix to a matrix of ASCII characters
///
/// \param brightness_matrix The brightness matrix to convert
/// \return Returns the generated ASCII character matrix
///
/////////////////////////////////////////////////
std::vector<std::vector<char>> convertToASCII(const std::vector<std::vector<int>>& brightness_matrix)
{
    const std::string_view characters{ "`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$" };
    std::vector<std::vector<char>> char_matrix(brightness_matrix.size(), std::vector<char>(brightness_matrix[0].size()));

    for (std::size_t x{ 0 }; x < char_matrix[0].size(); ++x)
    {
        for (std::size_t y{ 0 }; y < char_matrix.size(); ++y)
        {
            char_matrix[y][x] = characters[brightness_matrix[y][x] * (characters.size() - 1) / 255];
        }
    }

    return char_matrix;
}

/////////////////////////////////////////////////
/// \brief Prints the ASCII image to the console
///
/// \param char_matrix The matrix containing the ASCII characters that make up the image
/// \param image The original image from which the ASCII version was generated from
/// \param output_colour Determines the colour of the outputed image
///
/////////////////////////////////////////////////
void printCharImage(const std::vector<std::vector<char>>& char_matrix, const Magick::Image& image, AsciiColour output_colour)
{
    if (output_colour == ascii_matrix_green)
        std::cout << "\033[48;2;13;2;8m\033[38;2;0;143;17m";
    for (std::size_t y{ 0 }; y < char_matrix.size(); ++y)
    {
        for (std::size_t x{ 0 }; x < char_matrix[y].size(); ++x)
        {
            if (output_colour == ascii_colour)
            {
                Magick::ColorRGB rgb{ image.pixelColor(x, y) };
                std::string specifier{ "\033[38;2;" };
                int red{ static_cast<int>(255 * rgb.red()) };
                int green{ static_cast<int>(255 * rgb.green()) };
                int blue{ static_cast<int>(255 * rgb.blue()) };
                specifier += (std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m");
                std::cout << specifier;
            }
            std::cout << char_matrix[y][x] << char_matrix[y][x] << char_matrix[y][x];
        }
        std::cout << '\n';
    }
    std::cout << "\033[0m";
}

/////////////////////////////////////////////////
/// \brief Parses the command line arguments and returns them via out parameters
///
/// \param argc     The number of arguments being passed
/// \param argv     A C-string array containing the arguments
/// \param file_name_out    The filename of the image to convert
/// \param output_colour_out    What colour the ASCII image should be
/// \param brightness_mapping_out   Which brightness mapping to use
/// \param invert_out   Whether to invert the image
/// \return     Returns whether argument parsing was successful
///
/////////////////////////////////////////////////
bool parseArguments(int argc, char* argv[], std::string& file_name_out, AsciiColour& output_colour_out,
                    BrightnessMapping& brightness_mapping_out, bool& invert_out)
{
    bool success{ true };

    // Get the path to the image
    if (argc >= 2)
    {
        file_name_out = argv[1];
    }
    else
    {
        std::cerr << "Path to image file required.\n";
        success = false;
    }

    // Parse options
    for (int i{ 2 }; i < argc && success; ++i)
    {
        std::string argument{ argv[i] };
        if (argument == "--output-colour")
        {
            if (i + 1 < argc)
            {
                std::string colour_arg{ argv[i + 1] };
                if (colour_arg == "matix_green")
                    output_colour_out = ascii_matrix_green;
                else if (colour_arg == "colour")
                    output_colour_out = ascii_colour;
                else
                {
                    std::cerr << "--output-colour expects one of the following arguments: matrix_green or colour.\n";
                    success = false;
                }
            }
            else
            {
                std::cerr << "--output-colour option requires one argument.\n";
                success = false;
            }
        }
        else if (argument == "--brightness-mapping")
        {
            if (i + 1 < argc)
            {
                std::string brightness_arg{ argv[i + 1] };
                if (brightness_arg == "min_max")
                    brightness_mapping_out = brightness_min_max;
                else if (brightness_arg == "luminosity")
                    brightness_mapping_out = brightness_luminosity;
                else if (brightness_arg == "average")
                    brightness_mapping_out = brightness_average;
                else
                {
                    std::cerr << "--brightness-mapping expects one of the following arguments: min_max, luminosity, or average.\n";
                    success = false;
                }
            }
            else
            {
                std::cerr << "--brightness-mapping option requires one argument.\n";
                success = false;
            }
        }
        else if (argument == "--invert")
        {
            invert_out = true;
        }
    }
    return success;
}

int main(int argc, char* argv[])
{
    std::string file_name{""};
    AsciiColour output_colour{ ascii_default };
    BrightnessMapping brightness_mapping{ brightness_average };
    bool invert{ false };

    if (!parseArguments(argc, argv, file_name, output_colour, brightness_mapping, invert))
        return 1;

    Magick::Image image{};
    try
    {
        image.read(file_name);
        // Resize image if too big, magic numbers for now
        if (image.columns() > 317 || image.rows() > 168)
            image.resize(Magick::Geometry(317, 168));
        if (invert)
            image.negate();
    }
    catch (Magick::Exception& error)
    {
        std::cerr << "Caught exception: " << error.what() << '\n';
        return 1;
    }

    std::vector<std::vector<int>> brightness_matrix{ constructBrightnessMatrix(image, brightness_mapping) };

    std::vector<std::vector<char>> char_matrix{ convertToASCII(brightness_matrix) };

    printCharImage(char_matrix, image, output_colour);

    return 0;
}
