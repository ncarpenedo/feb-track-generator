#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

struct Point {
  double x;
  double y;
};

enum AngleType { CLOCKWISE, COUNTERCLOCKWISE, STRAIGHT };

enum LineDirection {
    LINE_UP,
    LINE_DOWN,
    LINE_LEFT,
    LINE_RIGHT,
    LINE_UNKNOWN
};

// Function to calculate the angle between two points
AngleType calculateAngle(const Point& point1, const Point& point2) {
  double angle = atan2(point2.y - point1.y, point2.x - point1.x);

  if (angle > 0) {
    return CLOCKWISE;
  } else if (angle < 0) {
    return COUNTERCLOCKWISE;
  } else {
    return STRAIGHT;
  }
}

// Function to determine the direction of the line between two points
LineDirection getLineDirection(const Point& point1, const Point& point2) {
    if (point1.x < point2.x && point1.y == point2.y) {
        return LINE_RIGHT;
    } else if (point1.x > point2.x && point1.y == point2.y) {
        return LINE_LEFT;
    } else if (point1.x == point2.x && point1.y < point2.y) {
        return LINE_DOWN;
    } else if (point1.x == point2.x && point1.y > point2.y) {
        return LINE_UP;
    } else {
        return LINE_UNKNOWN;
    }
}

Point getPointWrapped(std::vector<Point>* points, int i) {
  int real_index = i % points->size();
  return points->at(real_index);
}

std::string makeOffsetLine(const Point& point1, const Point& point2, int offset) {
  std::ostringstream oss;
  int offset_x = 0, offset_y = 0;
  LineDirection type = getLineDirection(point1, point2);
  if (type == LINE_UP) {
    offset_y = -offset;
  } else if (type == LINE_DOWN) {
    offset_y = offset;
  } else if (type == LINE_LEFT) {
    offset_x = -offset;
  } else if (type == LINE_RIGHT) {
    offset_x = offset;
  }

  oss << "<line x1=\"" << point1.x  + offset_x << "\" y1=\"" << point1.y + offset_y << "\" "
      << "x2=\"" << point2.x - offset_x << "\" y2=\"" << point2.y - offset_y << "\" "
      << "style=\"stroke: black;\"/>" << std::endl;
  return oss.str();
}

std::string makeOutline(std::vector<Point>* points, int radius, int offset) {
  // Write lines connecting points
  std::ostringstream oss;
  for (size_t i = 0; i < points->size(); ++i) {
    oss << makeOffsetLine(getPointWrapped(points, i), getPointWrapped(points, i + 1), radius);
  }
  return oss.str();
}

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
  svgFile << "  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
          << std::endl;
  svgFile << "<svg width=\"500\" height=\"500\" version=\"1.1\"" << std::endl;
  svgFile << "     xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

  svgFile << makeOutline(points, 10, 2);

  // Close SVG
  svgFile << "</svg>" << std::endl;
  svgFile.close();

  std::cout << "SVG file '" << filename << "' created successfully."
            << std::endl;
}

std::unique_ptr<std::vector<Point>> readPointsFromCSV(
    const std::string& filename) {
  std::unique_ptr<std::vector<Point>> points =
      std::make_unique<std::vector<Point>>();

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
  std::unique_ptr<std::vector<Point>> points =
      std::make_unique<std::vector<Point>>();
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
std::pair<double, double> findMinValues(
    const std::unique_ptr<std::vector<Point>>& points) {
  if (!points || points->empty()) {
    std::cerr << "Invalid or empty input pointer." << std::endl;
    return std::make_pair(0.0, 0.0);  // Return a default value
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
void offsetPoints(const std::unique_ptr<std::vector<Point>>& points,
                  double offset_x, double offset_y) {
  if (!points) {
    std::cerr << "Invalid input pointer." << std::endl;
    return;
  }

  for (Point& point : *points) {
    point.x += offset_x;
    point.y += offset_y;
  }
}

void correctPoints(const std::unique_ptr<std::vector<Point>>& points,
                   int padding) {
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