#define N 20

int model = 1;
const float EPSILON = 0.000001;

// inicjalizacja położenia obserwatora
static float viewer[] = { 0.0, 0.0, 10.0 };

// kąt obrotu obiektu
static float angleY = 0.5 * M_PI;
static float angleX = 0.0;
static float zstep = 0.1;

// przelicznik pikseli na stopnie
static GLfloat pix2angle;

// stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty zostać lewy klawisz
static GLint status = 0;

// poprzednia pozycja kursora myszy w osiach x i y
static int x_pos_old = 0;
static int y_pos_old = 0;

// różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_x = 0;
static int delta_y = 0;

// odległość obserwatora od początku układu współrzędnych
float R = 10.0;

// Funkcja blokuje wartość d tak, by znajdowała się w zakresie (min, max), tzn.
// jeżeli d > max to d = max, jeżeli d < min to d = min
float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

// Dodatnia reszta z dzielenia przez siebie dwóch liczb zmiennoprzecinkowych.
// Potrzebne do "zawijania" kąta theta
float fmodfp(float a, float b) {
    if (a < 0) {
        a += b;
    }
    else if (a > b) {
        a -= b;
    }
    return a;
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Zdefiniowanie położenia obserwatora
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    Axes();

    if (status == 1) {
        // tutaj `angleY` odnosi się do kąta dookoła osi Y (theta)
        // a `angleX` do kąta dookoła osi X, czyli azymutu (phi)
        // natomiast `delta_{x,y}` odnosi się do osi powierzchni ekranu
        angleY = fmodfp(angleY + delta_x * 0.02, 2 * M_PI);
        angleX = fmodfp(angleX + delta_y * 0.02, 2 * M_PI);

        // blokujemy przekroczenie osi y od górnej strony
        if (0.5 * M_PI < angleX && angleX < 1.0 * M_PI) {
            angleX = 0.5 * M_PI - EPSILON;
        }
        // blokujemy przekroczenie osi y od dolnej strony
        else if (1.0 * M_PI < angleX && angleX < 1.5 * M_PI) {
            angleX = 1.5 * M_PI + EPSILON;
        }
    }
    else if (status == 2) {
        // zmieniamy odległość obserwatora od początku układu współrzędnych
        R += delta_y * zstep;
    }

    // wyliczenie współrzędnych następnego położenia obserwatora
    viewer[0] = R * cos(angleY) * cos(angleX);
    viewer[1] = R * sin(angleX);
    viewer[2] = R * sin(angleY) * cos(angleX);

    // jeżeli rysujemy jajo, musimy je przesująć w dół
    if (model != 4) glTranslatef(0.0f, -5.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    if (model == 1) drawEggPoints(N);
    else if (model == 2) drawEggLines(N);
    else if (model == 3) drawEggTriangles(N);
    else if (model == 4) glutWireTeapot(3.0);

    glFlush();
    glutSwapBuffers();
}

void changeSize(int horizontal, int vertical) {
    // przeliczenie pikseli na stopnie
    pix2angle = 360.0 / (float)horizontal;

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
    gluPerspective(70, aspectRatio, 1.0, 30.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych
// globalnych
void mousePressed(int btn, int state, int x, int y) {
    // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
    x_pos_old = x;
    y_pos_old = y;

    // jeżeli wciśnięty lewy klawisz myszy, obracamy, jeżeli prawy,
    // przybliżamy/oddalamy
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        status = 1;
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        status = 2;
    }
    else
        status = 0;
}

// Wylicza poruszenie myszą i przerysowuje scenę
void mouseMoved(int x, int y) {
    // obliczenie różnicy położenia kursora myszy
    delta_x = x - x_pos_old;
    delta_y = y - y_pos_old;

    // podstawienie bieżącego położenia jako poprzednie
    x_pos_old = x;
    y_pos_old = y;

    glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {
    if (key == 'p') model = 1;
    else if (key == 'w') model = 2;
    else if (key == 's') model = 3;
    else if (key == 'c') model = 4;

    renderScene();
}
