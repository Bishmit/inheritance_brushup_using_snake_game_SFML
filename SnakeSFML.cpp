#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector> 

#define width 800.f
#define height 800.f

enum class Direction { None, Up, Down, Left, Right };

class GameObject {
protected:
    sf::Vector2f pos;
    sf::RectangleShape rs;
    bool movement = false;
    sf::Text text;

public:
    GameObject(float pos_x, float pos_y, float size_x, float size_y, sf::Color color) {
        pos.x = pos_x;
        pos.y = pos_y;
        rs.setPosition(pos);
        rs.setSize(sf::Vector2f(size_x, size_y));
        rs.setFillColor(color);
    }

    virtual void render(sf::RenderWindow& bind) {
        rs.setPosition(pos);
        bind.draw(rs);
    }

    virtual bool isColliding(const GameObject& other) {
        return rs.getGlobalBounds().intersects(other.rs.getGlobalBounds());
    }
    /* This is the method from which we can check the collision detection
    In SFML we can check collison by
    if(entity_a.getSprite().getGlobalBounds().intersects(entity_b.getSprite().getGlobalBounds()))
     {
    // A collision happened.
      }
    */
    bool checkCollision(const sf::RectangleShape& a, const sf::RectangleShape& b) {
        return a.getGlobalBounds().intersects(b.getGlobalBounds());

    }

};

class snake : public GameObject {
    Direction direction;
    std::vector<sf::RectangleShape> body; // Add a vector to store the body segments

public:
    snake(float pos_x, float pos_y) : GameObject(pos_x, pos_y, 10, 10, sf::Color::Green) {
        direction = Direction::None;
    }

    void move() {
        sf::Vector2f oldPos = pos;
        if (!movement) {
            // Save the current head position
            if (direction == Direction::Up) {
                pos.y -= 10;
            }
            else if (direction == Direction::Down) {
                pos.y += 10;
            }
            else if (direction == Direction::Left) {
                pos.x -= 10;
            }
            else if (direction == Direction::Right) {
                pos.x += 10;
            }
        }

        // Move the body segments
        if (!body.empty()) {
            for (int i = body.size() - 1; i > 0; i--) {
                body[i].setPosition(body[i - 1].getPosition());
            }
            body[0].setPosition(oldPos);
        }
    }
    void setDirection(Direction newDirection) {
        direction = newDirection;
    }

    void grow() {
        sf::RectangleShape segment;
        segment.setSize(sf::Vector2f(10, 10));
        segment.setFillColor(sf::Color::Yellow); // Set the color to match the snake
        segment.setPosition(pos);
        body.push_back(segment);
    }

    void boundaryCollide() {
        // to make the snake teleport through wall
        if (pos.x >= width - 10) pos.x = 0;
        else if (pos.x <= 10) pos.x = width - 10;

        if (pos.y >= height - 10) pos.y = 10;
        else if (pos.y <= 10) pos.y = height - 10;

        // check the self collison of snake 
        for (size_t i = 1; i < body.size(); i++) {
            if (checkCollision(body[0], body[i]))
                movement = true;
        }
    }


    // Override the render method to also render the body segments
    void render(sf::RenderWindow& bind) {
        GameObject::render(bind); // Render the head

        // Render the body segments
        for (const auto& segment : body) {
            bind.draw(segment);
        }
    }
};

class foodspawn : public GameObject {
public:
    foodspawn(int pos_x, int pos_y) : GameObject(pos_x, pos_y, 10, 10, sf::Color::White) {
    }
};

class boundary : public GameObject {
public:
    boundary(int pos_x, int pos_y, float size_x, float size_y) : GameObject(pos_x, pos_y, size_x, size_y, sf::Color::Magenta) {
    }

};

int main() {
    sf::RenderWindow window(sf::VideoMode(width, height), "Snake");
    window.setFramerateLimit(10);
    boundary leftBoundary(0, 0, 10, height);
    boundary topBoundary(0, 0, width, 10);
    boundary rightBoundary(width - 10, 0, 10, height);
    boundary bottomBoundary(0, height - 10, width, 10);
    snake sn(width / 2, height / 2);
    srand(static_cast<unsigned>(time(NULL)));

    int width_food = rand() % static_cast<int>(width - 50);
    int height_food = rand() % static_cast<int>(height - 50);
    foodspawn fs(width_food, height_food);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                sn.setDirection(Direction::Up);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                sn.setDirection(Direction::Down);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                sn.setDirection(Direction::Left);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                sn.setDirection(Direction::Right);
            }
        }


        sn.move();
        sn.boundaryCollide();

        // Check for collision between snake and food
        while (sn.isColliding(fs)) {
            // Handle collision, e.g., make the food disappear and generate a new one
            int new_width_food = rand() % static_cast<int>(width);
            int new_height_food = rand() % static_cast<int>(height);
            fs = foodspawn(new_width_food, new_height_food);
            sn.grow(); // Extend the snake's body
        }

        window.clear();
        leftBoundary.render(window); topBoundary.render(window); rightBoundary.render(window); bottomBoundary.render(window);
        sn.render(window);
        fs.render(window);
        window.display();
    }
}

