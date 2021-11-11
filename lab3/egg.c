/*************************************************************************************/
//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacją osi 
//  układu współrzednych
/*************************************************************************************/

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define N 30

typedef float point3[3];
point3 points[N][N];
point3 colours[N][N];
static GLfloat theta[] = {0.0, 0.0, 0.0}; // trzy kąty obrotu

int model = 1;
bool rotate = true;


// Funkcja rysująca osie układu współrzędnych
void spinEgg() {
    if(!rotate) {
        return;
    }

    theta[0] -= 0.2;
    if( theta[0] > 360.0 ) theta[0] -= 360.0;

    theta[1] -= 0.2;
    if( theta[1] > 360.0 ) theta[1] -= 360.0;

    theta[2] -= 0.2;
    if( theta[2] > 360.0 ) theta[2] -= 360.0;

    glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
}

void drawColoured(int u, int v) {
    glColor3fv(colours[u][v]);
    glVertex3fv(points[u][v]);
}

void Axes() {
    point3  x_min = {-5.0, 0.0, 0.0};
    point3  x_max = { 5.0, 0.0, 0.0};
    // początek i koniec obrazu osi x

    point3  y_min = {0.0, -5.0, 0.0};
    point3  y_max = {0.0,  5.0, 0.0};
    // początek i koniec obrazu osi y

    point3  z_min = {0.0, 0.0, -5.0};
    point3  z_max = {0.0, 0.0,  5.0};
    //  początek i koniec obrazu osi y

    glBegin(GL_LINES); // rysowanie osi x
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

                // TODO: simplify this
                glVertex3fv(points[i][j]);
                glVertex3fv(points[(i+1)%n][j]);
                glVertex3fv(points[i][j+1]);
                glVertex3fv(points[(i+1)%n][j+1]);
            }
        }

        // draw diagonals
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

        // connect opposite sides of the egg
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
/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)

 

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej

    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej

    glTranslatef(0.0f, -5.0f, 0.0f);
    glRotatef(35.0f, 1.0f, 0.0f, 0.0f);

    glRotatef(theta[0], 1.0, 0.0, 0.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    glRotatef(theta[2], 0.0, 0.0, 1.0);

    glColor3f(1.0f, 1.0f, 1.0f);
    if(model == 1) drawEggPoints(N);
    else if(model == 2) drawEggLines(N);
    else if(model == 3) drawEggTriangles(N);

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

 

    glutSwapBuffers();
    //

 }

/*************************************************************************************/

// Funkcja ustalająca stan renderowania

 

void MyInit(void)
{

glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
// Kolor czyszcący (wypełnienia okna) ustawiono na czarny

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.

 

void ChangeSize(GLsizei horizontal, GLsizei vertical )
{

    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio  określającej proporcję
    // wymiarów okna 

    if(vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0

        vertical = 1;  

    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkościokna okna widoku (viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)  

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji 

    glLoadIdentity();
    // Czyszcznie macierzy bieżącej

    AspectRatio = (GLfloat)horizontal/(GLfloat)vertical;
    // Wyznaczenie współczynnika  proporcji okna
    // Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
    // przestrzeni ograniczającej pozwalającej zachować właściwe
    // proporcje rysowanego obiektu.
    // Do okreslenia przestrzeni ograniczjącej służy funkcja
    // glOrtho(...)

    if(horizontal <= vertical)

        glOrtho(-7.5,7.5,-7.5/AspectRatio,7.5/AspectRatio,10.0, -10.0); 

    else

        glOrtho(-7.5*AspectRatio,7.5*AspectRatio,-7.5,7.5,10.0,-10.0);                     

    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu                                    

    glLoadIdentity();
    // Czyszcenie macierzy bieżącej

}

void keys(unsigned char key, int x, int y)
{
    if(key == 'p') model = 1;
    if(key == 'w') model = 2;
    if(key == 's') model = 3;
    if(key == 'a') rotate = !rotate;
   
    RenderScene(); // przerysowanie obrazu sceny
}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

 

int main(int argc, char** argv)
{
    printf("Grafika komputerowa lab3: OpenGL - modelowanie obiektów 3-D\n");
    printf("Autor: Marcel Guzik\n\n");
    printf("sterowanie:\n");
    printf("\ta - zapauzowanie rotacji\n");
    printf("modele:\n");
    printf("\tp - siatka punktów\n");
    printf("\tw - widok szkieletu (wireframe)\n");
    printf("\ts - kolorowa siatka trójkątów\n");

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);

    glutInitWindowSize(800, 800);

    glutCreateWindow("Grafika komputerowa lab3 - Marcel Guzik");

    generateEggVertices(N);
    generateColours(N);

    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Bedzie ona wywoływana za każdym razem
    // gdy zajdzie potrzba przeryswania okna 

    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // zazmiany rozmiaru okna      

    MyInit();
    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania 

    glEnable(GL_DEPTH_TEST);
    // Włączenie mechanizmu usuwania powierzchni niewidocznych

    glutIdleFunc(spinEgg);
    glutKeyboardFunc(keys);

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT

}
