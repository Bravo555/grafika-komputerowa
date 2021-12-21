#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#define __USE_MISC
#include <math.h>
#include <stdbool.h>

#define N 101
const float EPSILON = 0.000001;

typedef float point3[3];

point3 points[N][N];
point3 normals[N][N];
point3 colours[N][N];
bool drawNormals = false;

// tablica zawiera kolejno kąt elewacji, azymutu, oraz odległość
typedef float angles[3];

angles viewerAngles = {0.0, 0.5 * M_PI, 10.0};
angles light0Angles = {0.0, 0.25 * M_PI, 10.0};
angles light1Angles = {0.0, 0.75 * M_PI, 10.0};

// Aktualnie wybrane kąty które będą modyfikowane przez poruszanie myszą
float* currentAngles = light0Angles;

int model = 4;

static float zstep = 0.1;


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


GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{

/*************************************************************************************/

// Struktura dla nagłówka pliku  TGA


    #pragma pack(1)           
    typedef struct                       
    {
        GLbyte    idlength;             
        GLbyte    colormaptype;          
        GLbyte    datatypecode;            
        unsigned short    colormapstart; 
        unsigned short    colormaplength;
        unsigned char     colormapdepth;  
        unsigned short    x_orgin;        
        unsigned short    y_orgin;        
        unsigned short    width;         
        unsigned short    height;        
        GLbyte    bitsperpixel;                  
        GLbyte    descriptor;            
    }TGAHEADER;
    #pragma pack(8)

    FILE *pFile;                   
    TGAHEADER tgaHeader;           
    unsigned long lImageSize;       
    short sDepth;                   
    GLbyte    *pbitsperpixel = NULL; 

          
/*************************************************************************************/ 

// Wartości domyślne zwracane w przypadku błędu

    *ImWidth = 0;               
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
   
    pFile = fopen(FileName, "rb");
    if(pFile == NULL)
      return NULL;

/*************************************************************************************/
// Przeczytanie nagłówka pliku 


    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
                

/*************************************************************************************/

// Odczytanie szerokości, wysokości i głębi obrazu

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;


/*************************************************************************************/
// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)
   
    if(tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

/*************************************************************************************/

// Obliczenie rozmiaru bufora w pamięci


    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


/*************************************************************************************/   

// Alokacja pamięci dla danych obrazu


     pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
   
      if(pbitsperpixel == NULL)
        return NULL;

    if(fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
        {
        free(pbitsperpixel);
        return NULL;
        }
   

/*************************************************************************************/

// Ustawienie formatu OpenGL


    switch(sDepth)

        {

        case 3:    

            *ImFormat = GL_BGR_EXT;

            *ImComponents = GL_RGB8;

            break;

        case 4:

            *ImFormat = GL_BGRA_EXT;

            *ImComponents = GL_RGBA8;

            break;

        case 1:

            *ImFormat = GL_LUMINANCE;

            *ImComponents = GL_LUMINANCE8;

            break;

        };

     

    fclose(pFile);

       

    return pbitsperpixel;

}

void angles_to_coords(float* angles, point3 coords) {
    coords[0] = angles[2] * cos(angles[1]) * cos(angles[0]);
    coords[1] = angles[2] * sin(angles[0]);
    coords[2] = angles[2] * sin(angles[1]) * cos(angles[0]);
}

void mouseMoved(GLsizei x, GLsizei y) {
    delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
    x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie

    delta_y = y - y_pos_old;
    y_pos_old = y;

    if(status == 1) {
        float new_elevation, new_azimuth;
        if(currentAngles == viewerAngles) {
            new_elevation = currentAngles[0] + delta_y * 0.02;
            new_azimuth = currentAngles[1] + delta_x * 0.02;
        } else {
            new_elevation = currentAngles[0] - delta_y * 0.02;
            new_azimuth = currentAngles[1] - delta_x * 0.02;
        }

        currentAngles[0] = fmodfp(new_elevation, 2 * M_PI);
        currentAngles[1] = fmodfp(new_azimuth, 2 * M_PI);
        if(0.5 * M_PI < currentAngles[0] && currentAngles[0] < 1.0 * M_PI) {
            currentAngles[0] = 0.5 * M_PI - EPSILON;
        } else if(1.0 * M_PI < currentAngles[0] && currentAngles[0] < 1.5 * M_PI) {
            currentAngles[0] = 1.5 * M_PI + EPSILON;
        }
    }
    else if(status == 2) {
        currentAngles[2] += delta_y * zstep;
    }

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

void drawVertexWithNormal(int u, int v) {
    glNormal3fv(normals[u][v]);
    glVertex3fv(points[u][v]);
}

void drawVertexTextured(int u, int v) {
    glNormal3fv(normals[u][v]);
    glTexCoord2f((float)v / (N-1), (float)u / (N-1));
    glVertex3fv(points[u][v]);
}

void drawEggPoints(int n) {
    glBegin(GL_POINTS);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                drawVertexWithNormal(i, j);
            }
        }
    glEnd();
}

void drawEggLines(int n) {
    glBegin(GL_LINES);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n-1; ++j) {
                drawVertexWithNormal(i,j);
                drawVertexWithNormal(i,(j+1));

                drawVertexWithNormal(i,j);
                drawVertexWithNormal((i+1)%n,j);
                drawVertexWithNormal(i,j+1);
                drawVertexWithNormal((i+1)%n,j+1);
            }
        }

        // rysuj przekątne
        for(int i = 0; i < n/2; ++i) {
            for(int j = 0; j < n-1; ++j) {
                drawVertexWithNormal(i,j);
                drawVertexWithNormal((i+1)%n,(j+1)%n);
            }
        }
        for(int i = n/2; i < n; ++i) {
            for(int j = 1; j < n; ++j) {
                drawVertexWithNormal(i,j);
                drawVertexWithNormal((i+1)%n,(j-1)%n);
            }
        }

        // połącz przeciwne strony jaja
        for(int i = 1; i < n/2; ++i) {
            drawVertexWithNormal((n/2)-i,n-1);
            drawVertexWithNormal((n/2)+i,0);

            drawVertexWithNormal((n/2)-i,0);
            drawVertexWithNormal((n/2)+i,n-1);

            drawVertexWithNormal((n/2)-i,n-1);
            drawVertexWithNormal((n/2)+i-1,0);

            if(i != n/2-1) {
                drawVertexWithNormal((n/2)-i,0);
                drawVertexWithNormal((n/2)+i+1,n-1);
            }
        }

    glEnd();
}

void drawNormal(int u, int v) {
    float* p = points[u][v];
    float* n = normals[u][v];
    glVertex3fv(p);
    glVertex3f(p[0] + n[0], p[1] + n[1], p[2] + n[2]);
}

void drawEggTriangles(int n) {
    glEnable(GL_LIGHTING);
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

    if(drawNormals) {
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);

            for(int u = 0; u < n; ++u) {
                for(int v = 0; v < n; ++v) {
                    drawNormal(u, v);
                }
            }

        glEnd();
    }
}

void drawEggTextured(int n) {
    glEnable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n-1; ++j) {
                drawVertexTextured(i, j);
                drawVertexTextured(i, j+1);
                drawVertexTextured((i+1)%n, j);

                drawVertexTextured(i, j+1);
                drawVertexTextured((i+1)%n, j);
                drawVertexTextured((i+1)%n, j+1);
            }
        }

        // connect opposite sides of the egg
        for(int i = 0; i < n/2; ++i) {
            drawVertexTextured((n/2)-i, n-1);
            drawVertexTextured((n/2)+i, 0);
            drawVertexTextured((n/2)+i+1, 0);

            drawVertexTextured((n/2)-i, n-1);
            drawVertexTextured((n/2)-i-1, n-1);
            drawVertexTextured((n/2)+i+1, 0);

            drawVertexTextured((n/2)+i, n-1);
            drawVertexTextured((n/2)-i, 0);
            drawVertexTextured((n/2)-i-1, 0);

            drawVertexTextured((n/2)+i, n-1);
            drawVertexTextured((n/2)+i+1, n-1);
            drawVertexTextured((n/2)-i-1, 0);
        }
    glEnd();

    if(drawNormals) {
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);

            for(int u = 0; u < n; ++u) {
                for(int v = 0; v < n; ++v) {
                    drawNormal(u, v);
                }
            }

        glEnd();
    }
}

void generateEggVerticesWithNormals(int n) {
    float step = 1.0f / (n - 1);

    for(int i = 0; i < n; ++i) {
        float u = i * step;
        if(u == 0.0) u += EPSILON;
        if(u == 1.0) u -= EPSILON;
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
            float nz = xnu * ynv - xnv * ynu;

            if(z > 0.0f || (z == 0.0f && x > 0.0f)) {
                nx = -nx;
                ny = -ny;
                nz = -nz;
            }

            float n_len = sqrt(nx * nx + ny * ny + nz * nz);

            if(n_len != 0.0) {
                nx /= n_len;
                ny /= n_len;
                nz /= n_len;
            }

            normals[i][j][0] = nx;
            normals[i][j][1] = ny;
            normals[i][j][2] = nz;
        }
    }
}

void drawPyramid(float height) {
    glEnable(GL_CULL_FACE);
    glBegin(GL_QUADS);
        glVertex3f(-2.0, -2.0, -2.0);
        glVertex3f(2.0, -2.0, -2.0);
        glVertex3f(2.0, -2.0, 2.0);
        glVertex3f(-2.0, -2.0, 2.0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(2.0, -2.0, 2.0);
        glTexCoord2f(0.25f, 0.0f);
        glVertex3f(2.0, -2.0, -2.0);
        glTexCoord2f(0.125f, 1.0f);
        glVertex3f(0.0, height, 0.0);
        
        glTexCoord2f(0.25f, 0.0f);
        glVertex3f(-2.0, -2.0, 2.0);
        glTexCoord2f(0.5f, 0.0f);
        glVertex3f(2.0, -2.0, 2.0);
        glTexCoord2f(0.375f, 1.0f);
        glVertex3f(0.0, height, 0.0);

        glTexCoord2f(0.5f, 0.0f);
        glVertex3f(-2.0, -2.0, -2.0);
        glTexCoord2f(0.75f, 0.0f);
        glVertex3f(-2.0, -2.0, 2.0);
        glTexCoord2f(0.625f, 1.0f);
        glVertex3f(0.0, height, 0.0);

        glTexCoord2f(0.75f, 0.0f);
        glVertex3f(2.0, -2.0, -2.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-2.0, -2.0, -2.0);
        glTexCoord2f(0.875f, 1.0f);
        glVertex3f(0.0, height, 0.0);
    glEnd();
    glDisable(GL_CULL_FACE);
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);

    glLoadIdentity();
    float currentPosition[4] = {0.0, 0.0, 0.0, 1.0};

    angles_to_coords(viewerAngles, currentPosition);
    gluLookAt(currentPosition[0], currentPosition[1], currentPosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    angles_to_coords(currentAngles, currentPosition);

    angles_to_coords(light0Angles, currentPosition);
    glLightfv(GL_LIGHT0, GL_POSITION, currentPosition);
    angles_to_coords(light1Angles, currentPosition);
    glLightfv(GL_LIGHT1, GL_POSITION, currentPosition);

    Axes();

    if(model < 5) glTranslatef(0.0f, -5.0f, 0.0f);

    glEnable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    if(model == 1) drawEggPoints(N);
    else if(model == 2) drawEggLines(N);
    else if(model == 3) drawEggTriangles(N);
    else if(model == 4) drawEggTextured(N);
    else if(model == 5) glutSolidTeapot(3.0);
    else if(model == 6) {
        glDisable(GL_LIGHTING);
        drawPyramid(2.0f);
        glEnable(GL_LIGHTING);
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
    gluPerspective(70, aspectRatio, 1.0, 30.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyPressed(unsigned char key, int x, int y) {
    switch (key)
    {
    case '1':
        currentAngles = light0Angles;
        break;
    case '2':
        currentAngles = light1Angles;
        break;
    case '3':
        currentAngles = viewerAngles;
        break;

    case 'p': model = 1; break;
    case 'w': model = 2; break;
    case 's': model = 3; break;
    case 't': model = 4; break;
    case 'c': model = 5; break;
    case 'v': model = 6; break;

    case 'n': drawNormals = !drawNormals;
    
    default:
        break;
    }

    glutPostRedisplay();
}

void init() {
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;

    GLbyte* texture = LoadTGAImage("tekstury/P4_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(texture);


    // współczynniki ka =[kar,kag,kab] dla światła otoczenia
    float mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego
    float mat_diffuse[]  = {1.0, 1.0, 1.0, 1.0};

    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego
    float mat_specular[] = {1.0, 1.0, 1.0, 1.0};

    // współczynnik n opisujący połysk powierzchni
    float mat_shininess  = {20.0};


    float light_ambient[] = {0.1, 0.0, 0.0, 1.0};
    float light_ambient2[] = {0.0, 0.0, 0.1, 1.0};
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    float light_diffuse[] = {1.0, 0.0, 0.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    float light_specular[]= {1.0, 1.0, 1.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    float light_diffuse2[] = {0.0, 0.0, 1.0, 1.0};

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

    float pos[4] = {0.0, 0.0, 0.0, 1.0};
    angles_to_coords(light0Angles, pos);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    angles_to_coords(light1Angles, pos);
    glLightfv(GL_LIGHT1, GL_POSITION, pos);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
/*************************************************************************************/
// Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_LIGHT1);     // włączenie źródła o numerze 1
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora
    glEnable(GL_TEXTURE_2D);
}

int main(int argc, char** argv) {
    printf("Grafika komputerowa lab5: OpenGL - oświetlenie\n");
    printf("Autor: Marcel Guzik\n\n");
    printf("mysz:\n");
    printf("\tLPM - przesuwa po powierzchni jaja\n");
    printf("\tPPM - oddala/przybliża do powierzchni jaja\n");
    printf("Wybor obiektu do przesuwania\n");
    printf("\t1 - światło czerwone\n");
    printf("\t2 - światło niebieskie\n");
    printf("\t3 - pozycja obserwatora\n");
    printf("modele:\n");
    printf("\tp - jajo, siatka punktów\n");
    printf("\tw - jajo, widok szkieletu (wireframe)\n");
    printf("\ts - jajo, kolorowa siatka trójkątów\n");
    printf("\tc - czajnik\n");
    printf("n - rysuj wektory normalne\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Grafika komputerowa lab4 - Marcel Guzik");

    generateEggVerticesWithNormals(N);

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
