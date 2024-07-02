#include <GL/glut.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>

int startTime;
const int WIDTH = 800;
const int HEIGHT = 600;
const int BOTTOM_HALF = HEIGHT / 2;

bool previousScreen = false;
bool nextScreen = false;

float lineSpacingY = 7.5;
float lineHeight = 2.0;
float lineSpacingX = 7.5;
float lineWidth = 2.0;

enum Screen {
    MENU_SCREEN,
    GAME_SCREEN,
    GAME_OVER_SCREEN,
    INSTRUCTIONS_SCREEN
};

enum FruitType {
    APPLE,
    PEAR,
    PINEAPPLE,
    ORANGE
};

enum ObjectType {
    FRUIT,
    BOMB
};

struct Object {
    float x, y;
    float speedY;
    bool sliced;
    bool goingUp;
    FruitType fruitType;
    ObjectType objType;
};

std::vector<Object> objects;
int fruitsSliced = 0;
Screen currentScreen = MENU_SCREEN;

void displayRasterText(float x, float y, float z, const char* stringToDisplay) {
    int length;
    glRasterPos3f(x, y, z);
    length = strlen(stringToDisplay);

    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, stringToDisplay[i]);
    }
}

void displayRasterText1(float x, float y, float z, const char* stringToDisplay) {
    int length;
    glRasterPos3f(x, y, z);
    length = strlen(stringToDisplay);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, stringToDisplay[i]);
    }
}

void resetTimer() {
    startTime = glutGet(GLUT_ELAPSED_TIME); // Set the start time to the current time
}

void init() {
    glClearColor(0.7, 0.9, 0.9, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    srand(time(NULL));
}

void drawObject(float x, float y, FruitType fruitType) {
    switch (fruitType) {
    case APPLE:
        glColor3f(1.0, 0.0, 0.0);
        // Draw first circle
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * 3.14159; angle += 3.14159 / 12) {
            glVertex2f(x + 15 * cos(angle) - 6, y + 15 * sin(angle));
        }
        glEnd();

        // Draw second circle
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * 3.14159; angle += 3.14159 / 12) {
            glVertex2f(x + 15 * cos(angle) + 6, y + 15 * sin(angle));
        }
        glEnd();

        // Draw the stem
        glColor3f(0.0, 0.5, 0.0);
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex2f(x, y + 10);
        glVertex2f(x, y + 20);
        glEnd();
        break;
    case PEAR:
        glColor3f(2.01, 2.04, 0.63);
        // Draw upper circle
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * 3.14159; angle += 3.14159 / 12) {
            glVertex2f(x + 10 * cos(angle), y + 15 * sin(angle) + 6); // Shift y-coordinate by 10 to place it above the center
        }
        glEnd();

        // Draw lower circle
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * 3.14159; angle += 3.14159 / 12) {
            glVertex2f(x + 15 * cos(angle), y + 15 * sin(angle) - 6); // Shift y-coordinate by 10 to place it below the center
        }
        glEnd();

        // Draw the stem
        glColor3f(0.0, 0.5, 0.0);
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex2f(x, y + 15);
        glVertex2f(x, y + 25);
        glEnd();
        break;

    case PINEAPPLE:
        glColor3f(1.0, 1.0, 0.0);
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * 3.14159; angle += 3.14159 / 12) {
            glVertex2f(x + 20 * cos(angle), y + 30 * sin(angle));
        }
        glEnd();

        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2.0);

        // Draw horizontal lines
        for (float posY = y - 20; posY < y + 20; posY += lineSpacingY) {
            glBegin(GL_LINES);
            //Coordinates to stay within oval of pineapple
            glVertex2f(x - 15, posY);
            glVertex2f(x + 15, posY);
            glEnd();
        }
        // Draw vertical lines
        for (float posX = x - 10; posX < x + 10; posX += lineSpacingX) {
            glBegin(GL_LINES);
            //Coordinates to stay within oval of pineapple
            glVertex2f(posX, y - 20);
            glVertex2f(posX, y + 20);
            glEnd();
        }
        glLineWidth(1.0);

        // Draw pineapple leaves
        glColor3f(0.0, 0.5, 0.0);

        // First leaf
        glBegin(GL_TRIANGLES);
        glVertex2f(x, y + 30); // Top vertex
        glVertex2f(x - 10, y + 40); // Bottom left vertex
        glVertex2f(x + 10, y + 40); // Bottom right vertex
        glEnd();

        // Second leaf
        glBegin(GL_TRIANGLES);
        glVertex2f(x, y + 40); // Top vertex
        glVertex2f(x - 10, y + 50); // Bottom left vertex
        glVertex2f(x + 10, y + 50); // Bottom right vertex
        glEnd();
        break;
    case ORANGE:
        glColor3f(1.0, 0.647, 0.0);
        float radius = 20.0;
        int numSegments = 8;

        // Draw the main body of the orange as a circle
        glBegin(GL_POLYGON);
        for (int i = 0; i < numSegments; ++i) {
            float angle = i * (2 * M_PI / numSegments);
            float xPos = x + radius * cos(angle);
            float yPos = y + radius * sin(angle);
            glVertex2f(xPos, yPos);
        }
        glEnd();

        // Draw segments using triangles
        glColor3f(1.0, 0.5, 0.0);
        for (int i = 0; i < numSegments; ++i) {
            glBegin(GL_TRIANGLES);
            float angle1 = i * (2 * M_PI / numSegments);
            float angle2 = (i + 1) * (2 * M_PI / numSegments);
            float xPos1 = x + radius * cos(angle1);
            float yPos1 = y + radius * sin(angle1);
            float xPos2 = x + radius * cos(angle2);
            float yPos2 = y + radius * sin(angle2);
            float midX = x + 0.5 * radius * cos((angle1 + angle2) / 2);
            float midY = y + 0.5 * radius * sin((angle1 + angle2) / 2);
            glVertex2f(xPos1, yPos1);
            glVertex2f(xPos2, yPos2);
            glVertex2f(midX, midY);
            glEnd();
        }
        break;
    }
}

void drawBomb(float x, float y) {
    // Draw rope
    glColor3f(0.4, 0.4, 0.4);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x, y + 15);
    glVertex2f(x, y + 30);
    glEnd();

    // Draw bomb body
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 6) {
        glVertex2f(x + 15 * cos(angle), y + 15 * sin(angle));
    }
    glEnd();

    // Draw cross on bomb body
    glColor3f(0.5, 0.5, 0.5);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x - 5, y - 5);
    glVertex2f(x + 5, y + 5);
    glVertex2f(x + 5, y - 5);
    glVertex2f(x - 5, y + 5);
    glEnd();

    // Draw spark
    glColor3f(1.0, 0.8, 0.0); // Yellow color for spark
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x - 5, y + 30); // Start point of spark line 1
    glVertex2f(x + 5, y + 30); // End point of spark line 1
    glVertex2f(x, y + 25); // Start point of spark line 2
    glVertex2f(x, y + 35); // End point of spark line 2
    glVertex2f(x - 5, y + 35); // Start point of spark line 3
    glVertex2f(x + 5, y + 25); // End point of spark line 3
    glVertex2f(x - 5, y + 25); // Start point of spark line 4
    glVertex2f(x + 5, y + 35); // End point of spark line 4
    glEnd();
}

void displayGameScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw background
    glBegin(GL_QUADS);
    glColor3ub(0xAD, 0xD8, 0xE6);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    // Display objects
    for (const Object& obj : objects) {
        if (!obj.sliced) {
            if (obj.objType == FRUIT) {
                drawObject(obj.x, obj.y, obj.fruitType);
            }
            else {
                drawBomb(obj.x, obj.y);
            }
        }
    }

    glColor3f(0, 0, 0);
    glRasterPos2f(10, HEIGHT - 20);
    std::string slicedStr = "Fruits Sliced: " + std::to_string(fruitsSliced);
    for (char c : slicedStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glRasterPos2f(10, HEIGHT - 45);
    std::string scoreStr = "Score: " + std::to_string(3 * fruitsSliced);
    for (char c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int elapsedTime = (currentTime - startTime) / 1000;
    int remainingSeconds = 45 - elapsedTime;
    glPushMatrix();
    glTranslatef(WIDTH - 240, HEIGHT - 30, 0);
    float scale = 0.2;
    glScalef(scale, scale, 1.0);
    if (remainingSeconds >= 10) {
        std::string timerStr = "Time Left: 00:" + std::to_string(remainingSeconds);
        for (char c : timerStr) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
        }
    }
    else if (remainingSeconds < 10) {
        std::string timerStr = "Time Left: 00:0" + std::to_string(remainingSeconds);
        for (char c : timerStr) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
        }
    }
    if (remainingSeconds == 0) {
        currentScreen = GAME_OVER_SCREEN;
    }
    glPopMatrix();

    glutSwapBuffers();
}

void displayMenuScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    float buttonWidth = WIDTH / 2.5;
    float buttonHeight = HEIGHT / 12;
    float buttonMargin = HEIGHT / 20;
    float borderMargin = HEIGHT / 30;
    float footerMargin = HEIGHT / 20;
    float middleY = HEIGHT / 2;

    // Draw border
    glLineWidth(10);
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(borderMargin, borderMargin, 0.5);
    glVertex3f(borderMargin, HEIGHT - borderMargin, 0.5);
    glVertex3f(WIDTH - borderMargin, HEIGHT - borderMargin, 0.5);
    glVertex3f(WIDTH - borderMargin, borderMargin, 0.5);
    glEnd();
    glLineWidth(1);

    // Draw start game button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY + buttonMargin + buttonHeight + 100, 0.5);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY + buttonMargin + 100, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY + buttonMargin + 100, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY + buttonMargin + buttonHeight + 100, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText((WIDTH - 120) / 2, middleY + buttonMargin + (buttonHeight / 2) - 10 + 100, 0.4, "Start Game");

    // Draw instructions button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY + 100, 0.5);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY - buttonHeight + 100, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY - buttonHeight + 100, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY + 100, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText((WIDTH - 140) / 2, middleY - (buttonHeight / 2) + 90, 0.4, "Instructions");

    // Draw quit button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY - buttonMargin - buttonHeight + 50, 0.5);
    glVertex3f((WIDTH - buttonWidth) / 2, middleY - buttonMargin + 50, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY - buttonMargin + 50, 0.5);
    glVertex3f((WIDTH + buttonWidth) / 2, middleY - buttonMargin - buttonHeight + 50, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText((WIDTH - 60) / 2, middleY - buttonMargin - (buttonHeight / 2) + 40, 0.4, "Quit");

    // Draw footer text
    glColor3f(0, 0, 0);
    displayRasterText1(borderMargin + 175, footerMargin + 70, 0, "Developed By: Ishika Garg(102103581)     Aayushi Bansal(102103578)");
    displayRasterText1(borderMargin + 300, footerMargin + 90, 0, "Computer Graphics Project");
    displayRasterText1(borderMargin + 300, footerMargin + 110, 0, "Submitted to: Diksha Arora");

    glutSwapBuffers();
}

void displayGameOverScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    float borderMargin = HEIGHT / 30;

    // Draw game over text
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(WIDTH / 2 - 75, HEIGHT / 2 + 150);
    std::string gameOverStr = "GAME OVER";
    for (char c : gameOverStr) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    //Draw border
    glLineWidth(10);
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(borderMargin, borderMargin, 0.5);
    glVertex3f(borderMargin, HEIGHT - borderMargin, 0.5);
    glVertex3f(WIDTH - borderMargin, HEIGHT - borderMargin, 0.5);
    glVertex3f(WIDTH - borderMargin, borderMargin, 0.5);
    glEnd();
    glLineWidth(1);

    // Display score
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(WIDTH / 2 - 40, HEIGHT / 2 + 70);
    std::string scoreStr = "Score: " + std::to_string(3 * fruitsSliced);
    for (char c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Draw restart button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f((WIDTH - 100) / 2, HEIGHT / 4 + 120, 0.5);
    glVertex3f((WIDTH - 100) / 2, HEIGHT / 4 + 170, 0.5);
    glVertex3f((WIDTH + 100) / 2, HEIGHT / 4 + 170, 0.5);
    glVertex3f((WIDTH + 100) / 2, HEIGHT / 4 + 120, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText((WIDTH - 80) / 2, HEIGHT / 4 + 140, 0.4, "Restart");

    // Draw quit button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f((WIDTH - 100) / 2, HEIGHT / 4 + 20, 0.5);
    glVertex3f((WIDTH - 100) / 2, HEIGHT / 4 + 70, 0.5);
    glVertex3f((WIDTH + 100) / 2, HEIGHT / 4 + 70, 0.5);
    glVertex3f((WIDTH + 100) / 2, HEIGHT / 4 + 20, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText((WIDTH - 60) / 2, HEIGHT / 4 + 40, 0.4, "Quit");

    glutSwapBuffers();
}

void backButton() {
    // Draw back button
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f(690, 50, 0.5);
    glVertex3f(690, 100, 0.5);
    glVertex3f(760, 100, 0.5);
    glVertex3f(760, 50, 0.5);
    glEnd();
    glColor3f(0, 0, 0);
    displayRasterText(700, 70, 0.4, "Back");
}

void InstructionsScreenDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0, 0, 0);
    glRasterPos2f(15, 500);
    std::string slicedStr = "INSTRUCTIONS";
    for (char c : slicedStr) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    displayRasterText(20, 400, 0.4, "-> Use left mouse click to slice the fruits.");
    displayRasterText(20, 350, 0.4, "-> You will get 3 points for slicing each fruit.");
    displayRasterText(20, 300, 0.4, "-> You need to dodge the bombs.");
    displayRasterText(20, 250, 0.4, "-> If you slice the bombs the game will be over.");
    displayRasterText(20, 200, 0.4, "-> You will get 45 seconds of game play.");
    displayRasterText(20, 150, 0.4, "-> The objective is to maximize the score.");
    backButton();
    if (previousScreen)
        nextScreen = false, previousScreen = false; //as set by backButton()
    glutSwapBuffers();
}

void display() {
    switch (currentScreen) {
    case GAME_SCREEN:
        displayGameScreen();
        break;
    case MENU_SCREEN:
        displayMenuScreen();
        break;
    case GAME_OVER_SCREEN:
        displayGameOverScreen();
        break;
    case INSTRUCTIONS_SCREEN:
        InstructionsScreenDisplay();
        break;
    }
}

void addObjects(size_t count, ObjectType objType) {
    for (size_t i = 0; i < count; ++i) {
        Object obj;
        obj.x = rand() % WIDTH;
        obj.y = 0;
        obj.speedY = (rand() % 3) + 2; // Randomly select speed
        obj.sliced = false;
        obj.goingUp = true;
        obj.objType = objType;
        if (objType == FRUIT) {
            obj.fruitType = static_cast<FruitType>(rand() % 4); // Randomly select fruit type
        }
        objects.push_back(obj);
    }
}

void update(int value) {
    // Remove sliced or disappeared objects
    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [&](const Object& obj) {
            return obj.sliced || obj.y < 0;
        }), objects.end());

    // Move existing objects
    for (Object& obj : objects) {
        if (obj.goingUp) {
            obj.y += obj.speedY;
            if (obj.y > BOTTOM_HALF) {
                obj.goingUp = false;
            }
        }
        else {
            obj.y -= obj.speedY;
        }
    }

    // Randomly decide whether to add new objects based on a more controlled random factor
    static int objectTimer = 0; // Counter to control object addition
    objectTimer++;
    if (objectTimer > (30 + rand() % 70)) {  // Random interval between 30 and 70 updates
        int randNum = rand() % 3; // 3 possible object types (0 to 2)

        if (randNum == 0) { // 1 in 4 chance of adding a bomb
            addObjects(1, BOMB);
        }
        else {
            addObjects(rand() % 2 + 1, FRUIT); // Add 1 to 2 fruits
        }

        objectTimer = 0;  // Reset the timer
    }

    glutPostRedisplay();
    glutTimerFunc(50, update, 0);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = HEIGHT - y;
        for (Object& obj : objects) {
            if (!obj.sliced && x >= obj.x - 20 && x <= obj.x + 20 && y >= obj.y - 20 && y <= obj.y + 20) {
                obj.sliced = true;
                if (obj.objType != BOMB) {
                    fruitsSliced++;
                }
                else {
                    currentScreen = GAME_OVER_SCREEN;
                }
            }
        }
    }
}

void mouseClick(int button, int state, int x, int y) {
    float buttonWidth, buttonHeight, buttonMargin, middleY;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (currentScreen) {
        case GAME_SCREEN:
            mouse(button, state, x, y);
            break;
        case MENU_SCREEN:
            buttonWidth = WIDTH / 2.5;
            buttonHeight = HEIGHT / 12;
            buttonMargin = HEIGHT / 20;
            middleY = HEIGHT / 2;

            // Check if the click is within the quit button area
            if (x >= (WIDTH - buttonWidth) / 2 && x <= (WIDTH + buttonWidth) / 2 &&
                y >= middleY - 20 && y <= middleY + buttonHeight - 20) {
                exit(0); // Quit the game
            }

            // Check if the click is within the start button area
            else if (x >= (WIDTH - buttonWidth) / 2 && x <= (WIDTH + buttonWidth) / 2 &&
                y >= middleY - 180 && y <= middleY + buttonHeight - 180) {
                currentScreen = GAME_SCREEN;
            }

            // Check if the click is within the instructions button area
            else if (x >= (WIDTH - buttonWidth) / 2 && x <= (WIDTH + buttonWidth) / 2 &&
                y >= middleY - 100 && y <= middleY + buttonHeight - 100) {
                currentScreen = INSTRUCTIONS_SCREEN;
            }
            break;
        case GAME_OVER_SCREEN:
            if (x >= (WIDTH - 100) / 2 && x <= (WIDTH + 100) / 2 &&
                y >= HEIGHT / 4 + 120 && y <= HEIGHT / 4 + 170) {
                // Reset game state
                fruitsSliced = 0;
                objects.clear();
                addObjects(3, FRUIT);
                resetTimer();
                currentScreen = GAME_SCREEN;
            }

            // Check if the click is within the quit button area
            else if (x >= (WIDTH - 100) / 2 && x <= (WIDTH + 100) / 2 &&
                y >= 380 && y <= 430) {
                exit(0);
            }
            break;
        case INSTRUCTIONS_SCREEN:
            // Check if the click is within the back button area
            if (x >= 690 && x <= 760 && y >= 500) {
                currentScreen = MENU_SCREEN;
            }
            break;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Fruit Ninja Clone");

    init();

    addObjects(3, FRUIT);  // Start with 3 fruits

    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}