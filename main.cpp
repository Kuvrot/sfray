/* SFML Raycast Engine
 *
 * Example Program
 *
 * by Dan Alexander
 */
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"
#include "BSPGenerator.h"
#include "Entity.hpp"
#include "Raycaster.hpp"
#include "Map.hpp"

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// GFX SETTINGS
int gfx_width = WINDOW_WIDTH;
int gfx_height = WINDOW_HEIGHT;

int main(int, char const**)
{
    srand(1); // using a seed to help debug changes
    sfray::Map worldMap;
    worldMap.CreateFromBSP(MAP_WIDTH, MAP_HEIGHT);

    sfray::Raycaster raycaster;
    worldMap.LoadTexture(1, resourcePath() + "stonewall.png");
    worldMap.LoadTexture(2, resourcePath() + "stonewall3.png");
    worldMap.LoadTexture(3, resourcePath() + "stonewall4.png");
//    worldMap.LoadTexture(0, resourcePath() + "floor.png");
//    worldMap.LoadTexture(-1, resourcePath() + "floor2.png");
    worldMap.LoadTexture(4, resourcePath() + "pillar.png");
    worldMap.LoadTexture(5, resourcePath() + "barrel.png");

//	worldMap.LoadTexture(1, resourcePath() + "stonewall_64.png");
//    worldMap.LoadTexture(2, resourcePath() + "stonewall3_64.png");
//    worldMap.LoadTexture(3, resourcePath() + "stonewall4_64.png");
    worldMap.LoadTexture(0, resourcePath() + "floor_64.png");
    worldMap.LoadTexture(-1, resourcePath() + "floor2_64.png");

    for (unsigned int i = 0; i < worldMap.Rooms.size(); ++i){
        sfray::Entity* ent = new sfray::Entity();
        ent->setTextureUID((rand() % 2 == 0 ? 4 : 5));
        ent->setPosition(worldMap.Rooms[i].x + worldMap.Rooms[i].width / 2,
                         worldMap.Rooms[i].y + worldMap.Rooms[i].height / 2);
        worldMap.addEntity(ent);
    }

    raycaster.setMap(worldMap);

    raycaster.setSize(320, 240);
//	raycaster.setCeilingRenderMethod(sfray::Ceiling_None);
//	raycaster.setFloorRenderMethod(sfray::Floor_None);
//	raycaster.setWallRenderMethod(sfray::Wall_Color);
//	raycaster.setEntityRenderMethod(sfray::Entity_None);

    sfray::Camera camera;
    camera.setPosition(worldMap.Rooms[0].x + 1, worldMap.Rooms[0].y + 1);
    camera.setDirectionInDegrees(0);

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Raycast Engine Example");

    // GAME TIME CLOCK
    sf::Clock clock;

    float fps = 0.0f;
    float fps_timer = 0.0f;

    // Framerate counter TEXT OBJECT
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")){
        throw "Could not load font";
    }
    sf::Text fps_text;
    fps_text.setFont(font);
    fps_text.setCharacterSize(32);
    fps_text.setColor(sf::Color::Yellow);
    fps_text.setPosition(10, 10);

    // Start the game loop. This is the normal basics used in SFML.
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed : exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // render settings for debug
            // Z: Change floor render method
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z){
                int current = raycaster.getFloorRenderMethod();
                current += 1;
                if (current > 2){
                    current = 0;
                }
                raycaster.setFloorRenderMethod((sfray::FloorRenderMethod)current);
            }
            // X: Change ceiling render method
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X){
                int current = raycaster.getCeilingRenderMethod();
                current += 1;
                if (current > 2){
                    current = 0;
                }
                raycaster.setCeilingRenderMethod((sfray::CeilingRenderMethod)current);
            }
            // C: Change wall render method
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C){
                int current = raycaster.getWallRenderMethod();
                current += 1;
                if (current > 2){
                    current = 0;
                }
                raycaster.setWallRenderMethod((sfray::WallRenderMethod)current);
            }
            // V: change entity render method
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::V){
                int current = raycaster.getEntityRenderMethod();
                current += 1;
                if (current > 1){
                    current = 0;
                }
                raycaster.setEntityRenderMethod((sfray::EntityRenderMethod)current);
            }
        }

        double delta = clock.restart().asSeconds();
        fps = 1.0f / delta;

        double rotSpeed = 3.0 * delta;
        float degreesToRotate = 128.0f * delta;
        double moveSpeed = 5.0 * delta;
        double moveCheck = 0.5;

        camera.moved = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            camera.moved = true;
            // rotate camera left
            camera.rotateByDegrees(degreesToRotate);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            camera.moved = true;
            // rotate camera right
            camera.rotateByDegrees(-degreesToRotate);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            // move camera forwards
            camera.moved = true;
            sf::Vector2f pos = camera.getPosition();
            if (worldMap.GetTile(int(camera.getPosition().x + camera.getDirection().x * moveCheck),int(camera.getPosition().y)).IsWall == false){
                pos.x += camera.getDirection().x * moveSpeed;
            }
            if (worldMap.GetTile(int(camera.getPosition().x),int(camera.getPosition().y + camera.getDirection().y * moveCheck)).IsWall == false){
                pos.y += camera.getDirection().y * moveSpeed;
            }
            camera.setPosition(pos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            // move camera backwards
            camera.moved = true;
            sf::Vector2f pos = camera.getPosition();
            if (worldMap.GetTile(int(camera.getPosition().x - camera.getDirection().x * moveCheck),int(camera.getPosition().y)).IsWall == false){
                pos.x -= camera.getDirection().x * moveSpeed;
            }
            if (worldMap.GetTile(int(camera.getPosition().x),int(camera.getPosition().y - camera.getDirection().y * moveCheck)).IsWall == false){
                pos.y -= camera.getDirection().y * moveSpeed;
            }
            camera.setPosition(pos);
        }

        // DRAWING BEGINS HERE

        // Clear the screen
        window.clear();

        // Have the raycasting engine draw for our camera
        raycaster.drawForCamera(window, camera);

        // RESET THE VIEW
        window.setView(window.getDefaultView());

        // Draw the framerate counter
        fps_timer -= delta;
        if (fps_timer <= 0.0f){
            fps_timer = 1.0f;
            fps_text.setString(std::to_string((int)fps));
        }
        window.draw(fps_text);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}


