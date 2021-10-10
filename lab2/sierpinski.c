
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>

/*************************************************************************************/


// Funkcaja określająca, co ma być rysowane
// (zawsze wywoływana, gdy trzeba przerysować scenę)


const float CARPET_START = -100.0f;
const float CARPET_END = 100.0f;

void SierpinskiSquareRecurse(float start_x, float start_y, float end_x, float end_y, int levels) {
    float square_size = (end_x - start_x) / 3;

    glColor3f(0.0f, 1.0f, 0.0f);
    if(levels == 0) {

        float offset_x = (float)((rand() % 201) - 100) / 500; // [-1; 1]
        float offset_y = (float)((rand() % 201) - 100) / 500; // [-1; 1]

        start_x = start_x + offset_x;
        end_x = end_x + offset_x;
        start_y = start_y + offset_y;
        end_y = end_y + offset_y;

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
                float next_start_y = start_y - square_size * y;
                float next_end_x = start_x + square_size * (x+1);
                float next_end_y = start_y - square_size * (y+1);
                SierpinskiSquareRecurse(next_start_x, next_start_y, next_end_x, next_end_y, levels - 1);
            }
        }
    }
}

void RenderScene(void)

{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);                                                 
        glColor3f(0.0f, 0.0f, 1.0f);

        SierpinskiSquareRecurse(CARPET_START, -CARPET_START, CARPET_END, -CARPET_END, 5);

    glEnd();

    glFlush();
}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania

 

void MyInit(void)

{
    srand(4);

   glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
   // Kolor okna wnętrza okna - ustawiono na szary

}

/*************************************************************************************/

// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego



void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna

{

     GLfloat AspectRatio;

     // Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna

 

    if(vertical==0)
    // Zabezpieczenie pzred dzieleniem przez 0

        vertical = 1;

     glViewport(0, 0, horizontal, vertical);
     // Ustawienie wielkościokna okna urządzenia (Viewport)
     // W tym przypadku od (0,0) do (horizontal, vertical)

    glMatrixMode(GL_PROJECTION); 
    // Określenie układu współrzędnych obserwatora

    glLoadIdentity();
    // Określenie przestrzeni ograniczającej

    AspectRatio = (GLfloat)horizontal/(GLfloat)vertical;
    // Wyznaczenie współczynnika proporcji okna

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // określenie okna obserwatora.
    // Pozwala to zachować właściwe proporcje rysowanego obiektu
    // Do określenia okna obserwatora służy funkcja glOrtho(...)

 

    if(horizontal <= vertical)

    glOrtho(-100.0,100.0,-100.0/AspectRatio,100.0/AspectRatio,1.0,-1.0); 

     else

    glOrtho(-100.0*AspectRatio,100.0*AspectRatio,-100.0,100.0,1.0,-1.0); 

    glMatrixMode(GL_MODELVIEW);
    // Określenie układu współrzędnych     

    glLoadIdentity();

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

 

int main(int argc, char** argv) {
    glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   // Ustawienie trybu wyświetlania
   // GLUT_SINGLE - pojedynczy bufor wyświetlania
   // GLUT_RGBA - model kolorów RGB

 

   glutCreateWindow("Drugi program w OpenGL");
   // Utworzenie okna i określenie treści napisu w nagłówku okna

   glutDisplayFunc(RenderScene);
   // Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
   // Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
   // trzeba będzie przerysować okno


   glutReshapeFunc(ChangeSize);
   // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
   // zmiany rozmiaru okna

   MyInit();
   // Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
   // inicjalizacje konieczneprzed przystąpieniem do renderowania


   glutMainLoop();
   // Funkcja uruchamia szkielet biblioteki GLUT

}
