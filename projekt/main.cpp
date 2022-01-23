#include <cstdio>
#include <cmath>
#include <vector>
#include <GL/gl.h>
#include <GL/glut.h>

#include "texture.cpp"

const float EPSILON = 0.000001;

typedef float point3[3];
typedef float angles[3];

struct Planet {
    point3 position;
    float color[4];
    float size;
    bool texture;
};

std::vector<Planet> planets;

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
angles viewerAngles = {0.0, 0.5 * M_PI, 2.0};

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
        viewerAngles[2] = clamp(viewerAngles[2] + delta_y * zstep, 0.01, 100.0);
    }

    // przerysowanie obrazu sceny
    glutPostRedisplay();
}

// Funkcja rysująca osie układu współrzędnych
void axes() {
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
        glColor4f(0.5f, 0.0f, 0.0f, 0.1f);  // kolor rysowania osi - czerwony
        glVertex3fv(x_min);
        glVertex3fv(x_max);

        glColor4f(0.0f, 0.5f, 0.0f, 0.1f);  // kolor rysowania - zielony
        glVertex3fv(y_min);
        glVertex3fv(y_max);

        glColor4f(0.0f, 0.0f, 0.5f, 0.1f);  // kolor rysowania - niebieski
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
    glDisable(GL_LIGHTING);

    float sun_pos[] = {0.0, 0.0, 0.0, 1.0};
    float sun_color[] = {1.0, 1.0, 0.0, 1.0};
    float sun_size = 0.5;

    float viewer_pos[4] = {0.0, 0.0, 0.0, 1.0};
    angles_to_coords(viewerAngles, viewer_pos);

    gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    axes();

    glLoadIdentity();
    gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glColor4fv(sun_color);
    glutSolidSphere(sun_size, 100, 100);


    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_pos);
    for(auto& planet: planets) {
        glDisable(GL_TEXTURE_2D);
        glLoadIdentity();
        gluLookAt(viewer_pos[0], viewer_pos[1], viewer_pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        glTranslatefv(planet.position);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, planet.color);

        GLUquadric* quadric = gluNewQuadric();
        if(planet.texture) {
            glEnable(GL_TEXTURE_2D);
            gluQuadricTexture(quadric, true);
        }

        gluSphere(quadric, planet.size, 100, 100);
    }

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
    gluPerspective(70, aspectRatio, 0.01, 100000.0);
}

void keyPressed(unsigned char key, int x, int y) {
}

void init() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    // współczynniki ka =[kar,kag,kab] dla światła otoczenia
    float mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego
    float mat_diffuse[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego
    float mat_specular[] = {1.0, 1.0, 1.0, 1.0};

    // współczynnik n opisujący połysk powierzchni
    float mat_shininess  = {20.0};


    float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    float light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    float light_specular[]= {1.0, 1.0, 1.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    float att_constant  = {1.0};
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    float att_linear    = {0.05};
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    float att_quadratic  = {0.001};
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    // Ustawienie patrametrów materiału
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    // Ustawienie parametrów źródła
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    float pos[4] = {0.0, 0.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

    int width, height, components;
    GLenum format;
    GLbyte* tex = LoadTGAImage("textures/earth.tga", &width, &height, &components, &format);
    glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(tex);

    planets = {
        // mercury
        {
            {0.7, 0.0, 0.0},
            {0.2, 0.2, 0.2, 1.0},
            0.048
        },

        // venus
        {
            {1.1, 0.0, 0.0},
            {0.8, 0.8, 0.8, 1.0},
            0.060
        },

        // earth
        {
            {1.5, 0.0, 0.0},
            {0.0, 0.0, 1.0, 1.0},
            0.064,
            true
        },

        // mars
        {
            {2.2, 0.0, 0.0},
            {0.6, 0.2, 0.0, 1.0},
            0.034
        },
    };


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(640, 480);
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
