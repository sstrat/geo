//#include "shader_s.h"
//#include "string.h"
//#include <GLFW/glfw3.h>

const unsigned int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const unsigned int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);



struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;

float textWidth(std::string text, float scale) {
    std::string::const_iterator c;
    float w = 0;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        w += ch.Size.x;
    }
    return scale * w;
}

float textHeight(std::string text, float scale) {
    std::string::const_iterator c;
    float h = 0;
    c = text.begin();
    Character ch = Characters[*c];
    h += ch.Size.y;

    return scale * h;
}

struct Color {
    double r, g, b;
};


Point glToEuclid(Point p, Point origin, float add, float unit) {

    if (p.x <= origin.x) {
        p.x = -add * (origin.x - p.x) / unit;
    }
    else {
        p.x = add * (p.x - origin.x) / unit;
    }

    if (p.y <= origin.y) {
        p.y = -add * (origin.y - p.y) / unit;
    }
    else {
        p.y = add * (p.y - origin.y) / unit;
    }
    return p;
}

Point euclidToGl(Point p, Point origin, float add, float unit) {

    p.x = (p.x * unit) / add + origin.x;
    p.y = (p.y * unit) / add + origin.y;

    return p;
}

//render text
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgramObjectARB(0);
}

void line(Point point1, Point point2, Color color, float thickness) {
    glColor3f(color.r, color.g, color.b);
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glVertex2f(point1.x, point1.y);
    glVertex2f(point2.x, point2.y);
    glEnd();
}

void triangle(Point p1, Point p2, Point p3, Color color){

    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLES);

    glVertex2f(p1.x,p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    
    glEnd();

}

Point centerOfShape(Shape shape) {
    Point point = { 0, 0 };
    for (int index = 0; index < shape.points.size(); index++) {
        point.x += shape.points[index].x;
        point.y += shape.points[index].y;
    }
    point.x /= shape.points.size();
    point.y /= shape.points.size();
    return point;
}

void drawShapes(vector<Shape> shapes, Point origin, float add, float unit, Color lineColor, Color fillColor ,float thickness) {
    for (int shape = 0; shape < shapes.size(); shape++) {
        if (shapes[shape].points.size() > 0) {
            for (int point = 0; point < shapes[shape].points.size() - 1; point++) {
                
                triangle(euclidToGl(shapes[shape].points[0], origin, add, unit), euclidToGl(shapes[shape].points[point+1], origin, add, unit), euclidToGl(shapes[shape].points[point], origin, add, unit), fillColor);
                line(euclidToGl(shapes[shape].points[point], origin, add, unit), euclidToGl(shapes[shape].points[point + 1], origin, add, unit), lineColor, thickness);
            }

            line(euclidToGl(shapes[shape].points[0], origin, add, unit), euclidToGl(shapes[shape].points[shapes[shape].points.size() - 1], origin, add, unit), lineColor, thickness);
        }
    }
}


void rectangle(double x1, double y1, double x2, double y2, Color color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, 0);
    glVertex3f(x2, y1, 0);
    glVertex3f(x1, y2, 0);
    glVertex3f(x2, y2, 0);
    glVertex3f(x1, y2, 0);
    glVertex3f(x2, y1, 0);
    glEnd();
}

struct Button {
    double x1, y1, x2, y2;
    std::string name;
    Color color;
    float scale;

    void putButton(Shader shader) {

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y1, 0);
        glVertex3f(x1, y2, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(x1, y2, 0);
        glVertex3f(x2, y1, 0);
        glEnd();

        RenderText(shader, name, (x1 + x2) / 2 - textWidth(name, scale) / 2, (y1 + y2) / 2 - textHeight(name, scale) / 2, scale, glm::vec3(0, 0, 0));
        glUseProgramObjectARB(0);
    }
};


int buttonPress(vector<Button> buttons, Point mouse) {

    for (int index = 0; index < buttons.size(); index++) {
        if (mouse.x > buttons[index].x1 && mouse.x < buttons[index].x2 && mouse.y > buttons[index].y1 && mouse.y < buttons[index].y2) {
            return index;
        }
    }
    return -1;

}

void clear(vector<Point> &points, vector<Point> &hull, vector<Shape> &shapes, vector<vector<float>> &graph, vector<int> &path) {

    points.clear();
    hull.clear();
    for (int index = 0; index < shapes.size(); index++) {
        shapes[index].points.clear();
    }
    shapes.clear();
    for (int index = 0; index < graph.size(); index++) {
        graph[index].clear();
    }
    graph.clear();
    path.clear();
}

void visibiltyGraphButton(vector<vector<float>> &mat, vector<Point> points, vector<Shape> &shapes, Point origin, float add, float unit) {
 
    makeVisibilityGraph(shapes, points[points.size()-1], points[points.size() - 2], mat, origin, add, unit);

}

void drawingState(int state, vector<Point> &points, vector<Shape> &shapes, Point p, Point origin, float add, float unit) {
    if (state == 0) {

        points.push_back(glToEuclid(p, origin, add, unit)); 
    }
    else if (state == 1) {
        
        shapes[shapes.size() - 1].points.push_back(glToEuclid(p, origin, add, unit));
    }
    
   
}

vector<Color> makeColorPalette() {

    vector<Color> colors;

    Color color; 
    //green
    color = { 87. / 255, 255. / 255, 54. / 255 };
    colors.push_back(color);
    color = { 25. / 255, 156. / 255, 44. / 255 };
    colors.push_back(color);
    //orange
    color = { 250. / 255, 110. / 255, 27. / 255 };
    colors.push_back(color);
    color = { 204. / 255, 89. / 255, 22. / 255 };
    colors.push_back(color);
    //blue
    color = { 14. / 255, 124. / 255, 143. / 255 };
    colors.push_back(color);
    color = { 24. / 255, 177. / 255, 250. / 255 };
    colors.push_back(color);
    //red
    color = { 250. / 255, 24. / 255, 17. / 255 };
    colors.push_back(color);
    color = { 0,0,0};
    colors.push_back(color);
    return colors;
}


vector<Button> makeButtons(int width, int height){ 
    vector<Button> buttons;
    Button button;
    vector<Color> colors = makeColorPalette();

    for (int index = 0; index < 2; index++) {
        button.x1 = SCREEN_WIDTH - width - 10;
        button.y1 = SCREEN_HEIGHT - (index + 1) * height - 10;
        button.x2 = SCREEN_WIDTH - 10;
        button.y2 = SCREEN_HEIGHT - index * height - 10 - 10;
        button.color = colors[0];
        button.name = to_string(index + 1);
        button.scale = 0.7;
        buttons.push_back(button);

    }

    button.x1 = SCREEN_WIDTH - width - 10;
    button.y1 = SCREEN_HEIGHT - 3 * height - 10;
    button.x2 = SCREEN_WIDTH - 10;
    button.y2 = SCREEN_HEIGHT - 2 * height - 10 - 10;
    button.color = colors[2];
    button.name = to_string(3);
    button.scale = 0.7;
    buttons.push_back(button);



    for (int index = 3; index < 9; index++) {
        button.x1 = SCREEN_WIDTH - width - 10;
        button.y1 = SCREEN_HEIGHT - (index + 1) * height - 10;
        button.x2 = SCREEN_WIDTH - 10;
        button.y2 = SCREEN_HEIGHT - index * height - 10 - 10;
        button.color = colors[5];
        button.name = to_string(index + 1);
        button.scale = 0.7;
        buttons.push_back(button);

    }
    buttons[0].name = "punct";
    buttons[1].name = "forma";
    buttons[2].name = "sterge";
    buttons[3].name = "cale";
    buttons[4].name = "hull";
    buttons[5].name = "apartin";
    buttons[5].scale = 0.6;
    buttons[6].name = "distanta";
    buttons[6].scale = 0.55;
    buttons[7].name = "aria";
    buttons[7].scale = 0.7;
    buttons[8].name = "cerc";
    buttons[8].scale = 0.7;


    return buttons;
}

void drawButtons(vector<Button> buttons, Shader shader) {
  

    for (int index = 0; index < buttons.size(); index++) {
        buttons[index].putButton(shader);
    }
    
}

void hoverButton(vector<Color> colors, Button &button, int index, bool press){
    

    if (index <= 1 && press == 1) {
        button.color = colors[1];
    }
    else if (index <= 1 && press == 0) {
        button.color = colors[0];
    }

    if (index == 2 && press == 1) {
        button.color = colors[3];
    }
    else if (index == 2 && press == 0) {
        button.color = colors[2];
    }

    if (index > 2 && press == 1) {
        button.color = colors[4];
    }
    else if (index > 2 && press == 0) {
        button.color = colors[5];
    }
    
}

void makeAxis(Point p, double unit, int divide) {
    int subUnit = unit / divide;
    glLineWidth(1);
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    for (float index = p.x; index <= SCREEN_WIDTH; index += subUnit) {
        glVertex2f(index, p.y);
        glVertex2f(index, 0);
        glVertex2f(index, p.y);
        glVertex2f(index, SCREEN_HEIGHT);

    }
    for (float index = p.x; index >= 0; index -= subUnit) {
        glVertex2f(index, p.y);
        glVertex2f(index, 0);
        glVertex2f(index, p.y);
        glVertex2f(index, SCREEN_HEIGHT);
    }

    for (float index = p.y; index <= SCREEN_HEIGHT; index += subUnit) {
        glVertex2f(p.x, index);
        glVertex2f(0, index);
        glVertex2f(p.x, index);
        glVertex2f(SCREEN_WIDTH, index);
    }
    for (float index = p.y; index >= 0; index -= subUnit) {
        glVertex2f(p.x, index);
        glVertex2f(0, index);
        glVertex2f(p.x, index);
        glVertex2f(SCREEN_WIDTH, index);
    }
    glEnd();
    glLineWidth(2);
    glColor3f(0.71, 0.71, 0.71);
    glBegin(GL_LINES);
    for (float index = p.x; index <= SCREEN_WIDTH; index += unit) {
        glVertex2f(index, p.y);
        glVertex2f(index, 0);
        glVertex2f(index, p.y);
        glVertex2f(index, SCREEN_HEIGHT);

    }
    for (float index = p.x; index >= 0; index -= unit) {
        glVertex2f(index, p.y);
        glVertex2f(index, 0);
        glVertex2f(index, p.y);
        glVertex2f(index, SCREEN_HEIGHT);
    }

    for (float index = p.y; index <= SCREEN_HEIGHT; index += unit) {
        glVertex2f(p.x, index);
        glVertex2f(0, index);
        glVertex2f(p.x, index);
        glVertex2f(SCREEN_WIDTH, index);
    }
    for (float index = p.y; index >= 0; index -= unit) {
        glVertex2f(p.x, index);
        glVertex2f(0, index);
        glVertex2f(p.x, index);
        glVertex2f(SCREEN_WIDTH, index);
    }
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(p.x, p.y);
    glVertex2f(p.x, 0);
    glVertex2f(p.x, p.y);
    glVertex2f(p.x, SCREEN_HEIGHT);
    glVertex2f(p.x, p.y);
    glVertex2f(0, p.y);
    glVertex2f(p.x, p.y);
    glVertex2f(SCREEN_WIDTH, p.y);
    glEnd();

}

void circle(Point p, float size, Color color, int thickness) {
    glLineWidth(thickness);
    int steps = 100;
    float angle = (3.14159265359 * 2.0f) / steps;
    glColor3f(color.r, color.g, color.b);
    float prevX = p.x;
    float prevY = p.y - size;


    for (int index = 0; index <= steps; index++) {
        float newX = p.x + size * sin(angle * index);
        float newY = p.y - size * cos(angle * index);
        
        glPointSize(size);
        glBegin(GL_LINES);
        glVertex3f(prevX, prevY, 0);
        glVertex3f(newX, newY, 0);
        glEnd();

        prevX = newX;
        prevY = newY;
    }

}


void putPoint(Point p, float size, Color color) {

    int steps = 100;
    float angle = (3.14159265359 * 2.0f) / steps;
    glColor3f(color.r, color.g, color.b);
    float prevX = p.x;
    float prevY = p.y - size;


    for (int index = 0; index <= steps; index++) {
        float newX = p.x + size * sin(angle * index);
        float newY = p.y - size * cos(angle * index);
        glPointSize(size);
        glBegin(GL_TRIANGLES);
        glVertex3f(p.x, p.y, 0);
        glVertex3f(prevX, prevY, 0);
        glVertex3f(newX, newY, 0);
        glEnd();

        prevX = newX;
        prevY = newY;
    }
}

void drawInscribedCircle(Shape triangle, Point origin, float add, float unit, Color color, int thickness) {

    Point center = inscribedCircleCoords(triangle.points[0], triangle.points[1], triangle.points[2]);
    Point aux = center;
    float radius = calculRaza(triangle.points[0], triangle.points[1], triangle.points[2]);

    aux.x += radius;
    float radiusGl = distance(euclidToGl(center,origin,add,unit), euclidToGl(aux, origin, add, unit));
    Point centerGl = euclidToGl(center, origin, add, unit);
    circle(centerGl, radiusGl, color, thickness);
    
}

void drawPointsInDomain(vector<Point> points, Shape shape, Point origin, float add, float unit, Color color) {

    for (int index = 0; index < points.size(); index++) {
        if (isPointInPolygon(shape, points[index])) {
            putPoint(euclidToGl( points[index],origin,add,unit), 10, color);
        }
    }
}

void drawConvexHull(vector<Point> points, Point origin, float add, float unit, Color color,int thickness) {

    vector<Point> hull = convexHull(points);
    for (int index = 0; index < hull.size() - 1; index++) {
        line(euclidToGl(hull[index], origin, add, unit), euclidToGl(hull[index + 1], origin, add, unit), color, thickness);
    }

}

void drawShortestDistance(Shader shader,vector<Point> points, Point origin, float add, float unit, Color color, int thickness) {
    float dis = 0;
    Segment seg = minimDistanceBetween2Points(points, dis);
    line(euclidToGl(seg.p1, origin, add, unit),euclidToGl(seg.p2, origin, add, unit), color, thickness);

    Button label;
    label.color = { 1,1,1 };
    label.x1 = 0;
    label.y1 = SCREEN_HEIGHT - 70;
    label.x2 = 190;
    label.y2 = SCREEN_HEIGHT;
    label.name = to_string(dis);
    label.scale = 0.85;
    label.putButton(shader);

    
}
//put area
void dysplayAreaOfShape(Shader shader, Shape shape) {
    
    float area = areaOfShape(shape);

    Button label;
    label.color = { 1,1,1 };
    label.x1 = 0;
    label.y1 = SCREEN_HEIGHT - 70;
    label.x2 = 190;
    label.y2 = SCREEN_HEIGHT;
    label.name = to_string(area);
    label.scale = 0.85;
    label.putButton(shader);


}

