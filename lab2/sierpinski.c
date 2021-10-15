#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

const float CARPET_START = -1.0f;
const float CARPET_END = 1.0f;
const float DISPERSION_MAX = 0.0;

void setRandomColor() {
    float r = 1.0f; // (float)(rand() % 101) / 100;
    float g = 1.0f; // (float)(rand() % 101) / 100;
    float b = 1.0f; // (float)(rand() % 101) / 100;

    glColor3f(r, g, b);
}

void SierpinskiSquareRecurse(float start_x, float start_y, float end_x, float end_y, int levels) {
    // no checking for aspect ratio in the function
    // to ensure a proper square is drawn, calling code must ensure aspect ratio = 1
    float square_size = (end_x - start_x) / 3.0f;

    if(levels == 0) {
        // if(end_x - start_x < end_y - start_y) {
        //     printf("%.12f\n", (end_x - start_x) - (end_y - start_y));
        // }

        setRandomColor();
        glVertex2f(start_x, start_y);
        glVertex2f(end_x, start_y);
        glVertex2f(end_x, end_y);
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
                // printf("p1: %.9f %.9f p2:%.9f %.9f\n", next_start_x, next_start_y, next_end_x, next_end_y);
                SierpinskiSquareRecurse(next_start_x, next_start_y, next_end_x, next_end_y, levels - 1);
            }
        }
    }
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
        SierpinskiSquareRecurse(CARPET_START, CARPET_START, CARPET_END, CARPET_END, 8);
    glEnd();

    glFlush();
}

/*************************************************************************************/
// Funkcja ustalająca stan renderowania
void MyInit() {
    srand(4);   // https://xkcd.com/221/
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

/*************************************************************************************/
// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego
// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    // Deklaracja zmiennej aspectRatio określającej proporcję wymiarów okna
     GLfloat aspectRatio;

    // Zabezpieczenie pzred dzieleniem przez 0
    vertical = (vertical == 0) ? 1 : vertical;

     glViewport(0, 0, horizontal, vertical);
     // Ustawienie wielkościokna okna urządzenia (Viewport)
     // W tym przypadku od (0,0) do (horizontal, vertical)

    glMatrixMode(GL_PROJECTION); 
    // Określenie układu współrzędnych obserwatora

    glLoadIdentity();
    // Określenie przestrzeni ograniczającej

    glScalef(100.0f, 100.0f, 1.0f);
    glTranslatef(0.5f, 0.5f, 0.0f);

    aspectRatio = (GLfloat)horizontal/(GLfloat)vertical;
    // Wyznaczenie współczynnika proporcji okna

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // określenie okna obserwatora.
    // Pozwala to zachować właściwe proporcje rysowanego obiektu
    // Do określenia okna obserwatora służy funkcja glOrtho(...)

    float scale_x = 1.0f, scale_y = 1.0f;

    // aka if aspectRatio <= 1
    if(horizontal <= vertical)
        scale_y = scale_y / aspectRatio;
    else
        scale_x = scale_x * aspectRatio;
    glOrtho(-scale_x, scale_x, scale_y, -scale_y, 1.0, -1.0); 

    // Określenie układu współrzędnych
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// void MyKeyboardFunc(unsigned char Key, int x, int y) {
// switch(Key)
// {
//     case ‘w’: glTranslate(0); break;
//     case ‘a’: MenuHandler(1); break;
//     case ‘s’: MenuHandler(2); break;
//     case ‘d’: MenuHandler(2); break;
// }

/*************************************************************************************/
// Główny punkt wejścia programu. Program działa w trybie konsoli
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // Ustawienie trybu wyświetlania
    // GLUT_SINGLE - pojedynczy bufor wyświetlania
    // GLUT_RGBA - model kolorów RGB
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    // Utworzenie okna i określenie treści napisu w nagłówku okna
    glutCreateWindow("Drugi program w OpenGL");

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
