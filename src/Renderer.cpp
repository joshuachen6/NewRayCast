#include "Renderer.h"
#include "Physics.h"
#include <boost/lockfree/queue.hpp>
#include <boost/range/irange.hpp>
#include <cmath>
#include <execution>
#include <filesystem>
#include <format>
#include <memory>

sf::Sprite *Renderer::get_column(sf::Texture *texture, Vertex &vertex,
                                 sf::Vector2f &collision, int cols) {
  sf::Sprite *sprite = new sf::Sprite(*texture);
  double scale = texture->getSize().x / vertex.length();
  double left = sqrt(pow(collision.x - vertex.start.x, 2) +
                     pow(collision.y - vertex.start.y, 2));
  sprite->setTextureRect(
      sf::IntRect(left * scale, 0, cols, texture->getSize().y));
  return sprite;
}

void Renderer::draw_minimap(World &world, Player &camera) {
  // the range of the minimap:
  double range = 3.5 * METER;
  double on_screen_radius = window->getSize().y / 6;
  double padding = on_screen_radius / 5;
  double scale = on_screen_radius / range * 0.8;
  sf::Vector2f center(window->getSize().x - on_screen_radius - padding,
                      on_screen_radius + padding);

  sf::CircleShape dish(on_screen_radius);
  dish.setOutlineColor(sf::Color::White);
  dish.setOutlineThickness(on_screen_radius / 100);
  dish.setFillColor(sf::Color::Black);
  dish.setPosition(center - sf::Vector2f(on_screen_radius, on_screen_radius));
  window->draw(dish);

  sf::Vector2f pos = Physics::squash(camera.location);
  for (const std::unique_ptr<Vertex> &vertex : world.vertices) {
    if (Physics::distance(pos, vertex->start) < range &&
        Physics::distance(pos, vertex->start) < range) {
      sf::Vertex line[] = {
          sf::Vertex(Physics::ham(Physics::scale(vertex->start - pos, scale),
                                  sf::Vector2f(1, -1)) +
                     center),
          sf::Vertex(Physics::ham(Physics::scale(vertex->end - pos, scale),
                                  sf::Vector2f(1, -1)) +
                     center)};

      window->draw(line, on_screen_radius / 50, sf::Lines);
    }
  }

  for (const std::unique_ptr<Entity> &entity : world.entities) {
    sf::Vector2f entity_pos = Physics::squash(entity->location);
    if (Physics::distance(pos, entity_pos) + entity->radius < range) {
      double raidus = entity->radius * scale;
      sf::CircleShape indicator(raidus);
      indicator.setFillColor(sf::Color::Red);
      indicator.setPosition(
          Physics::ham(Physics::scale(entity_pos - pos, scale),
                       sf::Vector2f(1, -1)) +
          center - sf::Vector2f(raidus, raidus));
      window->draw(indicator);
    }
  }

  if (debug) {
    for (const std::unique_ptr<Entity> &entity : world.entities) {
      sf::Vector2f entity_center =
          Physics::ham(
              Physics::scale(Physics::squash(entity->location) - pos, scale),
              sf::Vector2f(1, -1)) +
          center;
      sf::Vertex vel[] = {
          sf::Vertex(entity_center),
          sf::Vertex(entity_center +
                     Physics::ham(Physics::scale(entity->velocity, scale),
                                  sf::Vector2f(1, -1)))};
      vel[0].color = sf::Color::Cyan;
      vel[1].color = sf::Color::Green;
      window->draw(vel, 2, sf::Lines);
    }
  }

  sf::Vector2f offset;
  for (int i = 0; i < 4; i++) {
    double radius = camera.radius * scale * 1 / (i + 1);
    sf::CircleShape cursor(radius);
    cursor.setFillColor(sf::Color::Blue);
    cursor.setPosition(center - sf::Vector2f(radius, radius) + offset);
    window->draw(cursor);
    offset += sf::Vector2f(radius * std::cos(camera.location.z),
                           -radius * std::sin(camera.location.z));
  }
}

sf::Text Renderer::text_of(std::string text) {
  sf::Text output;
  output.setFont(font);
  output.setString(text);
  return output;
}

Renderer::Renderer(sf::RenderWindow &window) {
  this->window = &window;
  render_texture.create(window.getSize().x, window.getSize().y);
  std::filesystem::path resourceFolder("resources");
  font.loadFromFile(resourceFolder / "font.ttf");
  noise_shader.loadFromFile(resourceFolder / "noise.glsl",
                            sf::Shader::Fragment);
  debug = false;
  show_minimap = true;
}

void Renderer::update(World &world, Player &camera, double fov, double rays,
                      double dt) {
  render_texture.clear();

  sf::Sprite sky;
  sky.setTexture(*world.load_texture(world.sky_texture));
  sky.setScale(
      double(render_texture.getSize().x) / sky.getTexture()->getSize().x,
      (render_texture.getSize().y / 2.0) / sky.getTexture()->getSize().y);
  render_texture.draw(sky);
  sf::Sprite ground;
  ground.setTexture(*world.load_texture(world.ground_texture));
  ground.setScale(
      double(render_texture.getSize().x) / ground.getTexture()->getSize().x,
      (render_texture.getSize().y / 2.0) / ground.getTexture()->getSize().y);
  ground.setPosition(0, render_texture.getSize().y / 2);
  render_texture.draw(ground);

  double offset = fov / rays;
  double xoffset = render_texture.getSize().x / rays;
  boost::integer_range<int> ops =
      boost::irange<int>((int)(-rays / 2), (int)(rays / 2));
  boost::lockfree::queue<sf::Sprite *> sprite_queue(0);

  std::for_each(
      std::execution::par_unseq, ops.begin(), ops.end(), [&](const int &i) {
        double angle = Physics::scale_angle(offset * i + camera.location.z);

        std::vector<CastResult> hits =
            Physics::cast_ray(world, camera.location, angle);
        if (hits.size()) {
          for (int j = hits.size() - 1; j >= 0; j--) {
            CastResult &closest = hits[j];
            if (closest.entity == &camera) {
              continue;
            }
            sf::Texture *texture = world.load_texture(closest.vertex->texture);
            if (texture) {
              sf::Sprite *sprite = get_column(
                  texture, *closest.vertex, closest.point, std::ceil(xoffset));
              double trueDistance = closest.distance * cos(offset * i);
              double vScale = (closest.vertex->height / trueDistance) *
                              (((double)render_texture.getSize().y) /
                               sprite->getTextureRect().height);
              double height = render_texture.getSize().y / 2 -
                              sprite->getTextureRect().height / 2 * vScale;
              double dist =
                  (METER - closest.vertex->height - closest.vertex->z) /
                  (2 * trueDistance) * render_texture.getSize().y;
              sprite->setScale(1, vScale);
              sprite->setPosition(
                  sf::Vector2f(-i * xoffset + render_texture.getSize().x / 2 -
                                   sprite->getTextureRect().width / 2,
                               height + dist));

              // make sprites darker to create fog effect
              if (closest.distance > 0) {
                double darkness =
                    std::fmax(1, std::sqrt(closest.distance / METER));
                sprite->setColor(sf::Color(255 / darkness, 255 / darkness,
                                           255 / darkness, 255 / darkness));
              }

              sprite_queue.push(sprite);
            }
          }
        }
      });
  while (!sprite_queue.empty()) {
    sf::Sprite *sprite;
    sprite_queue.pop(sprite);
    render_texture.draw(*sprite);
    delete sprite;
  }

  render_texture.display();
  noise_shader.setUniform("time", float(dt));
  noise_shader.setUniform("scale", 0.1f);
  noise_shader.setUniform(
      "resolution", sf::Vector2f(window->getSize().x, window->getSize().y));

  window->clear();
  sf::Sprite scene;
  scene.setTexture(render_texture.getTexture());
  scene.setScale(double(window->getSize().x) / scene.getTexture()->getSize().x,
                 double(window->getSize().y) / scene.getTexture()->getSize().y);
  window->draw(scene, &noise_shader);

  // draw hud here so its not noised
  if (show_minimap) {
    draw_minimap(world, camera);
  }

  if (debug) {
    sf::Text title = text_of("Camera Debug");
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Underlined);
    title.setCharacterSize(24);
    window->draw(title);

    sf::Text pos = text_of(std::format(
        "Position ({}, {})", int(camera.location.x), int(camera.location.y)));
    pos.setFillColor(sf::Color::Cyan);
    pos.setCharacterSize(16);
    pos.setPosition(0, 24);
    window->draw(pos);

    sf::Text vel = text_of(std::format(
        "Velocity ({}, {})", int(camera.velocity.x), int(camera.velocity.y)));
    vel.setFillColor(sf::Color::Blue);
    vel.setCharacterSize(16);
    vel.setPosition(0, 40);
    window->draw(vel);

    sf::Text yaw = text_of(
        std::format("Yaw {}", int(Physics::to_degrees(camera.location.z))));
    yaw.setFillColor(sf::Color::Green);
    yaw.setCharacterSize(16);
    yaw.setPosition(0, 56);
    window->draw(yaw);

    sf::Text fps = text_of(std::format("FPS {}", int(1 / dt)));
    fps.setFillColor(sf::Color::White);
    fps.setCharacterSize(32);
    fps.setPosition(0, 88);
    window->draw(fps);
  }

  window->display();
}
