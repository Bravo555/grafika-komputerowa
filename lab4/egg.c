#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#include "funcs.c"


typedef float point3[3];
point3 points[N][N];
point3 colours[N][N];


// Funkcja rysująca osie układu współrzędnych
void Axes() {
    point3 x_min = { -10.0, 0.0, 0.0 };
    point3 x_max = { 10.0, 0.0, 0.0 };

    point3 y_min = { 0.0, -10.0, 0.0 };
    point3 y_max = { 0.0,  10.0, 0.0 };

    point3 z_min = { 0.0, 0.0, -10.0 };
    point3 z_max = { 0.0, 0.0,  10.0 };

    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3fv(x_min);
        glVertex3fv(x_max);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3fv(y_min);
        glVertex3fv(y_max);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3fv(z_min);
        glVertex3fv(z_max);
    glEnd();
}

// Funkcja ustawia poprawny kolor rysowania dla punktu jaja z powierzchni
// parametrycznej a następnie rysuje ten punkt w przestrzeni 3D
void drawColoured(int u, int v) {
    glColor3fv(colours[u][v]);
    glVertex3fv(points[u][v]);
}

// Funkcja rysuje jajo w postaci chmury punktów
void drawEggPoints(int n) {
    glBegin(GL_POINTS);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            glVertex3fv(points[i][j]);
        }
    }
    glEnd();
}

// Funkcja rysuje jajo w postaci siatki połączonych linii
void drawEggLines(int n) {
    glBegin(GL_LINES);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            glVertex3fv(points[i][j]);
            glVertex3fv(points[i][(j + 1)]);

            glVertex3fv(points[i][j]);
            glVertex3fv(points[(i + 1) % n][j]);
            glVertex3fv(points[i][j + 1]);
            glVertex3fv(points[(i + 1) % n][j + 1]);
        }
    }

    // rysuj przekątne
    for (int i = 0; i < n / 2; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            glVertex3fv(points[i][j]);
            glVertex3fv(points[(i + 1) % n][(j + 1) % n]);
        }
    }
    for (int i = n / 2; i < n; ++i) {
        for (int j = 1; j < n; ++j) {
            glVertex3fv(points[i][j]);
            glVertex3fv(points[(i + 1) % n][(j - 1) % n]);
        }
    }

    // połącz przeciwne strony jaja
    for (int i = 1; i < n / 2; ++i) {
        glVertex3fv(points[(n / 2) - i][n - 1]);
        glVertex3fv(points[(n / 2) + i][0]);

        glVertex3fv(points[(n / 2) - i][0]);
        glVertex3fv(points[(n / 2) + i][n - 1]);

        glVertex3fv(points[(n / 2) - i][n - 1]);
        glVertex3fv(points[(n / 2) + i - 1][0]);

        if (i != n / 2 - 1) {
            glVertex3fv(points[(n / 2) - i][0]);
            glVertex3fv(points[(n / 2) + i + 1][n - 1]);
        }
    }

    glEnd();
}

// Funkcja rysuje jajo w postaci siatki kolorowych trójkątów
void drawEggTriangles(int n) {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            drawColoured(i, j);
            drawColoured(i, j + 1);
            drawColoured((i + 1) % n, j);

            drawColoured(i, j + 1);
            drawColoured((i + 1) % n, j);
            drawColoured((i + 1) % n, j + 1);
        }
    }

    // połącz przeciwne strony jaja
    for (int i = 0; i < n / 2; ++i) {
        drawColoured((n / 2) - i, n - 1);
        drawColoured((n / 2) + i, 0);
        drawColoured((n / 2) + i + 1, 0);

        drawColoured((n / 2) - i, n - 1);
        drawColoured((n / 2) - i - 1, n - 1);
        drawColoured((n / 2) + i + 1, 0);

        drawColoured((n / 2) + i, n - 1);
        drawColoured((n / 2) - i, 0);
        drawColoured((n / 2) - i - 1, 0);

        drawColoured((n / 2) + i, n - 1);
        drawColoured((n / 2) + i + 1, n - 1);
        drawColoured((n / 2) - i - 1, 0);
    }
    glEnd();
}

// Generuje pozycje punktów jaja i zapisuje je do tablicy `points`
void generateEggVertices(int n) {
    // jaka jest odległość pomiędzy kolejnymi punktami powierzchni
    // parametrycznej wzdłuż każdej z dwóch osi
    float step = 1.0f / n;

    for (int i = 0; i < n; ++i) {
        float u = i * step;
        for (int j = 0; j < n; ++j) {
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

// Funkcja generuje losowe kolory jaja i zapisuje je do tablicy `colours`
void generateColours(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float r = (float)rand() / RAND_MAX;
            float g = (float)rand() / RAND_MAX;
            float b = (float)rand() / RAND_MAX;

            colours[i][j][0] = r;
            colours[i][j][1] = g;
            colours[i][j][2] = b;
        }
    }
}

int main(int argc, char** argv) {
    printf("Grafika komputerowa lab4: OpenGL - interakcja z uzytkownikiem\n");
    printf("Autor: Marcel Guzik\n\n");
    printf("sterowanie:\n");
    printf("\tLPM by obracać położenie obserwatora\n");
    printf("\tPPM by zbliżać/oddalać widok\n");
    printf("modele:\n");
    printf("\tp - jajo, siatka punktów\n");
    printf("\tw - jajo, widok szkieletu (wireframe)\n");
    printf("\ts - jajo, kolorowa siatka trójkątów\n");
    printf("\tc - czajnik\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Grafika komputerowa lab4 - Marcel Guzik");

    generateEggVertices(N);
    generateColours(N);

    // Zdefiniowanie funkcji zwrotnych dla: rysowania sceny, zmiany rozmiaru
    // okna, wciśnięcia przycisku myszy, ruchu myszą, wciśnięcia klawisza
    // klawiatury
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mouseMoved);
    glutKeyboardFunc(keyPressed);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}
