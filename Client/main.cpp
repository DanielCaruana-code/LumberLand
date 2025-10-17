#include <iostream>
#include "Includes.h"
#include "tileson.hpp"
#include "memory"
/*
How to play game, I don't know how to properly implement flashing text,

You are the gardener who waters trees.

Go To Well, Get water.

Get Water Points and walk to trees to get points.

Walking into hazards like cactie hurt you and the skeleton out right kills you so be sure to avoid him

Walking up to the guard trades some points for a speed boost to get more points faster

The point of the game is to get the highest points
*/
#pragma region Foward declarations and user variables
//forward declerations
class Collidable;
class CollidableRectangle;
class CollidablePolygon;

int buffTimer = 500;
int points = 0;
int waterBucketUses = 0;
int health = 100;
int* Points = &points;
int* WaterBucketUses = &waterBucketUses;
int* Health = &health;
bool isPlayerDead = false;
bool buffedPlayer = false;
#pragma endregion  Foward declarations and user variables
#pragma region Animation
// player Animations
const int NUM_FRAMES_IDLE = 5,
NUM_FRAMES_RUNUP = 5,
NUM_FRAMES_RUNDOWN = 5,
NUM_FRAMES_RUNRIGHT = 5,
NUM_FRAMES_RUNLEFT = 5;

Texture2D idleFrames[NUM_FRAMES_IDLE],
runDownFrames[NUM_FRAMES_RUNDOWN],
runUpFrames[NUM_FRAMES_RUNDOWN],
runRightFrames[NUM_FRAMES_RUNDOWN],
runLeftFrames[NUM_FRAMES_RUNDOWN];


// Enemy Animations
const int NUM_EFRAMES_RUNUP = 4,
NUM_EFRAMES_RUNDOWN = 4,
NUM_EFRAMES_RUNLEFT = 4,
NUM_EFRAMES_RUNRIGHT = 4;

Texture2D eRunDownFrames[NUM_EFRAMES_RUNDOWN],
eRunUpFrames[NUM_EFRAMES_RUNDOWN],
eRunLeftFrames[NUM_EFRAMES_RUNLEFT],
eRunRightFrames[NUM_EFRAMES_RUNRIGHT];

enum Animations { IDLE, RUNDown, RUNUp, RUNRight, RUNLeft, ERUNUp, ERUNDown, ERUNLeft, ERUNRight };
Animations currentState = IDLE;

void LoadAnimations() {
    //Paladin Load Anim
    for (int i = 0; i < NUM_FRAMES_IDLE; i++) {
        char fileName[100];
        sprintf(fileName, "images/PaladinIdle/Idle%d.png", i);
        idleFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_FRAMES_RUNDOWN; i++) {
        char fileName[100];
        sprintf(fileName, "images/PaladinDown/Down%d.png", i);
        runDownFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_FRAMES_RUNUP; i++) {
        char fileName[100];
        sprintf(fileName, "images/PaladinUp/UP%d.png", i);
        runUpFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_FRAMES_RUNRIGHT; i++) {
        char fileName[100];
        sprintf(fileName, "images/PaladinRight/Right%d.png", i);
        runRightFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_FRAMES_RUNLEFT; i++) {
        char fileName[100];
        sprintf(fileName, "images/PaladinLeft/LEFT%d.png", i);
        runLeftFrames[i] = LoadTexture(fileName);
    }
    //Skeleton Load Anim
    for (int i = 0; i < NUM_EFRAMES_RUNDOWN; i++) {
        char fileName[100];
        sprintf(fileName, "images/SkeletonMoveDown/Skeleton%d.png", i);
        eRunDownFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_EFRAMES_RUNUP; i++) {
        char fileName[100];
        sprintf(fileName, "images/SKeletonMoveUp/Skeleton%d.png", i);
        eRunUpFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_EFRAMES_RUNRIGHT; i++) {
        char fileName[100];
        sprintf(fileName, "images/SkeletonMoveRight/Skeleton%d.png", i);
        eRunRightFrames[i] = LoadTexture(fileName);

    }
    for (int i = 0; i < NUM_EFRAMES_RUNLEFT; i++) {
        char fileName[100];
        sprintf(fileName, "images/SkeletonMoveLeft/Skeleton%d.png", i);
        eRunLeftFrames[i] = LoadTexture(fileName);

    }
}

void UnloadAnimations() {
    //Paladin Deload Anim
    for (int i = 0; i < NUM_FRAMES_IDLE; i++) {
        UnloadTexture(idleFrames[i]);
    }
    for (int i = 0; i < NUM_FRAMES_RUNDOWN; i++) {
        UnloadTexture(runDownFrames[i]);
    }
    for (int i = 0; i < NUM_FRAMES_RUNUP; i++) {
        UnloadTexture(runUpFrames[i]);
    }
    for (int i = 0; i < NUM_FRAMES_RUNRIGHT; i++) {
        UnloadTexture(runRightFrames[i]);
    }
    for (int i = 0; i < NUM_FRAMES_RUNLEFT; i++) {
        UnloadTexture(runLeftFrames[i]);
    }
    //Skeleton Deload Anim
    for (int i = 0; i < NUM_EFRAMES_RUNUP; i++) {
        UnloadTexture(eRunUpFrames[i]);
    }
    for (int i = 0; i < NUM_EFRAMES_RUNRIGHT; i++) {
        UnloadTexture(eRunRightFrames[i]);
    }
    for (int i = 0; i < NUM_EFRAMES_RUNLEFT; i++) {
        UnloadTexture(eRunLeftFrames[i]);
    }
    for (int i = 0; i < NUM_EFRAMES_RUNDOWN; i++) {
        UnloadTexture(eRunDownFrames[i]);
    }
}
#pragma endregion Animations 
#pragma region Classes
//base class for collidable objects
class Collidable {
public:
    std::string tag;

    Collidable(const std::string& tag) : tag(tag) {}

    virtual bool CheckCollision(const Rectangle& playerRect) = 0; // Pure virtual function for checking collision with a rectangle
    virtual ~Collidable() = default;
};

//class for collidable rectangles
class CollidableRectangle : public Collidable {
private:
    Rectangle rectangle;

public:
    CollidableRectangle(const Rectangle& rect, const std::string& tag = "")
        : Collidable(tag), rectangle(rect) {
    }

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRecs(this->rectangle, playerRect);
    }

};

//class for collidable polygons
class CollidablePolygon : public Collidable {
private:
    std::vector<Vector2> polygonPoints;

    //There's a bug in CheckCollisionPointPoly RayLibV5.0. CustomCheckCollisionPointPoly() is the fixed code which is published here: https://github.com/raysan5/raylib/commit/ef92ced370815a47d6d417ce6394b3156dd4f528
    bool CustomCheckCollisionPointPoly(Vector2 point, Vector2* points, int pointCount)
    {
        bool inside = false;
        if (pointCount > 2)
        {
            for (int i = 0, j = pointCount - 1; i < pointCount; j = i++)
            {
                if ((points[i].y > point.y) != (points[j].y > point.y) &&
                    (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
                {
                    inside = !inside;
                }
            }
        }
        return inside;
    }

    // Function to check collision between a rectangle and a polygon
    bool CheckCollisionRectPoly(Rectangle rect, std::vector<Vector2>& polygonPoints) {
        // Define the corners of the rectangle
        Vector2 corners[4] = {
            {rect.x, rect.y}, // Top-left
            {rect.x + rect.width, rect.y}, // Top-right
            {rect.x, rect.y + rect.height}, // Bottom-left
            {rect.x + rect.width, rect.y + rect.height} // Bottom-right
        };

        // Check if any corner is inside the polygon
        for (const auto& corner : corners) {
            if (CustomCheckCollisionPointPoly(corner, polygonPoints.data(), polygonPoints.size())) {
                return true; // Collision detected
            }
        }

        // Check if any polygon vertex is within the rectangle
        for (const auto& point : polygonPoints) {
            if (CheckCollisionPointRec(point, rect)) {
                return true; // Collision detected
            }
        }

        return false; // No collision detected
    }

public:
    CollidablePolygon(const std::vector<Vector2>& points, const std::string& tag = "")
        : Collidable(tag), polygonPoints(points) {
    }

    bool CheckCollision(const Rectangle& playerRect)  override {
        return CheckCollisionRectPoly(playerRect, polygonPoints);
    }
};

// NPC class definition
class NPC {
private:
    Vector2 position; // Stores the current position of the NPC
    int speed;        // Movement speed of the NPC
    int currentPoint; // Index of the current target point in the path
    Vector2 direction;
    float rotationSpeed;
    Animations enemyState;


public:
    // Constructor for NPC, initializes position, speed, and starting point
    NPC(Vector2 startPos, int moveSpeed) : position(startPos), speed(moveSpeed), currentPoint(0), direction({ 0,-1 }), rotationSpeed(1.0f) {}

    void Update(Vector2 targetP) {
        Vector2 targetPDirection = Vector2Subtract(targetP, position);

        targetPDirection = Vector2Normalize(targetPDirection);

        // Calculate angle between current direction and target (0-360 degrees)
        float angle = Vector2Angle(direction, targetPDirection) * RAD2DEG;
        if (angle < 0) angle += 360;

        if (targetPDirection.x > 0 ) {
            enemyState = ERUNRight;
        }
        else if (targetPDirection.y > 0) {
            enemyState = ERUNDown;
        }
        else if (targetPDirection.x < 0) {
            enemyState = ERUNLeft;
        }
        else if(targetPDirection.y < 0){
            enemyState = ERUNUp;
        }

        float rotationAmount = rotationSpeed * GetFrameTime();
        direction = Vector2Rotate(direction, rotationAmount * (angle > 180 ? 1 : -1));

        // Check for wall collisions and reverse ball direction if collided
        if (position.x <= 0 || position.x >= GetScreenWidth()) {
            position.x *= -1; // Reverse horizontal direction
        }
        if (position.y <= 0 || position.y >= GetScreenHeight()) {
            position.y *= -1; // Reverse vertical direction
        }
    }

    // Function to move the NPC towards a target point
    void moveTo(Vector2 targetP) {
        Vector2 direction = Vector2Subtract(targetP, position); // Calculate the direction vector towards the target
        if (Vector2Length(direction) > speed) {
            direction = Vector2Scale(Vector2Normalize(direction), speed); // Normalize and scale the direction vector
            position = Vector2Add(position, direction); // Update the position of the NPC
        }
        else {
            position = targetP; // If close enough to the target, set position to the target point
            isPlayerDead = true;
        }
    }

    void moveToSimplified(Vector2 target) {
        position = Vector2MoveTowards(position, target, speed);
    }

    // Function to draw the NPC
    void draw() {

        Vector2 endpoint = Vector2Add(position, Vector2Scale(direction, 1));
        const float frameChangeRate = 0.2f;
        float timeCounter = 0.0f;
        int enemyFrame = 0;

        timeCounter += GetFrameTime();
        if (timeCounter >= frameChangeRate) {
            timeCounter = 0.0f;
            enemyFrame++;
        }

        switch (enemyState) {
        case ERUNDown:
            DrawTextureEx(eRunDownFrames[enemyFrame], position, 0, 1.5f, WHITE);
            break;
        case ERUNUp:
            DrawTextureEx(eRunUpFrames[enemyFrame], position, 0, 1.5f, WHITE);
            break;
        case ERUNLeft:
            DrawTextureEx(eRunLeftFrames[enemyFrame], position, 0, 1.5f, WHITE);
            break;
        case ERUNRight:
            DrawTextureEx(eRunRightFrames[enemyFrame], position, 0, 1.5f, WHITE);
            break;
        }

        if (enemyState == ERUNUp) {
            if (enemyFrame >= NUM_EFRAMES_RUNUP) enemyFrame = 0;
        }
        else if (enemyState == ERUNDown) {
            if (enemyFrame >= NUM_EFRAMES_RUNDOWN) enemyFrame = 0;
        }
        else if (enemyState == ERUNLeft) {
            if (enemyFrame >= NUM_EFRAMES_RUNRIGHT) enemyFrame = 0;
        }
        else if (enemyState == ERUNRight) {
            if (enemyFrame >= NUM_EFRAMES_RUNLEFT) enemyFrame = 0;
        }

    }
};

#pragma endregion Classes
# pragma region Functions
void DrawUI() {
    DrawText(TextFormat("Points: %d", *Points), 1, 1 , 24,BLACK);
    DrawText(TextFormat("Water Bucket Uses: %d", *WaterBucketUses), 650, 1, 24, BLACK);
    DrawText(TextFormat("Health: %d", *Health), 150, 1, 24, BLACK);
    DrawText(TextFormat("Buff Timer: %d", buffTimer),720,600, 24,BLACK);
    if (isPlayerDead == true) {
        DrawText(TextFormat("You Died!"), 380, 300, 50, RED);

    }
}

void CheckPlayerIsBuffed() {
    if (buffedPlayer == true) {
        buffTimer--;

    }
    if (buffTimer <= 0) {
        buffTimer = 500;
        buffedPlayer = false;
    }
}

void CheckPlayerDead() {
    if (health <= 0) {
        isPlayerDead = true;
    }
} 

#pragma endregion Functions 
#pragma region Main
int main()
{
    // Initialize Raylib window
    int screenWidth = 960;
    int screenHeight = 640;

    float scaleFactor = 2.0f; // This can be any positive float. 2.0 means twice as big.

    InitWindow(screenWidth, screenHeight, "LumberLand");
    LoadAnimations();
    SetTargetFPS(60);

    NPC npc({ 500,200 }, 1);
    float timeCounter = 0.0f;
    int currentFrame = 0;
    const float frameChangeRate = 0.2f; // Time in seconds to change frames

    // Load the Tiled map using Tileson
    tson::Tileson tileson;
    std::unique_ptr<tson::Map> map = tileson.parse("map/LumberLand.tmj");

    if (map->getStatus() != tson::ParseStatus::OK)
    {
        fmt::print("Error: {}\n", "Failed to load map!");
        return -1;
    }

    // Load the tileset texture (assuming there is one tileset or you want the first)
    Texture2D tilesetTexture = LoadTexture(map->getTilesets()[0].getImage().u8string().c_str()); // Assuming there is one tileset or you want the first, adjust as necessary

    if (tilesetTexture.id == 0)
    {
        fmt::print("Error: {}\n", "Tileset texture not loaded!");
        return -1;
    }

    std::vector<std::unique_ptr<Collidable>> collidables;

    // Iterate through the map layers
    for (auto& layer : map->getLayers())
    {
        if (layer.getType() == tson::LayerType::TileLayer)
        {

            // Iterate through the tile objects in the layer
            for (auto& [pos, tileObject] : layer.getTileObjects())
            {
                // Check if the tile has an object group with objects
                if (tileObject.getTile()->getObjectgroup().getObjects().size() > 0)
                {
                    // Check if the tile has a "collidable" property set to true
                    if (tileObject.getTile()->getProperties().hasProperty("Collision")) {
                        if (tileObject.getTile()->getProperties().getProperty("Collision")->getValue<bool>()) {
                            fmt::print("Loading Collidable Object \n");

                            std::string tag;
                            if (tileObject.getTile()->getProperties().hasProperty("Tag")) {
                                tag = tileObject.getTile()->getProperties().getProperty("Tag")->getValue<std::string>();
                            }

                            // Iterate through the objects in the tile's object group
                            for (auto& obj : tileObject.getTile()->getObjectgroup().getObjects())
                            {

                                if (obj.getPolygons().empty()) { //we assume a rectangle boudning box if no polygon is detected
                                    // Calculate the scaled position and size of the tile
                                    Rectangle colliderRect = {
                                        (float)(tileObject.getPosition().x + obj.getPosition().x) * scaleFactor,
                                        (float)(tileObject.getPosition().y + obj.getPosition().y) * scaleFactor,
                                        (float)obj.getSize().x * scaleFactor,
                                        (float)obj.getSize().y * scaleFactor
                                    };

                                    // Create a CollidableRectangle and add it to the collidables vector
                                    collidables.push_back(std::make_unique<CollidableRectangle>(colliderRect, tag));
                                }

                                if (!obj.getPolygons().empty()) { //if there are polygons, we assume the object is a polygon
                                    const std::vector<tson::Vector2i>& polygonPoints = obj.getPolygons();

                                    std::vector<Vector2> scaledPolygonPoints;

                                    for (const auto& point : polygonPoints) {
                                        // Scale the polygon points based on the scale factor
                                        Vector2 scaledPoint = {
                                            (float)(tileObject.getPosition().x + obj.getPosition().x + point.x) * scaleFactor,
                                            (float)(tileObject.getPosition().y + obj.getPosition().y + point.y) * scaleFactor
                                        };
                                        scaledPolygonPoints.push_back(scaledPoint);
                                    }

                                    // Create a CollidablePolygon and add it to the collidables vector
                                    collidables.push_back(std::make_unique<CollidablePolygon>(scaledPolygonPoints, tag));

                                }
                            }
                        }

                    }
                }
            }

        }
    }


    // Define player properties
    Vector2 playerPosition = { 300, 200 }; // Example position
    const int playerSize = 16; // adjust based on your player's sprite size
    int PlayerHealth = 5;
    Rectangle playerRect = { playerPosition.x, playerPosition.y, playerSize , playerSize };
    float playerSpeed = 150.0f;
    // Define EnemyProperties
    Vector2 enemyPosition = { 500,200 };
    const int enemySize = 16;



    // Main game loop
    while (!WindowShouldClose())
    {
        if (buffedPlayer == true) {
            playerSpeed = 300.0f;
        }
        else {
           playerSpeed = 150.0f;
        }
        //update
        npc.moveTo(playerPosition);
        npc.Update(playerPosition);
        float deltaTime = GetFrameTime();

        if (IsKeyDown(KEY_DOWN)) {
            currentState = RUNDown;
        }
        else if (IsKeyDown(KEY_UP)) {
            currentState = RUNUp;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            currentState = RUNRight;
        }
        else if (IsKeyDown(KEY_LEFT)) {
            currentState = RUNLeft;
        }
        else {
            currentState = IDLE;
        }

        // Calculate potential new position based on input
        Vector2 potentialPosition = playerPosition;
        potentialPosition.x += (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * playerSpeed * deltaTime;
        potentialPosition.y += (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * playerSpeed * deltaTime;

        // Create a rectangle for the potential new position
        Rectangle potentialPlayerRect = { potentialPosition.x, potentialPosition.y, playerRect.width, playerRect.height };

        // Check for collisions
        bool collisionDetected = false;


        // Iterate through the collidable objects and check for collisions
        for (const auto& collidable : collidables)
        {
            if (collidable->CheckCollision(potentialPlayerRect))
            {
                collisionDetected = true;
                fmt::print("Collision detected with {}\n", collidable->tag);
                if (collidable->tag == "Well"){
                    waterBucketUses++;
                    fmt::print("Filling Up Water Bucket");
                }
                else if (collidable->tag == "Tree" && waterBucketUses != 0 ) {
                    fmt::print("Trees Watered, You earn Points");
                    waterBucketUses--;
                    points++;
                } 
                else if (collidable->tag == "Cactus" && health > 0) {
                    health--;
                    fmt::print("Ouch that hurt, you LOST health");
                    
                }
                else if (collidable->tag == "Soldier") {
                    if (points >= 100) {
                        points -= 100;
                        buffedPlayer = true;
                    }
                }

            }
            
        }

        // Update the player's position if no collision is detected
        if (!collisionDetected)
        {
            playerPosition = potentialPosition;
        }

        // Update player rectangle's position for drawing
        playerRect.x = playerPosition.x;
        playerRect.y = playerPosition.y;

        timeCounter += GetFrameTime();
        if (timeCounter >= frameChangeRate) {
            timeCounter = 0.0f;
            currentFrame++;
        }

        if (currentState == IDLE) {
            if (currentFrame >= NUM_FRAMES_IDLE) currentFrame = 0;
        }
        else if (currentState == RUNUp) {
            if (currentFrame >= NUM_FRAMES_RUNUP) currentFrame = 0;
        }
        else if (currentState == RUNDown) {
            if (currentFrame >= NUM_FRAMES_RUNDOWN) currentFrame = 0;
        }
        else if (currentState == RUNLeft) {
            if (currentFrame >= NUM_FRAMES_RUNRIGHT) currentFrame = 0;
        }
        else if (currentState == RUNRight) {
            if (currentFrame >= NUM_FRAMES_RUNLEFT) currentFrame = 0;
        }


        BeginDrawing();
        ClearBackground(WHITE);

        // Draw the map layers
        for (auto& layer : map->getLayers())
        {
            if (layer.getType() == tson::LayerType::TileLayer)
            {
                // Iterate through the tile objects in the layer
                for (auto& [pos, tileObject] : layer.getTileObjects())
                {
                    tson::Rect drawingRect = tileObject.getDrawingRect();

                    // Calculate the destination rectangle for drawing the tile
                    Rectangle destRect = {
                        (float)tileObject.getPosition().x * scaleFactor,
                        (float)tileObject.getPosition().y * scaleFactor,
                        (float)map->getTileSize().x * scaleFactor,
                        (float)map->getTileSize().y * scaleFactor
                    };



                    // Calculate the source rectangle for the tile in the tileset texture
                    Rectangle sourceRect = {
                        (float)drawingRect.x,
                        (float)drawingRect.y,
                        (float)drawingRect.width,
                        (float)drawingRect.height
                    };

                    // Draw the tile
                    DrawTexturePro(tilesetTexture, sourceRect, destRect, Vector2{ 0, 0 }, 0.0f, WHITE);
                }
            }
        }
        npc.draw();
        DrawUI();
        CheckPlayerDead();
        CheckPlayerIsBuffed();
        //draw the player
        switch (currentState) {
        case IDLE:
            DrawTextureEx(idleFrames[currentFrame], playerPosition, 0, 1.5f, WHITE);
            break;
        case RUNDown:
            DrawTextureEx(runDownFrames[currentFrame], playerPosition, 0, 1.5f, WHITE);
            break;
        case RUNUp:
            DrawTextureEx(runUpFrames[currentFrame], playerPosition, 0, 1.5f, WHITE);
            break;
        case RUNLeft:
            DrawTextureEx(runLeftFrames[currentFrame], playerPosition, 0, 1.5f, WHITE);
            break;
        case RUNRight:
            DrawTextureEx(runRightFrames[currentFrame], playerPosition, 0, 1.5f, WHITE);
            break;
        }


        EndDrawing();
    }

    // Unload the tileset texture
    UnloadTexture(tilesetTexture);
    UnloadAnimations();
    // Close the Raylib window
    CloseWindow();

    return 0;
}

#pragma endregion Main