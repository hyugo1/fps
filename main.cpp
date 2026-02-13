//g++ -std=c++11 main.cpp -lncurses -o main
// ./main
#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <cmath>
#include <locale.h>
#include <vector>
#include <algorithm>

using namespace std;

int screenWidth = 120;
int screenHeight = 40;

float PlayerX = 8.0f;
float PlayerY = 8.0f;
float PlayerA = 0.0f;

int mapWidth = 16;
int mapHeight = 16;

float FOV = 3.14159f / 4.0f;
float depth = 16.0f;


int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    setlocale(LC_ALL, "");
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h\n");

    //create screen buffer
    wchar_t *screen = new wchar_t[screenWidth * screenHeight];
    
    // Wolfenstein Map    
    string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..........#...#";
    map += "#..........#...#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#........#######";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

   //game loop
    while (1) {       
        //timing 
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count(); 

        int ch = getch();
        
        // mouse movement
        if (ch == KEY_MOUSE) {
            MEVENT event;
            if (getmouse(&event) == OK) {
                static int lastMouseX = screenWidth / 2;
                static bool firstMouse = true;
                
                if (firstMouse) {
                    lastMouseX = event.x;
                    firstMouse = false;
                }
                int deltaX = event.x - lastMouseX;
                // Adjust sensitivity here
                PlayerA += deltaX * 0.05f; 
                lastMouseX = event.x;
            }
        }
        
        
        // key controls
        if (ch == 'a' || ch == 'A')
        PlayerA -= (0.8f * fElapsedTime);
        if (ch == 'd' || ch == 'D')
            PlayerA += (0.8f * fElapsedTime);
        if (ch == 'w' || ch == 'W') {
            PlayerX += sinf(PlayerA) * 5.0f * fElapsedTime;
            PlayerY += cosf(PlayerA) * 5.0f * fElapsedTime;
            
            //collision detection
            if (map[(int)PlayerY * mapWidth + (int)PlayerX] == '#') {
                PlayerX -= sinf(PlayerA) * 5.0f * fElapsedTime;
                PlayerY -= cosf(PlayerA) * 5.0f * fElapsedTime;
            }

        }
        if (ch == 's' || ch == 'S') {
            PlayerX -= sinf(PlayerA) * 5.0f * fElapsedTime;
            PlayerY -= cosf(PlayerA) * 5.0f * fElapsedTime;

            // Collision detection
            if (map[(int)PlayerY * mapWidth + (int)PlayerX] == '#') {
                PlayerX += sinf(PlayerA) * 5.0f * fElapsedTime;
                PlayerY += cosf(PlayerA) * 5.0f * fElapsedTime;
            }
        }
        if (ch == 'q' || ch == 'Q')
            break;

        for (int x = 0; x < screenWidth; x++) {
            float RayAngle = (PlayerA - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV;

            //raytracing
            float DistanceToWall = 0;
            bool hitWall = false;
            bool boundary = false;

            float eyeX = sinf(RayAngle);
            float eyeY = cosf(RayAngle);
            
            while (!hitWall && DistanceToWall < depth) {
                DistanceToWall += 0.1f;

                int testX = (int)(PlayerX + eyeX * DistanceToWall);
                int testY = (int)(PlayerY + eyeY * DistanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight) {
                    hitWall = true;
                    DistanceToWall = depth;
                }
                else {
                    //ray is inbounds so test to see if the ray cell is a wall block
                    if (map[testY * mapWidth + testX] == '#') {
                        hitWall = true;

                        // Boundary detection
                        // distance, dot product
                        vector<pair<float, float>> p;
                        for (int tx = 0; tx < 2; tx++) {
                            for (int ty = 0; ty < 2; ty++) {
                                float vx = (float)testX + tx - PlayerX;
                                float vy = (float)testY + ty - PlayerY;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (eyeX * vx / d) + (eyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }
                        }
                        sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {
                            return left.first < right.first;
                    });
                        float bound = 0.01f;
                        if (acos(p.at(0).second) < bound) boundary = true;
                        if (acos(p.at(1).second) < bound) boundary = true;
                        
                    }
                }
            }
            //calc distance to ceiling and floor
            int ceiling = (int)((float)(screenHeight / 2.0) - screenHeight / ((float)DistanceToWall));
            int floor = screenHeight - ceiling;

            // wchar_t shade = ' ';
            // if (DistanceToWall <= depth / 4.0f)       shade = ACS_BLOCK;    // Solid block
            // else if (DistanceToWall < depth / 3.0f)   shade = ACS_BOARD;    // Board of squares
            // else if (DistanceToWall < depth / 2.0f)   shade = ACS_CKBOARD;  // Checker board
            // else if (DistanceToWall < depth)          shade = ACS_BULLET;   // Bullet
            // else                                      shade = ' ';

            // const char* shade = " ";
            // if (DistanceToWall <= depth / 4.0f)       shade = "█";  // U+2588
            // else if (DistanceToWall < depth / 3.0f)   shade = "▓";  // U+2593
            // else if (DistanceToWall < depth / 2.0f)   shade = "▒";  // U+2592
            // else if (DistanceToWall < depth)          shade = "░";  // U+2591
            // else                                      shade = " ";
            
            // const char* shade = " ";
            // if (DistanceToWall <= depth / 4.0f)       shade = "#";
            // else if (DistanceToWall < depth / 3.0f)   shade = "x";
            // else if (DistanceToWall < depth / 2.0f)   shade = ".";
            // else if (DistanceToWall < depth)          shade = "-";
            // else                                      shade = " ";

            wchar_t shade = L' ';
            int shadeIndex = std::min(5, std::max(0, (int)((DistanceToWall / depth) * 5.0f)));
            const wchar_t shades[] = { L'#', L'O', L'o', L'.', L'-', L' ' };
            shade = shades[shadeIndex];

            if (boundary) shade = L' ';
            // if (boundary) shade = 'I';

            for (int y = 0; y < screenHeight; y++) {
                if (y < ceiling) {
                    screen[y * screenWidth + x] = ' ';
                }
                else if (y > ceiling && y <= floor) {
                    // screen[y * screenWidth + x] = shade[0];
                    screen[y * screenWidth + x] = shade;
                }
                else {
                    // Floor
                    wchar_t floorShade = ' ';
                    float b = 1.0f - (((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f));
                    if (b < 0.25)       floorShade = '#';
                    else if (b < 0.5)   floorShade = 'x';
                    else if (b < 0.75)  floorShade = '.';
                    else if (b < 0.9)   floorShade = '-';
                    else                floorShade = ' ';
                    screen[y * screenWidth + x] = floorShade;
                }
            }
        }
        
        clear();
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                mvaddch(y, x, screen[y * screenWidth + x]);
            }
        }

        // display stats
        mvprintw(0, 0, "X=%3.2f Y=%3.2f A=%3.2f FPS=%3.2f ", PlayerX, PlayerY, PlayerA, 1.0f / fElapsedTime);

        // display map
        for (int nx = 0; nx < mapWidth; nx++) {
            for (int ny = 0; ny < mapHeight; ny++) {
                mvaddch(ny + 1, nx, map[ny * mapWidth + nx]);
            }
        }

        // Add player marker on the map
        // mvaddch((int)PlayerY + 1, (int)PlayerX, 'P');        
       
        mvaddch((int)PlayerY + 1, (int)PlayerX, 'P');
        // Show where player is looking
        int lookX = (int)(PlayerX + sinf(PlayerA) * 1.0f);
        int lookY = (int)(PlayerY + cosf(PlayerA) * 1.0f);
        if (lookX >= 0 && lookX < mapWidth && lookY >= 0 && lookY < mapHeight) {
            mvaddch(lookY + 1, lookX, '*');
        }
       
        // Show player with directional arrow
        // wchar_t playerChar = 'P';
        // float angle = PlayerA;
        // // Normalize angle to 0-2π
        // while (angle < 0) angle += 2 * 3.14159f;
        // while (angle >= 2 * 3.14159f) angle -= 2 * 3.14159f;

        // // Choose arrow based on angle (8 directions)
        // if (angle < 3.14159f / 8 || angle >= 15 * 3.14159f / 8)      playerChar = '^';
        // else if (angle < 3 * 3.14159f / 8)                           playerChar = '/';
        // else if (angle < 5 * 3.14159f / 8)                           playerChar = '>';
        // else if (angle < 7 * 3.14159f / 8)                           playerChar = '\\';
        // else if (angle < 9 * 3.14159f / 8)                           playerChar = 'v';
        // else if (angle < 11 * 3.14159f / 8)                          playerChar = '/';
        // else if (angle < 13 * 3.14159f / 8)                          playerChar = '<';
        // else                                                         playerChar = '\\';

        // mvaddch((int)PlayerY + 1, (int)PlayerX, playerChar);

        refresh();
        napms(16);
    }
    
    printf("\033[?1003l\n");
    endwin();
    delete[] screen;
    
    return 0;
}