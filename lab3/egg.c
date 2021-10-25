/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacją osi 
//  układu współrzednych

/*************************************************************************************/

#include <GL/gl.h>
#include <GL/glut.h>


typedef float point3[3];

/*************************************************************************************/

// Funkcja rysująca osie układu współrzędnych

 

void Axes(void)
{

    point3  x_min = {-5.0, 0.0, 0.0};
    point3  x_max = { 5.0, 0.0, 0.0};
    // początek i koniec obrazu osi x

    point3  y_min = {0.0, -5.0, 0.0};
    point3  y_max = {0.0,  5.0, 0.0};
    // początek i koniec obrazu osi y

    point3  z_min = {0.0, 0.0, -5.0};
    point3  z_max = {0.0, 0.0,  5.0};
    //  początek i koniec obrazu osi y

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x

        glVertex3fv(x_min);
        glVertex3fv(x_max);

    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

        glVertex3fv(y_min);
        glVertex3fv(y_max);                           

    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

        glVertex3fv(z_min);
        glVertex3fv(z_max);

    glEnd();

}

void drawEgg(int n) {
    point3 points[n][n];
    float step = 1.0f / n;

    for(int i = 0; i < n; ++i) {
        float u = i * step;
        for(int j = 0; j < n; ++j) {
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

            points[i][j][0] = x;
            points[i][j][1] = y;
            points[i][j][2] = z;

        }
    }

    glBegin(GL_POINTS);

        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                glVertex3fv(points[i][j]);
            }
        }

    glEnd();
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

    glRotated(60.0, 1.0, 1.0, 1.0 );

    glColor3f(1.0f, 1.0f, 1.0f);

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

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

 

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Układ współrzędnych 3-D");
                  
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

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT

}
