#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define N 20
const float EPSILON = 0.000001;

typedef float point3[3];
point3 points[N][N];
point3 normals[N][N];
point3 colours[N][N];

int model = 3;


// inicjalizacja położenia obserwatora
static float viewer[]= {0.0, 0.0, 10.0};

static float angleY = 0.5 * M_PI;   // kąt obrotu obiektu
static float angleX = 0.0;
static float zstep = 0.1;

// przelicznik pikseli na stopnie
static GLfloat pix2angle;

static GLint status = 0;       // stan klawiszy myszy
                               // 0 - nie naciśnięto żadnego klawisza
                               // 1 - naciśnięty zostać lewy klawisz

static int x_pos_old=0;       // poprzednia pozycja kursora myszy
static int y_pos_old=0;

static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą
                                      // i poprzednią kursora myszy
static int delta_y = 0;

float R = 10.0;

// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych
void mousePressed(int btn, int state, int x, int y) {
    x_pos_old=x;        // przypisanie aktualnie odczytanej pozycji kursora
                        // jako pozycji poprzedniej
    y_pos_old = y;
    if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        status = 1;          // wcięnięty został lewy klawisz myszy
    }
    else if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        status = 2;
    }
    else
        status = 0;          // nie został wcięnięty żaden klawisz
}

void mouseMoved(GLsizei x, GLsizei y) {
    delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
    x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie

    delta_y = y - y_pos_old;
    y_pos_old = y;

    glutPostRedisplay();     // przerysowanie obrazu sceny
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

void drawColoured(int u, int v) {
    glColor3fv(colours[u][v]);
    glVertex3fv(points[u][v]);
}

void drawEggPoints(int n) {
    glBegin(GL_POINTS);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                glVertex3fv(points[i][j]);
            }
        }
    glEnd();
}

void drawEggLines(int n) {
    glBegin(GL_LINES);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n-1; ++j) {
                glVertex3fv(points[i][j]);
                glVertex3fv(points[i][(j+1)]);

                glVertex3fv(points[i][j]);
                glVertex3fv(points[(i+1)%n][j]);
                glVertex3fv(points[i][j+1]);
                glVertex3fv(points[(i+1)%n][j+1]);
            }
        }

        // rysuj przekątne
        for(int i = 0; i < n/2; ++i) {
            for(int j = 0; j < n-1; ++j) {
                glVertex3fv(points[i][j]);
                glVertex3fv(points[(i+1)%n][(j+1)%n]);
            }
        }
        for(int i = n/2; i < n; ++i) {
            for(int j = 1; j < n; ++j) {
                glVertex3fv(points[i][j]);
                glVertex3fv(points[(i+1)%n][(j-1)%n]);
            }
        }

        // połącz przeciwne strony jaja
        for(int i = 1; i < n/2; ++i) {
            glVertex3fv(points[(n/2)-i][n-1]);
            glVertex3fv(points[(n/2)+i][0]);

            glVertex3fv(points[(n/2)-i][0]);
            glVertex3fv(points[(n/2)+i][n-1]);

            glVertex3fv(points[(n/2)-i][n-1]);
            glVertex3fv(points[(n/2)+i-1][0]);

            if(i != n/2-1) {
                glVertex3fv(points[(n/2)-i][0]);
                glVertex3fv(points[(n/2)+i+1][n-1]);
            }
        }

    glEnd();
}

void drawEggTriangles(int n) {
    glBegin(GL_TRIANGLES);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n-1; ++j) {
                drawColoured(i, j);
                drawColoured(i, j+1);
                drawColoured((i+1)%n, j);

                drawColoured(i, j+1);
                drawColoured((i+1)%n, j);
                drawColoured((i+1)%n, j+1);
            }
        }

        // connect opposite sides of the egg
        for(int i = 0; i < n/2; ++i) {
            drawColoured((n/2)-i, n-1);
            drawColoured((n/2)+i, 0);
            drawColoured((n/2)+i+1, 0);

            drawColoured((n/2)-i, n-1);
            drawColoured((n/2)-i-1, n-1);
            drawColoured((n/2)+i+1, 0);

            drawColoured((n/2)+i, n-1);
            drawColoured((n/2)-i, 0);
            drawColoured((n/2)-i-1, 0);

            drawColoured((n/2)+i, n-1);
            drawColoured((n/2)+i+1, n-1);
            drawColoured((n/2)-i-1, 0);
        }
    glEnd();
}

void drawVertexWithNormal(int u, int v) {
    glNormal3fv(normals[u][v]);
    glVertex3fv(points[u][v]);
}

void drawEggTrianglesWithNormals(int n) {
    glBegin(GL_TRIANGLES);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n-1; ++j) {
                drawVertexWithNormal(i, j);
                drawVertexWithNormal(i, j+1);
                drawVertexWithNormal((i+1)%n, j);

                drawVertexWithNormal(i, j+1);
                drawVertexWithNormal((i+1)%n, j);
                drawVertexWithNormal((i+1)%n, j+1);
            }
        }

        // connect opposite sides of the egg
        for(int i = 0; i < n/2; ++i) {
            drawVertexWithNormal((n/2)-i, n-1);
            drawVertexWithNormal((n/2)+i, 0);
            drawVertexWithNormal((n/2)+i+1, 0);

            drawVertexWithNormal((n/2)-i, n-1);
            drawVertexWithNormal((n/2)-i-1, n-1);
            drawVertexWithNormal((n/2)+i+1, 0);

            drawVertexWithNormal((n/2)+i, n-1);
            drawVertexWithNormal((n/2)-i, 0);
            drawVertexWithNormal((n/2)-i-1, 0);

            drawVertexWithNormal((n/2)+i, n-1);
            drawVertexWithNormal((n/2)+i+1, n-1);
            drawVertexWithNormal((n/2)-i-1, 0);
        }
    glEnd();
}

void generateEggVerticesWithNormals(int n) {
    float step = 1.0f / n;

    for(int i = 0; i < n; ++i) {
        float u = i * step;
        for(int j = 0; j < n; ++j) {
            float v = j * step;
            float u_step = u;
            float x = 0.0f, y = 0.0f, z = 0.0f;
            // oznacza OŚ-N(ormal)-(u/v)
            float xnu = 0.0f, xnv = 0.0f, ynu = 0.0f, ynv = 0.0f, znu = 0.0f, znv = 0.0f;

            float n_coeff_xz = -45.0f;

            x -= 45.0f * u_step;
            z -= 45.0f * u_step;

            n_coeff_xz += 360.0f * u_step;
            ynu += 320.0f * u_step;

            // u^2
            u_step *= u;
            x += 180.0f * u_step;
            y += 160.0f * u_step;
            z += 180.0f * u_step;

            n_coeff_xz -= 810.0f * u_step;
            ynu -= 960.0f * u_step;

            // u^3
            u_step *= u;
            x -= 270.0f * u_step;
            y -= 320.0f * u_step;
            z -= 270.0f * u_step;

            n_coeff_xz += 900.0f * u_step;
            ynu += 640.0f * u_step;

            // u^4
            u_step *= u;
            x += 225.0f * u_step;
            y += 160.0f * u_step;
            z += 225.0f * u_step;

            n_coeff_xz -= 450.0f * u_step;

            // u^5
            u_step *= u;
            x -= 90.0f * u_step;
            z -= 90.0f * u_step;

            x *= cos(M_PI * v);
            z *= sin(M_PI * v);

            // obliczyć skladowe normalnych
            xnu = n_coeff_xz * cos(M_PI * v);
            xnv = M_PI * -z;

            ynu = ynu;
            ynv = 0;

            znu = n_coeff_xz * sin(M_PI * v);
            znv = -M_PI * -x;

            points[i][j][0] = x;
            points[i][j][1] = y;
            points[i][j][2] = z;

            float nx = ynu * znv - znu * ynv;
            float ny = znu * xnv - znv * xnu;
            float nz = xnu * ynv - xnu * ynv;

            float n_len = sqrt(nx * nx + ny * ny + nz * nz);

            nx /= n_len;
            ny /= n_len;
            nz /= n_len;

            normals[i][j][0] = nx;
            normals[i][j][1] = ny;
            normals[i][j][2] = nz;
        }
    }
}

void generateEggVertices(int n) {
    float step = 1.0f / n;

    for(int i = 0; i < n; ++i) {
        float u = i * step;
        for(int j = 0; j < n; ++j) {
            float v = j * step;
            float u_step = u;
            float x = 0.0f, y = 0.0f, z = 0.0f;

            x -= 45.0f * u_step;
            z -= 45.0f * u_step;

            // u^2
            u_step *= u;
            x += 180.0f * u_step;
            y += 160.0f * u_step;
            z += 180.0f * u_step;

            // u^3
            u_step *= u;
            x -= 270.0f * u_step;
            y -= 320.0f * u_step;
            z -= 270.0f * u_step;

            // u^4
            u_step *= u;
            x += 225.0f * u_step;
            y += 160.0f * u_step;
            z += 225.0f * u_step;

            // u^5
            u_step *= u;
            x -= 90.0f * u_step;
            z -= 90.0f * u_step;

            x *= cos(M_PI * v);
            z *= sin(M_PI * v);

            points[i][j][0] = x;
            points[i][j][1] = y;
            points[i][j][2] = z;

        }
    }
}

void generateColours(int n) {
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            float r = (float)rand() / RAND_MAX;
            float g = (float)rand() / RAND_MAX;
            float b = (float)rand() / RAND_MAX;

            colours[i][j][0] = r;
            colours[i][j][1] = g;
            colours[i][j][2] = b;
        }
    }
}

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

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba przerysować scenę)
void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny

    glLoadIdentity();

    // Zdefiniowanie położenia obserwatora
    gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    Axes();

    if(status == 1) {
        angleY = fmodfp(angleY + delta_x * 0.02, 2 * M_PI);
        angleX = fmodfp(angleX + delta_y * 0.02, 2 * M_PI);
        if(0.5 * M_PI < angleX && angleX < 1.0 * M_PI) {
            angleX = 0.5 * M_PI - EPSILON;
        } else if(1.0 * M_PI < angleX && angleX < 1.5 * M_PI) {
            angleX = 1.5 * M_PI + EPSILON;
        }
    }
    else if(status == 2) {
        R += delta_y * zstep;
    }


    viewer[0] = R * cos(angleY) * cos(angleX);
    viewer[1] = R * sin(angleX);
    viewer[2] = R * sin(angleY) * cos(angleX);

    if(model != 4) glTranslatef(0.0f, -5.0f, 0.0f);

    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny

    glColor3f(1.0f, 1.0f, 1.0f);
    if(model == 1) drawEggPoints(N);
    else if(model == 2) drawEggLines(N);
    else if(model == 3) drawEggTrianglesWithNormals(N);
    else if(model == 4) glutSolidTeapot(3.0);


    glFlush();
    // Przekazanie poleceń rysujących do wykonania


    glutSwapBuffers();
}

void changeSize(GLsizei horizontal, GLsizei vertical ) {
    pix2angle = 360.0/(float)horizontal;  // przeliczenie pikseli na stopnie

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji

    glLoadIdentity();
    // Czyszcznie macierzy bieżącej

    gluPerspective(70, 1.0, 1.0, 30.0);
    // Ustawienie parametrów dla rzutu perspektywicznego


    if(horizontal <= vertical)
        glViewport(0, (vertical-horizontal)/2, horizontal, horizontal);

    else
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    // Ustawienie wielkości okna okna widoku (viewport) w zależności
    // relacji pomiędzy wysokością i szerokością okna

    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej
}

void keyPressed(unsigned char key, int x, int y)
{
}

void init() {
    // współczynniki ka =[kar,kag,kab] dla światła otoczenia
    float mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego
    float mat_diffuse[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego
    float mat_specular[] = {1.0, 1.0, 1.0, 1.0};

    // współczynnik n opisujący połysk powierzchni
    float mat_shininess  = {20.0};

    // Definicja źródła światła

    // położenie źródła
    float light_position[] = {0.0, 0.0, 10.0, 1.0};   

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

/*************************************************************************************/
// Ustawienie parametrów źródła

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


/*************************************************************************************/
// Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora
}

int main(int argc, char** argv) {
    printf("Grafika komputerowa lab5: OpenGL - oświetlenie\n");
    printf("Autor: Marcel Guzik\n\n");
    printf("modele:\n");
    printf("\tp - jajo, siatka punktów\n");
    printf("\tw - jajo, widok szkieletu (wireframe)\n");
    printf("\ts - jajo, kolorowa siatka trójkątów\n");
    printf("\tc - czajnik\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Grafika komputerowa lab4 - Marcel Guzik");

    generateEggVerticesWithNormals(N);
    generateColours(N);

    init();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mouseMoved);
    glutKeyboardFunc(keyPressed);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    glutMainLoop();

}
