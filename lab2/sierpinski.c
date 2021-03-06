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
// Funkcja s??u????ca do kontroli zachowania proporcji rysowanych obiekt??w
// niezale??nie od rozmiar??w okna graficznego
// Parametry horizontal i vertical (szeroko???? i wysoko???? okna) s??
// przekazywane do funkcji za ka??dym razem, gdy zmieni si?? rozmiar okna
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

    // Ustawienie wielko??ci okna okna urz??dzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)
    glViewport(hozOff, verOff, width, height);

    // Okre??lenie uk??adu wsp????rz??dnych obserwatora
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    // Okre??lenie przestrzeni ograniczaj??cej

    // aspectRatio = (GLfloat)horizontal/(GLfloat)vertical;
    // Wyznaczenie wsp????czynnika proporcji okna

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // okre??lenie okna obserwatora.
    // Pozwala to zachowa?? w??a??ciwe proporcje rysowanego obiektu
    // Do okre??lenia okna obserwatora s??u??y funkcja glOrtho(...)

    float scale_x = 100.0f, scale_y = 100.0f;
    glOrtho(-scale_x, scale_x, scale_y, -scale_y, 1.0, -1.0);

    // Okre??lenie uk??adu wsp????rz??dnych
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // Ustawienie trybu wy??wietlania
    // GLUT_SINGLE - pojedynczy bufor wy??wietlania
    // GLUT_RGBA - model kolor??w RGB
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    // Utworzenie okna i okre??lenie tre??ci napisu w nag????wku okna
    glutCreateWindow("Dywan Sierpinskiego");

    // Okre??lenie, ??e funkcja RenderScene b??dzie funkcj?? zwrotn?? (callback)
    // Biblioteka GLUT b??dzie wywo??ywa??a t?? funkcj?? za ka??dym razem, gdy
    // trzeba b??dzie przerysowa?? okno
    glutDisplayFunc(RenderScene);

    // take keyboard input
    // glutKeyboardFunc(MyKeyboardFunc);
    // Dla aktualnego okna ustala funkcj?? zwrotn?? odpowiedzialn?? za
    // zmiany rozmiaru okna
    glutReshapeFunc(ChangeSize);

    // Funkcja MyInit (zdefiniowana powy??ej) wykonuje wszelkie 
    // inicjalizacje konieczneprzed przyst??pieniem do renderowania
    MyInit();

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();
}
