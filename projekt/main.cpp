#include <cstdio>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

const float EPSILON = 0.000001;

typedef float point3[3];
typedef float angles[3];

static float zstep = 0.1;

// stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty został lewy klawisz
// 2 - naciśnięty został prawy klawisz
static GLint status = 0;

// poprzednia pozycja kursora myszy
double x_pos_old = 0;
double y_pos_old = 0;

// różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
double delta_x = 0;
double delta_y = 0;
angles viewerAngles = {0.0, 0.5 * M_PI, 10.0};

float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

float fmodfp(float a, float b) {
    if(a < 0) {
        a += b;
    } else if(a > b) {
        a -= b;
    }
    return a;
}

void angles_to_coords(float* angles, point3 coords) {
    coords[0] = angles[2] * cos(angles[1]) * cos(angles[0]);
    coords[1] = angles[2] * sin(angles[0]);
    coords[2] = angles[2] * sin(angles[1]) * cos(angles[0]);
}

// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych
void mousePressed(GLFWwindow* window, int btn, int state, int mods) {
    // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
    if(state == GLFW_PRESS) {
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if(btn == GLFW_MOUSE_BUTTON_LEFT) {
            status = 1;          // wcięnięty został lewy klawisz myszy
        }
        else if(btn == GLFW_MOUSE_BUTTON_RIGHT) {
            status = 2;
        }
    }
    else if(state == GLFW_RELEASE) {
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        status = 0;
    }
}

void mouseMoved(GLFWwindow* window, double x, double y) {
    // obliczenie różnicy położenia kursora myszy i podstawienie bieżącego
    // położenia jako poprzednie
    delta_x = x - x_pos_old;
    x_pos_old = x;

    delta_y = y - y_pos_old;
    y_pos_old = y;

    if(delta_x != 0 || delta_y != 0) {
        std::cout << delta_x << " " << delta_y << std::endl;
    }

    if(status == 1) {
        float new_elevation, new_azimuth;
        new_elevation = viewerAngles[0] + delta_y * 0.02;
        new_azimuth = viewerAngles[1] + delta_x * 0.02;

        viewerAngles[0] = fmodfp(new_elevation, 2 * M_PI);
        viewerAngles[1] = fmodfp(new_azimuth, 2 * M_PI);
        if(0.5 * M_PI < viewerAngles[0] && viewerAngles[0] < 1.0 * M_PI) {
            viewerAngles[0] = 0.5 * M_PI - EPSILON;
        } else if(1.0 * M_PI < viewerAngles[0] && viewerAngles[0] < 1.5 * M_PI) {
            viewerAngles[0] = 1.5 * M_PI + EPSILON;
        }
    }
    else if(status == 2) {
        viewerAngles[2] += delta_y * zstep;
    }

    // przerysowanie obrazu sceny
}

// Funkcja rysująca osie układu współrzędnych
void Axes() {
    point3 x_min = {-10.0, 0.0, 0.0};
    point3 x_max = { 10.0, 0.0, 0.0};
    // początek i koniec obrazu osi x

    point3 y_min = {0.0, -10.0, 0.0};
    point3 y_max = {0.0,  10.0, 0.0};
    // początek i koniec obrazu osi y

    point3 z_min = {0.0, 0.0, -10.0};
    point3 z_max = {0.0, 0.0,  10.0};
    //  początek i koniec obrazu osi y

    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
        glVertex3fv(x_min);
        glVertex3fv(x_max);

        glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
        glVertex3fv(y_min);
        glVertex3fv(y_max);

        glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
        glVertex3fv(z_min);
        glVertex3fv(z_max);
    glEnd();
}

void glTranslatefv(float arr[]) {
    return glTranslatef(arr[0], arr[1], arr[2]);
}

void renderScene(GLFWwindow* window) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float sun_pos[] = {0.0, 0.0, 0.0};
    float sun_size = 0.5;

    float mer_pos[] = {0.7, 0.0, 0.0};

    float ven_pos[] = {1.1, 0.0, 0.0};

    float ear_pos[] = {1.5, 0.0, 0.0};
    float ear_size = 0.05;

    float viewer_pos[4] = {0.0, 0.0, 0.0, 1.0};
    angles_to_coords(viewerAngles, viewer_pos);

    gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    Axes();

    glColor3f(1.0f, 1.0f, 0.0f);
    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, sun_size, 100, 100);

    glLoadIdentity();
    gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatefv(ear_pos);
    glColor3f(0.0f, 0.0f, 1.0f);
    gluSphere(quadric, ear_size, 100, 100);

    glFlush();
    glfwSwapBuffers(window);
}

void changeSize(GLFWwindow* window, int horizontal, int vertical) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(vertical == 0) return;
    glViewport(0, 0, horizontal, vertical);

    // Ustawienie parametrów dla rzutu perspektywicznego:
    // 70 stopni pola widzenia w osi y
    // proporcje między osią x i y, do uzyskania pola widzenia w osi x
    // minimalna odległość od obserwatora
    // maksymalna odległość od obserwatora
    float aspectRatio = (float)horizontal / (float)vertical;
    gluPerspective(70, aspectRatio, 1.0, 100000.0);
}

void keyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void init(GLFWwindow* window) {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    changeSize(window, width, height);
}

void errorCallback(int error, const char* desc) {
    std::cerr << "GLFW error: " << desc << std::endl;
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(errorCallback);

    if(!glfwInit()) {
        std::cerr << "Error initializing glfw" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "GK lab7: projekt ukladu slonecznego - Marcel Guzik", NULL, NULL);
    if (!window) {
        // Window or OpenGL context creation failed
        std::cerr << "Error initializing window" << std::endl;
        return -2;
    }

    glfwMakeContextCurrent(window);

    init(window);

    bool rawMouse = glfwRawMouseMotionSupported();
    std::cout << "raw mouse supported: " << rawMouse << std::endl;
    if(rawMouse) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetKeyCallback(window, keyPressed);
    glfwSetFramebufferSizeCallback(window, changeSize);
    glfwSetCursorPosCallback(window, mouseMoved);
    glfwSetMouseButtonCallback(window, mousePressed);


    // glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window)) {
        renderScene(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
