#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

const float CARPET_START = -100.0f;
const float CARPET_END = 100.0f;
const float DISPERSION_MAX = 0.5;
const int LEVELS = 3;

// The viewport has to be a multiple of this variable in both axes. this effectively dictates how closely the squares
// are aligned against the individual pixels: 1 - there is no alignment, 3^LEVEL - each square has to be at least 1
// pixel in size. For high values of LEVEL, squares have to be smaller than a pixel, thus a smaller value than 3^LEVEL
// is required. So, the value of this variable should be 3^N, where N is level up to which align the squares.
// 243 = pow(3, LEVELS) errors with: ""function call must have a constant value in a constant expression""
const int FRAGMENTS = 27;

void setRandomColor() {
    float r = (float)rand() / RAND_MAX;
    float g = (float)rand() / RAND_MAX;
    float b = (float)rand() / RAND_MAX;

    glColor3f(r, g, b);
}

void SierpinskiSquareRecurse(float start_x, float start_y, float end_x, float end_y, int levels) {
    // no checking for aspect ratio in the function
    // to ensure a proper square is drawn, calling code must ensure aspect ratio = 1
    float square_size = (end_x - start_x) / 3.0f;

    if(levels == 0) {
        float offset_x = (float)(rand() - (RAND_MAX / 2)) / RAND_MAX * DISPERSION_MAX; // [-1; 1]
        float offset_y = (float)(rand() - (RAND_MAX / 2)) / RAND_MAX * DISPERSION_MAX; // [-1; 1]

        start_x = start_x + offset_x;
        end_x = end_x + offset_x;
        start_y = start_y + offset_y;
        end_y = end_y + offset_y;

        setRandomColor();
        glVertex2f(start_x, start_y);
        setRandomColor();
        glVertex2f(end_x, start_y);
        setRandomColor();
        glVertex2f(end_x, end_y);
        setRandomColor();
        glVertex2f(start_x, end_y);
        return;
    }

    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            if(y != 1 || x != 1) {
                float next_start_x = start_x + square_size * x;
                float next_start_y = start_y + square_size * y;
                float next_end_x = start_x + square_size * (x+1);
                float next_end_y = start_y + square_size * (y+1);
                SierpinskiSquareRecurse(next_start_x, next_start_y, next_end_x, next_end_y, levels - 1);
            }
        }
    }
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
        SierpinskiSquareRecurse(CARPET_START, CARPET_START, CARPET_END, CARPET_END, LEVELS);
    glEnd();

    glFlush();
}

void MyInit() {
    srand(4);   // https://xkcd.com/221/
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/*************************************************************************************/
// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego
// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    // Zabezpieczenie pzred dzieleniem przez 0
    if(vertical == 0) return;

    // round to the nearest multiple of FRAGMENTS
    int width = horizontal / FRAGMENTS * FRAGMENTS;
    int height = vertical / FRAGMENTS * FRAGMENTS;

    int hozOff = 0, verOff = 0;
    if(vertical < horizontal) {
        width = height;
    } else if(horizontal < vertical) {
        height = width;
    }
    hozOff = (horizontal - width) / 2;
    verOff = (vertical - height) / 2;

    // Ustawienie wielkości okna okna urządzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)
    glViewport(hozOff, verOff, width, height);

    // Określenie układu współrzędnych obserwatora
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    // Określenie przestrzeni ograniczającej

    // aspectRatio = (GLfloat)horizontal/(GLfloat)vertical;
    // Wyznaczenie współczynnika proporcji okna

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // określenie okna obserwatora.
    // Pozwala to zachować właściwe proporcje rysowanego obiektu
    // Do określenia okna obserwatora służy funkcja glOrtho(...)

    float scale_x = 100.0f, scale_y = 100.0f;
    glOrtho(-scale_x, scale_x, scale_y, -scale_y, 1.0, -1.0);

    // Określenie układu współrzędnych
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // Ustawienie trybu wyświetlania
    // GLUT_SINGLE - pojedynczy bufor wyświetlania
    // GLUT_RGBA - model kolorów RGB
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    // Utworzenie okna i określenie treści napisu w nagłówku okna
    glutCreateWindow("Dywan Sierpinskiego");

    // Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
    // Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
    // trzeba będzie przerysować okno
    glutDisplayFunc(RenderScene);

    // take keyboard input
    // glutKeyboardFunc(MyKeyboardFunc);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
    // zmiany rozmiaru okna
    glutReshapeFunc(ChangeSize);

    // Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
    // inicjalizacje konieczneprzed przystąpieniem do renderowania
    MyInit();

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();
}
