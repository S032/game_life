#include <SFML/Graphics.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;
int x = 60, y = 60;
int resolution = 1000;

sf::RenderWindow window(sf::VideoMode(resolution, resolution), "SFML works!");
vector<vector<int>> matrix(x + 2, vector<int>(y + 2));
vector<vector<int>> buffer_matrix(x + 2, vector<int>(y + 2));
vector<string> string_matrix;

struct Shift {
  int x = 0;
  int y = 0;
};

const vector<Shift> Shifts = {{-1, 0}, {-1, 1}, {0, 1},  {1, 1},
                              {1, 0},  {1, -1}, {0, -1}, {-1, -1}};

void copy_vector() {
  for (size_t i = 0; i != matrix.size(); ++i) {
    for (size_t j = 0; j != matrix[i].size(); ++j) {
      matrix[i][j] = buffer_matrix[i][j];
    }
  }
  for (size_t i = 0; i != buffer_matrix.size(); ++i) {
    for (size_t j = 0; j != buffer_matrix[i].size(); ++j) {
      buffer_matrix[i][j] = 0;
    }
  }
}

void create_map(int x, int y) {

  vector<vector<int>> map(x, vector<int>(y));

  ofstream matrix("matrix.txt");

  if (matrix.is_open()) {
    for (size_t i = 0; i != map.size(); ++i) {
      for (size_t i = 0; i != map.size(); ++i) {
        matrix << "0" << " ";
      }
      matrix << endl;
    }
  }
}

void vector_prt(vector<vector<int>> matrix) {
  for (size_t i = 1; i + 1 != matrix.size(); ++i) {
    for (size_t j = 1; j + 1 != matrix[i].size(); ++j) {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void str_vector_prt(vector<string> matrix) {
  for (size_t i = 0; i != matrix.size(); ++i) {
    for (size_t j = 0; j != matrix[i].size(); ++j) {
      cout << matrix[i][j];
    }
    cout << endl;
  }
}

void map_reader() {
  ifstream map("matrix.txt");
  string line;

  if (map.is_open()) {
    while (getline(map, line)) {
      string_matrix.push_back(line);
    }
  }

  for (size_t i = 0; i != string_matrix.size(); ++i) { // удаление пробелов
    for (size_t j = 0; j != string_matrix[i].size(); ++j) {
      if (string_matrix[i][j] == ' ') {
        string_matrix[i].erase(j, 1);
        --j;
      }
    }
  }

  for (size_t i = 1; i + 1 != matrix.size();
       ++i) { // перенос строчной карты в основную
    for (size_t j = 1; j + 1 != matrix[i].size(); ++j) {
      size_t x = i - 1;
      size_t y = j - 1;

      if (string_matrix[x][y] == '1') {
        matrix[i][j] = 1;
      }
    }
  }
}

void rule_executor() {
  const int death = 0;
  const int life = 1;

  for (size_t i = 1; i + 1 != matrix.size(); ++i) {
    for (size_t j = 1; j + 1 != matrix[i].size(); ++j) {
      if (matrix[i][j] == 1) {
        int counter_neighbours = 0;
        for (auto shift : Shifts) {
          size_t x = i, y = j;
          if (x + shift.x == matrix.size() - 1)
            x = 0;
          if (x + shift.x == 0)
            x = matrix.size() - 1;
          if (y + shift.y == matrix.size() - 1)
            y = 0;
          if (y + shift.y == 0)
            y = matrix.size() - 1;

          auto &cell = matrix[x + shift.x][y + shift.y];
          if (cell == 1)
            counter_neighbours++;
        }
        if (counter_neighbours < 2 && counter_neighbours > 3) {
          buffer_matrix[i][j] = death;
        }
        if (counter_neighbours == 2 || counter_neighbours == 3)
          buffer_matrix[i][j] = life;
      } else if (matrix[i][j] == 0) {
        int counter_neighbours = 0;
        for (auto shift : Shifts) {
          size_t x = i, y = j;
          if (x + shift.x == matrix.size() - 1)
            x = 0;
          if (x + shift.x == 0)
            x = matrix.size() - 1;
          if (y + shift.y == matrix.size() - 1)
            y = 0;
          if (y + shift.y == 0)
            y = matrix.size() - 1;

          auto &cell = matrix[x + shift.x][y + shift.y];
          if (cell == 1)
            counter_neighbours++;
        }
        if (counter_neighbours == 3) {
          buffer_matrix[i][j] = life;
        } else
          buffer_matrix[i][j] = death;
      }
    }
  }
  copy_vector();
}

void screenmaker(float size) {

  for (size_t i = 1; i + 1 != matrix.size(); ++i) {
    for (size_t j = 1; j + 1 != matrix.size(); ++j) {
      if (matrix[i][j] == 0) {
        sf::RectangleShape square(sf::Vector2f(size, size));
        square.setPosition((j * size) * 1.4, (i * size) * 1.4);
        square.setFillColor(sf::Color::White);
        square.setOutlineThickness(2.f);
        square.setOutlineColor(sf::Color(128, 128, 128));
        window.draw(square);
      } else {
        sf::RectangleShape square(sf::Vector2f(size, size));
        square.setPosition((j * size) * 1.4, (i * size) * 1.4);
        square.setFillColor(sf::Color::Black);
        square.setOutlineThickness(2.f);
        square.setOutlineColor(sf::Color(128, 128, 128));
        window.draw(square);
      }
    }
  }
}

void updateScreen(int FPS) {
  using namespace std::chrono_literals;
  std::chrono::milliseconds oneSecond = 1000ms;
  std::chrono::milliseconds oneFrame = oneSecond / FPS;

  for (int i = 0; i < FPS; ++i) {
    window.clear();
    screenmaker(10);
    rule_executor();
    window.display();
    std::this_thread::sleep_for(oneFrame);
  }
  std::this_thread::sleep_for(1s - (oneFrame * FPS));
}

int main() {
  // create_map(x, y); //generate zero map in the matrix.txt
  map_reader();
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    updateScreen(15);
  }
}
