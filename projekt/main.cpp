#include <cstdio>
#include <cmath>
#include <GL/gl.h>
#include <GL/glut.h>

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
static int x_pos_old = 0;
static int y_pos_old = 0;

// różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_x = 0;
static int delta_y = 0;
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
void mousePressed(int btn, int state, int x, int y) {
    // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
    x_pos_old = x;
    y_pos_old = y;

    if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        status = 1;          // wcięnięty został lewy klawisz myszy
    }
    else if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        status = 2;
    }
    else {
        status = 0;          // nie został wcięnięty żaden klawisz
    }
}

void mouseMoved(GLsizei x, GLsizei y) {
    // obliczenie różnicy położenia kursora myszy i podstawienie bieżącego
    // położenia jako poprzednie
    delta_x = x - x_pos_old;
    x_pos_old = x;

    delta_y = y - y_pos_old;
    y_pos_old = y;

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
    glutPostRedisplay();
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

void renderScene(void) {
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
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(sun_size, 100, 100);

    gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatefv(ear_pos);
    glColor3f(0.0f, 0.0f, 1.0f);
    glutSolidSphere(ear_size, 100, 100);

    glFlush();
    glutSwapBuffers();
}

void changeSize(int horizontal, int vertical) {
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

void keyPressed(unsigned char key, int x, int y) {
}

void init() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("GK lab7: projekt ukladu slonecznego - Marcel Guzik");

    init();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mouseMoved);
    glutKeyboardFunc(keyPressed);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutMainLoop();

    return 0;
}
