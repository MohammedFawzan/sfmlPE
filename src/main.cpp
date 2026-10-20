#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

uint width = 1920;
uint height = 1080;
float grav = 0.1f;
float wind = 0.0f;
float bloss = 0.9;
float fric = 0.99;
bool veloC = true;
float veloM = 20;
float srad = 32.f;
float sthr = 0.0f;
float sint = 2.5f;
int sn = 16;
int vt = 0;

int st = 4;
sf::Texture texture("assets/textures/wall.png");
class ball {
public:
  sf::Vector2f position;
  sf::CircleShape shape;
  float radius;
  sf::Vector2f velocity;
  double density;
  double mass;
  sf::Color col;
  ball(sf::Vector2f p, float r, sf::Vector2f v, sf::Color c, double d) {
    position = p;
    radius = r;
    density = d;
    velocity = v;
    col = c;
    shape.setRadius(radius);
    shape.setFillColor(c);
    shape.setPosition(position);
    shape.setOrigin({radius, radius});
    mass = 3.1415f * pow(r, 2) * d;
  }
  void update() {
    gravity();
    friction();
  }
  void move() {
    if (abs(velocity.x) < 0.05f) {
      velocity.x = 0.f;
    };
    if (abs(velocity.y) < 0.05f) {
      velocity.y = 0.f;
    };
    position += {velocity.x / st, velocity.y / st};
    shape.setPosition(position);
  }
  void clamp() {
    if (position.x > width - radius) {
      velocity.x *= -bloss;
      shape.setPosition({static_cast<float>(width) - radius, position.y});
    } else if (position.x < radius) {
      velocity.x *= -bloss;
      shape.setPosition({radius, position.y});
    }
    position = shape.getPosition();
    if (position.y > height - radius) {
      velocity.y *= -bloss;
      if (abs(velocity.y) < 0.9) {
        velocity.y = 0;
      }
      shape.setPosition({position.x, static_cast<float>(height) - radius});
    } else if (position.y < 0 + radius) {
      velocity.y *= -bloss;
      shape.setPosition({position.x, radius});
    }
    position = shape.getPosition();
  }
  void gravity() {
    // if (velocity.y < 10.f){
    velocity.y += grav;
    velocity.x += wind;
    //}
  }
  void friction() {
    velocity.x *= fric;
    velocity.y *= fric;
  }
};

class wall {
public:
  sf::Vector2f position;
  sf::RectangleShape shape;
  sf::Texture texture;
  sf::Vector2f size;
  sf::Color col;
  wall(sf::Vector2f p, sf::Vector2f s, sf::Color c, const sf::Texture *t) {
    col = c;
    position = p;
    shape.setTexture(t);
    shape.setPosition(p);
    shape.setSize(s);
    shape.setFillColor(c);
    size = s;
    shape.setTextureRect(
        sf::IntRect({0, 0}, {static_cast<int>(s.x), static_cast<int>(s.y)}));
  }
};
vector<wall> walls;
vector<ball> balls;

void spawn(sf::Vector2i pos, float rad, sf::Vector2f v, sf::Color c) {
  balls.push_back(ball(static_cast<sf::Vector2f>(pos), rad, v, c, 1.0));
}
void spawn(sf::Vector2i pos, float rad, sf::Vector2f v) {
  balls.push_back(ball(static_cast<sf::Vector2f>(pos), rad, v,
                       sf::Color(rand() % 255, rand() % 255, rand() % 255, 255),
                       1.0));
}
void create(sf::Vector2f pos, sf::Vector2f size) {
  walls.push_back(wall(static_cast<sf::Vector2f>(pos), size,
                       sf::Color(rand() % 255, rand() % 255, rand() % 255, 255),
                       &texture));
}
void create(sf::Vector2i pos, sf::Vector2f size, sf::Color c) {
  walls.push_back(wall(static_cast<sf::Vector2f>(pos), size, c, &texture));
}

vector<sf::Sound> sound;
vector<sf::SoundBuffer> buffer;
sf::SoundBuffer sBuffer;

void audio() {
  if (!sBuffer.loadFromFile("assets/sfx/bounce.wav")) {
  }
  int t = 8;
  sound.reserve(t);
  for (int i = 0; i < t; i++) {
    sound.emplace_back(sBuffer);
  }
}
void tone(float f, float m) {
  for (auto &s : sound) {
    if (s.getStatus() != sf::Sound::Status::Playing) {
      s.setPitch(0.5f + (f * 0.05f));
      s.setVolume(min(static_cast<int>(max(m, 1.f)), 75));
      s.play();
      break;
    }
  }
}

void collide(ball &b1, ball &b2) {
  float xdiff = b2.position.x - b1.position.x;
  float ydiff = b2.position.y - b1.position.y;
  float rt = sqrt((xdiff * xdiff) + (ydiff * ydiff));
  if (b1.radius + b2.radius >= rt) {
    float theta = atan2(ydiff, xdiff);
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    float overlap = 0.5 * (b1.radius + b2.radius - rt);
    b1.shape.setPosition(b1.position -
                         sf::Vector2f(cosTheta, sinTheta) * overlap);
    b2.shape.setPosition(b2.position +
                         sf::Vector2f(cosTheta, sinTheta) * overlap);
    b1.position = b1.shape.getPosition();
    b2.position = b2.shape.getPosition();

    float res1x = b1.velocity.y * sinTheta + b1.velocity.x * cosTheta;
    float res1y = b1.velocity.y * cosTheta - b1.velocity.x * sinTheta;
    float res2x = b2.velocity.y * sinTheta + b2.velocity.x * cosTheta;
    float res2y = b2.velocity.y * cosTheta - b2.velocity.x * sinTheta;
    if (res1x - res2x < 0) {
      return;
    }
    float v1 = (res1x * (b1.mass - b2.mass) + (2) * b2.mass * res2x) /
               (b1.mass + b2.mass);
    float v2 = (res2x * (b2.mass - b1.mass) + (2) * b1.mass * res1x) /
               (b2.mass + b1.mass);

    b1.velocity.x = v1 * cosTheta - res1y * sinTheta;
    b1.velocity.y = v1 * sinTheta + res1y * cosTheta;

    b2.velocity.x = v2 * cosTheta - res2y * sinTheta;
    b2.velocity.y = v2 * sinTheta + res2y * cosTheta;
    float thres = 5.0;
    if (abs(res1x - res2x) > 2.f) {
      tone(abs(res1x - res2x), b1.radius);
    }
  }
}
void wallide(ball &b, wall w) {
  float left = w.shape.getPosition().x;
  float right = w.shape.getPosition().x + w.shape.getSize().x;
  float top = w.shape.getPosition().y;
  float bottom = w.shape.getPosition().y + w.shape.getSize().y;
  if (b.position.x >= left and b.position.x <= right and b.position.y >= top and
      b.position.y <= bottom) {
    float dLeft = b.position.x - left;
    float dRight = right - b.position.x;
    float dTop = b.position.y - top;
    float dBottom = bottom - b.position.y;
    float d = std::min({dLeft, dRight, dTop, dBottom});
    float x1;
    float y1;
    float overlap;
    if (d == dLeft) {
      x1 = -1.0f;
      y1 = 0.0f;
      overlap = b.radius + dLeft;
    } else if (d == dRight) {
      x1 = 1.0f;
      y1 = 0.0f;
      overlap = b.radius + dRight;
    } else if (d == dTop) {
      x1 = 0.0f;
      y1 = -1.0f;
      overlap = b.radius + dTop;
    } else {
      x1 = 0.0f;
      y1 = 1.0f;
      overlap = b.radius + dBottom;
    }
    b.position = b.position + sf::Vector2f{x1, y1} * overlap;
    b.shape.setPosition(b.position);
    float xy = b.velocity.x * x1 + b.velocity.y * y1;
    if (xy < 0) {
      b.velocity.x -= (1 + bloss) * xy * x1;
      b.velocity.y -= (1 + bloss) * xy * y1;
    }
  }
  float x = std::clamp(b.position.x, left, right);
  float y = std::clamp(b.position.y, top, bottom);
  float xdiff = b.position.x - x;
  float ydiff = b.position.y - y;
  float t = std::sqrt((xdiff * xdiff) + (ydiff * ydiff));
  if (b.radius > t) {
    if (t == 0.0f)
      return;

    float x1 = xdiff / t;
    float y1 = ydiff / t;
    float overlap = b.radius - t;

    b.position = b.position + sf::Vector2f{x1, y1} * overlap;
    b.shape.setPosition(b.position);

    float xy = b.velocity.x * x1 + b.velocity.y * y1;
    if (xy < 0) {
      b.velocity.x -= (1 + bloss) * xy * x1;
      b.velocity.y -= (1 + bloss) * xy * y1;
    }
  }
}

sf::Clock clock1;
int main() {
  bool wdrag;
  bool drag;
  texture.setRepeated(true);
  audio();
  float rad = 50;
  sf::Font font("assets/fonts/VCR_OSD_MONO.ttf");
  vector<sf::Text> text = {sf::Text(font), sf::Text(font), sf::Text(font),
                           sf::Text(font), sf::Text(font), sf::Text(font),
                           sf::Text(font), sf::Text(font), sf::Text(font)};
  sf::RectangleShape mwall({0, 0});
  sf::Vector2f mwallpos(0.0f, 0.0f);
  sf::Vector2f cmos(0.f, 0.f);
  sf::CircleShape mouseVelo(5.0f);
  sf::CircleShape curs(rad);
  curs.setFillColor(sf::Color::Transparent);
  mouseVelo.setFillColor(sf::Color::Transparent);
  curs.setOutlineColor(sf::Color::White);
  curs.setOutlineThickness(-5);
  vector<sf::RectangleShape> ui = {
      sf::RectangleShape({static_cast<float>(width) / 4, 20}),
      sf::RectangleShape({static_cast<float>(width) / 4, 20}),
      sf::RectangleShape({static_cast<float>(width) / 4, 20}),
      sf::RectangleShape({static_cast<float>(width) / 4, 20}),
      sf::RectangleShape({static_cast<float>(width) / 5, 20}),
      sf::RectangleShape({static_cast<float>(width) / 5, 20}),
      sf::RectangleShape({static_cast<float>(width) / 5, 20}),
      sf::RectangleShape({static_cast<float>(width) / 5, 20}),
      sf::RectangleShape({static_cast<float>(width) / 5, 20})};

  sf::Shader shader;
  sf::Shader pShader;

  // load only the vertex shader
  if (!shader.loadFromFile("assets/shaders/glow.frag",
                           sf::Shader::Type::Fragment)) {
    return 1;
  }
  if (!pShader.loadFromFile("assets/shaders/pGlow.frag",
                            sf::Shader::Type::Fragment)) {
    return 1;
  }
  pShader.setUniform("u_texture", sf::Shader::CurrentTexture);
  pShader.setUniform("u_radius", srad);
  pShader.setUniform("u_intensity", sint);
  pShader.setUniform("u_threshold", sthr);
  pShader.setUniform("u_N", sn);

  ui[1].setPosition({static_cast<float>(width) / 4, 0});
  ui[2].setPosition({static_cast<float>(width) / 2, 0});
  ui[3].setPosition({3 * static_cast<float>(width) / 4, 0});
  ui[4].setPosition({0, 20});
  ui[5].setPosition({static_cast<float>(width) / 5, 20});
  ui[6].setPosition({static_cast<float>(width) * 2 / 5, 20});
  ui[7].setPosition({static_cast<float>(width) * 3 / 5, 20});
  ui[8].setPosition({static_cast<float>(width) * 4 / 5, 20});

  ui[1].setFillColor(sf::Color(0, 0, 155, 255));
  ui[2].setFillColor(sf::Color(100, 0, 100, 255));
  ui[3].setFillColor(sf::Color(0, 100, 20, 255));
  ui[4].setFillColor(sf::Color::Cyan);
  ui[5].setFillColor(sf::Color::Green);
  ui[6].setFillColor(sf::Color::Magenta);
  ui[7].setFillColor(sf::Color::Yellow);
  ui[8].setFillColor(sf::Color(100, 100, 100, 255));

  text[1].setPosition({static_cast<float>(width) / 4, 0});
  text[2].setPosition({static_cast<float>(width) / 2, 0});
  text[3].setPosition({3 * static_cast<float>(width) / 4, 0});
  text[4].setPosition({0, 20});
  text[5].setPosition({static_cast<float>(width) / 5, 20});
  text[6].setPosition({static_cast<float>(width) * 2 / 5, 20});
  text[7].setPosition({static_cast<float>(width) * 3 / 5, 20});
  text[8].setPosition({static_cast<float>(width) * 4 / 5, 20});

  for (int i = 0; i < text.size(); i++) {
    text[i].setCharacterSize(20);
    text[i].setFillColor(sf::Color::Black);
  }

  curs.setOrigin({rad, rad});
  mouseVelo.setFillColor(sf::Color(0, 0, 0, 0));
  sf::RenderTexture canvas;
  if (!canvas.resize({width, height})) {
    return 1;
  };
  canvas.setSmooth(true);
  sf::Sprite canvasSprite(canvas.getTexture());
  auto window =
      sf::RenderWindow(sf::VideoMode({width, height}), "Simple Physics Engine",
                       sf::Style::Titlebar | sf::Style::Resize);
  window.setMouseCursorVisible(false);
  window.setFramerateLimit(60);
  sf::View view1(sf::FloatRect(
      {0.f, 0.f}, {static_cast<float>(width), static_cast<float>(height)}));
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {

      if (const auto *resized = event->getIf<sf::Event::Resized>()) {
        width = resized->size.x;
        height = resized->size.y;
        sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
        window.setView(sf::View(visibleArea));
      };

      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Z) {
          ofstream bout("balls.txt");
          for (const auto &b : balls) {
            bout << b.position.x << " " << b.position.y << " " << b.radius
                 << " " << b.velocity.x << " " << b.velocity.y << " "
                 << static_cast<int>(b.col.r) << " "
                 << static_cast<int>(b.col.g) << " "
                 << static_cast<int>(b.col.b) << " "
                 << static_cast<int>(b.col.a) << "\n";
          }
          ofstream wout("walls.txt");
          for (const auto &w : walls) {
            wout << w.position.x << " " << w.position.y << " " << w.size.x
                 << " " << w.size.y << " " << static_cast<int>(w.col.r) << " "
                 << static_cast<int>(w.col.g) << " "
                 << static_cast<int>(w.col.b) << " "
                 << static_cast<int>(w.col.a) << "\n";
          }
        }
      }
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::X) {
          balls = {};
          walls = {};
          ifstream bsave("balls.txt");
          float px, py, rad, vx, vy;
          int r, g, b, a;
          while (bsave >> px >> py >> rad >> vx >> vy >> r >> g >> b >> a) {
            spawn(sf::Vector2i{static_cast<int>(px), static_cast<int>(py)}, rad,
                  sf::Vector2f{vx, vy}, sf::Color(r, g, b, a));
          }
          bsave.close();

          ifstream wsave("walls.txt");
          float wx, wy, wsx, wsy;
          int wr, wg, wb, wa;
          while (wsave >> wx >> wy >> wsx >> wsy >> wr >> wg >> wb >> wa) {
            create(sf::Vector2i{static_cast<int>(wx), static_cast<int>(wy)},
                   sf::Vector2f{wsx, wsy}, sf::Color(wr, wg, wb, wa));
          }
          wsave.close();
        }
      }

      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (const auto *MouseButtonPressed =
              event->getIf<sf::Event::MouseButtonPressed>()) {
        if (MouseButtonPressed->button == sf::Mouse::Button::Left) {
          mouseVelo.setPosition(
              static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
          mouseVelo.setFillColor(sf::Color(255, 0, 0, 255));
          mouseVelo.setOrigin({5, 5});
          drag = true;
        }
      }
      if (const auto *MouseButtonReleased =
              event->getIf<sf::Event::MouseButtonReleased>()) {
        if (MouseButtonReleased->button == sf::Mouse::Button::Left) {
          sf::Vector2f mpos =
              static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
          sf::Vector2f v = mouseVelo.getPosition() - mpos;
          v = {v.x / 5, v.y / 5};
          mouseVelo.setFillColor(sf::Color(0, 0, 0, 0));
          drag = false;
          spawn(static_cast<sf::Vector2i>(curs.getPosition()), rad, v);
        }
      }

      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F10 and !wdrag) {
          mwallpos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
          wdrag = true;
        }
      }
      if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == sf::Keyboard::Key::F10) {
          wdrag = false;
          sf::Vector2f currentMouse =
              static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

          sf::Vector2f st(std::min(mwallpos.x, currentMouse.x),
                          std::min(mwallpos.y, currentMouse.y));
          sf::Vector2f si(std::abs(currentMouse.x - mwallpos.x),
                          std::abs(currentMouse.y - mwallpos.y));
          create(st, si);
        }
      }

      if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
        spawn(sf::Mouse::getPosition(window), rad, {0, 0});
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        balls = {};
        walls = {};
      }
      if (const auto *mouseWheelScrolled =
              event->getIf<sf::Event::MouseWheelScrolled>()) {
        if (sf::Mouse::getPosition().y < 20) {
          if (sf::Mouse::getPosition().x > 3 * static_cast<float>(width) / 4) {
            fric += mouseWheelScrolled->delta * 0.01;

          } else if (sf::Mouse::getPosition().x >
                     static_cast<float>(width) / 2) {
            bloss += mouseWheelScrolled->delta * 0.01;

          } else if (sf::Mouse::getPosition().x >
                     static_cast<float>(width) / 4) {
            wind += mouseWheelScrolled->delta * 0.01;
          } else {
            grav += mouseWheelScrolled->delta * 0.01;
          }
        } else if (sf::Mouse::getPosition().y < 40) {
          if (sf::Mouse::getPosition().x < static_cast<float>(width) / 5) {
            srad += mouseWheelScrolled->delta * 0.1;
            pShader.setUniform("u_radius", srad);
          } else if (sf::Mouse::getPosition().x <
                     static_cast<float>(width) * 2 / 5) {
            sthr += mouseWheelScrolled->delta * 0.01;
            pShader.setUniform("u_threshold", sthr);
          } else if (sf::Mouse::getPosition().x <
                     static_cast<float>(width) * 3 / 5) {
            sint += mouseWheelScrolled->delta * 0.01;
            pShader.setUniform("u_intensity", sint);
          } else if (sf::Mouse::getPosition().x <
                     static_cast<float>(width) * 4 / 5) {
            sn += mouseWheelScrolled->delta;
            pShader.setUniform("u_N", sn);
          } else {
            vt += mouseWheelScrolled->delta;
            vt = clamp(vt, 0, 2);
          }

        } else {
          rad += mouseWheelScrolled->delta;
          if (rad < 1)
            rad = 1;
          curs.setRadius(rad);
          curs.setOrigin({rad, rad});
        }
      }
    }
    for (int i = 0; i < balls.size(); i++) {
      for (int s = 0; s < st; s++) {
        balls[i].move();
        balls[i].clamp();
        for (int j = i + 1; j < balls.size(); j++) {
          if (i != j) {
            collide(balls[i], balls[j]);
          }
        }
        for (int k = 0; k < walls.size(); k++) {
          wallide(balls[i], walls[k]);
        }
      }
    }

    canvas.clear(sf::Color::Black);
    for (auto &ball : balls) {
      ball.update();
      if (vt == 1) {
        shader.setUniform("u_time", clock1.getElapsedTime().asSeconds());
        canvas.draw(ball.shape, &shader);
      } else if (vt == 0) {
        float v = std::clamp(sqrt(ball.velocity.x * ball.velocity.x +
                                  ball.velocity.y * ball.velocity.y) /
                                 veloM,
                             0.0f, 1.0f);
        ball.shape.setFillColor(
            sf::Color(std::clamp(2.0f * v - 1.0f, 0.0f, 1.0f) * 255.f,
                      std::sin(v * 3.14159f) * 255.f,
                      std::clamp(2.0f * (1.0f - v), 0.0f, 1.0f) * 255.f, 255));
        canvas.draw(ball.shape);
      } else {
        ball.shape.setFillColor(ball.col);
        canvas.draw(ball.shape);
      }
    }
    for (auto &wall : walls) {
      if (vt == 1) {
        shader.setUniform("u_time", clock1.getElapsedTime().asSeconds());
        canvas.draw(wall.shape, &shader);
      } else {
        canvas.draw(wall.shape);
      }
    }

    canvas.display();
    float currentTime = clock1.getElapsedTime().asSeconds();
    pShader.setUniform("u_resolution", sf::Vector2f(width, height));
    pShader.setUniform("u_time", currentTime);
    window.clear();
    window.draw(canvasSprite, &pShader);
    if (wdrag) {
      sf::Vector2f cmos =
          static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
      sf::Vector2f st(std::min(mwallpos.x, cmos.x),
                      std::min(mwallpos.y, cmos.y));
      sf::Vector2f si(std::abs(cmos.x - mwallpos.x),
                      std::abs(cmos.y - mwallpos.y));
      mwall.setPosition(st);
      mwall.setSize(si);
      mwall.setFillColor(sf::Color(255, 255, 255, 100));
      window.draw(mwall);
    }
    if (drag) {
      sf::Vertex aimLine[] = {
          sf::Vertex{static_cast<sf::Vector2f>(mouseVelo.getPosition())},
          sf::Vertex{
              static_cast<sf::Vector2f>(sf::Mouse::getPosition(window))}};
      window.draw(aimLine, 2, sf::PrimitiveType::Lines);
    }
    curs.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition()));
    text[0].setString("gravity is: " + to_string(grav));
    text[1].setString("wind is: " + to_string(wind));
    text[2].setString("restitution is: " + to_string(bloss));
    text[3].setString("friction is: " + to_string(fric));

    text[4].setString("shader radius is: " + to_string(srad));
    text[5].setString("shader threshold is: " + to_string(sthr));
    text[6].setString("shader intensity is: " + to_string(sint));
    text[7].setString("shader resolution is:" + to_string(sn));
    text[8].setString("colour type is: " + to_string(vt));

    for (int i = 0; i < text.size(); i++) {
      window.draw(ui[i]);
      window.draw(text[i]);
    }
    window.draw(curs);
    window.draw(mouseVelo);

    window.display();
  }
}
