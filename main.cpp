#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <string>
#include <sstream>

struct Point {
    double x;
    double y;
};

// Function to write SVG file with lines connecting points
void writeSVG(std::vector<Point>* points, const std::string& filename) {
    std::ofstream svgFile(filename);
    if (!svgFile) {
        std::cerr << "Error opening the SVG file." << std::endl;
        return;
    }

    // Write SVG header
    svgFile << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
    svgFile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << std::endl;
    svgFile << "  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
    svgFile << "<svg width=\"500\" height=\"500\" version=\"1.1\"" << std::endl;
    svgFile << "     xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

    // Write lines connecting points
    for (size_t i = 0; i < points->size() - 1; ++i) {
        svgFile << "<line x1=\"" << points->at(i).x << "\" y1=\"" << points->at(i).y << "\" "
                << "x2=\"" << points->at(i+1).x << "\" y2=\"" << points->at(i+1).y << "\" "
                << "style=\"stroke: black;\"/>" << std::endl;
    }

    // Close SVG
    svgFile << "</svg>" << std::endl;
    svgFile.close();

    std::cout << "SVG file '" << filename << "' created successfully." << std::endl;
}

std::unique_ptr<std::vector<Point>> readPointsFromCSV(const std::string& filename) {
    std::unique_ptr<std::vector<Point>> points = std::make_unique<std::vector<Point>>();
    
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening the CSV file." << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream lineStream(line);
        Point point;
        char comma;
        if (lineStream >> point.x >> comma >> point.y) {
            points->push_back(point);
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }

    inputFile.close();
    return points;
}

std::unique_ptr<std::vector<Point>> readPointsFromUserInput() {
    std::unique_ptr<std::vector<Point>> points = std::make_unique<std::vector<Point>>();
    int numPoints;

    std::cout << "Enter the number of points: ";
    std::cin >> numPoints;

    for (int i = 0; i < numPoints; ++i) {
        Point point;
        std::cout << "Enter coordinates for point " << i + 1 << " (x y): ";
        std::cin >> point.x >> point.y;
        points->push_back(point);
    }
    return points;
}

void invertYValues(const std::unique_ptr<std::vector<Point>>& points) {
    if (!points) {
        std::cerr << "Invalid input pointer." << std::endl;
        return;
    }

    for (Point& point : *points) {
        point.y = -point.y;
    }
}

// Function to find the minimum x and y values in a vector of points
std::pair<double, double> findMinValues(const std::unique_ptr<std::vector<Point>>& points) {
    if (!points || points->empty()) {
        std::cerr << "Invalid or empty input pointer." << std::endl;
        return std::make_pair(0.0, 0.0); // Return a default value
    }

    double minX = points->at(0).x;
    double minY = points->at(0).y;

    for (const Point& point : *points) {
        if (point.x < minX) {
            minX = point.x;
        }
        if (point.y < minY) {
            minY = point.y;
        }
    }

    return std::make_pair(minX, minY);
}

// Function to offset each point in a vector by given values
void offsetPoints(const std::unique_ptr<std::vector<Point>>& points, double offset_x, double offset_y) {
    if (!points) {
        std::cerr << "Invalid input pointer." << std::endl;
        return;
    }

    for (Point& point : *points) {
        point.x += offset_x;
        point.y += offset_y;
    }
}

void correctPoints(const std::unique_ptr<std::vector<Point>>& points, int padding) {
    if (!points) {
        std::cerr << "Invalid input pointer." << std::endl;
        return;
    }

    invertYValues(points);
    std::pair<double, double> mins = findMinValues(points);
    offsetPoints(points, -mins.first + padding, -mins.second + padding);
}

void printPoints(const std::unique_ptr<std::vector<Point>>& points) {
    if (!points) {
        std::cout << "Vector is empty." << std::endl;
        return;
    }

    std::cout << "Vector of Points:" << std::endl;
    for (const Point& point : *points) {
        std::cout << "Point: (" << point.x << ", " << point.y << ")" << std::endl;
    }
}


int main() {
    auto points = readPointsFromCSV("points.csv");
    correctPoints(points, 50);
    printPoints(points);
    writeSVG(points.get(), "output.svg");

    return 0;
}