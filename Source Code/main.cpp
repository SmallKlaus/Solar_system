#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include<iostream>
#include<math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Sphere {
    private:
    //class variables
    GLfloat radius;
    GLfloat center[3];
    GLint subdivs;
    GLfloat SphereColor[4];
    GLuint texture;
    GLboolean has_ring = GL_FALSE; 
    GLuint ring_width = 0;
    GLuint ring_radius = 0;
    GLboolean rotatesAroundItself =0;
    GLfloat aroundAngle{ 0 };
    public:
    void texturazing(const char* image_name) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char* data = stbi_load(image_name, &width, &height, &nrChannels, 0);
        if (data)
        {
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
        else
        {
            std::cout << "Failing to load texture!! (" <<image_name<<')' << std::endl;
        }
        stbi_image_free(data);
    }
    //Two constructors
    Sphere(GLfloat rad, GLfloat subs, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean rot) {
        radius = rad;
        center[0] = 0;
        center[1] = 0;
        center[2] = 0;
        SphereColor[0] = r;
        SphereColor[1] = g;
        SphereColor[2] = b;
        SphereColor[3] = a;
        subdivs = subs;
        rotatesAroundItself = rot;
    }
    Sphere(GLfloat rad, GLfloat subs, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean rot) {
        radius = rad;
        center[0] = x;
        center[1] = y;
        center[2] = z;
        SphereColor[0] = r;
        SphereColor[1] = g;
        SphereColor[2] = b;
        SphereColor[3] = a;
        subdivs = subs;
        rotatesAroundItself = rot;
    }
    Sphere(GLfloat rad, GLfloat subs, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean hasring, GLuint width, GLfloat r_rad, GLboolean rot) {
        radius = rad;
        center[0] = x;
        center[1] = y;
        center[2] = z;
        SphereColor[0] = r;
        SphereColor[1] = g;
        SphereColor[2] = b;
        SphereColor[3] = a;
        subdivs = subs;
        has_ring = hasring;
        ring_width = width;
        ring_radius = r_rad;
        rotatesAroundItself = rot;
    }
    GLfloat& get_CurrentaroundAngle() { return aroundAngle; }
    //Drawing a sphere using triangles
    void SphereDrawing() {
        GLfloat PI = M_PI;
        GLfloat x, y, z, alpha, beta;// Storage for coordinates and angles
        
        for (alpha = 0.0f; alpha < PI; alpha += PI / subdivs) {
            glBegin(GL_TRIANGLE_STRIP);
            for (beta = 0.0f; beta < 2.01 * PI; beta += PI / subdivs) {
                x = (float)(radius * cos(beta) * sin(alpha));
                y = (float)(radius * sin(beta) * sin(alpha));
                z = (float)(radius * cos(alpha));
                glTexCoord2f(beta / (2.0f * PI), alpha / PI);
                glNormal3f((x) / radius, (y) / radius, (z) /radius);
                glVertex3f(x, y, z);

                x = (float)(radius * cos(beta) * sin(alpha + PI / subdivs));
                y = (float)(radius * sin(beta) * sin(alpha + PI / subdivs));
                z = (float)(radius * cos(alpha + PI / subdivs)); 
                glTexCoord2f(beta / (2.0f * PI), alpha / PI + 1.0f / subdivs);
                glNormal3f((x) / radius, (y) / radius, (z) / radius);
                glVertex3f(x, y, z);
            }
            glEnd();
        }
    }
    void Draw() {
        glTranslatef(center[0], center[1], center[2]);
        glMaterialfv(GL_FRONT, GL_AMBIENT, SphereColor);
        if (has_ring) {
            GLfloat rad = ring_radius;
            GLfloat increment = M_PI / 48;
            GLfloat max = 2 * M_PI;
            glLineWidth(1);
            
            glRotatef(aroundAngle, 0, 1, 0);
            for (int j = 0; j < ring_width; j++) {
                glPushMatrix();
                glRotatef(20, 0, 0, 1);
                glBegin(GL_LINE_LOOP);
                for (GLfloat i = 0; i < max; i += increment)
                {
                    glVertex3f(cos((double)i) * rad, 0, sin((double)i) * rad);
                }
                glEnd();
                glPopMatrix();
                rad += 0.2;
            }
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glPushMatrix();
        if (rotatesAroundItself) {
            glRotatef(aroundAngle, 0, 1, 0);
        }
        SphereDrawing();
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    GLfloat get_radius() { return radius; }
    GLfloat* get_center() { return center; }
    GLfloat* get_color() { return SphereColor; }
    GLuint& get_texID() { return texture; }
};

class Planet : public Sphere {
    private:
    GLfloat CurrentOrbitAngle{0.0f};
    public:
    Planet(GLfloat rad, GLfloat subs, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean rot) :
        Sphere(rad, subs, x, y, z, r, g, b, a, rot) {
    }
    Planet(GLfloat rad, GLfloat subs, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean hasring, GLuint width, GLfloat r_rad, GLboolean rot) :
        Sphere(rad, subs, x, y, z, r, g, b, a, hasring, width, r_rad, rot) {
    }
    GLfloat& get_CurrentOrbitAngle() { return CurrentOrbitAngle; }
    void DrawTrajectory() 
    {   
        GLfloat Grey[4] = {0.5, 0.5, 0.5, 0};
        GLfloat rad = this->get_center()[0];
        GLfloat increment = M_PI / 48;
        GLfloat max = 2 * M_PI;
        glLineWidth(0.1);
        glMaterialfv(GL_FRONT, GL_AMBIENT, Grey);
        glBegin(GL_LINE_LOOP);
        for (GLfloat i = 0; i < max; i += increment)
        {
            glVertex3f(cos((double)i) * rad, 0, sin((double)i) * rad);
        }
        glEnd();

    }
    void DrawPlanet() {
        this->DrawTrajectory();
        glPushMatrix();
        Draw();
        glPopMatrix();
    }
    void RotateAroundTheSun() {
        glPushMatrix();
        glRotatef(CurrentOrbitAngle, 0, 1, 0);
        this->DrawPlanet();
        glPopMatrix();
    }
};

void My_Display();
void My_Reshape(int, int);
void Key_Func(int, int, int);
void My_Mouse(int, int, int, int);
void My_Motion(int, int);
void Timer(int);

GLfloat frequency;
//keyboard movement vars;
GLdouble xmove, ymove, zmove = 0.0;
//mouse rotation vars;
bool has_rotation_started = false;
GLfloat xstart, ystart = 0.0;
GLfloat xrotate, yrotate = 15.0;
GLfloat zoom = -100;
//drawing objects;
Sphere space = Sphere(400.0f, 200, 0.67, 0.84, 0.9, 0, GL_FALSE);
Sphere SUN = Sphere(20.0f, 180.0f,0,0,0, 0.8f, 0.4f, 0, 0, GL_TRUE);
Planet mercury = Planet(0.21, 100, 23.9, 0.0, 0, 0.7, 0.7, 0.7, 0, GL_TRUE);
Planet venus = Planet(0.6, 100, 27.2, 0,0, 0.7, 0.3, 0.3, 0, GL_TRUE);
Planet earth = Planet(0.6, 100, 30, 0, 0, 0.3, 0.3, 0.7, 0, GL_TRUE);
Planet mars = Planet(0.3, 100, 35.2, 0,0, 0.9, 0.2, 0.2, 0, GL_TRUE);
Planet jupiter = Planet(6.12, 100, 72, 0, 0, 0.4, 0.4, 0.4, 0, GL_TRUE, 5, 6.5, GL_TRUE);
Planet saturn = Planet(5.1, 100, 115.4, 0,0, 0.9, 0.8, 0.6, 0, GL_TRUE, 5, 5.6, GL_TRUE);
Planet uranus = Planet(2.1, 100, 210.2, 0, 0, 0.67, 0.84, 0.9, 0, GL_TRUE, 10, 2.4, GL_TRUE);
Planet neptune = Planet(2.1, 100,320.6, 0,0, 0.2, 0.2, 1, 0, GL_TRUE, 5,  2.4, GL_TRUE);

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    space.texturazing("space.jpg");
    SUN.texturazing("sun.jpg");
    mercury.texturazing("mercury.jpg");
    venus.texturazing("venus.jpg");
    earth.texturazing("earth.jpg");
    mars.texturazing("mars.jpg");
    jupiter.texturazing("jupiter.jpg");
    saturn.texturazing("saturn.jpg");
    uranus.texturazing("uranus.jpg");
    neptune.texturazing("neptune.jpg");
    glBindTexture(GL_TEXTURE_2D, 0);
    
    GLfloat ambient_light[] = {0.9,0.9,0.9,1};
    GLfloat mat_specular[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
    GLfloat low_shininess[] = { 5.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}


int main(int argc, char** argv)
{
   {
        std::cout << "This is a simple simulation of our solar system, you can input the years to go by and the speed at which the animation plays.";
        std::cout << "You can move the scene using the arrow keys, rotate using the left mouse button and dragging around, and zoom in and out using the mouse wheel." << std::endl;
        std::cout << "Enter the frequency at which earth orbits around the sun (you can increase it and decrease it mid animation using page down and page up keys): ";
        std::cin >> frequency;
        glutInit(&argc, argv);
        //Window Initialization
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowPosition(500, 80);
        glutInitWindowSize(920, 920);
        glutCreateWindow("Experiments Window");
        init();
        //Display Callback
        glutDisplayFunc(My_Display);

        //Reshape Callback
        glutReshapeFunc(My_Reshape);

        //Timer Callback
        glutTimerFunc(0, Timer, 0);

        //KeyPress Callback
        glutSpecialFunc(Key_Func);

        //MouseButton Callback
        glutMouseFunc(My_Mouse);

        //MouseMotion Callback
        glutMotionFunc(My_Motion);
        
        glutMainLoop();
    }
}

void My_Display()
{
    
    //Clearing The Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat light_pos[] = { 0-xmove, 100-ymove, 0-zmove, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    
    //Moving around
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0+xmove, 0+ymove,zoom+ zmove);
    glRotatef(20+xrotate, 1, 0, 0);
    glRotatef(yrotate, 0, 1, 0);
    space.Draw();
    SUN.Draw();
    mercury.RotateAroundTheSun();
    venus.RotateAroundTheSun();
    earth.RotateAroundTheSun();
    mars.RotateAroundTheSun();
    jupiter.RotateAroundTheSun();
    saturn.RotateAroundTheSun();
    uranus.RotateAroundTheSun();
    neptune.RotateAroundTheSun();
    glPopMatrix();
    //Displaying on Screen
    glutSwapBuffers();
}

void My_Reshape(int w, int h)
{
    //Viewport
    glViewport(0, 0, w, h);

    //Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Perspective Projection
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 2.0, 800.0);

    //Default Matrix Mode
    glMatrixMode(GL_MODELVIEW);

}

void Key_Func(int but, int x, int y)
{
    switch (but) {
    case GLUT_KEY_UP:
        ymove -= 2;
        std::cout << ymove << std::endl;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        ymove += 2;
        std::cout << zmove << std::endl;
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        xmove += 2;
        std::cout << xmove << std::endl;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        xmove -= 2;
        std::cout << xmove << std::endl;
        glutPostRedisplay();
        break;
    case GLUT_KEY_PAGE_UP:
        frequency += 0.04;
        std::cout << frequency << std::endl;
        break;
    case GLUT_KEY_PAGE_DOWN:
        if (frequency-0.04 >= 0) {
            frequency -= 0.04;
            std::cout << frequency << std::endl;
        }
        else {
            frequency = 0;
            std::cout << "frequncy is 0" << std::endl;
        }
            
        break;
    }
}

void My_Mouse(int button, int state, int _x, int _y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            has_rotation_started = true;
            xstart = _x;
            ystart = _y;
        }
        if (state == GLUT_UP) {
            has_rotation_started = false;
        }
        break;
    case 3:
        if (state == GLUT_DOWN) {
            zoom += 10;
        }
        break;
    case 4:
        if (state == GLUT_UP) {
            zoom -= 5;
        }
        break;
    }
    

}

void My_Motion(int _x, int _y) {
    
    if (has_rotation_started) {
        xrotate += _y - ystart;
        yrotate += _x - xstart;
        xstart = _x;
        ystart = _y;
        glutPostRedisplay();
    }
}

void Timer(int) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, Timer, 0);
    SUN.get_CurrentaroundAngle() += (frequency / 0.0739726) * 360 / 60;
    mercury.get_CurrentOrbitAngle() += (frequency / 0.241) * 360 / 60;
    mercury.get_CurrentaroundAngle() += (frequency / 0.150685) * 360 / 60;
    venus.get_CurrentOrbitAngle() += (frequency / 0.6164) * 360 / 60;
    venus.get_CurrentaroundAngle() -= (frequency / 0.665753) * 360 / 60;
    earth.get_CurrentOrbitAngle() += frequency * 360 / 60;
    earth.get_CurrentaroundAngle() += (frequency / 0.00273973) * 360 / 60;
    mars.get_CurrentOrbitAngle() += (frequency / 1.8821) * 360 / 60;
    mars.get_CurrentaroundAngle() += (frequency / 0.00273973) * 360 / 60;
    jupiter.get_CurrentOrbitAngle() += (frequency / 12) * 360 / 60;
    jupiter.get_CurrentaroundAngle() += (frequency / 0.00109589) * 360 / 60;
    saturn.get_CurrentOrbitAngle() += (frequency / 29) * 360 / 60;
    saturn.get_CurrentaroundAngle() += (frequency / 0.00109589) * 360 / 60;
    uranus.get_CurrentOrbitAngle() += (frequency / 84) * 360 / 60;
    uranus.get_CurrentaroundAngle() += (frequency / 0.00194064) * 360 / 60;
    neptune.get_CurrentOrbitAngle() += (frequency / 165) * 360 / 60;
    neptune.get_CurrentaroundAngle() += (frequency / 0.00182648) * 360 / 60;
    
}