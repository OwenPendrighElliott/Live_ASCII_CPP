#include <opencv2/opencv.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

std::vector<int> THRESHOLDS = { 15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 140, 150, 256 };
std::vector<std::string> VALUES = { "##", "#%", "+#", "/#", "\\+", "++", "+*", "*+", "**", "*-", ".o", "..", " -", "' ", "  " };

std::string ascii_vec2ascii(std::vector<std::string> ascii_vec) {
    std::string ascii = "";
    for (auto const& el : ascii_vec) {
        ascii.append(el);
    }
    return ascii;
}

std::string im2ascii(cv::Mat im) {
    // TODO: use the fact that size is known ahead of time to make this way more optimised (use a 2d char array and make this loop parallelisable

    cv::Mat greyscale;

    cv::resize(im, im, cv::Size(), 0.14, 0.14, 1);

    cv::cvtColor(im, greyscale, cv::COLOR_RGB2GRAY);

    std::vector<std::string> ascii_vec;

    for (int i = 0; i < greyscale.rows; i++) {
        const unsigned char* row = greyscale.ptr<unsigned char>(i);
        for (int j = 0; j < greyscale.cols; j++) {
            for (int k = 0; k < THRESHOLDS.size(); k++) {
                if (row[j] < THRESHOLDS[k]) {
                    ascii_vec.push_back(VALUES[k]);
                    break;
                }
            }
        }
        ascii_vec.push_back("\n");
    }

    std::string ascii = ascii_vec2ascii(ascii_vec);

    return ascii;
}

int main(int argc, char** argv) {

    char rewindstr[20]; // character array for rewinding output with sprinf_s

    std::map<int, char> ascii_map;

    cv::Mat frame;

    cv::VideoCapture vc;

    vc.open(0, 0);

    if (!vc.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //system("cls");
    while (1) {
        vc.read(frame);

        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        std::string ascii = im2ascii(frame);

        int n = std::count(ascii.begin(), ascii.end(), '\n');
        sprintf_s(rewindstr, "\x1b[%dA", n + 2); // 
        puts(rewindstr);

        std::cout << ascii;

        if (cv::waitKey(27) >= 0) {
            break;
        }
    }

    return 0;
}
