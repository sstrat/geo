#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iomanip>
#include <sstream>

#include "shader_s.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "depend.h"
#include "compgeo.h"
#include "ui.h"


using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
//void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

// settings

const int buttonLength = 200;



/// Holds all state information relevant to a character as loaded using FreeType


bool isTwoOrFive = false;
float add = 2;
double unit = 120;
float nrunit = 1;
int divide = 4;
int count = 0;

void putUnits(Shader& shader, float x, float y, double unit, float scale) {
    float numberUnit = 0;
    int precision = 0;
   
    std::stringstream stream;
    std::string nrString;
    
    
    stream << std::fixed << std::setprecision(precision) << numberUnit;
    nrString = stream.str();
    RenderText(shader, nrString, x + textWidth(nrString, scale) / 2, y + 5, scale, glm::vec3(0, 0, 0));
    numberUnit += add;
    stream.str("");

    if ((int)add != (float)add) {
        float addCopy = add;
        while ((int)addCopy != (float)addCopy) {

            precision++;
            addCopy *= 10;
        }
    }

    numberUnit = add;
    for (float index = x + unit; index <= SCREEN_WIDTH; index += unit) {
        stream << std::fixed << std::setprecision(precision) << numberUnit;
        nrString = stream.str();
        
        RenderText(shader, nrString, index - textWidth(nrString,scale)/2, y+5, scale, glm::vec3(0, 0, 0));
        numberUnit += add;
        stream.str("");

    }
    numberUnit = add;
    for (float index = x - unit; index >= 0; index -= unit) {
        stream << std::fixed << std::setprecision(precision) << -numberUnit;
        nrString = stream.str();
        RenderText(shader, nrString, index - (textWidth(nrString, scale) + textWidth("-", scale)) / 2, y+5, scale, glm::vec3(0, 0, 0));
        numberUnit += add;
        stream.str("");
    }
    numberUnit = add;
    for (float index = y + unit; index <= SCREEN_HEIGHT; index += unit) {
        stream << std::fixed << std::setprecision(precision) << numberUnit;
        nrString = stream.str();
        RenderText(shader, nrString, x+5, index - textHeight(nrString, scale) / 2, scale, glm::vec3(0, 0, 0));
        numberUnit += add;
        stream.str("");
    }
    numberUnit = add;
    for (float index = y - unit; index >= 0; index -= unit) {
        stream << std::fixed << std::setprecision(precision) << -numberUnit;
        nrString = stream.str();
        RenderText(shader, nrString, x+5, index - textHeight(nrString, scale) / 2, scale, glm::vec3(0, 0, 0));
        numberUnit += add;
        stream.str("");
    }
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    if (unit < 90) {
        isTwoOrFive = !isTwoOrFive;
        unit = 120;

        if (isTwoOrFive) {
            add = 5;
            divide = 5;
            
        }
        else {
            add = 2;
            divide = 4;
            nrunit *= 10;
        }
        add *= nrunit;;
    }

    if (unit > 200) {
        isTwoOrFive = !isTwoOrFive;
        unit = 120;

        if (isTwoOrFive) {
            add = 5;
            divide = 5;
            nrunit /= 10;
        }
        else {
            add = 2;
            divide = 4;
        }
        add *= nrunit;

    }

    if (yoffset > 0) {
        // "scrolled up"
        unit -= divide;
    }
    else {
       //  "scrolled down"
        unit += divide;
        
    }
}

vector<Point> makeRaandomPoints(vector<Point> points, int n) {
    srand(time(0));
    Point p;
    for (int index = 0; index < n; index++) {
        if (rand() % 2 == 0) {
            p.x = (float)rand() / (float)(RAND_MAX / 10);
        }
        else {
            p.x = -(float)rand() / (float)(RAND_MAX / 10);
        }
        if (rand() % 2 == 0) {
            p.y = (float)rand() / (float)(RAND_MAX / 10);
        }
        else {
            p.y = -(float)rand() / (float)(RAND_MAX / 10);
        }
        points.push_back(p);
    }
    return points;
}

void displayPoints(vector<Point> points, Point origin, int n, Color color) {
   
    for (int index = 0; index < n; index++) {
     
        putPoint(euclidToGl(points[index],origin,add,unit), 10, color);
    }
}
int main() {

    

    cout << GetSystemMetrics(SM_CXSCREEN) << " " << GetSystemMetrics(SM_CYSCREEN)<<"\n";

    // glfw: initialize and configure
    // ------------------------------
    
    glfwInit();
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------

    

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ComputationalGeometryGUI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // OpenGL state
    // ------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // compile and setup the shader
    // ----------------------------
    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way
    
    Shader shader("vtext.txt", "ftext.txt");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // configure VAO/VBO for texture quads
    // -----------------------------------
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glOrtho(0.f, SCREEN_WIDTH, 0.f, SCREEN_HEIGHT, 0.f, 1.f);

    GLfloat x = SCREEN_WIDTH / 2, y = SCREEN_HEIGHT / 2;
    double xpos = 0, ypos = 0, curentx = 0, curenty = 0, originx = 0, originy = 0;
    bool isPressed = false;
    int whatToDraw = 0;
    int drawState = -1;

    Point origin = { 0 }, curent = { 0 }, pos = { 0 },o;
    o.x = SCREEN_WIDTH / 2;
    o.y = SCREEN_HEIGHT / 2;
    Color color = {1,0,0};

    
    vector<Button> buttons = makeButtons(100, 50);
    vector<Point> points /*= makeRaandomPoints(points, 5)*/;
    vector<Point> hull;
    vector<Shape> shapes;
    vector<vector<float>> mat;
    vector<int> path;
    vector<Color> colors = makeColorPalette();
 
    // render loop
    // -----------
 
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        //processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        glfwSetScrollCallback(window, scroll_callback);

        if (state == GLFW_PRESS) {
            glfwGetCursorPos(window, &pos.x, &pos.y);

            pos.y = SCREEN_HEIGHT - pos.y;

            if (isPressed == false) {
                origin.x = o.x;
                origin.y = o.y;
                curent.x = pos.x;
                curent.y = pos.y;
                isPressed = true;

                if (drawState != -1) {
                    if (buttonPress(buttons, pos) == -1) {
                        drawingState(drawState, points, shapes, pos, o, add, unit);
                        //cout << points[points.size() - 1].x << " , " << points[points.size() - 1].y << " ;";
                    }
                }

                if (buttonPress(buttons, pos) != -1) {

                    if (buttonPress(buttons, pos) == 3) {
                        visibiltyGraphButton(mat, points, shapes, origin, add, unit);
                        path = dijkstra(mat, 0);
                    }
                    else if (buttonPress(buttons, pos) == 2) {
                        clear(points,hull,shapes,mat, path);
                        whatToDraw = 0;
                    }
                    else if (buttonPress(buttons, pos) == 4) {
                        if (points.size() >= 2) {
                            hull = convexHull(points);
                        }
                        whatToDraw = 3;
                    }
                    else if (buttonPress(buttons, pos) == 5) {
                        whatToDraw = 1;
                    }
                   
                    else if (buttonPress(buttons, pos) == 6) {
                        whatToDraw = 2;
                    }
                    else if (buttonPress(buttons, pos) == 7) {
                        whatToDraw = 4;
                    }
                    else if (buttonPress(buttons, pos) == 8) {
                        whatToDraw = 5;
                    }
                    drawState = buttonPress(buttons, pos);
                    hoverButton( colors,buttons[drawState],drawState, 1);
                    if (buttonPress(buttons, pos) == 1) {
                        Shape shape;
                        shapes.push_back(shape);
                    }

                }
                
            }

            o.x = origin.x + (pos.x - curent.x);
            o.y = origin.y + (pos.y - curent.y);

        }
        else if (state == GLFW_RELEASE) {
            isPressed = false;
            if (drawState != -1) {
                hoverButton(colors, buttons[drawState], drawState, 0);
            }
        }
        
        glUseProgramObjectARB(0);
        
        
        makeAxis(o, unit, divide);
        putUnits(shader, o.x, o.y, unit, 0.5);
        
        drawShapes(shapes, o, add, unit, colors[4], colors[5], 4);
        
        if (mat.size() > 0) {
            for (int i = 0; i < nrPointsOnMap(shapes); i++) {
                for (int j = 0; j < nrPointsOnMap(shapes); j++) {
                    if (mat[i][j] > 0 && mat[i][j] != INT_MAX) {
                        //cout << matrixToSegment(shapes, i, j).p1.x << " " << matrixToSegment(shapes, i, j).p1.y << " , " << matrixToSegment(shapes, i, j).p2.x << " " << matrixToSegment(shapes, i, j).p2.y << " : ";
                        line(euclidToGl(matrixToSegment(shapes, i, j).p1, o, add, unit), euclidToGl(matrixToSegment(shapes, i, j).p2, o, add, unit), colors[6], 2);
                    }
                }
                //cout << "\n";
            }
            
            for (int index = path.size()-1; index > 0; index--) {
                line(euclidToGl(matrixToSegment(shapes, path[index], path[index-1]).p1, o, add, unit), euclidToGl(matrixToSegment(shapes, path[index], path[index - 1]).p2, o, add, unit), colors[0], 6);
               
            }

         
        }
        if (hull.size() > 0) {
            if (points.size() >= 2) {
                drawConvexHull(hull, o, add, unit, colors[4], 5);
            }
        }
        
        
        displayPoints(points, o, points.size(), color);
        if (whatToDraw == 1) {
            if (shapes.size() > 0 && shapes[0].points.size() >= 3) {
                //drawInscribedCircle(shapes[0], o, add, unit, colors[6], 3);
                drawPointsInDomain(points, shapes[0], o, add, unit, colors[0]);
            }
        }
        else if (whatToDraw == 2) {
            if (points.size() >= 2) {
                drawShortestDistance(shader, points, o, add, unit, colors[4], 4);
            }
        }
        else if (whatToDraw == 4) {
            if (shapes.size() > 0 && shapes[0].points.size() >= 3) {
                dysplayAreaOfShape(shader, shapes[0]);
            }
        }
        else if (whatToDraw == 5) {
            if (shapes.size() > 0 && shapes[0].points.size() >= 3) {
                drawInscribedCircle(shapes[0], o, add, unit, colors[6], 3);
               
            }
        }
        //convexHull(points,o,add,unit, colors[4], 4);
        
        drawButtons(buttons, shader);
        
        //drawingState(drawState, points, shapes, pos, o, add, unit);
        
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// render line of text
// -------------------
