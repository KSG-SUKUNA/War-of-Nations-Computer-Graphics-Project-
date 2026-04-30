#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PI    3.14159265358979f
#define TWO_PI 6.28318530717959f

static int   g_scene  = 1;
static float g_time   = 0.0f;
static float g_dt     = 0.0f;
static int   g_lastMs = 0;

static float armEndXApprox(float sx, float sc2);
static float rf(float lo,float hi)
{
    return lo+(hi-lo)*(rand()/(float)RAND_MAX);
}
static int   ri(int lo,int hi)
{
    return lo+rand()%(hi-lo+1);
}

// ================================================================
// FORWARD DECLARATIONS
// ================================================================
static void drawCircle(float cx,float cy,float r,int segs,int fill);
static void drawRect(float x,float y,float w,float h,int fill);
static void drawText(float x,float y,const char*s,float sc);

// ================================================================
// PARTICLES
// ================================================================
#define MAX_P 1500
struct Particle
{
    float x,y,vx,vy,ax,ay;
    float r,g,b,a;
    float life,maxLife,size;
    int   type,alive;
};
static Particle P[MAX_P];

static void pSpawn(float x,float y,float vx,float vy,float ax,float ay,
                   float r,float g,float b,float a,
                   float life,float sz,int type)
{
    for(int i=0; i<MAX_P; i++)
    {
        if(!P[i].alive)
        {
            P[i].x=x;
            P[i].y=y;
            P[i].vx=vx;
            P[i].vy=vy;
            P[i].ax=ax;
            P[i].ay=ay;
            P[i].r=r;
            P[i].g=g;
            P[i].b=b;
            P[i].a=a;
            P[i].life=life;
            P[i].maxLife=life;
            P[i].size=sz;
            P[i].type=type;
            P[i].alive=1;
            return;
        }
    }
}
static void pUpdate(float dt)
{
    for(int i=0; i<MAX_P; i++)
    {
        if(!P[i].alive)continue;
        P[i].vx+=P[i].ax*dt;
        P[i].vy+=P[i].ay*dt;
        P[i].x+=P[i].vx*dt;
        P[i].y+=P[i].vy*dt;
        P[i].life-=dt;
        if(P[i].life<=0)P[i].alive=0;
    }
}
static void pDraw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for(int i=0; i<MAX_P; i++)
    {
        if(!P[i].alive)continue;
        float t=P[i].life/P[i].maxLife;
        float sz=P[i].size;
        float alpha=P[i].a*t;
        if(P[i].type==0)
        {
            float hg=P[i].g*t;
            glColor4f(P[i].r*0.5f,hg*0.3f,0,alpha*0.3f);
            glBegin(GL_POLYGON);
            for(int k=0; k<12; k++)
            {
                float a=TWO_PI*k/12;
                glVertex2f(P[i].x+cosf(a)*sz*1.6f,P[i].y+sinf(a)*sz*1.6f);
            }
            glEnd();
            glColor4f(P[i].r,hg,0,alpha);
            glBegin(GL_POLYGON);
            for(int k=0; k<12; k++)
            {
                float a=TWO_PI*k/12;
                glVertex2f(P[i].x+cosf(a)*sz,P[i].y+sinf(a)*sz);
            }
            glEnd();
        }
        else if(P[i].type==1)
        {
            sz*=(1.0f+(1.0f-t)*1.5f);
            glColor4f(P[i].r,P[i].g,P[i].b,alpha*0.55f);
            glBegin(GL_POLYGON);
            for(int k=0; k<18; k++)
            {
                float a=TWO_PI*k/18;
                float jit=0.85f+0.15f*sinf(a*3+P[i].life*2);
                glVertex2f(P[i].x+cosf(a)*sz*jit,P[i].y+sinf(a)*sz*jit);
            }
            glEnd();
        }
        else if(P[i].type==2)
        {
            glColor4f(1,0.9f,0.3f,alpha);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            glVertex2f(P[i].x,P[i].y);
            glVertex2f(P[i].x-P[i].vx*g_dt*4,P[i].y-P[i].vy*g_dt*4);
            glEnd();
        }
        else if(P[i].type==3)
        {
            glColor4f(P[i].r,P[i].g,P[i].b,alpha);
            float angle=P[i].life*5;
            glPushMatrix();
            glTranslatef(P[i].x,P[i].y,0);
            glRotatef(angle*57.3f,0,0,1);
            drawRect(-sz/2,-sz/2,sz,sz,1);
            glPopMatrix();
        }
        else if(P[i].type==4)
        {
            glColor4f(0.7f,0.65f,0.6f,alpha*0.7f);
            glBegin(GL_POLYGON);
            for(int k=0; k<6; k++)
            {
                float a=TWO_PI*k/6;
                glVertex2f(P[i].x+cosf(a)*sz,P[i].y+sinf(a)*sz);
            }
            glEnd();
        }
        else if(P[i].type==5)
        {
            glColor4f(0.7f,0,0,alpha);
            glBegin(GL_POLYGON);
            for(int k=0; k<10; k++)
            {
                float a=TWO_PI*k/10;
                glVertex2f(P[i].x+cosf(a)*sz,P[i].y+sinf(a)*sz);
            }
            glEnd();
        }
        else if(P[i].type==6)
        {
            glColor4f(P[i].r,P[i].g,P[i].b,alpha*0.8f);
            glBegin(GL_POLYGON);
            for(int k=0; k<8; k++)
            {
                float a=TWO_PI*k/8;
                glVertex2f(P[i].x+cosf(a)*sz,P[i].y+sinf(a)*sz);
            }
            glEnd();
        }
    }
    glLineWidth(1.0f);
}

static void explode(float x,float y,float power,int big)
{
    int fc=big?70:35,sc2=big?28:14,dc=big?22:10;
    for(int i=0; i<fc; i++)
    {
        float ang=rf(0,TWO_PI),sp=rf(1,power*4);
        float r=rf(0.9f,1.0f),gr=rf(0.2f,0.65f);
        pSpawn(x,y,cosf(ang)*sp,sinf(ang)*sp+rf(0.5f,2),0,-4,r,gr,0,1,rf(0.4f,1.8f),rf(3,big?11:8),0);
    }
    for(int i=0; i<sc2; i++)
    {
        float ang=rf(0,TWO_PI),sp=rf(3,power*9);
        pSpawn(x,y,cosf(ang)*sp,sinf(ang)*sp,0,-6,1,0.9f,0.3f,1,rf(0.3f,1.0f),1.5f,2);
    }
    for(int i=0; i<sc2; i++)
    {
        float v=rf(0.5f,2.5f),ang=rf(0,TWO_PI);
        float gr=rf(0.15f,0.38f);
        pSpawn(x+rf(-3,3),y+rf(0,5),cosf(ang)*v,v+rf(0.5f,2),0,0,gr,gr,gr,0.8f,rf(1.5f,4.5f),rf(7,big?18:13),1);
    }
    for(int i=0; i<dc; i++)
    {
        float ang=rf(0,TWO_PI),sp=rf(2,power*5);
        pSpawn(x,y,cosf(ang)*sp,sinf(ang)*sp+2,0,-8,rf(0.2f,0.4f),rf(0.15f,0.28f),0.1f,1,rf(0.5f,2.8f),rf(2,6),3);
    }
}

// ================================================================
// DRAW PRIMITIVES
// ================================================================
static void drawCircle(float cx,float cy,float r,int segs,int fill)
{
    if(fill)glBegin(GL_POLYGON);
    else glBegin(GL_LINE_LOOP);
    for(int i=0; i<segs; i++)
    {
        float a=TWO_PI*i/segs;
        glVertex2f(cx+cosf(a)*r,cy+sinf(a)*r);
    }
    glEnd();
}
static void drawRect(float x,float y,float w,float h,int fill)
{
    if(fill)glBegin(GL_QUADS);
    else glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);
    glVertex2f(x+w,y);
    glVertex2f(x+w,y+h);
    glVertex2f(x,y+h);
    glEnd();
}
static void drawText(float x,float y,const char*s,float sc)
{
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(sc*0.1f,sc*0.1f,1);
    for(; *s; s++)glutStrokeCharacter(GLUT_STROKE_ROMAN,*s);
    glPopMatrix();
}

// ================================================================
// BUILDINGS - ALL windows lit, per-hit-spot fires
// ================================================================
#define MAX_B 17
#define MAX_HIT_SPOTS 10
struct Building
{
    float x,w,h,r,g,b;
    int   dest;
    float firePhase;
    int   winLit[15][20];
    float B_seed;
    float hitX[MAX_HIT_SPOTS];
    float hitY[MAX_HIT_SPOTS];
    int   hitCount;
};
static Building B[MAX_B];

static void initBuildings(int allDestroyed)
{
    srand(1234);
    float bx=-160.0f;
    float palettes[5][3]= {{0.3f,0.3f,0.35f},{0.28f,0.25f,0.28f},{0.22f,0.28f,0.3f},
        {0.35f,0.32f,0.28f},{0.25f,0.22f,0.3f}
    };
    for(int i=0; i<MAX_B; i++)
    {
        B[i].x=bx;
        B[i].w=rf(10,20);
        B[i].h=rf(18,60);
        int p=i%5;
        B[i].r=palettes[p][0];
        B[i].g=palettes[p][1];
        B[i].b=palettes[p][2];
        B[i].dest=allDestroyed;
        B[i].firePhase=rf(0,TWO_PI);
        B[i].hitCount=0;
        for(int k=0; k<MAX_HIT_SPOTS; k++)
        {
            B[i].hitX[k]=0;
            B[i].hitY[k]=0;
        }
        // ALL windows lit initially
        for(int wc=0; wc<15; wc++)
            for(int wr=0; wr<20; wr++)
                B[i].winLit[wc][wr]=1;
        B[i].B_seed=rf(0,100);
        bx+=B[i].w+rf(2,8);
    }
    srand((unsigned)time(NULL));
}

static void drawBuildingWindows(int i,float gy)
{
    Building&bd=B[i];
    float wx=bd.x+2.5f;
    int col=0;
    while(wx<bd.x+bd.w-4&&col<15)
    {
        float wy=gy+3.0f;
        int row=0;
        while(wy<gy+bd.h-4&&row<20)
        {
            if(bd.winLit[col][row])
            {
                // All windows glow warm yellow/orange
                float flicker=0.82f+0.18f*sinf(g_time*14+bd.B_seed+col*1.7f+row*2.5f);
                // Random colour variety: some cooler blue-white, most warm yellow
                float wr2=(col+row)%3==0?0.72f:0.95f;
                float wg2=(col+row)%3==0?0.80f:0.88f;
                float wb2=(col+row)%3==0?0.95f:0.42f;
                glColor4f(wr2*flicker,wg2*flicker,wb2*flicker,1.0f);
                glBegin(GL_QUADS);
                glVertex2f(wx,wy);
                glVertex2f(wx+2.2f,wy);
                glVertex2f(wx+2.2f,wy+3.0f);
                glVertex2f(wx,wy+3.0f);
                glEnd();
                // Outer glow halo
                glColor4f(wr2,wg2,wb2*0.5f,0.18f*flicker);
                glBegin(GL_QUADS);
                glVertex2f(wx-0.8f,wy-0.8f);
                glVertex2f(wx+3.0f,wy-0.8f);
                glVertex2f(wx+3.0f,wy+3.8f);
                glVertex2f(wx-0.8f,wy+3.8f);
                glEnd();
            }
            else
            {
                glColor4f(0.05f,0.05f,0.08f,1);
                glBegin(GL_QUADS);
                glVertex2f(wx,wy);
                glVertex2f(wx+2.2f,wy);
                glVertex2f(wx+2.2f,wy+3.0f);
                glVertex2f(wx,wy+3.0f);
                glEnd();
            }
            wy+=5.0f;
            row++;
        }
        wx+=4.5f;
        col++;
    }
}

static void drawSpotFire(float fx,float fy,float phase)
{
    float fp=phase+g_time*3.2f;
    for(int fl=0; fl<3; fl++)
    {
        float fh=(7+3*fl)*(0.55f+0.45f*sinf(fp+fl));
        float fw=3.5f+fl*1.5f;
        glColor4f(0.9f,0.28f,0.0f,0.88f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx-fw,fy);
        glVertex2f(fx+fw,fy);
        glVertex2f(fx+sinf(fp*1.4f+fl)*2,fy+fh);
        glEnd();
        glColor4f(1.0f,0.65f,0.05f,0.72f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx-fw*0.6f,fy);
        glVertex2f(fx+fw*0.6f,fy);
        glVertex2f(fx+sinf(fp*2.2f+fl)*1.5f,fy+fh*0.78f);
        glEnd();
        glColor4f(1.0f,0.95f,0.5f,0.5f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx-fw*0.3f,fy);
        glVertex2f(fx+fw*0.3f,fy);
        glVertex2f(fx+sinf(fp*3.1f+fl)*0.8f,fy+fh*0.52f);
        glEnd();
    }
    glColor4f(1.0f,0.38f,0.0f,0.07f+0.04f*sinf(fp));
    drawCircle(fx,fy+8,13,16,1);
}

static void drawBuildings(float gy)
{
    for(int i=0; i<MAX_B; i++)
    {
        Building&bd=B[i];
        if(!bd.dest)
        {
            // shadow
            glColor4f(0,0,0,0.25f);
            glBegin(GL_QUADS);
            glVertex2f(bd.x+2,gy-2);
            glVertex2f(bd.x+bd.w+2,gy-2);
            glVertex2f(bd.x+bd.w,gy);
            glVertex2f(bd.x,gy);
            glEnd();
            // face gradient
            glBegin(GL_QUADS);
            glColor3f(bd.r*0.6f,bd.g*0.6f,bd.b*0.6f);
            glVertex2f(bd.x,gy);
            glVertex2f(bd.x+bd.w,gy);
            glColor3f(bd.r,bd.g,bd.b);
            glVertex2f(bd.x+bd.w,gy+bd.h);
            glVertex2f(bd.x,gy+bd.h);
            glEnd();
            // side face
            glBegin(GL_QUADS);
            glColor3f(bd.r*0.4f,bd.g*0.4f,bd.b*0.4f);
            glVertex2f(bd.x+bd.w,gy);
            glVertex2f(bd.x+bd.w+3,gy+3);
            glColor3f(bd.r*0.7f,bd.g*0.7f,bd.b*0.7f);
            glVertex2f(bd.x+bd.w+3,gy+bd.h+3);
            glVertex2f(bd.x+bd.w,gy+bd.h);
            glEnd();
            // top
            glBegin(GL_QUADS);
            glColor3f(bd.r*1.1f,bd.g*1.1f,bd.b*1.1f);
            glVertex2f(bd.x,gy+bd.h);
            glVertex2f(bd.x+bd.w,gy+bd.h);
            glVertex2f(bd.x+bd.w+3,gy+bd.h+3);
            glVertex2f(bd.x+3,gy+bd.h+3);
            glEnd();
            drawBuildingWindows(i,gy);
            // window light spill onto building face
            glColor4f(0.9f,0.82f,0.35f,0.04f);
            drawRect(bd.x,gy,bd.w,bd.h,1);
            // antenna
            glColor3f(bd.r*0.8f,bd.g*0.8f,bd.b*0.8f);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            glVertex2f(bd.x+bd.w/2,gy+bd.h+3);
            glVertex2f(bd.x+bd.w/2,gy+bd.h+9);
            glEnd();
            float blink=sinf(g_time*2+bd.B_seed);
            glColor3f(blink>0?0.9f:0.3f,0,0);
            drawCircle(bd.x+bd.w/2,gy+bd.h+9,0.8f,8,1);
            glColor4f(0,0,0,0.5f);
            glLineWidth(0.8f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(bd.x,gy);
            glVertex2f(bd.x+bd.w,gy);
            glVertex2f(bd.x+bd.w,gy+bd.h);
            glVertex2f(bd.x,gy+bd.h);
            glEnd();
            for(int h=0; h<bd.hitCount; h++)
            {
                drawSpotFire(bd.hitX[h],bd.hitY[h],bd.firePhase+h*1.7f);
                if(rf(0,1)<0.12f)
                    pSpawn(bd.hitX[h]+rf(-3,3),bd.hitY[h]+10,rf(-0.3f,0.3f),rf(0.8f,2.5f),0,0,
                           rf(0.18f,0.35f),rf(0.18f,0.35f),rf(0.15f,0.3f),0.85f,rf(1.8f,4.5f),rf(6,13),1);
            }
        }
        else
        {
            // RUBBLE
            float fp=bd.firePhase+g_time*3;
            glColor3f(0.28f,0.2f,0.15f);
            glBegin(GL_POLYGON);
            glVertex2f(bd.x,gy);
            glVertex2f(bd.x+bd.w*0.15f,gy+8);
            glVertex2f(bd.x+bd.w*0.3f,gy+4);
            glVertex2f(bd.x+bd.w*0.45f,gy+12);
            glVertex2f(bd.x+bd.w*0.6f,gy+5);
            glVertex2f(bd.x+bd.w*0.75f,gy+9);
            glVertex2f(bd.x+bd.w*0.9f,gy+3);
            glVertex2f(bd.x+bd.w,gy);
            glEnd();
            glColor3f(0.22f,0.17f,0.12f);
            for(int rk=0; rk<5; rk++)
            {
                float rx=bd.x+bd.w*0.1f*rk+2,ry=gy+rf(0,6);
                glBegin(GL_TRIANGLES);
                glVertex2f(rx,ry);
                glVertex2f(rx+3,ry);
                glVertex2f(rx+1.5f,ry+3);
                glEnd();
            }
            for(int fl=0; fl<3; fl++)
            {
                float fx=bd.x+bd.w*(0.25f+fl*0.25f);
                float fh=(8+4*fl)*(0.6f+0.4f*sinf(fp+fl));
                float fwid=4+2*fl;
                glColor4f(0.9f,0.3f,0.0f,0.85f);
                glBegin(GL_TRIANGLES);
                glVertex2f(fx-fwid,gy+6);
                glVertex2f(fx+fwid,gy+6);
                glVertex2f(fx+sinf(fp*1.3f+fl)*2,gy+6+fh);
                glEnd();
                glColor4f(1.0f,0.7f,0.1f,0.7f);
                glBegin(GL_TRIANGLES);
                glVertex2f(fx-fwid*0.6f,gy+6);
                glVertex2f(fx+fwid*0.6f,gy+6);
                glVertex2f(fx+sinf(fp*2.1f+fl)*1.5f,gy+6+fh*0.8f);
                glEnd();
                glColor4f(1.0f,1.0f,0.6f,0.5f);
                glBegin(GL_TRIANGLES);
                glVertex2f(fx-fwid*0.3f,gy+6);
                glVertex2f(fx+fwid*0.3f,gy+6);
                glVertex2f(fx+sinf(fp*3.0f+fl),gy+6+fh*0.6f);
                glEnd();
                glColor4f(1.0f,0.4f,0.0f,0.08f+0.04f*sinf(fp+fl));
                drawCircle(fx,gy+6+fh*0.4f,(fwid+fh)*0.5f,16,1);
            }
        }
        glLineWidth(1.0f);
    }
}

// ================================================================
// STREET LAMPS
// ================================================================
#define MAX_LAMPS 18
struct Lamp
{
    float x,y;
    int broken;
};
static Lamp lamps[MAX_LAMPS];

static void initLamps(float gy)
{
    float spacing=22.0f;
    float startX=-155.0f;
    for(int i=0; i<MAX_LAMPS; i++)
    {
        lamps[i].x=startX+i*spacing;
        lamps[i].y=gy;
        lamps[i].broken=0;
    }
}

static void drawLamp(int i)
{
    Lamp&L=lamps[i];
    float px=L.x, py=L.y;
    glColor3f(0.42f,0.42f,0.45f);
    glLineWidth(2.2f);
    glBegin(GL_LINES);
    glVertex2f(px,py);
    glVertex2f(px,py+16);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(px,py+16);
    glVertex2f(px+4,py+16);
    glEnd();
    glLineWidth(1.0f);
    if(!L.broken)
    {
        glColor3f(0.35f,0.35f,0.38f);
        drawRect(px+2,py+14,4,3,1);
        float flicker=0.8f+0.2f*sinf(g_time*8+i);
        glColor4f(0.98f,0.95f,0.72f,0.55f*flicker);
        glBegin(GL_TRIANGLES);
        glVertex2f(px+2,py+14);
        glVertex2f(px+6,py+14);
        glVertex2f(px-6,py+1);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f(px+2,py+14);
        glVertex2f(px+6,py+14);
        glVertex2f(px+14,py+1);
        glEnd();
        glColor4f(0.98f,0.92f,0.6f,0.3f*flicker);
        drawCircle(px+4,py+15,5,12,1);
        glColor4f(0.98f,0.92f,0.6f,0.08f*flicker);
        glBegin(GL_POLYGON);
        for(int k=0; k<14; k++)
        {
            float a=TWO_PI*k/14;
            glVertex2f(px+4+cosf(a)*12,py+sinf(a)*2.5f);
        }
        glEnd();
    }
    else
    {
        glColor3f(0.25f,0.25f,0.28f);
        drawRect(px+2,py+14,4,3,1);
        if(rf(0,1)<0.3f)
        {
            float sx=px+4,sy=py+14;
            pSpawn(sx,sy,rf(-2,2),rf(1,3),0,-5,1,0.8f,0.2f,1,rf(0.1f,0.3f),1.5f,2);
        }
    }
}

static void drawLamps()
{
    for(int i=0; i<MAX_LAMPS; i++) drawLamp(i);
}

// ================================================================
// STREET FILLERS - benches, trash cans, fire hydrants, mailboxes, trees
// ================================================================
#define MAX_FILLERS 30
struct StreetFiller
{
    float x,y;
    int type; // 0=bench 1=trash 2=hydrant 3=mailbox 4=tree 5=newsstand 6=bus_stop
    float seed;
};
static StreetFiller fillers[MAX_FILLERS];
static int fillerCount=0;

static void initFillers(float gy)
{
    fillerCount = 0;
    srand(4567);

    // 1. Set the exact X coordinates for your BENCHES
    float benchPositions[] = {-140.0f, -110.0f, -80.0f, 80.0f, 110.0f, 140.0f};
    int numBenches = 10;

    // Loop through and place all benches
    for(int i = 0; i < numBenches; i++)
    {
        fillers[fillerCount].x = benchPositions[i];
        fillers[fillerCount].y = gy - 18.2f;
        fillers[fillerCount].type = 0; // Type 0 is Bench
        fillers[fillerCount].seed = rf(0,100);
        fillerCount++;
    }

    // 2. Set the exact X coordinates for your TRASH CANS
    // Placed slightly to the right of each bench
    float trashPositions[] = {-130.0f, -100.0f, -70.0f, 90.0f, 120.0f, 150.0f};
    int numTrashCans = 6;

    // Loop through and place all trash cans
    for(int i = 0; i < numTrashCans; i++)
    {
        fillers[fillerCount].x = trashPositions[i];
        fillers[fillerCount].y = gy - 4.2f;
        fillers[fillerCount].type = 1; // Type 1 is Trash Can
        fillers[fillerCount].seed = rf(0,100);
        fillerCount++;
    }

    // (Optional) Add a couple of trees at the far edges
    fillers[fillerCount].x = -155.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = -55.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = -35.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = -15.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = 15.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = 35.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = 55.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;
    fillers[fillerCount].x = 155.0f;
    fillers[fillerCount].y = gy - 18.2f;
    fillers[fillerCount].type = 4;
    fillerCount++;


    srand((unsigned)time(NULL));
}

// Draw a bench
static void drawBench(float x,float y)
{
    // legs
    glColor3f(0.45f,0.32f,0.18f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x-3.5f,y);
    glVertex2f(x-3.5f,y+3.5f);
    glVertex2f(x+3.5f,y);
    glVertex2f(x+3.5f,y+3.5f);
    glEnd();
    // seat
    glColor3f(0.55f,0.40f,0.22f);
    drawRect(x-4.5f,y+3.0f,9,1.2f,1);
    // back
    drawRect(x-4.5f,y+4.5f,9,0.9f,1);
    // back supports
    glColor3f(0.45f,0.32f,0.18f);
    glBegin(GL_LINES);
    glVertex2f(x-3.0f,y+3.0f);
    glVertex2f(x-3.0f,y+5.4f);
    glVertex2f(x+3.0f,y+3.0f);
    glVertex2f(x+3.0f,y+5.4f);
    glEnd();
    glLineWidth(1.0f);
}

// Draw a trash can
static void drawTrash(float x,float y,float seed)
{
    float wobble=sinf(g_time*0.5f+seed)*0.3f;
    // body
    glColor3f(0.32f,0.32f,0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(x-2.0f,y);
    glVertex2f(x+2.0f,y);
    glVertex2f(x+1.7f+wobble,y+5.5f);
    glVertex2f(x-1.7f+wobble,y+5.5f);
    glEnd();
    // lid
    glColor3f(0.42f,0.42f,0.45f);
    drawRect(x-2.2f+wobble,y+5.3f,4.4f,1.0f,1);
    // handle
    glColor3f(0.28f,0.28f,0.30f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x-0.8f+wobble,y+6.3f);
    glVertex2f(x-0.8f+wobble,y+7.0f);
    glVertex2f(x+0.8f+wobble,y+7.0f);
    glVertex2f(x+0.8f+wobble,y+6.3f);
    glEnd();
    // stripes
    glColor4f(1,0.9f,0,0.5f);
    drawRect(x-2.0f,y+1.2f,4.0f,0.5f,1);
    drawRect(x-2.0f,y+3.2f,4.0f,0.5f,1);
}

// Draw a fire hydrant
static void drawHydrant(float x,float y)
{
    // base
    glColor3f(0.75f,0.10f,0.10f);
    glBegin(GL_POLYGON);
    glVertex2f(x-2.0f,y);
    glVertex2f(x+2.0f,y);
    glVertex2f(x+1.6f,y+2.0f);
    glVertex2f(x-1.6f,y+2.0f);
    glEnd();
    // body
    glBegin(GL_POLYGON);
    glVertex2f(x-1.6f,y+2.0f);
    glVertex2f(x+1.6f,y+2.0f);
    glVertex2f(x+1.4f,y+5.0f);
    glVertex2f(x-1.4f,y+5.0f);
    glEnd();
    // cap
    glColor3f(0.9f,0.18f,0.10f);
    drawCircle(x,y+5.0f,1.5f,12,1);
    // side nozzles
    glColor3f(0.65f,0.08f,0.08f);
    drawRect(x-3.0f,y+3.0f,1.5f,1.2f,1);
    drawRect(x+1.5f,y+3.0f,1.5f,1.2f,1);
    // shine
    glColor4f(1,0.4f,0.4f,0.3f);
    drawCircle(x-0.5f,y+3.5f,0.5f,8,1);
}

// Draw a mailbox
static void drawMailbox(float x,float y)
{
    // post
    glColor3f(0.38f,0.38f,0.42f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x,y);
    glVertex2f(x,y+4.5f);
    glEnd();
    glLineWidth(1.0f);
    // box body
    glColor3f(0.15f,0.20f,0.70f);
    drawRect(x-2.5f,y+4.5f,5,4,1);
    // rounded top
    glBegin(GL_POLYGON);
    for(int k=0; k<=10; k++)
    {
        float a=PI*k/10;
        glVertex2f(x+cosf(a)*2.5f,y+8.5f+sinf(a)*1.5f);
    }
    glEnd();
    // slot
    glColor4f(0,0,0,0.7f);
    drawRect(x-1.5f,y+7.0f,3.0f,0.5f,1);
    // USPS text hint
    glColor4f(1,1,1,0.4f);
    drawRect(x-2.0f,y+5.2f,4.0f,0.5f,1);
}

// Draw a street tree
static void drawTree(float x,float y,float seed)
{
    float sway=sinf(g_time*0.8f+seed)*1.2f;
    // trunk
    glColor3f(0.35f,0.22f,0.10f);
    glBegin(GL_QUADS);
    glVertex2f(x-1.2f,y);
    glVertex2f(x+1.2f,y);
    glVertex2f(x+0.8f+sway*0.3f,y+7);
    glVertex2f(x-0.8f+sway*0.3f,y+7);
    glEnd();
    // foliage layers (dark green for night)
    glColor4f(0.08f,0.28f,0.08f,0.95f);
    drawCircle(x+sway*0.5f,y+11,6.5f,16,1);
    glColor4f(0.10f,0.35f,0.10f,0.9f);
    drawCircle(x+sway*0.4f,y+14,5.0f,14,1);
    glColor4f(0.12f,0.38f,0.12f,0.85f);
    drawCircle(x+sway*0.3f,y+17,3.5f,12,1);
    // night-lit leaves: slight glow from lampposts
    glColor4f(0.55f,0.8f,0.35f,0.08f);
    drawCircle(x+sway*0.5f,y+11,6.5f,16,1);
}

// Draw a newsstand/kiosk
static void drawNewsstand(float x,float y)
{
    // body
    glColor3f(0.40f,0.22f,0.08f);
    drawRect(x-3.5f,y,7,6,1);
    // roof overhang
    glColor3f(0.52f,0.28f,0.10f);
    drawRect(x-4.5f,y+5.8f,9,1.2f,1);
    // window
    glColor4f(0.9f,0.85f,0.6f,0.8f);
    drawRect(x-2.5f,y+2.5f,5,2.8f,1);
    glColor4f(0,0,0,0.2f);
    glBegin(GL_LINES);
    glVertex2f(x,y+2.5f);
    glVertex2f(x,y+5.3f);
    glEnd();
    // sign
    glColor4f(0.9f,0.15f,0.15f,0.85f);
    drawRect(x-3.0f,y+5.3f,6,0.8f,1);
}

// Draw a bus stop sign
static void drawBusStop(float x,float y)
{
    // pole
    glColor3f(0.55f,0.55f,0.58f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(x,y);
    glVertex2f(x,y+14);
    glEnd();
    glLineWidth(1.0f);
    // sign board
    glColor3f(0.10f,0.35f,0.75f);
    drawRect(x-3.5f,y+11,7,3.5f,1);
    // BUS text hint
    glColor4f(1,1,1,0.8f);
    drawRect(x-2.5f,y+12,5,0.7f,1);
    drawRect(x-2.5f,y+13,5,0.7f,1);
    // shelter roof (partial)
    glColor4f(0.25f,0.25f,0.28f,0.8f);
    drawRect(x-5,y+7,10,0.8f,1);
    // bench at stop
    glColor3f(0.38f,0.38f,0.42f);
    drawRect(x-4,y+3.5f,8,1,1);
    // supports
    glLineWidth(1.5f);
    glColor3f(0.38f,0.38f,0.42f);
    glBegin(GL_LINES);
    glVertex2f(x-4,y);
    glVertex2f(x-4,y+7.8f);
    glVertex2f(x+4,y);
    glVertex2f(x+4,y+7.8f);
    glEnd();
    glLineWidth(1.0f);
}

static void drawFillers()
{
    for(int i=0; i<fillerCount; i++)
    {
        StreetFiller&f=fillers[i];
        switch(f.type)
        {
        case 0:
            drawBench(f.x,f.y);
            break;
        case 1:
            drawTrash(f.x,f.y,f.seed);
            break;
        case 2:
            drawHydrant(f.x,f.y);
            break;
        case 3:
            drawMailbox(f.x,f.y);
            break;
        case 4:
            drawTree(f.x,f.y,f.seed);
            break;
        case 5:
            drawNewsstand(f.x,f.y);
            break;
        case 6:
            drawBusStop(f.x,f.y);
            break;
        }
    }
}

// ================================================================
// CARS
// ================================================================
#define MAX_CARS 14
struct Car
{
    float x,y,speed;
    float cr,cg,cb;
    int   dir;
    int   exploded;
    float panicVx;
    int   panicking;
    float phase;
};
static Car cars[MAX_CARS];

static void initCars(float gy)
{
    float carCols[8][3]=
    {
        {0.72f,0.10f,0.10f},{0.12f,0.18f,0.65f},{0.12f,0.52f,0.14f},
        {0.62f,0.58f,0.10f},{0.52f,0.52f,0.55f},{0.62f,0.22f,0.08f},
        {0.08f,0.08f,0.08f},{0.72f,0.38f,0.10f}
    };
    for(int i=0; i<MAX_CARS; i++)
    {
        cars[i].dir=(i%2==0)?1:-1;
        cars[i].x=rf(-155,155);
        cars[i].y=gy+(i%2)*3.5f;
        cars[i].speed=rf(0.35f,0.75f);
        int c=ri(0,7);
        cars[i].cr=carCols[c][0];
        cars[i].cg=carCols[c][1];
        cars[i].cb=carCols[c][2];
        cars[i].exploded=0;
        cars[i].panicking=0;
        cars[i].panicVx=0;
        cars[i].phase=rf(0,TWO_PI);
    }
}

static void updateCars(float dt,int panic)
{
    for(int i=0; i<MAX_CARS; i++)
    {
        if(cars[i].exploded)continue;
        if(panic&&!cars[i].panicking)
        {
            cars[i].panicking=1;
            cars[i].panicVx=cars[i].dir*rf(1.5f,3.5f);
            cars[i].speed=rf(1.8f,3.5f); // much faster when panicking
        }
        float spd=cars[i].panicking?cars[i].speed*0.5f:cars[i].speed;
        cars[i].x+=cars[i].dir*spd*dt*55;
        if(cars[i].x>168)cars[i].x=-168;
        if(cars[i].x<-168)cars[i].x=168;
    }
}

static void drawCarObj(float cx,float cy,int dir,float cr2,float cg2,float cb2,int panicking)
{
    float d=(float)dir;
    // panic tilt
    float tilt = panicking ? (dir == 1 ? 12.0f : -12.0f) : 0.0f;
    glPushMatrix();
    glTranslatef(cx,cy,0);
    glRotatef(tilt,0,0,1);
    glTranslatef(-cx,-cy,0);

    // shadow (elongated when panicking)
    glColor4f(0,0,0,0.28f);
    float sw = panicking ? 11.0f : 8.0f;
    glBegin(GL_POLYGON);
    for(int k=0; k<10; k++)
    {
        float a=TWO_PI*k/10;
        glVertex2f(cx+cosf(a)*sw,cy-1+sinf(a)*1.8f);
    }
    glEnd();
    // lower body
    glColor3f(cr2,cg2,cb2);
    glBegin(GL_POLYGON);
    glVertex2f(cx-8*d,cy+0.5f);
    glVertex2f(cx+8*d,cy+0.5f);
    glVertex2f(cx+8*d,cy+4.5f);
    glVertex2f(cx-8*d,cy+4.5f);
    glEnd();
    // upper cabin
    glColor3f(cr2*0.82f,cg2*0.82f,cb2*0.82f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-4.5f*d,cy+4.5f);
    glVertex2f(cx+5*d,cy+4.5f);
    glVertex2f(cx+4*d,cy+8.5f);
    glVertex2f(cx-3.5f*d,cy+8.5f);
    glEnd();
    // windshield
    glColor4f(0.38f,0.58f,0.78f,0.75f);
    glBegin(GL_POLYGON);
    glVertex2f(cx+2.5f*d,cy+4.8f);
    glVertex2f(cx+4.5f*d,cy+4.8f);
    glVertex2f(cx+3.8f*d,cy+8.2f);
    glVertex2f(cx+2.0f*d,cy+8.2f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(cx-2.0f*d,cy+4.8f);
    glVertex2f(cx-4.0f*d,cy+4.8f);
    glVertex2f(cx-3.2f*d,cy+8.2f);
    glVertex2f(cx-1.5f*d,cy+8.2f);
    glEnd();
    // roof rack
    glColor3f(cr2*0.65f,cg2*0.65f,cb2*0.65f);
    drawRect(cx-2.5f*d,cy+8.4f,5,0.7f,1);
    // wheels
    glColor3f(0.08f,0.08f,0.08f);
    drawCircle(cx-4.8f*d,cy+1,2.4f,14,1);
    drawCircle(cx+4.8f*d,cy+1,2.4f,14,1);
    glColor3f(0.45f,0.45f,0.48f);
    drawCircle(cx-4.8f*d,cy+1,1.1f,10,1);
    drawCircle(cx+4.8f*d,cy+1,1.1f,10,1);
    glColor3f(0.65f,0.65f,0.68f);
    drawCircle(cx-4.8f*d,cy+1,0.5f,6,1);
    drawCircle(cx+4.8f*d,cy+1,0.5f,6,1);
    // headlights - brighter when panicking
    float hlBrightness = panicking ? 1.0f : 0.98f;
    glColor3f(hlBrightness,hlBrightness*0.95f,hlBrightness*0.72f);
    drawCircle(cx+7.2f*d,cy+2.8f,1.1f,8,1);
    float glowAlpha = panicking ? 0.32f : 0.18f;
    glColor4f(0.98f,0.95f,0.72f,glowAlpha);
    drawCircle(cx+7.2f*d,cy+2.8f,6,16,1);
    // tail lights
    glColor3f(0.88f,0.08f,0.08f);
    drawCircle(cx-7.2f*d,cy+2.8f,0.9f,8,1);
    glColor4f(0.88f,0.08f,0.08f,0.25f);
    drawCircle(cx-7.2f*d,cy+2.8f,4,12,1);
    // side stripe
    glColor4f(cr2*0.5f,cg2*0.5f,cb2*0.5f,0.7f);
    drawRect(cx-7.5f*d,cy+2.0f,15,0.8f,1);
    // door line
    glColor4f(0,0,0,0.35f);
    glLineWidth(0.7f);
    glBegin(GL_LINES);
    glVertex2f(cx+0.5f*d,cy+0.5f);
    glVertex2f(cx+0.5f*d,cy+4.3f);
    glEnd();
    glLineWidth(1.0f);

    // panic speed lines
    if(panicking)
    {
        glColor4f(1,1,1,0.25f);
        for(int sl=0; sl<4; sl++)
        {
            float sy2=cy+1.5f+sl*2.0f;
            float sx2=cx-8*d-3.0f;
            float len=5.0f+sl*2.0f;
            glBegin(GL_LINES);
            glVertex2f(sx2,sy2);
            glVertex2f(sx2-d*len,sy2);
            glEnd();
        }
    }

    glPopMatrix();
}

static void drawCars(float gy)
{
    for(int i=0; i<MAX_CARS; i++)
    {
        if(cars[i].exploded)continue;
        drawCarObj(cars[i].x,cars[i].y,cars[i].dir,cars[i].cr,cars[i].cg,cars[i].cb,cars[i].panicking);
    }
}

// ================================================================
// WALKING HUMANS
// ================================================================
#define MAX_HUMANS 22
struct Human
{
    float x,y,speed;
    int   dir;
    float skinR,skinG,skinB;
    float clothR,clothG,clothB;
    int   panicking;
    float panicDir;
    float animPhase;
    int   dead;
    float scream; // 0-1 scream timer
};
static Human humans[MAX_HUMANS];

static void initHumans(float footpathY)
{
    float skinPal[6][3]= {{0.80f,0.65f,0.52f},{0.70f,0.55f,0.42f},{0.85f,0.70f,0.56f},
        {0.60f,0.48f,0.36f},{0.50f,0.38f,0.28f},{0.88f,0.74f,0.60f}
    };
    float clothPal[6][3]= {{0.18f,0.26f,0.55f},{0.50f,0.15f,0.15f},{0.15f,0.35f,0.15f},
        {0.45f,0.35f,0.10f},{0.55f,0.55f,0.58f},{0.25f,0.18f,0.35f}
    };
    for(int i=0; i<MAX_HUMANS; i++)
    {
        humans[i].dir=(i%2==0)?1:-1;
        humans[i].x=rf(-150,150);
        humans[i].y=footpathY+(i%3)*1.5f;
        humans[i].speed=rf(0.08f,0.22f);
        int sp=ri(0,5);
        humans[i].skinR=skinPal[sp][0];
        humans[i].skinG=skinPal[sp][1];
        humans[i].skinB=skinPal[sp][2];
        int cp=ri(0,5);
        humans[i].clothR=clothPal[cp][0];
        humans[i].clothG=clothPal[cp][1];
        humans[i].clothB=clothPal[cp][2];
        humans[i].panicking=0;
        humans[i].panicDir=1;
        humans[i].dead=0;
        humans[i].scream=0;
        humans[i].animPhase=rf(0,TWO_PI);
    }
}

static void updateHumans(float dt,int panic)
{
    for(int i=0; i<MAX_HUMANS; i++)
    {
        if(humans[i].dead)continue;
        humans[i].animPhase+=dt*(panic?9.5f:2.2f);
        if(panic&&!humans[i].panicking)
        {
            humans[i].panicking=1;
            humans[i].panicDir=(humans[i].x>0)?1.0f:-1.0f;
            humans[i].speed=rf(0.8f,1.8f);
            humans[i].scream=rf(1.5f,4.0f); // scream timer
        }
        if(humans[i].scream>0) humans[i].scream-=dt;
        float spd=humans[i].panicking?humans[i].speed*1.0f:humans[i].speed;
        int moveDir=humans[i].panicking?(int)humans[i].panicDir:humans[i].dir;
        humans[i].x+=moveDir*spd*dt*55;
        if(humans[i].x>165)humans[i].x=-165;
        if(humans[i].x<-165)humans[i].x=165;
    }
}

static void drawHumanFigure(float hx,float hy,float sc,
                            float sr,float sg,float sb,
                            float cr2,float cg2,float cb2,
                            float anim,int panic,float scream)
{
    float legSwing=sinf(anim)*sc*(panic?5.5f:2.2f);
    float armSwing=sinf(anim+PI)*sc*(panic?8.0f:2.0f);
    float headBob =fabsf(sinf(anim))*sc*(panic?2.0f:0.5f);
    // lean forward when panicking
    float lean = panic ? -8.0f : 0.0f;

    glPushMatrix();
    glTranslatef(hx,hy,0);
    glRotatef(lean,0,0,1);

    // shadow
    glColor4f(0,0,0,0.22f);
    glBegin(GL_POLYGON);
    for(int k=0; k<8; k++)
    {
        float a=TWO_PI*k/8;
        glVertex2f(cosf(a)*sc*2.5f,-0.5f+sinf(a)*sc*0.5f);
    }
    glEnd();
    // legs
    glColor3f(cr2*0.7f,cg2*0.7f,cb2*0.7f);
    glLineWidth(sc*1.6f);
    glBegin(GL_LINES);
    glVertex2f(0,sc);
    glVertex2f(-sc+legSwing,-sc*3);
    glVertex2f(0,sc);
    glVertex2f(sc-legSwing,-sc*3);
    glEnd();
    // shoes
    glColor3f(0.12f,0.1f,0.08f);
    glLineWidth(1.0f);
    drawCircle(-sc+legSwing,-sc*3,sc*0.7f,8,1);
    drawCircle(sc-legSwing,-sc*3,sc*0.7f,8,1);
    // body
    glColor3f(cr2,cg2,cb2);
    glBegin(GL_POLYGON);
    glVertex2f(-sc*1.6f,sc);
    glVertex2f(sc*1.6f,sc);
    glVertex2f(sc*1.4f,sc*6);
    glVertex2f(-sc*1.4f,sc*6);
    glEnd();
    // neck
    glColor3f(sr,sg,sb);
    glBegin(GL_POLYGON);
    glVertex2f(-sc*0.5f,sc*5);
    glVertex2f(sc*0.5f,sc*5);
    glVertex2f(sc*0.4f,sc*7);
    glVertex2f(-sc*0.4f,sc*7);
    glEnd();
    // arms
    glColor3f(cr2,cg2,cb2);
    glLineWidth(sc*1.5f);
    glBegin(GL_LINES);
    // arms raised higher when panicking
    float armRaise = panic ? sc*3.5f : sc*2.0f;
    glVertex2f(-sc*1.6f,sc*5);
    glVertex2f(-sc*3+armSwing,armRaise);
    glVertex2f(sc*1.6f,sc*5);
    glVertex2f(sc*3-armSwing,armRaise);
    glEnd();
    // hands
    glColor3f(sr,sg,sb);
    drawCircle(-sc*3+armSwing,armRaise,sc*0.65f,6,1);
    drawCircle(sc*3-armSwing,armRaise,sc*0.65f,6,1);
    // head
    glColor3f(sr,sg,sb);
    drawCircle(0,sc*8+headBob,sc*2.2f,14,1);
    // hair
    glColor3f(sr*0.35f,sg*0.28f,sb*0.22f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a=-PI+PI*k/12;
        float rr=(a<0)?sc*2.4f:sc*1.8f;
        glVertex2f(cosf(a)*rr,sc*8+headBob+sinf(a)*rr*(a<0?0.65f:0.4f));
    }
    glEnd();
    // eyes
    glColor3f(0.08f,0.06f,0.05f);
    drawCircle(-sc*0.7f,sc*8.3f+headBob,sc*0.28f,6,1);
    drawCircle(sc*0.7f,sc*8.3f+headBob,sc*0.28f,6,1);
    // wide eyes when panicking
    if(panic)
    {
        glColor3f(1,1,1);
        drawCircle(-sc*0.7f,sc*8.4f+headBob,sc*0.22f,6,1);
        drawCircle(sc*0.7f,sc*8.4f+headBob,sc*0.22f,6,1);
        glColor3f(0.08f,0.06f,0.05f);
        drawCircle(-sc*0.7f,sc*8.35f+headBob,sc*0.14f,5,1);
        drawCircle(sc*0.7f,sc*8.35f+headBob,sc*0.14f,5,1);
        // open mouth / scream
        glColor3f(0.15f,0.08f,0.06f);
        drawCircle(0,sc*7.55f+headBob,sc*0.45f,8,1);
        // scream bubble
        if(scream>0.5f)
        {
            float alpha=fminf(scream-0.5f,1.0f);
            glColor4f(1,1,1,alpha*0.8f);
            drawCircle(sc*3.5f,sc*9.5f+headBob,sc*1.8f,10,1);
            glColor4f(1,1,1,alpha*0.8f);
            glBegin(GL_TRIANGLES);
            glVertex2f(sc*2.2f,sc*8.5f+headBob);
            glVertex2f(sc*3.0f,sc*8.0f+headBob);
            glVertex2f(sc*2.5f,sc*9.0f+headBob);
            glEnd();
            // "!" inside bubble
            glColor4f(0.8f,0.1f,0.1f,alpha);
            drawRect(sc*3.3f,sc*8.8f+headBob,sc*0.4f,sc*1.2f,1);
            drawRect(sc*3.3f,sc*8.5f+headBob,sc*0.4f,sc*0.3f,1);
        }
    }
    glLineWidth(1.0f);
    glPopMatrix();
}

static void drawHumans()
{
    for(int i=0; i<MAX_HUMANS; i++)
    {
        if(humans[i].dead)continue;
        drawHumanFigure(humans[i].x,humans[i].y,0.85f,
                        humans[i].skinR,humans[i].skinG,humans[i].skinB,
                        humans[i].clothR,humans[i].clothG,humans[i].clothB,
                        humans[i].animPhase,humans[i].panicking,humans[i].scream);
    }
}

// ================================================================
// SKY, STARS, MOON, SUN, CLOUDS, TERRAIN, etc.
// ================================================================
static void drawSky(float r0,float g0,float b0,
                    float r1,float g1,float b1,
                    float r2,float g2,float b2)
{
    glBegin(GL_QUADS);
    glColor3f(r0,g0,b0);
    glVertex2f(-160,100);
    glVertex2f(160,100);
    glColor3f(r1,g1,b1);
    glVertex2f(160,30);
    glVertex2f(-160,30);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(r1,g1,b1);
    glVertex2f(-160,30);
    glVertex2f(160,30);
    glColor3f(r2,g2,b2);
    glVertex2f(160,-60);
    glVertex2f(-160,-60);
    glEnd();
}
static void drawStars(float brightness)
{
    srand(999);
    for(int i=0; i<150; i++)
    {
        float sx=rf(-155,155),sy=rf(10,98);
        float tw=0.4f+0.6f*((float)(rand()%100)/100.0f);
        float flick=brightness*(0.5f+0.5f*sinf(g_time*1.5f+i*0.7f));
        glColor4f(tw,tw,tw*0.9f,flick);
        glPointSize(tw*2.0f);
        glBegin(GL_POINTS);
        glVertex2f(sx,sy);
        glEnd();
    }
    srand((unsigned)time(NULL));
}
static void drawMoon(float mx,float my)
{
    glColor4f(0.9f,0.85f,0.7f,0.08f);
    drawCircle(mx,my,18,32,1);
    glColor4f(0.9f,0.85f,0.7f,0.05f);
    drawCircle(mx,my,24,32,1);
    glColor3f(0.92f,0.88f,0.72f);
    drawCircle(mx,my,10,32,1);
    glColor3f(0.08f,0.08f,0.12f);
    drawCircle(mx+4,my,9,32,1);
    glColor3f(0.75f,0.72f,0.58f);
    drawCircle(mx-3,my+2,1.5f,12,1);
    drawCircle(mx-1,my-3,1.0f,10,1);
    drawCircle(mx+1,my+1,0.7f,8,1);
}
static void drawCloud(float cx,float cy,float scale,float alpha,float r,float g2,float b)
{
    glColor4f(r,g2,b,alpha);
    drawCircle(cx,cy,scale*8,20,1);
    drawCircle(cx+scale*6,cy+scale*2,scale*6,20,1);
    drawCircle(cx-scale*6,cy+scale*1,scale*5,20,1);
    drawCircle(cx+scale*2,cy+scale*4,scale*7,20,1);
    drawCircle(cx-scale*3,cy+scale*3,scale*5.5f,20,1);
}
static void drawClouds(float r,float g2,float b,float alpha)
{
    float offsets[]= {-80,-40,0,40,80};
    float ys[]= {55,65,50,60,58};
    float scs[]= {1.0f,0.7f,1.2f,0.8f,1.1f};
    for(int i=0; i<5; i++)
    {
        float cx=offsets[i]+sinf(g_time*0.05f+i)*5;
        drawCloud(cx,ys[i],scs[i],alpha,r,g2,b);
    }
}
static void drawTerrain(float gy,float r,float g2,float b)
{
    glColor3f(r*0.5f,g2*0.5f,b*0.5f);
    drawRect(-400,-80,820,gy+80+2,1);
    glColor3f(r,g2,b);
    glBegin(GL_POLYGON);
    glVertex2f(-160,gy-8);
    for(int x=-160; x<=160; x+=4)
    {
        float bump=sinf(x*0.08f)*1.5f+sinf(x*0.15f)*0.8f;
        glVertex2f((float)x,gy+bump);
    }
    glVertex2f(160,gy-8);
    glEnd();
    // road
    glColor3f(r*0.52f,g2*0.50f,b*0.48f);
    drawRect(-160,gy-4,320,7,1);
    // lane divider
    glColor4f(0.72f,0.68f,0.55f,0.55f);
    for(int x=-158; x<158; x+=22) drawRect((float)x,gy-1.5f,12,1.2f,1);
    // road edges
    glColor4f(0.82f,0.80f,0.72f,0.5f);
    drawRect(-160,gy-4,320,0.8f,1);
    drawRect(-160,gy+2.8f,320,0.8f,1);
    // footpath left and right
    glColor3f(r*0.72f,g2*0.70f,b*0.68f);
    drawRect(-160,gy+3,60,3.5f,1);
    drawRect(100,gy+3,60,3.5f,1);
    // footpath edge
    glColor3f(r*0.6f,g2*0.58f,b*0.56f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    glVertex2f(-160,gy+3);
    glVertex2f(-100,gy+3);
    glVertex2f(100,gy+3);
    glVertex2f(160,gy+3);
    glEnd();
    // footpath tiles
    glColor4f(0,0,0,0.08f);
    glLineWidth(0.6f);
    for(int x=-158; x<-100; x+=10)
    {
        glBegin(GL_LINES);
        glVertex2f((float)x,gy+3);
        glVertex2f((float)x,gy+6.5f);
        glEnd();
    }
    for(int x=102; x<160; x+=10)
    {
        glBegin(GL_LINES);
        glVertex2f((float)x,gy+3);
        glVertex2f((float)x,gy+6.5f);
        glEnd();
    }
    glLineWidth(1.0f);
}
static void drawVignette(float strength)
{
    glBegin(GL_QUADS);
    glColor4f(0,0,0,strength);
    glVertex2f(-160,-80);
    glColor4f(0,0,0,0);
    glVertex2f(0,-80);
    glColor4f(0,0,0,0);
    glVertex2f(0,100);
    glColor4f(0,0,0,strength);
    glVertex2f(-160,100);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0,0,0,0);
    glVertex2f(0,-80);
    glColor4f(0,0,0,strength);
    glVertex2f(160,-80);
    glColor4f(0,0,0,strength);
    glVertex2f(160,100);
    glColor4f(0,0,0,0);
    glVertex2f(0,100);
    glEnd();
}
static void drawExplosionFlash(float x,float y,float r,float alpha)
{
    glColor4f(1.0f,0.7f,0.2f,alpha);
    drawCircle(x,y,r,32,1);
    glColor4f(1.0f,1.0f,0.5f,alpha*0.5f);
    drawCircle(x,y,r*0.5f,24,1);
    glColor4f(1,1,1,alpha*0.25f);
    drawCircle(x,y,r*0.2f,16,1);
}



// ================================================================
// DETAILED FIGHTER JET
// ================================================================
static void drawJet(float jx,float jy,float sc,int dir)
{
    glPushMatrix();
    glTranslatef(jx,jy,0);
    glScalef(sc*dir,sc,1);
    float ft=0.6f+0.4f*sinf(g_time*20+jx);
    glColor4f(0.3f,0.5f,1.0f,0.8f*ft);
    glBegin(GL_TRIANGLES);
    glVertex2f(-7,-0.3f);
    glVertex2f(-7,0.3f);
    glVertex2f(-14*ft,0);
    glEnd();
    glColor4f(1.0f,0.8f,0.2f,0.9f*ft);
    glBegin(GL_TRIANGLES);
    glVertex2f(-7,-0.15f);
    glVertex2f(-7,0.15f);
    glVertex2f(-11*ft,0);
    glEnd();
    glColor3f(0.22f,0.24f,0.28f);
    glBegin(GL_POLYGON);
    glVertex2f(-7.0f,0.5f);
    glVertex2f(-6.0f,1.0f);
    glVertex2f(4.0f,0.8f);
    glVertex2f(9.0f,0.0f);
    glVertex2f(4.0f,-0.8f);
    glVertex2f(-6.0f,-1.0f);
    glVertex2f(-7.0f,-0.5f);
    glEnd();
    glColor3f(0.3f,0.3f,0.35f);
    glBegin(GL_TRIANGLES);
    glVertex2f(9.0f,0.0f);
    glVertex2f(7.0f,0.6f);
    glVertex2f(7.0f,-0.6f);
    glEnd();
    glColor3f(0.5f,0.75f,0.85f);
    glBegin(GL_POLYGON);
    glVertex2f(2.0f,0.8f);
    glVertex2f(5.5f,0.5f);
    glVertex2f(5.5f,-0.5f);
    glVertex2f(2.0f,-0.5f);
    glEnd();
    glColor4f(0.3f,0.5f,0.7f,0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(2.2f,0.75f);
    glVertex2f(5.3f,0.45f);
    glVertex2f(5.3f,-0.4f);
    glVertex2f(2.2f,-0.4f);
    glEnd();
    glColor3f(0.2f,0.22f,0.26f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f,0.5f);
    glVertex2f(3.5f,0.5f);
    glVertex2f(1.0f,6.5f);
    glVertex2f(-4.5f,6.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f,-0.5f);
    glVertex2f(3.5f,-0.5f);
    glVertex2f(1.0f,-6.5f);
    glVertex2f(-4.5f,-6.5f);
    glEnd();
    glColor3f(0.18f,0.2f,0.24f);
    glBegin(GL_POLYGON);
    glVertex2f(-4.0f,0.5f);
    glVertex2f(-2.0f,0.5f);
    glVertex2f(-3.0f,3.5f);
    glVertex2f(-5.5f,3.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-4.0f,-0.5f);
    glVertex2f(-2.0f,-0.5f);
    glVertex2f(-3.0f,-3.5f);
    glVertex2f(-5.5f,-3.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-5.5f,0.4f);
    glVertex2f(-2.5f,0.4f);
    glVertex2f(-3.5f,2.5f);
    glVertex2f(-6.0f,2.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-5.5f,-0.4f);
    glVertex2f(-2.5f,-0.4f);
    glVertex2f(-3.5f,-2.5f);
    glVertex2f(-6.0f,-2.0f);
    glEnd();
    glColor4f(0.5f,0.55f,0.6f,0.3f);
    glLineWidth(0.8f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-1.0f,0.5f);
    glVertex2f(3.0f,0.5f);
    glVertex2f(0.5f,6.0f);
    glEnd();
    glColor3f(0.4f,0.38f,0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-0.5f,3.5f);
    glVertex2f(2.0f,3.5f);
    glVertex2f(2.0f,4.2f);
    glVertex2f(-0.5f,4.2f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.5f,-3.5f);
    glVertex2f(2.0f,-3.5f);
    glVertex2f(2.0f,-4.2f);
    glVertex2f(-0.5f,-4.2f);
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
}

// Missile with heat trail
struct Missile
{
    float x,y,tx,ty,spd;
    int active;
};
#define MAX_M 16
static Missile M[MAX_M];
static void initMissiles()
{
    for(int i=0; i<MAX_M; i++)M[i].active=0;
}

static void drawMissileObj(float x,float y,float tx,float ty)
{
    float dx=tx-x,dy=ty-y,len=sqrtf(dx*dx+dy*dy);
    if(len<0.01f)return;
    float ang=atan2f(dy,dx)*57.3f;
    glPushMatrix();
    glTranslatef(x,y,0);
    glRotatef(ang,0,0,1);
    glBegin(GL_QUAD_STRIP);
    for(int t=0; t<12; t++)
    {
        float tt=(float)t/11,w=1.5f*(1.0f-tt);
        glColor4f(1.0f,0.5f+0.3f*tt,0.0f,0.6f*(1.0f-tt));
        glVertex2f(-t*2.5f,w);
        glVertex2f(-t*2.5f,-w);
    }
    glEnd();
    glColor3f(0.5f,0.5f,0.55f);
    glBegin(GL_POLYGON);
    glVertex2f(-3.5f,-0.8f);
    glVertex2f(-3.5f,0.8f);
    glVertex2f(2.0f,0.6f);
    glVertex2f(4.0f,0.0f);
    glVertex2f(2.0f,-0.6f);
    glEnd();
    glColor3f(1.0f,0.8f,0.2f);
    drawCircle(4.0f,0,1.0f,8,1);
    glPopMatrix();
}

// ================================================================
// SCENE STATE VARIABLES
// ================================================================
#define MAX_JETS 3
struct Jet
{
    float x,y,vx;
    int dir;
};
static Jet jets[MAX_JETS];
static float s1_flashX,s1_flashY,s1_flashA;
static int   s1_hits;
static int   s1_missileDropped;
static int   s1_panic;

static int   s2_phase        = 0;
static float s2_timer        = 0.0f;
static float s2_phaseT       = 0.0f;
static float s2_ivDrip       = 0.0f;
static float s2_sirenPhase   = 0.0f;
static float s2_ambX         = 180.0f;
static float s2_ambStopX     = 20.0f;
static float s2_stretchX     = 0.0f;
static float s2_stretchAlpha = 0.0f;
static float s2_icuAlpha     = 0.0f;
static float s2_dimAlpha     = 0.0f;
static float s2_msgAlpha     = 0.0f;
static int   s2_flatlined    = 0;
static float s2_flatlineT    = 0.0f;
static float s2_doctorX      = 85.0f;

// Scene 3 state
static float s3_wave=0,s3_speechT=0,s3_bannerFlash=0;
static float s3_armRaise=0;
static int   s3_bannerVisible=0;

// Scene 5 state
static float s5_glow=0;
static float s5_doveX[7],s5_doveY[7],s5_doveWing[7];
static float s5_flagWave=0;

// SCENE 4
#define S4_CITY_BLDS 15
#define S4_MAX_HITS  8

// Building types: 0=flat battlements, 1=dome, 2=minaret, 3=arch facade
struct S4CityBuilding
{
    float x, y, w, h;         // position and size
    float cr, cg, cb;          // color (sandstone variants)
    int   type;                 // architectural style
    float seed;                 // random seed for animation
    int   hitCount;             // number of times hit
    int   destroyed;            // fully destroyed flag
    float hitX[S4_MAX_HITS];   // fire spot x positions
    float hitY[S4_MAX_HITS];   // fire spot y positions
    float firePhase;            // fire animation phase
    int   winLit[8][5];        // window lit state
};
static S4CityBuilding s4_city[S4_CITY_BLDS];
static void initS4CityBuildings()
{
    // Layout: x, w, h, type
    float layout[S4_CITY_BLDS][4] =
    {
        {-158, 20, 52, 0}, {-136, 15, 68, 2}, {-119, 22, 44, 3},
        {-95,  24, 64, 1}, {-69,  16, 48, 0}, {-51,  26, 78, 2},
        {-23,  20, 54, 3}, {-1,   22, 70, 1}, {23,   18, 48, 0},
        {43,   24, 66, 2}, {69,   22, 56, 3}, {93,   20, 74, 1},
        {115,  26, 50, 0}, {143,  16, 68, 2}, {161,  20, 58, 3}
    };
    // Sandstone color variants
    float colors[5][3] =
    {
        {0.62f,0.50f,0.34f}, {0.58f,0.47f,0.31f},
        {0.68f,0.55f,0.38f}, {0.65f,0.53f,0.36f},
        {0.60f,0.48f,0.32f}
    };
    srand(9876);
    for(int i = 0; i < S4_CITY_BLDS; i++)
    {
        s4_city[i].x = layout[i][0];
        s4_city[i].y = -35.0f; // city ground = S4_CITY_BOT
        s4_city[i].w = layout[i][1];
        s4_city[i].h = layout[i][2];
        int ci = i % 5;
        s4_city[i].cr = colors[ci][0];
        s4_city[i].cg = colors[ci][1];
        s4_city[i].cb = colors[ci][2];
        s4_city[i].type = (int)layout[i][3];
        s4_city[i].seed = rf(0, 100);
        s4_city[i].hitCount = 0;
        s4_city[i].destroyed = 0;
        s4_city[i].firePhase = rf(0, TWO_PI);
        for(int k = 0; k < S4_MAX_HITS; k++)
        {
            s4_city[i].hitX[k] = 0;
            s4_city[i].hitY[k] = 0;
        }
        for(int wc = 0; wc < 8; wc++) for(int wr = 0; wr < 5; wr++)
                s4_city[i].winLit[wc][wr] = (rand()%3 != 0) ? 1 : 0;
    }
    srand((unsigned)time(NULL));
}


static void sceneInit(int s)
{
    for(int i=0; i<MAX_P; i++)P[i].alive=0;
    initMissiles();
    s1_flashA=0;
    s1_hits=0;
    s1_missileDropped=0;
    s1_panic=0;
    // --- FIX 4: RESET ALL scene 2 variables properly ---
    s2_phase=0;
    s2_timer=0;
    s2_phaseT=0;
    s2_ivDrip=0;
    s2_sirenPhase=0;
    s2_ambX=180.0f;
    s2_stretchX=0;
    s2_stretchAlpha=0;
    s2_icuAlpha=0;
    s2_dimAlpha=0;
    s2_msgAlpha=0;
    s2_flatlined=0;
    s2_flatlineT=0;
    s2_doctorX=55.0f;
    // ---------------------------------------------------
    s3_wave=0;
    s3_speechT=0;
    s3_bannerFlash=0;
    s3_armRaise=0;
    s3_bannerVisible=0;

    if(s==1)
    {
        initBuildings(0);
        initCars(-45);
        initLamps(-60);
        initHumans(-62);
        initFillers(-60);
        for(int i=0; i<MAX_JETS; i++)
        {
            jets[i].x=-130.0f-i*35.0f;
            jets[i].y=48.0f+i*9.0f;
            jets[i].vx=rf(0.55f,0.9f);
            jets[i].dir=1;
        }
    }
    if(s==5)
    {
        // Scene 5 uses scene 4's destroyed city - max damage all buildings
        initS4CityBuildings();
        for(int i=0; i<S4_CITY_BLDS; i++)
        {
            // Give each building significant damage (partially or fully destroyed)
            int dmgLevel = ri(3,S4_MAX_HITS);
            for(int h=0; h<dmgLevel; h++)
            {
                s4_city[i].hitX[h]=s4_city[i].x+s4_city[i].w*rf(0.1f,0.9f);
                s4_city[i].hitY[h]=s4_city[i].y+s4_city[i].h*rf(0.1f,0.9f);
                s4_city[i].hitCount=h+1;
            }
            if(ri(0,2)==0) s4_city[i].destroyed=1; // some fully destroyed
        }
    }

}


// ================================================================
// SCENE 1 UPDATE
// ================================================================
static void updateScene1(float dt)
{
    for(int i=0; i<MAX_JETS; i++)
    {
        jets[i].x+=jets[i].vx*dt*35;
        if(jets[i].x>180) jets[i].x=-180;
    }

    for(int i=0; i<MAX_JETS; i++)
    {
        if(ri(0,30)<2)
        {
            for(int m=0; m<MAX_M; m++)
            {
                if(!M[m].active)
                {
                    int bi=ri(0,MAX_B-1);
                    M[m].x=jets[i].x;
                    M[m].y=jets[i].y;
                    M[m].tx=B[bi].x+B[bi].w*rf(0.2f,0.8f);
                    M[m].ty=-45+B[bi].h*rf(0.3f,1.0f);
                    M[m].spd=rf(40,70);
                    M[m].active=1;
                    s1_missileDropped++;
                    if(s1_missileDropped>=1) s1_panic=1;
                    break;
                }
            }
        }
    }

    updateHumans(dt,s1_panic);
    updateCars(dt,s1_panic);

    for(int m=0; m<MAX_M; m++)
    {
        if(!M[m].active)continue;
        float dx=M[m].tx-M[m].x,dy=M[m].ty-M[m].y;
        float dist=sqrtf(dx*dx+dy*dy);
        if(dist<5.0f)
        {
            explode(M[m].tx,M[m].ty,2.8f,1);
            s1_flashX=M[m].tx;
            s1_flashY=M[m].ty;
            s1_flashA=0.85f;
            s1_hits++;

            // Check car hit
            for(int c=0; c<MAX_CARS; c++)
            {
                if(!cars[c].exploded&&
                        fabsf(cars[c].x-M[m].tx)<12&&
                        fabsf(cars[c].y-(-45))<12)
                {
                    cars[c].exploded=1;
                    explode(cars[c].x,cars[c].y+3,3.5f,1);
                    for(int d=0; d<20; d++)
                    {
                        float ang=rf(0,TWO_PI),sp=rf(2,6);
                        pSpawn(cars[c].x,cars[c].y+3,cosf(ang)*sp,sinf(ang)*sp+3,0,-6,
                               cars[c].cr,cars[c].cg,cars[c].cb,1,rf(0.5f,2.5f),rf(2,6),3);
                    }
                }
            }

            // Check lamp hit
            for(int l=0; l<MAX_LAMPS; l++)
            {
                if(!lamps[l].broken&&fabsf(lamps[l].x-M[m].tx)<8)
                {
                    lamps[l].broken=1;
                }
            }

            // Hit building at specific spot
            for(int b=0; b<MAX_B; b++)
            {
                if(M[m].tx>=B[b].x-3&&M[m].tx<=B[b].x+B[b].w+3)
                {
                    if(B[b].hitCount<MAX_HIT_SPOTS)
                    {
                        B[b].hitX[B[b].hitCount]=M[m].tx;
                        B[b].hitY[B[b].hitCount]=M[m].ty;
                        B[b].hitCount++;
                        // Turn off windows near hit
                        float relX=(M[m].tx-B[b].x)/B[b].w;
                        int col=(int)(relX*8);
                        for(int wc=col-1; wc<=col+1; wc++)
                            for(int wr=0; wr<20; wr++)
                                if(wc>=0&&wc<15)B[b].winLit[wc][wr]=0;
                    }
                    else
                    {
                        B[b].dest=1;
                    }
                }
            }
            M[m].active=0;
        }
        else
        {
            float sp=M[m].spd*dt;
            M[m].x+=dx/dist*sp;
            M[m].y+=dy/dist*sp;
        }
    }

    // ambient smoke from hit spots
    if(rf(0,1)<0.35f)
    {
        for(int b=0; b<MAX_B; b++)
        {
            for(int h=0; h<B[b].hitCount; h++)
            {
                if(rf(0,1)<0.3f)
                    pSpawn(B[b].hitX[h]+rf(-4,4),B[b].hitY[h]+12,
                           rf(-0.4f,0.4f),rf(0.8f,2.8f),0,0,
                           rf(0.18f,0.32f),rf(0.18f,0.32f),rf(0.15f,0.28f),
                           0.85f,rf(1.8f,4.5f),rf(6,13),1);
            }
            if(B[b].dest)
                pSpawn(B[b].x+rf(0,B[b].w),-45+B[b].h,rf(-0.5f,0.5f),rf(1,3),0,0,
                       rf(0.15f,0.3f),rf(0.15f,0.3f),rf(0.15f,0.3f),0.8f,rf(2,5),rf(5,12),1);
        }
    }
    // ash drift
    if(rf(0,1)<0.12f)
        pSpawn(rf(-120,120),rf(-30,60),rf(-0.2f,0.2f),rf(0.05f,0.2f),0,0,
               0.6f,0.55f,0.5f,0.5f,rf(4,8),rf(1,2.5f),4);
    if(s1_flashA>0)s1_flashA-=dt*2.5f;
    pUpdate(dt);
}

// ================================================================
//  SCENE 2 GLOBAL STATE
// ================================================================

static void updateScene2(float dt)
{
    s2_timer  += dt;
    s2_phaseT += dt;
    s2_ivDrip += dt;
    s2_sirenPhase += dt*4.5f;

    if(s2_phase==0 && s2_phaseT>4.5f)
    {
        s2_phase=1;
        s2_phaseT=0;
    }
    if(s2_phase==1 && s2_phaseT>3.5f)
    {
        s2_phase=2;
        s2_phaseT=0;
    }
    if(s2_phase==2 && s2_phaseT>4.2f)
    {
        s2_phase=3;
        s2_phaseT=0;
    }
    if(s2_phase==3 && s2_phaseT>6.0f)
    {
        s2_phase=4;
        s2_phaseT=0;
    }

    // FIX 1: Ambulance moves LEFT (from right), smoke spawns at REAR (left side = lower x)
    // Ambulance moves leftward so its rear is at ambX - bodyWidth
    if(s2_phase==0)
    {
        if(s2_ambX > s2_ambStopX)
            s2_ambX -= dt*44.0f;
        if(s2_ambX < s2_ambStopX) s2_ambX=s2_ambStopX;
        // Spawn exhaust smoke at the REAR of the ambulance (left/back end) = ambX - 34 (half-width of cab)
        // Since ambulance body spans ambX-34 to ambX+34, rear is at ambX-34
        if(rf(0,1)<0.35f)
            pSpawn(s2_ambX - 34.0f + rf(-2,2),  // rear of ambulance
                   -55 + 14 + rf(0,3),           // near roof level
                   rf(-1.5f,-0.3f),              // drift leftward/backward
                   rf(0.5f,1.5f),                // rise up
                   0, 0,
                   0.28f,0.28f,0.28f,0.65f,
                   rf(1.0f,2.5f),rf(4,9),1);
    }
    if(s2_phase==1)
    {
        s2_stretchAlpha=fminf(s2_phaseT/0.8f,1.0f);
        float pullDist=s2_phaseT*9.0f;
        s2_stretchX=s2_ambX-20-pullDist;
        if(s2_stretchX<s2_ambX-58)s2_stretchX=s2_ambX-58;
    }
    if(s2_phase==2)
    {
        s2_stretchX=(s2_ambX-58)-s2_phaseT*20.0f;
        if(s2_stretchX<-155)s2_stretchX=-155;
        s2_icuAlpha=fminf(s2_phaseT/3.8f,1.0f);
    }
    if(s2_phase>=3) s2_icuAlpha=1.0f;

    if(s2_phase==4)
    {
        if(!s2_flatlined && s2_phaseT>2.8f) s2_flatlined=1;
        if(s2_flatlined)
        {
            s2_flatlineT+=dt;
            if(s2_dimAlpha<0.52f) s2_dimAlpha+=dt*0.38f;
            if(s2_flatlineT>1.8f && s2_doctorX>5.0f)
                s2_doctorX-=dt*22.0f;
            if(s2_doctorX<5.0f) s2_doctorX=5.0f;
            if(s2_flatlineT>3.6f && s2_msgAlpha<1.0f)
                s2_msgAlpha+=dt*0.6f;
            if(s2_msgAlpha>1.0f)s2_msgAlpha=1.0f;
        }
    }
    if(s2_phase==3 && !s2_flatlined && rf(0,1)<0.14f)
        pSpawn(rf(-5,5),-28+rf(-1,1),rf(-0.2f,0.2f),rf(-0.5f,-0.08f),0,0,
               0.72f,0,0,1,rf(0.3f,1.0f),rf(1,2.2f),5);
    pUpdate(dt);
}


// ================================================================
// SCENE 1 DRAW
// ================================================================
static void drawScene1()
{
    drawSky(0.04f,0.03f,0.07f,0.12f,0.06f,0.02f,0.2f,0.08f,0.01f);
    drawStars(0.9f);
    drawMoon(80,70);
    if(s1_hits>0)
    {
        float fi=fminf(s1_hits*0.15f,1.0f);
        glColor4f(0.9f,0.25f,0.0f,0.22f*fi);
        glBegin(GL_QUADS);
        glVertex2f(-160,-45);
        glVertex2f(160,-45);
        glVertex2f(160,-10);
        glVertex2f(-160,-10);
        glEnd();
    }
    drawClouds(0.25f,0.12f,0.08f,0.55f);
    drawTerrain(-45,0.16f,0.14f,0.11f);
    drawLamps();
    drawFillers();   // draw fillers between lampposts
    drawBuildings(-41);
    drawCars(-45);
    drawHumans();
    pDraw();
    for(int m=0; m<MAX_M; m++)if(M[m].active)drawMissileObj(M[m].x,M[m].y,M[m].tx,M[m].ty);
    for(int i=0; i<MAX_JETS; i++)drawJet(jets[i].x,jets[i].y,2.0f,jets[i].dir);
    if(s1_flashA>0.01f)drawExplosionFlash(s1_flashX,s1_flashY,20,s1_flashA);
    drawVignette(0.5f);
}



// ================================================================
// SCENE 2 - ICU HOSPITAL
// ================================================================


// Ambulance vehicle (viewed from side, moving left; cab on RIGHT side)
static void drawAmbulance(float ax,float ay)
{
    float w=34,h=16;
    // shadow
    glColor4f(0,0,0,0.3f);
    glBegin(GL_POLYGON);
    for(int k=0; k<10; k++)
    {
        float a=TWO_PI*k/10;
        glVertex2f(ax+cosf(a)*w*0.9f,ay-2+sinf(a)*2.5f);
    }
    glEnd();
    // body (main box, cab on right = front/direction of travel is LEFT so cab is at LEFT = ax-w)
    // Actually ambulance came from right and moves left, so cab (engine front) faces left
    // cab front is at ax - w, rear door is at ax + w
    // body
    glColor3f(0.92f,0.92f,0.92f);
    glBegin(GL_POLYGON);
    glVertex2f(ax-w*0.7f,ay);
    glVertex2f(ax+w,ay);
    glVertex2f(ax+w,ay+h);
    glVertex2f(ax-w*0.7f,ay+h);
    glEnd();
    // cab (front, facing LEFT)
    glColor3f(0.80f,0.80f,0.80f);
    glBegin(GL_POLYGON);
    glVertex2f(ax-w*0.7f,ay);
    glVertex2f(ax-w,ay+h*0.4f);
    glVertex2f(ax-w,ay+h);
    glVertex2f(ax-w*0.7f,ay+h);
    glEnd();
    // windshield (on LEFT/front)
    glColor4f(0.4f,0.6f,0.8f,0.8f);
    glBegin(GL_POLYGON);
    glVertex2f(ax-w*0.72f,ay+h*0.45f);
    glVertex2f(ax-w*0.98f,ay+h*0.45f);
    glVertex2f(ax-w*0.97f,ay+h*0.95f);
    glVertex2f(ax-w*0.73f,ay+h*0.95f);
    glEnd();
    // red cross on side
    glColor3f(0.88f,0.06f,0.06f);
    drawRect(ax+w*0.05f,ay+h*0.25f,4,10,1);
    drawRect(ax-w*0.08f,ay+h*0.25f+3,10,4,1);
    // red stripe along bottom
    glColor3f(0.88f,0.06f,0.06f);
    drawRect(ax-w,ay,w*1.7f,3.5f,1);
    // "AMBULANCE" text (mirrored as seen from side)
    glColor3f(0.88f,0.06f,0.06f);
    drawText(ax-w*0.5f,ay+h*0.62f,"AMBULANCE",0.18f);
    // rear doors (RIGHT side) - open in phase 1
    glColor3f(0.75f,0.75f,0.75f);
    drawRect(ax+w-4,ay+1,4,h-2,1);
    drawRect(ax+w-8,ay+1,4,h-2,1);
    // wheels
    glColor3f(0.12f,0.12f,0.12f);
    drawCircle(ax-w*0.45f,ay,4.5f,14,1);
    drawCircle(ax+w*0.45f,ay,4.5f,14,1);
    glColor3f(0.45f,0.45f,0.48f);
    drawCircle(ax-w*0.45f,ay,2.0f,10,1);
    drawCircle(ax+w*0.45f,ay,2.0f,10,1);
    // headlights at FRONT (left side)
    glColor3f(0.98f,0.95f,0.72f);
    drawCircle(ax-w,ay+h*0.25f,1.5f,8,1);
    glColor4f(0.98f,0.95f,0.72f,0.2f);
    drawCircle(ax-w,ay+h*0.25f,8,16,1);
    // tail lights at REAR (right side)
    glColor3f(0.88f,0.08f,0.08f);
    drawCircle(ax+w,ay+h*0.25f,1.2f,8,1);
    // SIREN light on roof
    float sinPhase=sinf(s2_sirenPhase);
    if(sinPhase>0)
    {
        glColor4f(0.9f,0.05f,0.05f,0.9f);
        drawRect(ax-w*0.1f,ay+h,8,4,1);
        glColor4f(1,0.1f,0.1f,0.35f);
        drawCircle(ax-w*0.1f+4,ay+h+4,14,16,1);
    }
    else
    {
        glColor4f(0.05f,0.05f,0.95f,0.9f);
        drawRect(ax-w*0.1f,ay+h,8,4,1);
        glColor4f(0.1f,0.1f,1,0.35f);
        drawCircle(ax-w*0.1f+4,ay+h+4,14,16,1);
    }
}

// ================================================================
// FIX 2: Draw patient on stretcher as a REAL HUMAN FIGURE
// (not just a lump under a sheet)
// ================================================================
static void drawStretcherWithHuman(float sx,float sy,float alpha)
{
    // --- Stretcher frame ---
    glColor4f(0.55f,0.55f,0.58f,alpha);
    glLineWidth(2.5f);
    // side rails
    glBegin(GL_LINES);
    glVertex2f(sx-18,sy+2);
    glVertex2f(sx+18,sy+2);
    glVertex2f(sx-18,sy+8);
    glVertex2f(sx+18,sy+8);
    glEnd();
    // end bars
    glBegin(GL_LINES);
    glVertex2f(sx-18,sy+2);
    glVertex2f(sx-18,sy+8);
    glVertex2f(sx+18,sy+2);
    glVertex2f(sx+18,sy+8);
    glEnd();
    // legs
    glBegin(GL_LINES);
    glVertex2f(sx-14,sy+2);
    glVertex2f(sx-14,sy-5);
    glVertex2f(sx+14,sy+2);
    glVertex2f(sx+14,sy-5);
    glEnd();
    glLineWidth(1.0f);
    // mattress
    glColor4f(0.88f,0.86f,0.78f,alpha);
    drawRect(sx-17,sy+2,34,6,1);
    // white sheet covering lower body
    glColor4f(0.90f,0.88f,0.84f,alpha*0.85f);
    drawRect(sx-17,sy+3,24,5,1);

    // --- Patient human figure lying on stretcher ---
    // Body/torso (military uniform - olive/green)
    float sc = 0.9f; // scale
    float bx = sx + 12.0f; // head end (right side)
    float by = sy + 5.0f;  // vertical center on mattress

    // Torso
    glColor4f(0.22f, 0.30f, 0.18f, alpha);
    glBegin(GL_POLYGON);
    glVertex2f(bx - 14.0f, by - sc*1.8f);
    glVertex2f(bx + 2.0f,  by - sc*1.8f);
    glVertex2f(bx + 2.0f,  by + sc*1.8f);
    glVertex2f(bx - 14.0f, by + sc*1.8f);
    glEnd();

    // Legs
    glColor4f(0.18f,0.22f,0.14f,alpha);
    glBegin(GL_POLYGON);
    glVertex2f(sx-17, by - sc*1.6f);
    glVertex2f(bx-14, by - sc*1.6f);
    glVertex2f(bx-14, by + sc*1.6f);
    glVertex2f(sx-17, by + sc*1.6f);
    glEnd();

    // Boots
    glColor4f(0.10f,0.09f,0.07f,alpha);
    drawRect(sx-17, by - sc*1.8f, 5, sc*3.5f, 1);

    // Arms (at sides)
    glColor4f(0.70f,0.56f,0.44f,alpha); // skin
    // left arm
    glBegin(GL_POLYGON);
    glVertex2f(bx-14, by - sc*0.5f - sc*1.8f);
    glVertex2f(bx+2,  by - sc*0.5f - sc*1.8f);
    glVertex2f(bx+2,  by - sc*0.5f - sc*0.5f);
    glVertex2f(bx-14, by - sc*0.5f - sc*0.5f);
    glEnd();
    // right arm
    glBegin(GL_POLYGON);
    glVertex2f(bx-14, by + sc*0.5f + sc*0.5f);
    glVertex2f(bx+2,  by + sc*0.5f + sc*0.5f);
    glVertex2f(bx+2,  by + sc*0.5f + sc*1.8f);
    glVertex2f(bx-14, by + sc*0.5f + sc*1.8f);
    glEnd();

    // Medals on chest
    glColor4f(0.82f,0.65f,0.15f,alpha);
    for(int mm=0; mm<3; mm++) drawCircle(bx-6+mm*4.0f, by-sc*0.3f, 1.1f, 8, 1);

    // Head
    glColor4f(0.70f,0.56f,0.44f,alpha);
    drawCircle(bx+6.0f, by, sc*3.5f, 16, 1);

    // Military cap
    glColor4f(0.18f,0.26f,0.16f,alpha);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a = -PI + PI*k/12;
        float rr = (a<0) ? sc*3.8f : sc*2.8f;
        glVertex2f(bx+6.0f + cosf(a)*rr, by + sinf(a)*rr*(a<0?0.65f:0.4f));
    }
    glEnd();
    // Cap brim
    glColor4f(0.12f,0.18f,0.10f,alpha);
    glBegin(GL_POLYGON);
    glVertex2f(bx+4.0f, by - sc*3.8f);
    glVertex2f(bx+11.0f, by - sc*3.0f);
    glVertex2f(bx+10.5f, by - sc*2.0f);
    glVertex2f(bx+2.5f, by - sc*2.5f);
    glEnd();

    // Closed eyes (wounded)
    glColor4f(0.22f,0.14f,0.10f,alpha);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(bx+4.5f, by - sc*0.5f);
    glVertex2f(bx+7.0f, by - sc*0.5f);
    glVertex2f(bx+7.5f, by - sc*0.5f);
    glVertex2f(bx+9.0f, by - sc*0.5f);
    glEnd();

    // Beard/mustache (general appearance)
    glColor4f(0.20f,0.13f,0.09f,alpha);
    glBegin(GL_POLYGON);
    for(int k=0; k<=8; k++)
    {
        float a=PI+PI*k/8;
        glVertex2f(bx+6.0f+cosf(a)*2.8f, by+sc*0.5f+sinf(a)*1.4f);
    }
    glEnd();

    // Oxygen mask
    glColor4f(0.55f,0.72f,0.82f,0.55f*alpha);
    drawCircle(bx+6.0f, by+sc*0.2f, sc*2.4f, 12, 1);
    glColor4f(0.45f,0.62f,0.72f,0.35f*alpha);
    glLineWidth(0.8f);
    glBegin(GL_LINES);
    glVertex2f(bx+2.0f, by+sc*0.2f);
    glVertex2f(bx-4.0f, by+sc*1.5f);
    glEnd();
    glLineWidth(1.0f);

    // Blood seeping through sheet
    glColor4f(0.65f,0,0,0.75f*alpha);
    drawCircle(sx-2, by, 3.5f, 12, 1);
    drawCircle(sx+3, by+sc*0.8f, 2.0f, 10, 1);
}

// Medic/paramedic figure
static void drawMedic(float mx,float my,float sc2,float anim)
{
    float legSwing=sinf(anim)*sc2*2.8f;
    glColor3f(0.88f,0.88f,0.88f);
    glBegin(GL_POLYGON);
    glVertex2f(mx-sc2*2,my+sc2*7);
    glVertex2f(mx+sc2*2,my+sc2*7);
    glVertex2f(mx+sc2*1.8f,my);
    glVertex2f(mx-sc2*1.8f,my);
    glEnd();
    glColor3f(0.15f,0.45f,0.22f);
    glBegin(GL_POLYGON);
    glVertex2f(mx-sc2*1.6f,my+sc2*7);
    glVertex2f(mx+sc2*1.6f,my+sc2*7);
    glVertex2f(mx+sc2*1.4f,my+sc2*2);
    glVertex2f(mx-sc2*1.4f,my+sc2*2);
    glEnd();
    glColor3f(0.22f,0.24f,0.28f);
    glBegin(GL_QUADS);
    glVertex2f(mx-sc2*1.8f,my);
    glVertex2f(mx-sc2*0.2f,my);
    glVertex2f(mx-sc2*0.2f+legSwing*0.4f,my-sc2*5);
    glVertex2f(mx-sc2*1.8f+legSwing*0.4f,my-sc2*5);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(mx+sc2*0.2f,my);
    glVertex2f(mx+sc2*1.8f,my);
    glVertex2f(mx+sc2*1.8f-legSwing*0.4f,my-sc2*5);
    glVertex2f(mx+sc2*0.2f-legSwing*0.4f,my-sc2*5);
    glEnd();
    glColor3f(0.1f,0.09f,0.07f);
    drawRect(mx-sc2*2+legSwing*0.4f,my-sc2*5.3f,sc2*1.7f,sc2*0.6f,1);
    drawRect(mx+sc2*0.2f-legSwing*0.4f,my-sc2*5.3f,sc2*1.7f,sc2*0.6f,1);
    glColor3f(0.88f,0.88f,0.88f);
    glLineWidth(sc2*1.8f);
    glBegin(GL_LINES);
    glVertex2f(mx-sc2*2,my+sc2*5.5f);
    glVertex2f(mx-sc2*5,my+sc2*3);
    glVertex2f(mx+sc2*2,my+sc2*5.5f);
    glVertex2f(mx+sc2*5,my+sc2*3);
    glEnd();
    glColor3f(0.72f,0.58f,0.44f);
    drawCircle(mx-sc2*5,my+sc2*3,sc2*0.7f,8,1);
    drawCircle(mx+sc2*5,my+sc2*3,sc2*0.7f,8,1);
    glColor3f(0.72f,0.58f,0.44f);
    drawCircle(mx,my+sc2*9,sc2*2.2f,14,1);
    glColor3f(0.88f,0.88f,0.88f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a=-PI+PI*k/12;
        float rr=(a<0)?sc2*2.4f:sc2*1.8f;
        glVertex2f(mx+cosf(a)*rr,my+sc2*9+sinf(a)*rr*(a<0?0.62f:0.35f));
    }
    glEnd();
    glLineWidth(1.0f);
}

// Doctor figure
static void drawDoctor(float dx,float dy,float sc2,int bowed)
{
    glColor3f(0.88f,0.88f,0.88f);
    glBegin(GL_POLYGON);
    glVertex2f(dx-sc2*2.2f,dy+sc2*7);
    glVertex2f(dx+sc2*2.2f,dy+sc2*7);
    glVertex2f(dx+sc2*1.9f,dy);
    glVertex2f(dx-sc2*1.9f,dy);
    glEnd();
    glColor3f(0.75f,0.75f,0.78f);
    glBegin(GL_POLYGON);
    glVertex2f(dx-sc2*0.4f,dy+sc2*7);
    glVertex2f(dx+sc2*0.4f,dy+sc2*7);
    glVertex2f(dx+sc2*0.3f,dy+sc2*3);
    glVertex2f(dx-sc2*0.3f,dy+sc2*3);
    glEnd();
    glColor3f(0.25f,0.28f,0.32f);
    glBegin(GL_QUADS);
    glVertex2f(dx-sc2*1.9f,dy);
    glVertex2f(dx-sc2*0.1f,dy);
    glVertex2f(dx-sc2*0.1f,dy-sc2*5);
    glVertex2f(dx-sc2*1.9f,dy-sc2*5);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(dx+sc2*0.1f,dy);
    glVertex2f(dx+sc2*1.9f,dy);
    glVertex2f(dx+sc2*1.9f,dy-sc2*5);
    glVertex2f(dx+sc2*0.1f,dy-sc2*5);
    glEnd();
    glColor3f(0.1f,0.09f,0.08f);
    drawRect(dx-sc2*2,dy-sc2*5.2f,sc2*1.8f,sc2*0.6f,1);
    drawRect(dx+sc2*0.1f,dy-sc2*5.2f,sc2*1.8f,sc2*0.6f,1);
    glColor3f(0.72f,0.58f,0.46f);
    float hY=bowed?dy+sc2*8.2f:dy+sc2*9.0f;
    drawCircle(dx,hY,sc2*2.2f,14,1);
    glColor3f(0.18f,0.14f,0.10f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a=-PI+PI*k/12;
        float rr=(a<0)?sc2*2.4f:sc2*1.8f;
        glVertex2f(dx+cosf(a)*rr,hY+sinf(a)*rr*(a<0?0.65f:0.35f));
    }
    glEnd();
    glColor4f(0.85f,0.05f,0.05f,0.9f);
    drawRect(dx-sc2*0.18f,dy+sc2*5,sc2*0.36f,sc2*1.2f,1);
    drawRect(dx-sc2*0.55f,dy+sc2*5.4f,sc2*1.1f,sc2*0.35f,1);
    glColor3f(0.88f,0.88f,0.88f);
    glLineWidth(sc2*1.8f);
    if(bowed)
    {
        glBegin(GL_LINES);
        glVertex2f(dx-sc2*2.2f,dy+sc2*5.5f);
        glVertex2f(dx-sc2*4.5f,dy+sc2*2);
        glVertex2f(dx+sc2*2.2f,dy+sc2*5.5f);
        glVertex2f(dx+sc2*4.5f,dy+sc2*2);
        glEnd();
        glColor3f(0.72f,0.58f,0.46f);
        drawCircle(dx-sc2*4.5f,dy+sc2*2,sc2*0.7f,8,1);
        drawCircle(dx+sc2*4.5f,dy+sc2*2,sc2*0.7f,8,1);
    }
    else
    {
        glBegin(GL_LINES);
        glVertex2f(dx-sc2*2.2f,dy+sc2*5.5f);
        glVertex2f(dx-sc2*4,dy+sc2*1.5f);
        glVertex2f(dx+sc2*2.2f,dy+sc2*5.5f);
        glVertex2f(dx+sc2*4,dy+sc2*1.5f);
        glEnd();
        glColor3f(0.72f,0.58f,0.46f);
        drawCircle(dx-sc2*4,dy+sc2*1.5f,sc2*0.7f,8,1);
        drawCircle(dx+sc2*4,dy+sc2*1.5f,sc2*0.7f,8,1);
    }
    glLineWidth(1.0f);
}

static void drawGriever(float gx,float gy2,float sc2,int kneeling)
{
    if(kneeling)
    {
        glColor3f(0.20f,0.28f,0.18f);
        glBegin(GL_POLYGON);
        glVertex2f(gx-sc2*2,gy2+sc2*6);
        glVertex2f(gx+sc2*2,gy2+sc2*6);
        glVertex2f(gx+sc2*1.8f,gy2+sc2*1);
        glVertex2f(gx-sc2*1.8f,gy2+sc2*1);
        glEnd();
        glColor3f(0.15f,0.22f,0.14f);
        glBegin(GL_POLYGON);
        glVertex2f(gx-sc2*1.8f,gy2+sc2*1);
        glVertex2f(gx-sc2*0.2f,gy2+sc2*1);
        glVertex2f(gx-sc2*0.5f,gy2-sc2*4);
        glVertex2f(gx-sc2*2.8f,gy2-sc2*2);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(gx+sc2*0.2f,gy2+sc2*1);
        glVertex2f(gx+sc2*1.8f,gy2+sc2*1);
        glVertex2f(gx+sc2*2.8f,gy2-sc2*2);
        glVertex2f(gx+sc2*0.5f,gy2-sc2*4);
        glEnd();
    }
    else
    {
        glColor3f(0.82f,0.82f,0.82f);
        glBegin(GL_POLYGON);
        glVertex2f(gx-sc2*2,gy2+sc2*7);
        glVertex2f(gx+sc2*2,gy2+sc2*7);
        glVertex2f(gx+sc2*1.8f,gy2);
        glVertex2f(gx-sc2*1.8f,gy2);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(gx-sc2*1.8f,gy2);
        glVertex2f(gx+sc2*1.8f,gy2);
        glVertex2f(gx+sc2*2.2f,gy2-sc2*4);
        glVertex2f(gx-sc2*2.2f,gy2-sc2*4);
        glEnd();
    }
    glColor3f(0.72f,0.58f,0.46f);
    float hY=kneeling?gy2+sc2*9.0f:gy2+sc2*9.5f;
    drawCircle(gx,hY,sc2*2.0f,12,1);
    glColor3f(0.72f,0.58f,0.46f);
    glLineWidth(sc2*1.5f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*2,gy2+sc2*5.5f);
    glVertex2f(gx-sc2*3.5f,hY-sc2*1);
    glVertex2f(gx+sc2*2,gy2+sc2*5.5f);
    glVertex2f(gx+sc2*3.5f,hY-sc2*1);
    glEnd();
    drawCircle(gx-sc2*3.5f,hY-sc2*1,sc2*0.7f,8,1);
    drawCircle(gx+sc2*3.5f,hY-sc2*1,sc2*0.7f,8,1);
    glLineWidth(1.0f);
}

// ================================================================
// FIX 3: ICU patient drawn as a REAL HUMAN lying in bed
// ================================================================
static void drawICUPatient(float bx,float by,float bw,float bh,float ia,int flatlined,float flatlineT)
{
    // Body under sheet - drawn as actual human silhouette lying down
    float sc = 1.0f;
    float px = bx + bw - 20.0f; // head position (right end of bed)
    float py = by + bh * 0.5f + 2.0f; // vertical center

    // Legs (drawn first, under torso)
    glColor4f(0.22f, 0.30f, 0.18f, ia); // military trousers
    glBegin(GL_POLYGON);
    glVertex2f(bx+3,    py - sc*2.0f);
    glVertex2f(px-14,   py - sc*2.0f);
    glVertex2f(px-14,   py + sc*2.0f);
    glVertex2f(bx+3,    py + sc*2.0f);
    glEnd();

    // Boots/feet
    glColor4f(0.10f,0.09f,0.07f,ia);
    drawRect(bx+3, py - sc*2.2f, 5.5f, sc*4.4f, 1);

    // Torso (military jacket)
    glColor4f(0.22f, 0.30f, 0.18f, ia);
    glBegin(GL_POLYGON);
    glVertex2f(px-22,  py - sc*2.8f);
    glVertex2f(px-4,   py - sc*2.8f);
    glVertex2f(px-4,   py + sc*2.8f);
    glVertex2f(px-22,  py + sc*2.8f);
    glEnd();

    // Arms at sides (skin)
    glColor4f(0.70f, 0.56f, 0.44f, ia);
    // top arm
    glBegin(GL_POLYGON);
    glVertex2f(px-22, py - sc*4.2f);
    glVertex2f(px-5,  py - sc*4.2f);
    glVertex2f(px-5,  py - sc*2.9f);
    glVertex2f(px-22, py - sc*2.9f);
    glEnd();
    // bottom arm
    glBegin(GL_POLYGON);
    glVertex2f(px-22, py + sc*2.9f);
    glVertex2f(px-5,  py + sc*2.9f);
    glVertex2f(px-5,  py + sc*4.2f);
    glVertex2f(px-22, py + sc*4.2f);
    glEnd();

    // Medals/ribbons on chest
    glColor4f(0.82f, 0.65f, 0.15f, ia);
    for(int mm=0; mm<4; mm++) drawCircle(px-20+mm*4.5f, py-sc*0.8f, 1.3f, 8, 1);
    glColor4f(0.72f, 0.10f, 0.10f, ia);
    for(int mm=0; mm<4; mm++) drawRect(px-21+mm*4.5f, py, 2.2f, 1.0f, 1);

    // IV tube to arm
    glColor4f(0.65f, 0.82f, 0.65f, 0.6f*ia);
    glLineWidth(1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(px-10, py-sc*4.0f);
    glVertex2f(px-10, py-sc*7.0f);
    glEnd();

    // Head (skin)
    glColor4f(0.70f, 0.56f, 0.44f, ia);
    drawCircle(px+2.0f, py, sc*4.5f, 18, 1);

    // Military general's cap
    glColor4f(0.12f, 0.20f, 0.10f, ia);
    glBegin(GL_POLYGON);
    for(int k=0; k<=14; k++)
    {
        float a = -PI + PI*k/14;
        float rr = (a<0) ? sc*4.8f : sc*3.5f;
        glVertex2f(px+2.0f + cosf(a)*rr, py + sinf(a)*rr*(a<0?0.65f:0.40f));
    }
    glEnd();
    // Gold band on cap
    glColor4f(0.82f, 0.65f, 0.15f, ia);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for(int k=4; k<=10; k++)
    {
        float a = -PI + PI*k/14;
        glVertex2f(px+2.0f + cosf(a)*sc*4.8f, py + sinf(a)*sc*4.8f*0.65f);
    }
    glEnd();
    // Cap peak/brim
    glColor4f(0.08f,0.14f,0.06f,ia);
    glBegin(GL_POLYGON);
    glVertex2f(px-2.0f, py - sc*4.8f);
    glVertex2f(px+8.0f, py - sc*3.8f);
    glVertex2f(px+8.5f, py - sc*2.5f);
    glVertex2f(px+0.5f, py - sc*3.0f);
    glEnd();

    // Eyes (closed - wounded)
    glColor4f(0.22f, 0.14f, 0.10f, ia);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(px-0.5f, py - sc*1.0f);
    glVertex2f(px+2.5f, py - sc*1.0f);
    glVertex2f(px+3.2f, py - sc*1.0f);
    glVertex2f(px+5.5f, py - sc*1.0f);
    glEnd();
    // Nose
    glColor4f(0.55f,0.42f,0.32f,ia);
    glLineWidth(1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(px+2.0f, py - sc*0.5f);
    glVertex2f(px+3.5f, py + sc*0.8f);
    glVertex2f(px+2.2f, py + sc*1.0f);
    glEnd();

    // Mustache/beard (general look)
    glColor4f(0.22f, 0.14f, 0.08f, ia);
    glBegin(GL_POLYGON);
    for(int k=0; k<=10; k++)
    {
        float a=PI+PI*k/10;
        glVertex2f(px+2.0f+cosf(a)*3.2f, py+sc*1.2f+sinf(a)*1.5f);
    }
    glEnd();

    // Oxygen mask
    glColor4f(0.55f, 0.72f, 0.82f, 0.58f*ia);
    drawCircle(px+2.0f, py+sc*0.5f, sc*3.0f, 14, 1);
    glColor4f(0.45f, 0.62f, 0.72f, 0.35f*ia);
    glLineWidth(0.8f);
    glBegin(GL_LINES);
    glVertex2f(px-1.0f, py+sc*0.5f);
    glVertex2f(px-8.0f, py+sc*2.5f);
    glEnd();

    // White sheet partially covering lower body
    if(!flatlined || flatlineT < 2.0f)
    {
        float sheetA = flatlined ? fmaxf(0, 1.0f - (flatlineT-2.0f)*0.4f) : 1.0f;
        glColor4f(0.90f, 0.88f, 0.84f, sheetA*0.5f*ia);
        drawRect(bx+3, py-sc*2.2f, px-22 - (bx+3), sc*4.4f, 1);
    }

    // Sheet pulled over face after death
    if(flatlined && flatlineT > 2.0f)
    {
        float shA = fminf((flatlineT-2.0f)*0.4f, 0.82f)*ia;
        glColor4f(0.90f, 0.88f, 0.84f, shA);
        drawRect(bx+3, py-sc*2.5f, bw-6, sc*5.0f, 1);
    }

    // Blood stain on sheet/mattress
    glColor4f(0.60f, 0, 0, 0.85f*ia);
    drawCircle(bx+bw*0.35f, py, 7.0f, 16, 1);
    drawCircle(bx+bw*0.45f, py+sc*1.5f, 4.5f, 12, 1);
    glColor4f(0.40f, 0, 0, 0.72f*ia);
    drawCircle(bx+bw*0.40f, py, 4.5f, 12, 1);

    glLineWidth(1.0f);
}

static void drawScene2()
{
    // Night sky
    glColor3f(0.04f,0.04f,0.08f);
    drawRect(-160,-80,320,180,1);
    drawStars(0.6f);
    drawMoon(110,72);

    // Hospital building facade
    glColor3f(0.80f,0.80f,0.76f);
    drawRect(-145,-55,290,152,1);
    glColor3f(0.68f,0.68f,0.64f);
    drawRect(-145,95,290,4,1);
    glColor3f(0.62f,0.62f,0.58f);
    for(int cx=-140; cx<140; cx+=12) drawRect((float)cx,99,8,5,1);

    // Large red H sign on roof
    glColor4f(0.85f,0.04f,0.04f,0.95f);
    drawRect(-10,84,6,16,1);
    drawRect(4,84,6,16,1);
    drawRect(-10,91,20,5,1);

    // Hospital windows
    for(int row=0; row<6; row++)
    {
        for(int col=0; col<9; col++)
        {
            float wx=-132+col*30.0f, wy=18+row*12.0f;
            if((row+col)%3!=0)
            {
                float flk=0.82f+0.18f*sinf(g_time*8+row*1.2f+col*0.9f);
                glColor4f(0.90f*flk,0.86f*flk,0.52f*flk,0.88f);
            }
            else
            {
                glColor4f(0.10f,0.10f,0.16f,0.92f);
            }
            drawRect(wx,wy,20,8,1);
            glColor4f(0.62f,0.62f,0.60f,1);
            drawRect(wx,wy,20,8,0);
            glLineWidth(0.6f);
            glBegin(GL_LINES);
            glVertex2f(wx+10,wy);
            glVertex2f(wx+10,wy+8);
            glVertex2f(wx,wy+4);
            glVertex2f(wx+20,wy+4);
            glEnd();
            glLineWidth(1.0f);
        }
    }

    // Emergency entrance
    glColor3f(0.55f,0.55f,0.52f);
    drawRect(-40,-55,80,30,1);
    glColor3f(0.18f,0.18f,0.22f);
    glBegin(GL_POLYGON);
    glVertex2f(-30,-55);
    glVertex2f(30,-55);
    glVertex2f(30,-35);
    for(int k=0; k<=12; k++)
    {
        float a=PI+PI*k/12;
        glVertex2f(cosf(a)*30,-35+sinf(a)*20+20);
    }
    glEnd();
    glColor3f(0.06f,0.06f,0.06f);
    drawRect(-38,-30,76,8,1);
    float emFlk=0.8f+0.2f*sinf(g_time*5);
    glColor4f(1,0.18f,0.04f,emFlk);
    drawText(-32,-28,"EMERGENCY",0.32f);
    glColor4f(0.88f,0.04f,0.04f,0.92f);
    drawRect(-5,-20,4,12,1);
    drawRect(-9,-14,12,4,1);

    // Ground / road
    glColor3f(0.18f,0.17f,0.15f);
    drawRect(-160,-80,320,26,1);
    glColor4f(0.55f,0.52f,0.42f,0.5f);
    for(int rx=-155; rx<155; rx+=22)drawRect((float)rx,-65,13,2,1);
    glColor3f(0.30f,0.29f,0.27f);
    drawRect(-145,-56,290,2,1);

    // Streetlamps
    for(int lx=-130; lx<=130; lx+=52)
    {
        glColor3f(0.42f,0.42f,0.45f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f((float)lx,-56);
        glVertex2f((float)lx,-38);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f((float)lx,-38);
        glVertex2f((float)lx+5,-38);
        glEnd();
        glLineWidth(1.0f);
        glColor3f(0.35f,0.35f,0.38f);
        drawRect((float)lx+3,-40,4,3,1);
        float flk=0.8f+0.2f*sinf(g_time*7+lx);
        glColor4f(0.98f,0.92f,0.6f,0.3f*flk);
        drawCircle(lx+5,-39,6,12,1);
        glColor4f(0.98f,0.92f,0.6f,0.08f*flk);
        glBegin(GL_POLYGON);
        for(int k=0; k<14; k++)
        {
            float a=TWO_PI*k/14;
            glVertex2f(lx+5+cosf(a)*14,-56+sinf(a)*2.5f);
        }
        glEnd();
    }

    // PHASE 0-2: EXTERIOR
    if(s2_phase<=2 || (s2_phase==3 && s2_icuAlpha<1.0f))
    {
        float extAlpha=(s2_phase<=2)?1.0f:(1.0f-s2_icuAlpha);

        // Smoke particles (drawn BEFORE ambulance so they appear BEHIND it)
        pDraw();

        // Draw ambulance on top of smoke
        drawAmbulance(s2_ambX,-55);

        // Phase 1+: stretcher with human figure being pulled out
        if((s2_phase==1||s2_phase==2) && s2_stretchAlpha>0.01f)
        {
            float sa=s2_stretchAlpha*extAlpha;
            drawStretcherWithHuman(s2_stretchX,-47,sa);
            float mAnim=g_time*4.0f;
            if(s2_phase==2)
            {
                drawMedic(s2_stretchX-22,-55,1.2f,mAnim);
                drawMedic(s2_stretchX+20,-55,1.2f,mAnim+PI);
            }
            else
            {
                drawMedic(s2_ambX+15,-55,1.2f,0);
                drawMedic(s2_ambX+28,-55,1.2f,0.5f);
            }
        }

        // bystander staff
        float staffX[]= {-130,-110,75,95};
        for(int si=0; si<4; si++)
        {
            drawDoctor(staffX[si],-55,1.1f,0);
        }
    }

    // PHASE 3+: ICU INTERIOR
    if(s2_icuAlpha>0.01f)
    {
        float ia=s2_icuAlpha;

        // Dark ICU overlay
        glColor4f(0.06f,0.05f,0.04f,ia*0.92f);
        drawRect(-160,-80,320,180,1);

        // ICU room walls
        glColor4f(0.84f,0.82f,0.76f,ia);
        drawRect(-105,-55,210,83,1);
        glColor4f(0.78f,0.76f,0.72f,ia);
        drawRect(-105,26,210,3,1);
        glColor4f(0.56f,0.54f,0.50f,ia);
        drawRect(-105,-55,210,20,1);
        glColor4f(0,0,0,0.06f*ia);
        glLineWidth(0.5f);
        for(int tx=-104; tx<104; tx+=16)
        {
            glBegin(GL_LINES);
            glVertex2f((float)tx,-35);
            glVertex2f((float)tx,26);
            glEnd();
        }
        for(int ty=-32; ty<26; ty+=12)
        {
            glBegin(GL_LINES);
            glVertex2f(-105,(float)ty);
            glVertex2f(105,(float)ty);
            glEnd();
        }
        for(int tx=-104; tx<104; tx+=20)
        {
            glBegin(GL_LINES);
            glVertex2f((float)tx,-55);
            glVertex2f((float)tx,-35);
            glEnd();
        }
        for(int ty=-52; ty<=-36; ty+=8)
        {
            glBegin(GL_LINES);
            glVertex2f(-105,(float)ty);
            glVertex2f(105,(float)ty);
            glEnd();
        }
        glLineWidth(1.0f);

        // Red cross on wall
        glColor4f(0.82f,0.04f,0.04f,0.9f*ia);
        drawRect(65,4,7,20,1);
        drawRect(59,11,19,7,1);

        // ICU sign
        glColor4f(0.06f,0.06f,0.06f,ia);
        drawRect(30,18,24,7,1);
        float icuFlk=0.9f+0.1f*sinf(g_time*6);
        glColor4f(0.2f,0.8f,0.9f,icuFlk*ia);
        drawText(32,19,"ICU",0.22f);

        // Ceiling lights
        float lFlk=s2_flatlined?(0.3f+0.7f*fabsf(sinf(g_time*1.5f))):(0.92f+0.08f*sinf(g_time*7));
        glColor4f(0.96f,0.93f,0.82f,lFlk*ia);
        drawRect(-60,22,50,4,1);
        drawRect(15,22,50,4,1);
        glColor4f(0.96f,0.93f,0.82f,0.12f*lFlk*ia);
        glBegin(GL_POLYGON);
        glVertex2f(-60,26);
        glVertex2f(-10,26);
        glVertex2f(8,-32);
        glVertex2f(-78,-32);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(15,26);
        glVertex2f(65,26);
        glVertex2f(82,-32);
        glVertex2f(-3,-32);
        glEnd();

        // BED
        float bx=-42,by=-48,bw=80,bh=18;
        glColor4f(0.52f,0.55f,0.60f,ia);
        drawRect(bx,by,bw,3,1);
        drawRect(bx,by+bh,bw,2.5f,1);
        drawRect(bx-4,by,4,bh+2,1);
        drawRect(bx+bw,by,4,bh+2,1);
        glColor4f(0.45f,0.48f,0.52f,ia);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(bx-2,by);
        glVertex2f(bx-2,by-9);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(bx+bw+2,by);
        glVertex2f(bx+bw+2,by-9);
        glEnd();
        glLineWidth(1.0f);
        // mattress
        glColor4f(0.88f,0.86f,0.80f,ia);
        drawRect(bx+1,by+2,bw-2,bh-1,1);

        // IV STAND
        glColor4f(0.52f,0.52f,0.56f,ia);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(bx-14,by);
        glVertex2f(bx-14,by+48);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(bx-14,by+48);
        glVertex2f(bx-4,by+48);
        glEnd();
        glLineWidth(1.0f);
        glColor4f(0.60f,0.82f,0.60f,0.82f*ia);
        drawRect(bx-20,by+36,14,12,1);
        float bagLv=s2_flatlined?0.15f:0.55f;
        glColor4f(0.4f,0.70f,0.40f,0.5f*ia);
        drawRect(bx-19,by+37,12,12.0f*bagLv,1);
        glColor4f(0.2f,0.2f,0.22f,ia);
        drawRect(bx-19,by+40,12,4,1);
        glColor4f(0.8f,0.8f,0.8f,0.9f*ia);
        drawText(bx-18.5f,by+41,"IV",0.12f);
        glColor4f(0.65f,0.82f,0.65f,0.7f*ia);
        glLineWidth(1.2f);
        float dripPh=fmodf(s2_ivDrip*0.9f,1.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(bx-13,by+36);
        glVertex2f(bx-13,by+36-dripPh*22);
        glVertex2f(bx-7,by+36-dripPh*22);
        glVertex2f(bx-7,by+bh*0.55f);
        glEnd();
        if(!s2_flatlined)
        {
            float dA=dripPh*(1-dripPh)*4;
            glColor4f(0.5f,0.82f,0.5f,dA*ia);
            drawCircle(bx-13,by+36-dripPh*22,1.0f,6,1);
        }
        glLineWidth(1.0f);

        // HEART MONITOR
        float monX=bx+bw+14,monY=by+2,monW=54,monH=29;
        glColor4f(0.22f,0.24f,0.28f,ia);
        drawRect(monX-2,monY-2,monW+4,monH+4,1);
        glColor4f(0,0.06f,0,ia);
        drawRect(monX,monY,monW,monH,1);
        glColor4f(0.18f,0.20f,0.24f,ia);
        drawRect(monX+monW/2-3,monY-9,6,9,1);
        drawRect(monX+monW/2-9,monY-11,18,3,1);
        glColor4f(0.3f,0.3f,0.35f,0.8f*ia);
        drawText(monX+1,monY+monH-3,"MEDIX",0.10f);

        // ECG line
        glLineWidth(1.8f);
        if(!s2_flatlined)
        {
            float ecgPh=fmodf(g_time*1.4f,1.0f);
            glColor4f(0,0.9f,0.2f,ia);
            glBegin(GL_LINE_STRIP);
            for(int k=0; k<=52; k++)
            {
                float t=(float)k/52;
                float ex=monX+2+t*50;
                float ey,rel=t-ecgPh;
                if(rel<0)rel+=1;
                if(rel<0.06f)      ey=monY+monH*0.45f+sinf(rel/0.06f*PI)*11;
                else if(rel<0.12f) ey=monY+monH*0.45f-sinf((rel-0.06f)/0.06f*PI)*5;
                else               ey=monY+monH*0.45f;
                glVertex2f(ex,ey);
            }
            glEnd();
            glColor4f(0,0.85f,0.18f,0.9f*ia);
            drawText(monX+2,monY+2,"BPM: 78",0.13f);
        }
        else
        {
            float flashR=s2_flatlineT<0.5f?(0.5f+0.5f*sinf(s2_flatlineT*30)):1.0f;
            glColor4f(flashR,0,0,ia);
            glBegin(GL_LINE_STRIP);
            for(int k=0; k<=52; k++)
            {
                float t=(float)k/52;
                glVertex2f(monX+2+t*50,monY+monH*0.45f);
            }
            glEnd();
            float ftFlk=0.6f+0.4f*sinf(g_time*8);
            glColor4f(1,0,0,ftFlk*ia);
            drawText(monX+4,monY+2,"FLATLINE",0.14f);
            glColor4f(1,0,0,ftFlk*ia);
            drawCircle(monX+monW-5,monY+monH-5,3,10,1);
        }
        glColor4f(0.3f,0.6f,1.0f,0.85f*ia);
        drawText(monX+2,monY+7,"SPO2:",0.11f);
        if(!s2_flatlined)
        {
            glColor4f(0.3f,0.6f,1.0f,0.85f*ia);
            drawText(monX+22,monY+7,"98%",0.11f);
        }
        else
        {
            float spFlk=0.5f+0.5f*sinf(g_time*6);
            glColor4f(1,0.3f,0,spFlk*ia);
            drawText(monX+22,monY+7,"---",0.11f);
        }
        glLineWidth(1.0f);

        // FIX 3: Draw real human patient on bed
        drawICUPatient(bx, by, bw, bh, ia, s2_flatlined, s2_flatlineT);

        // Grieving figures
        drawGriever(-88,-36,1.3f,1);
        drawGriever(-70,-36,1.3f,0);
        drawGriever(-50,-36,1.3f,1);
        drawGriever(bx+bw+76,-36,1.3f,0);
        drawGriever(bx+bw+94,-36,1.3f,1);

        // Doctor
        int docBowed=(s2_flatlineT>3.0f)?1:0;
        drawDoctor(s2_doctorX,-36,1.45f,docBowed);

        pDraw();

        // Dim overlay on flatline
        if(s2_dimAlpha>0)
        {
            glColor4f(0,0,0,s2_dimAlpha*ia);
            drawRect(-160,-80,320,180,1);
        }

        // SORRY MESSAGE
        if(s2_msgAlpha>0.01f)
        {
            float ma=s2_msgAlpha*ia;
            glColor4f(0,0,0,0.75f*ma);
            drawRect(-135,-22,270,35,1);
            glColor4f(0.75f,0.62f,0.12f,0.9f*ma);
            drawRect(-135,-22,270,35,0);
            glColor4f(0.95f,0.92f,0.88f,ma);
            drawText(-125,-5,"SORRY, WE COULDN'T SAVE OUR LEADER",0.38f);
            glColor4f(0.80f,0.22f,0.22f,ma*0.95f);
            drawText(-80,-17,"-- DR. A.K.M TAMIM RAHMAN --",0.30f);
            float bl=0.5f+0.5f*sinf(g_time*4);
            glColor4f(1,0,0,bl*ma);
            drawCircle(-128,-4,3.5f,12,1);
            //drawText(-122,-5.5f,"CARDIAC ARREST",0.25f);
        }
    }
    else
    {
        // phases 0-2 exterior particles drawn above (before ambulance)
    }

    drawVignette(0.58f);
}

// ================================================================
// SCENE 3 - JANAZA / FUNERAL  v3.2 OVERHAUL
// Buildings: Scene-1 style (intact 3D with windows), fire at hit spots
// No flags, no stage, no podium
// Coffin on ground, General beside coffin
// More burning cars, larger mourners
// Realistic General human figure
// ================================================================

// Scene-1-style building with fire spots (for Scene 3 backdrop)
static void drawS3Building(float bx,float by,float bw,float bh,
                           float br,float bg,float bb,float bseed,
                           int winLitArr[][20])
{

    // shadow
    glColor4f(0,0,0,0.3f);
    glBegin(GL_QUADS);
    glVertex2f(bx+2,by-2);
    glVertex2f(bx+bw+2,by-2);
    glVertex2f(bx+bw,by);
    glVertex2f(bx,by);
    glEnd();
    // front face gradient
    glBegin(GL_QUADS);
    glColor3f(br*0.58f,bg*0.58f,bb*0.58f);
    glVertex2f(bx,by);
    glVertex2f(bx+bw,by);
    glColor3f(br,bg,bb);
    glVertex2f(bx+bw,by+bh);
    glVertex2f(bx,by+bh);
    glEnd();
    // 3D right side
    glBegin(GL_QUADS);
    glColor3f(br*0.38f,bg*0.38f,bb*0.38f);
    glVertex2f(bx+bw,by);
    glVertex2f(bx+bw+3,by+3);
    glColor3f(br*0.65f,bg*0.65f,bb*0.65f);
    glVertex2f(bx+bw+3,by+bh+3);
    glVertex2f(bx+bw,by+bh);
    glEnd();
    // top face
    glBegin(GL_QUADS);
    glColor3f(br*1.1f,bg*1.1f,bb*1.1f);
    glVertex2f(bx,by+bh);
    glVertex2f(bx+bw,by+bh);
    glVertex2f(bx+bw+3,by+bh+3);
    glVertex2f(bx+3,by+bh+3);
    glEnd();
    // windows
    // --- UPDATED WINDOW DRAWING LOGIC ---
    float winW = 2.2f;
    float winH = 3.0f;
    float stepX = 4.5f;
    float stepY = 5.0f;

    int col = 0;
    for(float wx = bx + 1.2f; wx < (bx + bw - winW); wx += stepX)
    {
        int row = 0;
        // The condition "wy < by + bh - 4" ensures windows go to the top
        for(float wy = by + 3.0f; wy < (by + bh - 4.0f); wy += stepY)
        {
            // Use 'row % 20' to safely loop through the array if building is very tall
            if(col < 10 && winLitArr[col][row % 20])
            {
                float flk = 0.7f + 0.3f * sinf(g_time * 12 + bseed + col + row);
                float fireGlow = 0.5f + 0.5f * sinf(g_time * 7 + bseed + col + row);

                glColor3f(0.95f * flk, 0.6f * flk * fireGlow, 0.2f * flk);

                glBegin(GL_QUADS);
                    glVertex2f(wx, wy);
                    glVertex2f(wx + winW, wy);
                    glVertex2f(wx + winW, wy + winH);
                    glVertex2f(wx, wy + winH);
                glEnd();

                // Fill gaps with glow spill
                glColor4f(0.0f, 0.0f, 0.0f, 0.0f * flk);
                glBegin(GL_QUADS);
                    glVertex2f(wx - 0.5f, wy - 0.5f);
                    glVertex2f(wx + winW + 0.5f, wy - 0.5f);
                    glVertex2f(wx + winW + 0.5f, wy + winH + 0.5f);
                    glVertex2f(wx - 0.5f, wy + winH + 0.5f);
                glEnd();
            }
            row++;
        }
        col++;
    }
    // antenna
    glColor3f(br*0.8f,bg*0.8f,bb*0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(bx+bw/2,by+bh+3);
    glVertex2f(bx+bw/2,by+bh+9);
    glEnd();
    float blink=sinf(g_time*2+bseed);
    glColor3f(blink>0?0.9f:0.3f,0,0);
    drawCircle(bx+bw/2,by+bh+9,0.8f,8,1);
    // outline
    glColor4f(0,0,0,0.5f);
    glLineWidth(0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bx,by);
    glVertex2f(bx+bw,by);
    glVertex2f(bx+bw,by+bh);
    glVertex2f(bx,by+bh);
    glEnd();
    glLineWidth(1.0f);
}

// Fire on building facade (spot fire, Scene 1 style)
static void drawBuildingFireSpot(float fx,float fy,float phase,float size)
{
    float fp=phase+g_time*3.2f;
    float fw=size,fh=size*3.5f;
    glColor4f(0.88f,0.26f,0.02f,0.88f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx-fw,fy);
    glVertex2f(fx+fw,fy);
    glVertex2f(fx+sinf(fp*1.4f)*fw*0.5f,fy+fh);
    glEnd();
    glColor4f(1.0f,0.62f,0.05f,0.72f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx-fw*0.6f,fy);
    glVertex2f(fx+fw*0.6f,fy);
    glVertex2f(fx+sinf(fp*2.2f)*fw*0.3f,fy+fh*0.72f);
    glEnd();
    glColor4f(1.0f,0.88f,0.5f,0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx-fw*0.3f,fy);
    glVertex2f(fx+fw*0.3f,fy);
    glVertex2f(fx+sinf(fp*3.0f)*fw*0.12f,fy+fh*0.5f);
    glEnd();
    // glow halo
    glColor4f(1.0f,0.36f,0.0f,0.05f+0.03f*sinf(fp));
    drawCircle(fx,fy+fh*0.35f,(fw+fh)*0.5f,16,1);
}

// Burning car wreck (charred remains, fires visible)
static void drawBurntCarWreck(float cx,float cy,float phase)
{
    // car shadow
    glColor4f(0,0,0,0.3f);
    glBegin(GL_POLYGON);
    for(int k=0; k<8; k++)
    {
        float a=TWO_PI*k/8;
        glVertex2f(cx+cosf(a)*9,cy-1+sinf(a)*2);
    }
    glEnd();
    // burnt body (charred dark orange/black)
    glColor3f(0.12f,0.08f,0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-9,cy+0.5f);
    glVertex2f(cx+9,cy+0.5f);
    glVertex2f(cx+9,cy+5);
    glVertex2f(cx-9,cy+5);
    glEnd();
    // collapsed roof
    glColor3f(0.09f,0.06f,0.04f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-6,cy+5);
    glVertex2f(cx+5,cy+5);
    glVertex2f(cx+3.5f,cy+8.5f);
    glVertex2f(cx-4.5f,cy+8.5f);
    glEnd();
    // cracked dark windows
    glColor3f(0.03f,0.03f,0.04f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-3,cy+5.2f);
    glVertex2f(cx+1,cy+5.2f);
    glVertex2f(cx+0.5f,cy+8);
    glVertex2f(cx-2,cy+8);
    glEnd();
    // flat/burst tires
    glColor3f(0.06f,0.05f,0.04f);
    drawCircle(cx-5.5f,cy+0.5f,2.5f,10,1);
    drawCircle(cx+5.5f,cy+0.5f,2.5f,10,1);
    // orange glow from fire underneath
    glColor4f(0.88f,0.32f,0.02f,0.08f+0.04f*sinf(g_time*3+cx));
    drawCircle(cx,cy+3,12,16,1);
    // car fires - separate flame columns
    float fp=phase+g_time*3.0f;
    // left fire
    float fhl = 7 + 3 * sinf(fp);
    float fwl = 2.5f;
    glColor4f(0.90f,0.28f,0.02f,0.88f);
    // Declare 'fhl' variable before using it


    glColor4f(0.90f, 0.28f, 0.02f, 0.85f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx - 5, cy + 5);
    glVertex2f(cx - 1, cy + 5);
    glVertex2f(cx - 3 + sinf(fp * 1.3f) * 1.5f, cy + 5 + fhl);
    glEnd();

    // right fire
    float fh2=6+2.5f*sinf(fp+1.5f);
    glColor4f(0.90f,0.28f,0.02f,0.85f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx+1,cy+5);
    glVertex2f(cx+5,cy+5);
    glVertex2f(cx+3+sinf(fp*1.6f+1)*1.5f,cy+5+fh2);
    glEnd();
    glColor4f(1.0f,0.62f,0.05f,0.70f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx+1.5f,cy+5);
    glVertex2f(cx+4.5f,cy+5);
    glVertex2f(cx+3+sinf(fp*2.4f+1)*1.0f,cy+5+fh2*0.72f);
    glEnd();
    // center fire (hood area)
    float fh3=5+2*sinf(fp+0.8f);
    glColor4f(0.95f,0.38f,0.04f,0.80f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx-2,cy+8.5f);
    glVertex2f(cx+2,cy+8.5f);
    glVertex2f(cx+sinf(fp*1.9f)*1.2f,cy+8.5f+fh3);
    glEnd();
}

// Mourner figure (larger, more visible, black clothes, grief pose)
static void drawMourner(float mx,float my,float sc2,float skinR,float skinG,float skinB,int pose)
{
    // shoes
    glColor3f(0.06f,0.05f,0.04f);
    drawRect(mx-sc2*2.1f,my-sc2*5.3f,sc2*1.9f,sc2*0.7f,1);
    drawRect(mx+sc2*0.2f,my-sc2*5.3f,sc2*1.9f,sc2*0.7f,1);
    // legs (dark trousers)
    glColor3f(0.08f,0.07f,0.09f);
    glBegin(GL_QUADS);
    glVertex2f(mx-sc2*2,my);
    glVertex2f(mx-sc2*0.2f,my);
    glVertex2f(mx-sc2*0.2f,my-sc2*5);
    glVertex2f(mx-sc2*2,my-sc2*5);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(mx+sc2*0.2f,my);
    glVertex2f(mx+sc2*2,my);
    glVertex2f(mx+sc2*2,my-sc2*5);
    glVertex2f(mx+sc2*0.2f,my-sc2*5);
    glEnd();
    // body (black mourning)
    glColor3f(0.07f,0.06f,0.08f);
    glBegin(GL_POLYGON);
    glVertex2f(mx-sc2*2.4f,my+sc2*7);
    glVertex2f(mx+sc2*2.4f,my+sc2*7);
    glVertex2f(mx+sc2*2.0f,my);
    glVertex2f(mx-sc2*2.0f,my);
    glEnd();
    // neck
    glColor3f(skinR,skinG,skinB);
    drawRect(mx-sc2*0.7f,my+sc2*7,sc2*1.4f,sc2*1.5f,1);
    // head
    drawCircle(mx,my+sc2*10,sc2*2.5f,14,1);
    // dark kufi/cap
    glColor3f(0.05f,0.05f,0.06f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a=-PI+PI*k/12;
        float rr=(a<0)?sc2*2.7f:sc2*1.8f;
        glVertex2f(mx+cosf(a)*rr,my+sc2*10+sinf(a)*rr*(a<0?0.65f:0.3f));
    }
    glEnd();
    // face details
    glColor3f(skinR*0.5f,skinG*0.45f,skinB*0.4f);
    drawCircle(mx-sc2*0.8f,my+sc2*10.4f,sc2*0.3f,6,1);
    drawCircle(mx+sc2*0.8f,my+sc2*10.4f,sc2*0.3f,6,1);
    // arms based on pose
    glColor3f(0.07f,0.06f,0.08f);
    glLineWidth(sc2*2.0f);
    if(pose==0)
    {
        // hands clasped / at front
        glBegin(GL_LINES);
        glVertex2f(mx-sc2*2.4f,my+sc2*6);
        glVertex2f(mx-sc2*0.8f,my+sc2*3);
        glVertex2f(mx+sc2*2.4f,my+sc2*6);
        glVertex2f(mx+sc2*0.8f,my+sc2*3);
        glEnd();
        glColor3f(skinR,skinG,skinB);
        drawCircle(mx-sc2*0.8f,my+sc2*3,sc2*0.9f,8,1);
        drawCircle(mx+sc2*0.8f,my+sc2*3,sc2*0.9f,8,1);
    }
    else if(pose==1)
    {
        // hands covering face (grief)
        glBegin(GL_LINES);
        glVertex2f(mx-sc2*2.4f,my+sc2*6);
        glVertex2f(mx-sc2*1.0f,my+sc2*10);
        glVertex2f(mx+sc2*2.4f,my+sc2*6);
        glVertex2f(mx+sc2*1.0f,my+sc2*10);
        glEnd();
        glColor3f(skinR,skinG,skinB);
        drawCircle(mx-sc2*1.0f,my+sc2*10,sc2*1.0f,8,1);
        drawCircle(mx+sc2*1.0f,my+sc2*10,sc2*1.0f,8,1);
    }
    else
    {
        // arms slightly raised (prayer/grief)
        glBegin(GL_LINES);
        glVertex2f(mx-sc2*2.4f,my+sc2*6);
        glVertex2f(mx-sc2*4.5f,my+sc2*8);
        glVertex2f(mx+sc2*2.4f,my+sc2*6);
        glVertex2f(mx+sc2*4.5f,my+sc2*8);
        glEnd();
        glColor3f(skinR,skinG,skinB);
        drawCircle(mx-sc2*4.5f,my+sc2*8,sc2*0.9f,8,1);
        drawCircle(mx+sc2*4.5f,my+sc2*8,sc2*0.9f,8,1);
    }
    glLineWidth(1.0f);
}

// Coffin (no platform - just the coffin box with flag drape)
static void drawCoffin(float cx,float cy)
{
    float cw=38.0f,ch=15.0f;
    // shadow
    glColor4f(0,0,0,0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-cw/2+4,cy-2);
    glVertex2f(cx+cw/2+4,cy-2);
    glVertex2f(cx+cw/2+2,cy+ch-1);
    glVertex2f(cx-cw/2+2,cy+ch-1);
    glEnd();
    // body
    glColor3f(0.12f,0.07f,0.04f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-cw/2+5,cy);
    glVertex2f(cx-cw/2,cy+ch*0.3f);
    glVertex2f(cx-cw/2+2,cy+ch);
    glVertex2f(cx+cw/2-2,cy+ch);
    glVertex2f(cx+cw/2,cy+ch*0.3f);
    glVertex2f(cx+cw/2-5,cy);
    glEnd();
    // wood grain
    glColor4f(0.22f,0.13f,0.07f,0.5f);
    glLineWidth(0.8f);
    for(int g=0; g<5; g++)
    {
        float gy2=cy+2.5f+g*2.4f;
        glBegin(GL_LINES);
        glVertex2f(cx-cw/2+3,gy2);
        glVertex2f(cx+cw/2-3,gy2);
        glEnd();
    }
    // flag drape (national colors)
    float drapeW=cw-3,drapeX=cx-drapeW/2;
    float strH=ch/3.0f;
    float stripeCols[3][3]= {{0.78f,0.0f,0.05f},{0.94f,0.92f,0.88f},{0.0f,0.50f,0.12f}};
    for(int s=0; s<3; s++)
    {
        glBegin(GL_QUAD_STRIP);
        for(int i=0; i<=18; i++)
        {
            float t=(float)i/18;
            float drape=sinf(t*PI)*1.8f;
            float shade=0.75f+0.25f*(1.0f-t)*t*4;
            glColor3f(stripeCols[s][0]*shade,stripeCols[s][1]*shade,stripeCols[s][2]*shade);
            glVertex2f(drapeX+t*drapeW,cy+s*strH+drape);
            glVertex2f(drapeX+t*drapeW,cy+(s+1)*strH+drape);
        }
        glEnd();
    }
    // black triangle on flag
    glColor3f(0.05f,0.05f,0.05f);
    float waveM=sinf(0.35f*PI*1.5f)*3.0f*0.35f;
    glBegin(GL_TRIANGLES);
    glVertex2f(drapeX,cy+0.5f);
    glVertex2f(drapeX,cy+ch-0.5f);
    glVertex2f(drapeX+drapeW*0.3f,cy+waveM+ch*0.5f);
    glEnd();
    // coffin outline
    glColor3f(0.04f,0.02f,0.01f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx-cw/2+5,cy);
    glVertex2f(cx-cw/2,cy+ch*0.3f);
    glVertex2f(cx-cw/2+2,cy+ch);
    glVertex2f(cx+cw/2-2,cy+ch);
    glVertex2f(cx+cw/2,cy+ch*0.3f);
    glVertex2f(cx+cw/2-5,cy);
    glEnd();
    glLineWidth(1.0f);
}

// ================================================================
// IMAM figure - white thobe, large turban, both hands raised in dua
// ix,iy = feet position (standing on top of the janaza stage)
// ================================================================
static void drawImam(float ix,float iy,float sc2){
    float skinR=0.68f,skinG=0.54f,skinB=0.42f;


    // ... (keep existing body/clothing code) ...

    // Dynamic arm height synchronized with General
    // Normal height is around 10.5, raised is 15.5.
    // We multiply the 5.0 unit difference by s3_armRaise.
    float dynamicRaise = 5.0f * s3_armRaise;
    float handY = iy + sc2 * (10.5f + dynamicRaise);

    // LEFT arm raised in dua
    glColor3f(0.92f, 0.90f, 0.88f); glLineWidth(sc2 * 2.8f);
    glBegin(GL_LINES);
    glVertex2f(ix - sc2 * 2.6f, iy + sc2 * 9.0f);
    glVertex2f(ix - sc2 * 5.5f, handY); // Uses handY
    glEnd();

    // Left hand
    glColor3f(skinR, skinG, skinB);
    drawCircle(ix - sc2 * 5.5f, handY, sc2 * 1.3f, 12, 1);

    // RIGHT arm raised in dua
    glColor3f(0.92f, 0.90f, 0.88f); glLineWidth(sc2 * 2.8f);
    glBegin(GL_LINES);
    glVertex2f(ix + sc2 * 2.6f, iy + sc2 * 9.0f);
    glVertex2f(ix + sc2 * 5.5f, handY); // Uses handY
    glEnd();

    // Right hand
    glColor3f(skinR, skinG, skinB);
    drawCircle(ix + sc2 * 5.5f, handY, sc2 * 1.3f, 12, 1);

    // ... (keep rest of head/turban code) ...




    // Sandals
    glColor3f(0.40f,0.30f,0.18f);
    drawRect(ix-sc2*2.1f,iy-sc2*0.5f,sc2*1.8f,sc2*0.5f,1);
    drawRect(ix+sc2*0.3f,iy-sc2*0.5f,sc2*1.8f,sc2*0.5f,1);

    // Long white thobe (full length robe to feet)
    glColor3f(0.93f,0.91f,0.89f);
    glBegin(GL_POLYGON);
    glVertex2f(ix-sc2*2.9f,iy);glVertex2f(ix+sc2*2.9f,iy);
    glVertex2f(ix+sc2*2.6f,iy+sc2*10.5f);glVertex2f(ix-sc2*2.6f,iy+sc2*10.5f);
    glEnd();
    // Thobe center seam / neckline
    glColor3f(0.82f,0.80f,0.78f);
    glBegin(GL_POLYGON);
    glVertex2f(ix-sc2*0.85f,iy+sc2*10.5f);glVertex2f(ix+sc2*0.85f,iy+sc2*10.5f);
    glVertex2f(ix+sc2*0.55f,iy+sc2*7.8f);glVertex2f(ix-sc2*0.55f,iy+sc2*7.8f);
    glEnd();
    // Green belt / sash
    glColor3f(0.10f,0.44f,0.18f);
    drawRect(ix-sc2*2.6f,iy+sc2*4.2f,sc2*5.2f,sc2*1.1f,1);


    glLineWidth(1.0f);

    // Neck
    glColor3f(skinR,skinG,skinB);
    drawRect(ix-sc2*0.9f,iy+sc2*10.5f,sc2*1.8f,sc2*2.0f,1);

    // Head / face
    drawCircle(ix,iy+sc2*14.0f,sc2*2.9f,18,1);

    // Full dark beard
    glColor3f(0.09f,0.07f,0.05f);
    glBegin(GL_POLYGON);
    for(int k=0;k<=14;k++){
        float a=PI*0.12f+PI*0.76f*k/14;
        glVertex2f(ix+cosf(a)*sc2*3.1f,iy+sc2*14.0f+sinf(a)*sc2*2.6f);
    }
    glEnd();
    // Long beard extension downward
    glBegin(GL_POLYGON);
    glVertex2f(ix-sc2*1.9f,iy+sc2*12.2f);glVertex2f(ix+sc2*1.9f,iy+sc2*12.2f);
    glVertex2f(ix+sc2*1.3f,iy+sc2*9.4f);glVertex2f(ix-sc2*1.3f,iy+sc2*9.4f);
    glEnd();
    // Mustache
    glBegin(GL_POLYGON);
    glVertex2f(ix-sc2*1.5f,iy+sc2*13.0f);glVertex2f(ix+sc2*1.5f,iy+sc2*13.0f);
    glVertex2f(ix+sc2*1.2f,iy+sc2*12.3f);glVertex2f(ix-sc2*1.2f,iy+sc2*12.3f);
    glEnd();

    // Eyes (slightly downcast in dua)
    glColor3f(0.92f,0.90f,0.88f);
    drawCircle(ix-sc2*1.1f,iy+sc2*14.7f,sc2*0.6f,10,1);
    drawCircle(ix+sc2*1.1f,iy+sc2*14.7f,sc2*0.6f,10,1);
    glColor3f(0.08f,0.06f,0.04f);
    drawCircle(ix-sc2*1.1f,iy+sc2*14.7f,sc2*0.36f,8,1);
    drawCircle(ix+sc2*1.1f,iy+sc2*14.7f,sc2*0.36f,8,1);
    // Eyelid droop (eyes partially closed)
    glColor3f(skinR*0.75f,skinG*0.75f,skinB*0.75f);glLineWidth(sc2*0.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(ix-sc2*1.7f,iy+sc2*14.7f);glVertex2f(ix-sc2*1.1f,iy+sc2*15.0f);glVertex2f(ix-sc2*0.5f,iy+sc2*14.7f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(ix+sc2*0.5f,iy+sc2*14.7f);glVertex2f(ix+sc2*1.1f,iy+sc2*15.0f);glVertex2f(ix+sc2*1.7f,iy+sc2*14.7f);
    glEnd();
    glLineWidth(1.0f);
    // Eyebrows
    glColor3f(0.09f,0.07f,0.05f);glLineWidth(sc2*0.65f);
    glBegin(GL_LINES);
    glVertex2f(ix-sc2*1.7f,iy+sc2*15.6f);glVertex2f(ix-sc2*0.4f,iy+sc2*15.9f);
    glVertex2f(ix+sc2*0.4f,iy+sc2*15.9f);glVertex2f(ix+sc2*1.7f,iy+sc2*15.6f);
    glEnd();
    glLineWidth(1.0f);

    // *** BIG WHITE TURBAN - multi-layer ***
    // Base dome (large white)
    glColor3f(0.95f,0.93f,0.91f);
    glBegin(GL_POLYGON);
    for(int k=0;k<=20;k++){
        float a=-PI+PI*k/20;
        float rr=(a<0)?sc2*3.4f:sc2*2.5f;
        glVertex2f(ix+cosf(a)*rr,iy+sc2*14.0f+sinf(a)*rr*(a<0?0.74f:0.52f));
    }
    glEnd();
    // Turban wrap ridges
    glColor3f(0.87f,0.85f,0.83f);
    for(int wrap=0;wrap<4;wrap++){
        float wy=iy+sc2*(16.0f-wrap*0.9f);
        float wr2=sc2*(3.3f-wrap*0.18f);
        glLineWidth(sc2*(1.3f-wrap*0.18f));
        glBegin(GL_LINE_STRIP);
        for(int k=7;k<=17;k++){
            float a=-PI+PI*k/20;
            glVertex2f(ix+cosf(a)*wr2,wy+sinf(a)*wr2*0.28f);
        }
        glEnd();
    }
    glLineWidth(1.0f);
    // Turban top knot
    glColor3f(0.91f,0.89f,0.87f);
    drawCircle(ix,iy+sc2*17.2f,sc2*1.35f,14,1);
    // Green scholar stripe on turban
    glColor3f(0.10f,0.44f,0.18f);glLineWidth(sc2*0.65f);
    glBegin(GL_LINE_STRIP);
    for(int k=5;k<=15;k++){
        float a=-PI+PI*k/20;
        glVertex2f(ix+cosf(a)*sc2*3.35f,iy+sc2*14.0f+sinf(a)*sc2*3.35f*0.74f);
    }
    glEnd();
    glLineWidth(1.0f);
}

// ================================================================
// JANAZA STAGE - raised platform beside the coffin
// stageX = center x of stage, stageY = ground level
// ================================================================
static void drawJanazaStage(float stageX,float stageY){
    float sw=80.0f;  // CHANGED: Widened significantly to span the middle gap
    float sh=10.0f;   // stage height (visible above ground)

    // Stage drop shadow
    glColor4f(0,0,0,0.45f);
    glBegin(GL_POLYGON);
    glVertex2f(stageX-sw/2+4,stageY-3);
    glVertex2f(stageX+sw/2+4,stageY-3);
    glVertex2f(stageX+sw/2+2,stageY+sh-1);
    glVertex2f(stageX-sw/2+2,stageY+sh-1);
    glEnd();

    // Stage front face - dark stone/marble gradient
    glBegin(GL_QUADS);
    glColor3f(0.16f,0.12f,0.09f);
    glVertex2f(stageX-sw/2,stageY);glVertex2f(stageX+sw/2,stageY);
    glColor3f(0.30f,0.24f,0.18f);
    glVertex2f(stageX+sw/2,stageY+sh);glVertex2f(stageX-sw/2,stageY+sh);
    glEnd();

    // Right 3D bevel face
    glBegin(GL_QUADS);
    glColor3f(0.10f,0.08f,0.06f);
    glVertex2f(stageX+sw/2,stageY);glVertex2f(stageX+sw/2+3,stageY+3);
    glColor3f(0.18f,0.14f,0.10f);
    glVertex2f(stageX+sw/2+3,stageY+sh+3);glVertex2f(stageX+sw/2,stageY+sh);
    glEnd();

    // Top face
    glBegin(GL_QUADS);
    glColor3f(0.35f,0.28f,0.20f);
    glVertex2f(stageX-sw/2,stageY+sh);glVertex2f(stageX+sw/2,stageY+sh);
    glVertex2f(stageX+sw/2+3,stageY+sh+3);glVertex2f(stageX-sw/2+3,stageY+sh+3);
    glEnd();

    // Horizontal decorative lines on stage face
    glColor4f(0.42f,0.32f,0.22f,0.65f);glLineWidth(0.9f);
    for(int L=1;L<5;L++){
        float ly=stageY+sh*(float)L/5.0f;
        glBegin(GL_LINES);
        glVertex2f(stageX-sw/2+1,ly);glVertex2f(stageX+sw/2-1,ly);
        glEnd();
    }

    // Central decorative panel
    glColor3f(0.22f,0.17f,0.12f);
    drawRect(stageX-9,stageY+2,18,sh-4,1);
    glColor4f(0.50f,0.38f,0.20f,0.75f);
    drawRect(stageX-9,stageY+2,18,sh-4,0);

    // Crescent moon + star on panel (Islamic symbol)
    // Crescent
    glColor4f(0.80f,0.64f,0.18f,0.90f);
    drawCircle(stageX,stageY+sh*0.5f,3.0f,20,0);
    glColor3f(0.22f,0.17f,0.12f);
    drawCircle(stageX+1.4f,stageY+sh*0.5f+0.4f,2.2f,16,1);
    // Star
    glColor4f(0.80f,0.64f,0.18f,0.90f);
    float starX=stageX+3.5f,starY=stageY+sh*0.5f+1.8f,starR=0.8f;
    glBegin(GL_POLYGON);
    for(int k=0;k<10;k++){
        float a=TWO_PI*k/10-PI/2;
        float r=(k%2==0)?starR:starR*0.45f;
        glVertex2f(starX+cosf(a)*r,starY+sinf(a)*r);
    }
    glEnd();

    // Steps on LEFT side of stage
    glColor3f(0.18f,0.14f,0.10f);
    drawRect(stageX-sw/2-7,stageY,7,sh*0.45f,1);
    glColor4f(0.38f,0.30f,0.20f,0.6f);
    drawRect(stageX-sw/2-7,stageY,7,sh*0.45f,0);
    glColor3f(0.22f,0.17f,0.12f);
    drawRect(stageX-sw/2-4,stageY+sh*0.45f,4,sh*0.55f,1);
    glColor4f(0.38f,0.30f,0.20f,0.6f);
    drawRect(stageX-sw/2-4,stageY+sh*0.45f,4,sh*0.55f,0);

    // Steps on RIGHT side of stage (Added for symmetry on big stage)
    glColor3f(0.18f,0.14f,0.10f);
    drawRect(stageX+sw/2,stageY,7,sh*0.45f,1);
    glColor4f(0.38f,0.30f,0.20f,0.6f);
    drawRect(stageX+sw/2,stageY,7,sh*0.45f,0);
    glColor3f(0.22f,0.17f,0.12f);
    drawRect(stageX+sw/2,stageY+sh*0.45f,4,sh*0.55f,1);
    glColor4f(0.38f,0.30f,0.20f,0.6f);
    drawRect(stageX+sw/2,stageY+sh*0.45f,4,sh*0.55f,0);

    // Lectern/podium on top of stage (imam stands behind it)
    float lecTop=stageY+sh+3.0f; // top surface of stage
    float lecX=stageX;
    // Lectern base
    glColor3f(0.32f,0.26f,0.16f);
    glBegin(GL_POLYGON);
    glVertex2f(lecX-4.5f,lecTop);glVertex2f(lecX+4.5f,lecTop);
    glVertex2f(lecX+3.5f,lecTop+7);glVertex2f(lecX-3.5f,lecTop+7);
    glEnd();
    // Lectern top slanted surface
    glColor3f(0.40f,0.32f,0.20f);
    glBegin(GL_POLYGON);
    glVertex2f(lecX-5.5f,lecTop+5.5f);glVertex2f(lecX+5.5f,lecTop+5.5f);
    glVertex2f(lecX+4.5f,lecTop+9.5f);glVertex2f(lecX-4.5f,lecTop+9.5f);
    glEnd();
    // Lectern golden rim
    glColor4f(0.68f,0.55f,0.22f,0.92f);glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(lecX-5.5f,lecTop+5.5f);glVertex2f(lecX+5.5f,lecTop+5.5f);
    glVertex2f(lecX+4.5f,lecTop+9.5f);glVertex2f(lecX-4.5f,lecTop+9.5f);
    glEnd();
    glLineWidth(1.0f);
    // Quran book on lectern (green cover, gold border)
    glColor3f(0.05f,0.24f,0.08f);
    drawRect(lecX-3,lecTop+7,6,3.5f,1);
    glColor4f(0.75f,0.62f,0.18f,0.95f);
    drawRect(lecX-3,lecTop+7,6,3.5f,0);
    // Page lines
    glColor4f(0.88f,0.86f,0.80f,0.75f);glLineWidth(0.6f);
    for(int pg=0;pg<3;pg++){
        float py=lecTop+7.7f+pg*0.7f;
        glBegin(GL_LINES);glVertex2f(lecX-2.4f,py);glVertex2f(lecX+2.4f,py);glEnd();
    }
    glLineWidth(1.0f);

    // Stage outline
    glColor4f(0.50f,0.38f,0.22f,0.85f);glLineWidth(1.4f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(stageX-sw/2,stageY);glVertex2f(stageX+sw/2,stageY);
    glVertex2f(stageX+sw/2,stageY+sh);glVertex2f(stageX-sw/2,stageY+sh);
    glEnd();
    glLineWidth(1.0f);
}


// The new Supreme General - realistic human figure standing beside coffin
// Arm raise: 0=normal, 1=fist raised
static void drawGeneral(float gx,float gy,float sc2,float armRaise)
{
    float skinR=0.72f,skinG=0.58f,skinB=0.44f;

    // Boots
    glColor3f(0.07f,0.06f,0.05f);
    drawRect(gx-sc2*2.3f,gy-sc2*5.5f,sc2*2.0f,sc2*0.8f,1);
    drawRect(gx+sc2*0.3f,gy-sc2*5.5f,sc2*2.0f,sc2*0.8f,1);
    // boot shine
    glColor3f(0.12f,0.10f,0.09f);
    drawRect(gx-sc2*2.1f,gy-sc2*5.3f,sc2*0.6f,sc2*0.3f,1);
    drawRect(gx+sc2*0.5f,gy-sc2*5.3f,sc2*0.6f,sc2*0.3f,1);

    // Trousers (dark military green with red stripe)
    glColor3f(0.10f,0.15f,0.10f);
    glBegin(GL_QUADS);
    glVertex2f(gx-sc2*2.3f,gy-sc2*5);
    glVertex2f(gx-sc2*0.25f,gy-sc2*5);
    glVertex2f(gx-sc2*0.25f,gy);
    glVertex2f(gx-sc2*2.3f,gy);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(gx+sc2*0.25f,gy-sc2*5);
    glVertex2f(gx+sc2*2.3f,gy-sc2*5);
    glVertex2f(gx+sc2*2.3f,gy);
    glVertex2f(gx+sc2*0.25f,gy);
    glEnd();
    // Red trouser stripe
    glColor3f(0.72f,0.06f,0.06f);
    glLineWidth(sc2*0.5f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*1.4f,gy-sc2*5);
    glVertex2f(gx-sc2*1.4f,gy);
    glVertex2f(gx+sc2*1.4f,gy-sc2*5);
    glVertex2f(gx+sc2*1.4f,gy);
    glEnd();
    glLineWidth(1.0f);

    // Uniform jacket (dark military green with details)
    glColor3f(0.13f,0.19f,0.13f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*3.0f,gy+sc2*7.5f);
    glVertex2f(gx+sc2*3.0f,gy+sc2*7.5f);
    glVertex2f(gx+sc2*2.5f,gy);
    glVertex2f(gx-sc2*2.5f,gy);
    glEnd();
    // Jacket lapels
    glColor3f(0.09f,0.13f,0.09f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*1.2f,gy+sc2*7.5f);
    glVertex2f(gx-sc2*3.0f,gy+sc2*7.5f);
    glVertex2f(gx-sc2*0.5f,gy+sc2*3.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(gx+sc2*1.2f,gy+sc2*7.5f);
    glVertex2f(gx+sc2*3.0f,gy+sc2*7.5f);
    glVertex2f(gx+sc2*0.5f,gy+sc2*3.5f);
    glEnd();
    // Button line
    glColor3f(0.08f,0.12f,0.08f);
    for(int b=0; b<4; b++)
    {
        float by2=gy+sc2*1.5f+b*sc2*1.5f;
        drawCircle(gx,by2,sc2*0.25f,6,1);
    }
    // Medals row
    float mc[5][3]= {{0.85f,0.68f,0.14f},{0.80f,0.75f,0.72f},{0.85f,0.68f,0.14f},{0.75f,0.10f,0.10f},{0.85f,0.68f,0.14f}};
    for(int m=0; m<5; m++)
    {
        glColor3f(mc[m][0],mc[m][1],mc[m][2]);
        drawCircle(gx-sc2*2.1f+m*sc2*0.98f,gy+sc2*6.0f,sc2*0.58f,8,1);
        // ribbon
        glColor3f(mc[m][0]*0.7f,mc[m][1]*0.3f,mc[m][2]*0.3f);
        drawRect(gx-sc2*2.2f+m*sc2*0.98f,gy+sc2*6.5f,sc2*0.5f,sc2*0.8f,1);
    }
    // Epaulettes (gold)
    glColor3f(0.82f,0.66f,0.10f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*3.0f,gy+sc2*7.3f);
    glVertex2f(gx-sc2*1.6f,gy+sc2*7.3f);
    glVertex2f(gx-sc2*1.8f,gy+sc2*7.8f);
    glVertex2f(gx-sc2*3.0f,gy+sc2*7.8f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(gx+sc2*1.6f,gy+sc2*7.3f);
    glVertex2f(gx+sc2*3.0f,gy+sc2*7.3f);
    glVertex2f(gx+sc2*3.0f,gy+sc2*7.8f);
    glVertex2f(gx+sc2*1.8f,gy+sc2*7.8f);
    glEnd();
    // Epaulette fringe stars
    glColor3f(0.92f,0.82f,0.22f);
    for(int s=0; s<3; s++)
    {
        drawCircle(gx-sc2*(2.8f-s*0.6f),gy+sc2*7.6f,sc2*0.25f,5,1);
        drawCircle(gx+sc2*(2.8f-s*0.6f),gy+sc2*7.6f,sc2*0.25f,5,1);
    }

    // LEFT arm (resting)
    glColor3f(0.13f,0.19f,0.13f);
    glLineWidth(sc2*2.2f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*3.0f,gy+sc2*7.0f);
    glVertex2f(gx-sc2*5.0f,gy+sc2*4.0f);
    glEnd();
    glColor3f(skinR,skinG,skinB);
    drawCircle(gx-sc2*5.0f,gy+sc2*4.0f,sc2*0.95f,8,1);

    // RIGHT arm - animated raise
    float armEndX=gx+sc2*(3.0f+armRaise*3.5f);
    float armEndY=gy+sc2*(4.0f+armRaise*7.0f);
    glColor3f(0.13f,0.19f,0.13f);
    glLineWidth(sc2*2.2f);
    glBegin(GL_LINES);
    glVertex2f(gx+sc2*3.0f,gy+sc2*7.0f);
    glVertex2f(armEndX,armEndY);
    glEnd();
    glLineWidth(1.0f);
    // Clenched fist (dark glove)
    glColor3f(0.06f,0.05f,0.04f);
    drawCircle(armEndX,armEndY,sc2*1.2f,8,1);
    glColor3f(0.08f,0.07f,0.06f);
    drawCircle(armEndX,armEndY,sc2*0.9f,8,1);

    // Neck
    glColor3f(skinR,skinG,skinB);
    drawRect(gx-sc2*0.9f,gy+sc2*7.5f,sc2*1.8f,sc2*2.0f,1);

    // HEAD - realistic human
    drawCircle(gx,gy+sc2*11.5f,sc2*3.0f,16,1);

    // Facial features
    // Eyebrows (stern, thick)
    glColor3f(0.12f,0.09f,0.06f);
    glLineWidth(sc2*0.8f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*2.2f,gy+sc2*12.5f);
    glVertex2f(gx-sc2*0.5f,gy+sc2*12.9f);
    glVertex2f(gx+sc2*0.5f,gy+sc2*12.9f);
    glVertex2f(gx+sc2*2.2f,gy+sc2*12.5f);
    glEnd();
    glLineWidth(1.0f);
    // Eyes (white + iris + pupil)
    glColor3f(0.92f,0.90f,0.88f);
    drawCircle(gx-sc2*1.1f,gy+sc2*12.0f,sc2*0.62f,10,1);
    drawCircle(gx+sc2*1.1f,gy+sc2*12.0f,sc2*0.62f,10,1);
    glColor3f(0.22f,0.18f,0.55f); // dark blue iris
    drawCircle(gx-sc2*1.1f,gy+sc2*12.0f,sc2*0.40f,8,1);
    drawCircle(gx+sc2*1.1f,gy+sc2*12.0f,sc2*0.40f,8,1);
    glColor3f(0.04f,0.03f,0.03f); // pupil
    drawCircle(gx-sc2*1.1f,gy+sc2*12.0f,sc2*0.22f,6,1);
    drawCircle(gx+sc2*1.1f,gy+sc2*12.0f,sc2*0.22f,6,1);
    // eye shine
    glColor3f(0.85f,0.85f,0.88f);
    drawCircle(gx-sc2*1.0f,gy+sc2*12.2f,sc2*0.10f,4,1);
    drawCircle(gx+sc2*1.2f,gy+sc2*12.2f,sc2*0.10f,4,1);
    // Nose
    glColor3f(skinR*0.7f,skinG*0.6f,skinB*0.55f);
    glLineWidth(sc2*0.6f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx,gy+sc2*11.0f);
    glVertex2f(gx+sc2*0.5f,gy+sc2*10.2f);
    glVertex2f(gx+sc2*1.0f,gy+sc2*10.4f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx,gy+sc2*11.0f);
    glVertex2f(gx-sc2*0.5f,gy+sc2*10.2f);
    glVertex2f(gx-sc2*1.0f,gy+sc2*10.4f);
    glEnd();
    glLineWidth(1.0f);
    // Mouth (firm, closed)
    glColor3f(skinR*0.58f,skinG*0.42f,skinB*0.38f);
    glLineWidth(sc2*0.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx-sc2*1.1f,gy+sc2*9.7f);
    glVertex2f(gx,gy+sc2*9.5f);
    glVertex2f(gx+sc2*1.1f,gy+sc2*9.7f);
    glEnd();
    glLineWidth(1.0f);
    // Thick full beard
    glColor3f(0.12f,0.09f,0.06f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=14; k++)
    {
        float a=PI*0.08f+PI*0.84f*k/14;
        glVertex2f(gx+cosf(a)*sc2*3.1f,gy+sc2*11.5f+sinf(a)*sc2*2.2f);
    }
    glEnd();
    // Mustache
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*1.5f,gy+sc2*10.1f);
    glVertex2f(gx+sc2*1.5f,gy+sc2*10.1f);
    glVertex2f(gx+sc2*1.2f,gy+sc2*9.5f);
    glVertex2f(gx-sc2*1.2f,gy+sc2*9.5f);
    glEnd();
    // Hair visible at sides
    glColor3f(0.10f,0.08f,0.05f);
    glBegin(GL_POLYGON);
    for(int k=8; k<=16; k++)
    {
        float a=-PI+PI*k/16;
        glVertex2f(gx+cosf(a)*sc2*3.0f,gy+sc2*11.5f+sinf(a)*sc2*3.0f*(a<0?0.75f:0.45f));
    }
    glEnd();

    // Military peaked cap
    glColor3f(0.09f,0.14f,0.09f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=16; k++)
    {
        float a=-PI+PI*k/16;
        float rr=(a<0)?sc2*3.4f:sc2*2.5f;
        glVertex2f(gx+cosf(a)*rr,gy+sc2*11.5f+sinf(a)*rr*(a<0?0.68f:0.42f));
    }
    glEnd();
    // Gold cap band
    glColor3f(0.85f,0.68f,0.12f);
    glLineWidth(sc2*0.6f);
    glBegin(GL_LINE_STRIP);
    for(int k=4; k<=12; k++)
    {
        float a=-PI+PI*k/16;
        glVertex2f(gx+cosf(a)*sc2*3.4f,gy+sc2*11.5f+sinf(a)*sc2*3.4f*0.68f);
    }
    glEnd();
    glLineWidth(1.0f);
    // Cap visor (brim sticking out right)
    glColor3f(0.06f,0.10f,0.06f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*2.0f,gy+sc2*14.0f);
    glVertex2f(gx+sc2*4.8f,gy+sc2*13.2f);
    glVertex2f(gx+sc2*5.2f,gy+sc2*11.8f);
    glVertex2f(gx+sc2*1.5f,gy+sc2*12.2f);
    glEnd();
    // Cap star badge
    glColor3f(0.92f,0.82f,0.22f);
    drawCircle(gx-sc2*0.5f,gy+sc2*14.4f,sc2*1.0f,5,1);
}



// Scene 3 pre-built buildings (fixed layout with defined fire spots)
struct S3Building
{
    float x,y,w,h,r,g,b,seed;
    int fires[4][2];
    int nfires;
    int winLit[10][20];
};
static S3Building s3blds[14];
static void initS3Buildings()
{
    srand(5678);
    float palettes[5][3]= {{0.3f,0.3f,0.35f},{0.28f,0.25f,0.28f},{0.22f,0.28f,0.3f},
        {0.35f,0.32f,0.28f},{0.25f,0.22f,0.3f}
    };
    float layout[][4]=
    {
        // left side
        {-158,18,24,1},{-132,14,48,0},{-116,16,38,2},{-98,18,55,3},{-80,14,32,4},
        {-64,16,45,1},{-48,12,28,2},
        // right side
        {44,18,52,3},{62,16,38,0},{80,14,44,1},{98,18,60,2},{116,16,35,3},
        {134,14,48,4},{150,16,32,1}
    };
    for(int i=0; i<14; i++)
    {
        s3blds[i].x=layout[i][0];
        s3blds[i].y=-52;
        s3blds[i].w=layout[i][1];
        s3blds[i].h=layout[i][2];
        int p=(int)layout[i][3];
        s3blds[i].r=palettes[p][0];
        s3blds[i].g=palettes[p][1];
        s3blds[i].b=palettes[p][2];
        s3blds[i].seed=rf(0,100);
        // fire spots (1-4 per building, scattered across face)
        s3blds[i].nfires=ri(2,4);
        for(int f=0; f<s3blds[i].nfires; f++)
        {
            s3blds[i].fires[f][0]=(int)(s3blds[i].x+s3blds[i].w*(0.1f+f*0.25f));
            s3blds[i].fires[f][1]=(int)(s3blds[i].y+s3blds[i].h*(0.2f+rf(0,0.6f)));
        }
        for(int wc=0; wc<10; wc++){
            for(int wr=0; wr<20; wr++){
                // Change this to 0 to turn ALL windows off
                s3blds[i].winLit[wc][wr] = 1;
            }
        }
    }
    srand((unsigned)time(NULL));
}

static void updateScene3(float dt)
{
    s3_wave+=dt*1.5f;
    s3_speechT+=dt;
    s3_bannerFlash+=dt;
    // Arm raise/lower cycle
    float armCycle=fmodf(s3_speechT,8.0f);
    if(armCycle<3.5f) s3_armRaise=fminf(armCycle/1.5f,1.0f);
    else s3_armRaise=fmaxf(0.0f,1.0f-(armCycle-3.5f)/1.5f);
    s3_bannerVisible=(s3_armRaise>0.65f)?1:0;
    // Smoke from buildings
    if(rf(0,1)<0.20f)
    {
        int bi=ri(0,13);
        pSpawn(s3blds[bi].x+s3blds[bi].w*0.5f+rf(-4,4),
               s3blds[bi].y+s3blds[bi].h+rf(0,5),
               rf(-0.2f,0.2f),rf(0.3f,1.0f),0,0,
               rf(0.12f,0.22f),rf(0.10f,0.18f),rf(0.09f,0.16f),0.65f,
               rf(3.0f,9.0f),rf(7,16),1);
    }
    // Ember sparks
    if(rf(0,1)<0.08f)
    {
        int bi=ri(0,13);
        pSpawn(s3blds[bi].x+rf(0,s3blds[bi].w),
               s3blds[bi].y+s3blds[bi].h*rf(0.3f,0.9f),
               rf(-0.6f,0.6f),rf(0.5f,2.0f),0,-0.5f,
               rf(0.9f,1.0f),rf(0.45f,0.65f),0.05f,0.9f,rf(0.4f,1.2f),rf(1,2),2);
    }
    pUpdate(dt);
}

static void drawScene3()
{
    // Dark smoke-orange night sky
    drawSky(0.05f,0.03f,0.06f,0.11f,0.05f,0.02f,0.20f,0.09f,0.01f);
    drawStars(0.45f);
    drawMoon(-78,72);
    // Heavy smoke clouds
    drawCloud(-60,57,1.0f,0.38f,0.18f,0.14f,0.10f);
    drawCloud(25,54,1.1f,0.32f,0.20f,0.16f,0.12f);
    drawCloud(90,60,0.9f,0.35f,0.17f,0.13f,0.09f);
    // Warm fire glow at horizon
    glColor4f(0.75f,0.20f,0.02f,0.12f);
    glBegin(GL_QUADS);
    glVertex2f(-160,-52);
    glVertex2f(160,-52);
    glVertex2f(160,-8);
    glVertex2f(-160,-8);
    glEnd();

    // Ground / road
    glColor3f(0.15f,0.13f,0.11f);
    drawRect(-160,-80,320,29,1);
    // Cracked road
    glColor3f(0.22f,0.19f,0.16f);
    drawRect(-160,-52,320,3,1);
    glColor4f(0.32f,0.30f,0.26f,0.38f);
    for(int rx=-150; rx<150; rx+=22)drawRect((float)rx,-51,11,1.2f,1);

    // ==== LEFT SIDE BUILDINGS (Scene-1 style with fires) ====
    for(int i=0; i<7; i++)
    {
        S3Building&sb=s3blds[i];
        drawS3Building(sb.x,sb.y,sb.w,sb.h,sb.r,sb.g,sb.b,sb.seed,sb.winLit);
        // Draw fire spots (separate, distinct)
        for(int f=0; f<sb.nfires; f++)
        {
            float fx=(float)sb.fires[f][0];
            float fy=(float)sb.fires[f][1];
            float fsz=2.2f+1.5f*sinf(g_time*0.5f+sb.seed+f);
            drawBuildingFireSpot(fx,fy,sb.seed+f*1.8f,fsz);
        }
    }
    // ==== RIGHT SIDE BUILDINGS ====
    for(int i=7; i<14; i++)
    {
        S3Building&sb=s3blds[i];
        drawS3Building(sb.x,sb.y,sb.w,sb.h,sb.r,sb.g,sb.b,sb.seed,sb.winLit);
        for(int f=0; f<sb.nfires; f++)
        {
            float fx=(float)sb.fires[f][0];
            float fy=(float)sb.fires[f][1];
            float fsz=2.2f+1.5f*sinf(g_time*0.5f+sb.seed+f);
            drawBuildingFireSpot(fx,fy,sb.seed+f*1.8f,fsz);
        }
    }

    // ==== MANY BURNING CARS scattered across full width ====
    // Fixed positions for scene 3 - spread across entire road
    float carPos[][3]=
    {
        {-148,-52,0.5f},{-130,-52,1.4f},{-112,-52,2.1f},{-94,-52,0.9f},{-76,-52,1.7f},
        {-58,-52,0.3f},{-38,-52,1.1f},
        {30,-52,1.9f},{50,-52,0.7f},{70,-52,2.3f},
        {90,-52,1.2f},{110,-52,0.6f},{128,-52,1.8f},{148,-52,1.0f}
    };
    for(int i=0; i<14; i++)
        drawBurntCarWreck(carPos[i][0],carPos[i][1],carPos[i][2]);

    // ==== PARTICLES (smoke, embers) ====
    pDraw();

    // ==== JANAZA STAGE ====
    float groundY   = -52.0f;
    float stageCX   = 0.0f;
    float stageH    = 10.0f;
    float stageBevelH = 3.0f;
    drawJanazaStage(stageCX, groundY);

    // ==== IMAM on stage top surface ====

    float imamFeetY = groundY + stageH + stageBevelH;
    float imamScale = 1.00f; // Reduced from 1.7f for a smaller size

    // Center Imam
    drawImam(stageCX, imamFeetY, imamScale);

    // Left Imam (offset by -18 units)
    drawImam(stageCX - 25.0f, imamFeetY, imamScale);

    // Right Imam (offset by +18 units)
    drawImam(stageCX + 25.0f, imamFeetY, imamScale);


    // ==== FUNERAL TORCHES flanking coffin ====


    // ==== COFFIN on ground (no platform, no stairs) ====
    // Small ground cloth beneath coffin
    float coffinY = -68.0f;
    glColor3f(0.10f,0.08f,0.06f);drawRect(-27, coffinY, 54, 3, 1);
    drawCoffin(0, coffinY);
    // ==== MOURNING CROWD (larger figures, more visible) ====
    float skinPals[4][3]= {{0.72f,0.58f,0.44f},{0.65f,0.52f,0.40f},{0.80f,0.65f,0.52f},{0.60f,0.48f,0.36f}};
    // Back rows
    for(int row=0; row<4; row++)
    {
        float rowY=-52-4-row*7.0f;
        int count=12-row*2;
        float startX=-140+row*8;
        float endX=140-row*8;
        for(int i=0; i<count; i++)
        {
            float cx=startX+i*(endX-startX)/(count-1);
            if(cx>-48&&cx<38)continue; // gap for coffin+general
            int sp=ri(0,3);
            int pose=ri(0,2);
            float sc2=0.92f+row*0.04f;
            drawMourner(cx,rowY,sc2,skinPals[sp][0],skinPals[sp][1],skinPals[sp][2],pose);
        }
    }
    // Front row (larger, very visible)
    float frontX[]= {-155,-138,-122,-106,-90,-74,35,52,68,84,100,116,132,150};
    int frontPoses[]= {1,0,2,1,0,2,1,0,2,1,0,2,1,0};
    for(int i=0; i<14; i++)
    {
        int sp=ri(0,3);
        drawMourner(frontX[i],-52,1.05f,skinPals[sp][0],skinPals[sp][1],skinPals[sp][2],frontPoses[i]);
    }
    // Kneeling figures very close to coffin
    drawMourner(-40,-52,1.1f,skinPals[0][0],skinPals[0][1],skinPals[0][2],1);
    drawMourner(-28,-52,1.1f,skinPals[1][0],skinPals[1][1],skinPals[1][2],0);
    drawMourner(17,-52,1.1f,skinPals[2][0],skinPals[2][1],skinPals[2][2],1);
    drawMourner(30,-52,1.1f,skinPals[3][0],skinPals[3][1],skinPals[3][2],0);

    // ==== GENERAL stands at RIGHT side of coffin ====
    // He is on the ground, no podium
    drawGeneral(-30,-65,1.2f,s3_armRaise);



    // ==== "DO OR DIE!" BANNER ====
   if(g_scene == 3)
    {

        // Solid alpha to prevent transparency-related flickering
        float ba = 1.0f;
        // Reduced dimensions
        float bWidth = 80.0f;  // Reduced from 140
        float bHeight = 12.0f; // Reduced from 17
        float bLeft = -bWidth / 2.0f;
        float bBottom = 5.0f;

        // --- Poles adjusted to new banner width ---
        glColor4f(0.12f, 0.10f, 0.08f, 1.0f);
        drawRect(-40.0f + 3.0f, -42.0f + 3.0f, 3.0f, 60.0f, 1); // Back-Left Pole
        drawRect(40.0f + 3.0f - 2.2f, -42.0f + 3.0f, 3.0f, 60.0f, 1); // Right Pole

        // Pole finials
        glColor4f(0.75f, 0.62f, 0.12f, 1.0f);
        drawCircle(-36.0f, 22.0f, 1.8f, 30, 1);
        drawCircle(42.0f, 22.0f, 1.8f, 30, 1);



       // --- Banner Cloth (Reduced Dark Red) ---
        glBegin(GL_QUAD_STRIP);
        int bs = 40;
        for(int i = 0; i <= bs; i++)
        {
            float bt = (float)i / bs;
            // Wave reduced for a more "fixed" look
            float bwave = sinf(bt * PI * 1.5f + s3_wave) * 0.8f * bt;
            float shade = ba * (0.85f + 0.15f * (1 - bt));
            glColor4f(0.50f * shade, 0.02f * shade, 0.02f * shade, ba * 0.95f);
            glVertex2f(bLeft + bt * bWidth, bBottom + bwave);
            glVertex2f(bLeft + bt * bWidth, bBottom + bHeight + bwave);
        }
        glEnd();
        // Simple Gold borders
        glColor4f(0.75f, 0.62f, 0.12f, ba * 0.9f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP); // Top
        for(int i = 0; i <= bs; i++) {
            float bt = (float)i / bs;
            glVertex2f(bLeft + bt * bWidth, bBottom + bHeight + sinf(bt * PI * 1.5f + s3_wave) * 0.8f * bt);
        }
        glEnd();
        glBegin(GL_LINE_STRIP); // Bottom
        for(int i = 0; i <= bs; i++) {
            float bt = (float)i / bs;
            glVertex2f(bLeft + bt * bWidth, bBottom + sinf(bt * PI * 1.5f + s3_wave) * 0.8f * bt);
        }
        glEnd();
        glLineWidth(1.0f);

        // --- Fixed Text (No Flicker, Centered) ---
        // Steady color (no s3_bannerFlash multiplier)
        glColor4f(1.0f, 0.95f, 0.80f, ba);
        // Centered text: width 60, scale 0.32 approximately fills the middle
        drawText(bLeft + 5, bBottom + 3, "DO  OR  DIE", 0.72f);


    }

    // Dark vignette + mood tint
    drawVignette(0.62f);
    glColor4f(0.30f,0.02f,0.02f,0.04f);
    drawRect(-160,-80,320,180,1);
}

// ================================================================
// SCENE 4 - DEDICATED MIDDLE-EASTERN CITY BUILDINGS
// These are SEPARATE from scene 1's B[] array
// ================================================================


// Draw one Middle-Eastern building (intact or progressively damaged)
static void drawS4Building(S4CityBuilding &bd)
{
    float bx = bd.x, by = bd.y, bw = bd.w, bh = bd.h;
    float br = bd.cr, bg2 = bd.cg, bb = bd.cb;
    float fp = bd.firePhase + g_time * 3.0f;

    // Damage factor: 0=pristine, 1=fully destroyed
    float dmg = (float)bd.hitCount / (float)S4_MAX_HITS;
    if(dmg > 1.0f) dmg = 1.0f;

    // Darken color with damage
    float darkFactor = 1.0f - dmg * 0.55f;
    float dr = br * darkFactor;
    float dg = bg2 * darkFactor;
    float db = bb * darkFactor;

    if(bd.destroyed)
    {
        // Fully destroyed: draw rubble heap
        glColor3f(0.24f, 0.18f, 0.12f);
        glBegin(GL_POLYGON);
        glVertex2f(bx-2, by);
        glVertex2f(bx+bw*0.1f, by+7);
        glVertex2f(bx+bw*0.22f, by+3);
        glVertex2f(bx+bw*0.38f, by+9);
        glVertex2f(bx+bw*0.55f, by+4);
        glVertex2f(bx+bw*0.7f, by+8);
        glVertex2f(bx+bw*0.85f, by+3);
        glVertex2f(bx+bw+2, by);
        glEnd();
        // Rubble debris blocks
        glColor3f(0.20f,0.16f,0.11f);
        for(int d=0; d<5; d++)
        {
            float rx=bx+bw*0.12f*d+rf(-2,2);
            float ry=by+rf(0,5);
            drawRect(rx,ry,rf(2,5),rf(1,4),1);
        }
        // Persistent fire on ruins
        for(int fl=0; fl<3; fl++)
        {
            float fx2=bx+bw*(0.2f+fl*0.28f);
            float fh2=(9+4*fl)*(0.6f+0.4f*sinf(fp+fl));
            float fw2=4+2*fl;
            glColor4f(0.9f,0.3f,0.0f,0.88f);
            glBegin(GL_TRIANGLES);
            glVertex2f(fx2-fw2,by+6);
            glVertex2f(fx2+fw2,by+6);
            glVertex2f(fx2+sinf(fp*1.3f+fl)*2,by+6+fh2);
            glEnd();
            glColor4f(1.0f,0.68f,0.08f,0.72f);
            glBegin(GL_TRIANGLES);
            glVertex2f(fx2-fw2*0.6f,by+6);
            glVertex2f(fx2+fw2*0.6f,by+6);
            glVertex2f(fx2+sinf(fp*2.1f+fl)*1.5f,by+6+fh2*0.76f);
            glEnd();
        }
        return;
    }

    // ---- Shadow ----
    glColor4f(0,0,0,0.20f);
    glBegin(GL_QUADS);
    glVertex2f(bx+2,by-1);
    glVertex2f(bx+bw+2,by-1);
    glVertex2f(bx+bw,by);
    glVertex2f(bx,by);
    glEnd();

    // ---- Main front face ----
    glBegin(GL_QUADS);
    glColor3f(dr*0.55f,dg*0.55f,db*0.55f);
    glVertex2f(bx,by);
    glVertex2f(bx+bw,by);
    glColor3f(dr,dg,db);
    glVertex2f(bx+bw,by+bh);
    glVertex2f(bx,by+bh);
    glEnd();

    // ---- Side 3D face ----
    glBegin(GL_QUADS);
    glColor3f(dr*0.38f,dg*0.38f,db*0.38f);
    glVertex2f(bx+bw,by);
    glVertex2f(bx+bw+3,by+3);
    glColor3f(dr*0.60f,dg*0.60f,db*0.60f);
    glVertex2f(bx+bw+3,by+bh+3);
    glVertex2f(bx+bw,by+bh);
    glEnd();

    // ---- Roof ----
    glBegin(GL_QUADS);
    glColor3f(dr*1.05f,dg*1.05f,db*1.05f);
    glVertex2f(bx,by+bh);
    glVertex2f(bx+bw,by+bh);
    glVertex2f(bx+bw+3,by+bh+3);
    glVertex2f(bx+3,by+bh+3);
    glEnd();

    // ---- Architectural features ----
    if(bd.type == 0)
    {
        // Flat roof battlements
        glColor3f(dr*1.05f,dg*1.05f,db*1.05f);
        drawRect(bx,by+bh,bw,2.5f,1);
        for(float mx2=bx; mx2<bx+bw; mx2+=4.5f) drawRect(mx2,by+bh+2.5f,2.8f,4,1);
        // Arched windows
        glColor3f(0.09f,0.07f,0.05f);
        for(int wi=0; wi<(int)(bw/8); wi++)
        {
            float wx2=bx+2+wi*8,wy2=by+bh*0.35f;
            drawRect(wx2,wy2,4.5f,8,1);
            glColor4f(dr*0.28f,dg*0.28f,db*0.28f,1);
            drawCircle(wx2+2.2f,wy2+8,2.3f,10,1);
            glColor3f(0.09f,0.07f,0.05f);
        }
    }
    else if(bd.type == 1)
    {
        // Dome
        drawRect(bx,by+bh,bw,2,1);
        float dc2=bx+bw/2,drad=bw*0.48f;
        glColor3f(dr*0.90f,dg*0.88f,db*0.75f);
        glBegin(GL_POLYGON);
        for(int k=0; k<=18; k++)
        {
            float a=PI*k/18;
            glVertex2f(dc2+cosf(a)*drad,by+bh+2+sinf(a)*drad*0.55f);
        }
        glEnd();
        glColor3f(0.80f,0.70f,0.20f);
        drawCircle(dc2,by+bh+2+drad*0.55f+2.5f,1.4f,10,1);
        glColor3f(0.09f,0.07f,0.05f);
        for(int wi=0; wi<(int)(bw/9); wi++)
        {
            float wx2=bx+3+wi*9,wy2=by+bh*0.25f;
            drawRect(wx2,wy2,4.5f,9,1);
            glColor4f(dr*0.32f,dg*0.32f,db*0.32f,1);
            drawCircle(wx2+2.2f,wy2+9,2.3f,10,1);
            glColor3f(0.09f,0.07f,0.05f);
        }
    }
    else if(bd.type == 2)
    {
        // Minaret
        float mw=bw*0.30f,mx3=bx+bw/2-mw/2;
        glColor3f(dr*0.84f,dg*0.80f,db*0.68f);
        drawRect(mx3,by,mw,bh+14,1);
        glColor3f(dr*1.06f,dg*1.00f,db*0.80f);
        drawRect(mx3-2.5f,by+bh,mw+5,3,1);
        glColor3f(dr*0.78f,dg*0.74f,db*0.60f);
        glBegin(GL_TRIANGLES);
        glVertex2f(mx3-1.5f,by+bh+14);
        glVertex2f(mx3+mw+1.5f,by+bh+14);
        glVertex2f(mx3+mw/2,by+bh+24);
        glEnd();
        glColor3f(0.80f,0.70f,0.20f);
        drawCircle(mx3+mw/2,by+bh+24.5f,1.2f,8,1);
        glColor3f(0.09f,0.07f,0.05f);
        for(int lw2=0; lw2<4; lw2++)drawRect(mx3+mw*0.28f,by+7+lw2*16,mw*0.44f,6,1);
        glColor3f(dr,dg,db);
        drawRect(bx,by,bw,bh*0.28f,1);
    }
    else
    {
        // Arch facade
        drawRect(bx,by+bh,bw,2.5f,1);
        float aw=bw*0.5f,ax3=bx+bw*0.25f;
        glColor3f(0.09f,0.07f,0.05f);
        drawRect(ax3,by,aw,bh*0.72f,1);
        glColor4f(dr*0.40f,dg*0.40f,db*0.40f,1);
        drawCircle(ax3+aw/2,by+bh*0.72f,aw/2,18,1);
        glColor3f(0.09f,0.07f,0.05f);
        drawCircle(ax3+aw/2,by+bh*0.72f,aw/2,18,1);
        glColor3f(0.09f,0.07f,0.05f);
        drawRect(bx+2,by+bh*0.5f,aw*0.35f,bh*0.25f,1);
        drawRect(bx+bw-2-aw*0.35f,by+bh*0.5f,aw*0.35f,bh*0.25f,1);
    }

    // ---- Hit fire spots ----
    for(int h=0; h<bd.hitCount; h++)
    {
        float fx3=bd.hitX[h], fy3=bd.hitY[h];
        float fph=bd.firePhase+h*1.8f+g_time*3.2f;
        float fsz=2.5f+1.5f*(float)h/S4_MAX_HITS;
        float fh3=(fsz*3.0f)*(0.55f+0.45f*sinf(fph));
        glColor4f(0.88f,0.26f,0.02f,0.90f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx3-fsz,fy3);
        glVertex2f(fx3+fsz,fy3);
        glVertex2f(fx3+sinf(fph*1.4f)*fsz*0.5f,fy3+fh3);
        glEnd();
        glColor4f(1.0f,0.60f,0.05f,0.72f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx3-fsz*0.6f,fy3);
        glVertex2f(fx3+fsz*0.6f,fy3);
        glVertex2f(fx3+sinf(fph*2.2f)*fsz*0.3f,fy3+fh3*0.72f);
        glEnd();
        glColor4f(1.0f,0.32f,0.0f,0.06f);
        drawCircle(fx3,fy3+fh3*0.6f,fh3*0.8f,14,1);
        // Smoke from fire
        if(rf(0,1)<0.08f)
            pSpawn(fx3+rf(-2,2),fy3+fh3,rf(-0.15f,0.15f),rf(0.3f,1.0f),0,0,
                   rf(0.14f,0.24f),rf(0.12f,0.20f),rf(0.10f,0.16f),0.65f,rf(2,6),rf(6,13),1);
    }

    // Charring/scorch marks (dark overlay increasing with damage)
    if(dmg > 0.1f)
    {
        glColor4f(0.04f,0.03f,0.02f,dmg*0.55f);
        drawRect(bx,by,bw,bh,1);
    }

    // Crumbling top edge when heavily damaged
    if(dmg > 0.5f)
    {
        glColor3f(0.20f,0.15f,0.10f);
        glBegin(GL_POLYGON);
        float chunkH = bh * (dmg - 0.3f) * 0.6f;
        glVertex2f(bx, by+bh);
        glVertex2f(bx+bw*0.1f, by+bh+sinf(bd.seed)*3);
        glVertex2f(bx+bw*0.2f, by+bh-2+sinf(bd.seed+1)*2);
        glVertex2f(bx+bw*0.4f, by+bh+sinf(bd.seed+2)*3);
        glVertex2f(bx+bw*0.6f, by+bh-1+sinf(bd.seed+3)*2);
        glVertex2f(bx+bw*0.8f, by+bh+sinf(bd.seed+4)*3);
        glVertex2f(bx+bw, by+bh);
        glVertex2f(bx+bw, by+bh - chunkH*0.4f);
        glVertex2f(bx, by+bh - chunkH*0.4f);
        glEnd();
    }
}

// Register a missile hit on a scene 4 building
static void hitS4Building(float mx, float my)
{
    for(int i = 0; i < S4_CITY_BLDS; i++)
    {
        S4CityBuilding &bd = s4_city[i];
        if(bd.destroyed) continue;
        if(mx >= bd.x-2 && mx <= bd.x+bd.w+2 && my >= bd.y && my <= bd.y+bd.h+25)
        {
            if(bd.hitCount < S4_MAX_HITS)
            {
                bd.hitX[bd.hitCount] = mx;
                bd.hitY[bd.hitCount] = fmaxf(bd.y + 3, fminf(my, bd.y+bd.h-3));
                bd.hitCount++;
            }
            if(bd.hitCount >= S4_MAX_HITS)
            {
                bd.destroyed = 1;
            }
            return;
        }
    }
}

// ================================================================
// SCENE 4 STATE
// ================================================================
#define S4_SKY_BOT   55.0f
#define S4_CITY_BOT  -35.0f
#define S4_SEA_TOP   -35.0f
#define S4_SEA_BOT   -80.0f
#define S4_SEA_MID   -57.5f

static float s4_seaOffset=0;
static float s4_flashX[5],s4_flashY[5],s4_flashA[5];

struct S4Jet
{
    float x,y,speed;
};
static S4Jet s4_jets[4];
static void initS4Jets()
{
    float yLevels[]= {92,84,77,88};
    float speeds[]= {28,22,32,25};
    for(int i=0; i<4; i++)
    {
        s4_jets[i].x=-160.0f-i*55.0f;
        s4_jets[i].y=yLevels[i];
        s4_jets[i].speed=speeds[i];
    }
}

struct S4Ship
{
    float x,y,phase,gunSmoke;
    int type;
};
static S4Ship s4_ships[3];
static float s4_shipFireTimer[3];
static void initS4Ships()
{
    float sx[]= {-80,0,80};
    float sy[]= {S4_SEA_MID+4,S4_SEA_MID+2,S4_SEA_MID+6};
    int   types[]= {0,2,1};
    for(int i=0; i<3; i++)
    {
        s4_ships[i].x=sx[i];
        s4_ships[i].y=sy[i];
        s4_ships[i].phase=rf(0,TWO_PI);
        s4_ships[i].gunSmoke=0;
        s4_ships[i].type=types[i];
        s4_shipFireTimer[i]=rf(0.5f,3.0f);
    }
}

struct ShipMissile
{
    float x,y,tx,ty,spd;
    int active;
};
#define MAX_SM 12
static ShipMissile SM[MAX_SM];
static void initShipMissiles()
{
    for(int i=0; i<MAX_SM; i++)SM[i].active=0;
}
static float s4_jetMissileTimer=0;

// ================================================================
// SCENE 4 - NAVAL ASSAULT with own Middle-Eastern city
// ================================================================
static void drawOcean(float waveOff)
{
    float seaTop=S4_SEA_TOP,seaBot=S4_SEA_BOT,seaH=seaTop-seaBot;
    glBegin(GL_QUADS);
    glColor3f(0.04f,0.10f,0.20f);
    glVertex2f(-160,seaBot);
    glVertex2f(160,seaBot);
    glColor3f(0.06f,0.16f,0.30f);
    glVertex2f(160,seaTop);
    glVertex2f(-160,seaTop);
    glEnd();
    for(int layer=0; layer<4; layer++)
    {
        float ly=seaTop-layer*1.8f;
        float speed=1.0f+layer*0.35f;
        float amp=2.0f-layer*0.35f;
        float alpha=0.4f-layer*0.07f;
        glBegin(GL_QUAD_STRIP);
        for(int x=-160; x<=160; x+=5)
        {
            float wx=(float)x;
            float wy=ly+sinf(wx*0.07f+waveOff*speed)*amp+cosf(wx*0.11f+waveOff*speed*0.65f)*amp*0.4f;
            float col=0.10f+0.06f*(float)layer/4;
            glColor4f(col,col*2.0f,col*3.2f,alpha);
            glVertex2f(wx,wy);
            glVertex2f(wx,wy-2.0f);
        }
        glEnd();
        if(layer<2)
        {
            glColor4f(0.70f,0.82f,0.90f,0.50f-layer*0.12f);
            glLineWidth(1.0f);
            glBegin(GL_LINE_STRIP);
            for(int x=-160; x<=160; x+=2)
            {
                float wx=(float)x;
                float wy=ly+sinf(wx*0.07f+waveOff*speed)*amp+cosf(wx*0.11f+waveOff*speed*0.65f)*amp*0.4f;
                glVertex2f(wx,wy);
            }
            glEnd();
        }
    }
    srand(77);
    for(int g=0; g<30; g++)
    {
        float gx=rf(-150,150);
        float gy2=seaTop-2+sinf(gx*0.07f+waveOff)*2;
        float alpha=0.25f+0.25f*sinf(g_time*2.0f+g*0.9f);
        glColor4f(0.8f,0.88f,0.95f,alpha*0.30f);
        glBegin(GL_LINES);
        glVertex2f(gx-2,gy2);
        glVertex2f(gx+2,gy2);
        glEnd();
    }
    srand((unsigned)time(NULL));
}

static void drawBattleshipS4(S4Ship &sh)
{
    float bx=sh.x,by=sh.y,bobY=sinf(g_time*0.75f+sh.phase)*1.8f;
    glPushMatrix();
    glTranslatef(bx,by+bobY,0);
    int type=sh.type;
    if(type==0)
    {
        glColor3f(0.18f,0.20f,0.24f);
        glBegin(GL_POLYGON);
        glVertex2f(-24,-4);
        glVertex2f(-20,-8);
        glVertex2f(20,-8);
        glVertex2f(26,0);
        glVertex2f(20,3);
        glVertex2f(-20,3);
        glEnd();
        glColor3f(0.42f,0.07f,0.07f);
        drawRect(-20,-8,40,1.5f,1);
        glColor3f(0.25f,0.27f,0.32f);
        drawRect(-16,-8,26,11,1);
        glColor3f(0.30f,0.32f,0.38f);
        drawRect(-8,3,13,7,1);
        glColor4f(0.55f,0.70f,0.80f,0.85f);
        for(int w2=0; w2<3; w2++)drawRect(-6+w2*4,4.5f,2.5f,3,1);
        glColor3f(0.35f,0.35f,0.40f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glVertex2f(-2,10);
        glVertex2f(-2,18);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-2,16);
        glVertex2f(4,16);
        glEnd();
        glLineWidth(1);
        float gAng=-0.3f+sinf(g_time*0.4f)*0.07f;
        glColor3f(0.20f,0.20f,0.24f);
        drawCircle(-14,-2,3.0f,12,1);
        glColor3f(0.22f,0.22f,0.26f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(-14,-2);
        glVertex2f(-14+cosf(gAng)*13,-2+sinf(gAng)*13);
        glEnd();
        glLineWidth(1);
        glColor3f(0.18f,0.18f,0.22f);
        for(int m=0; m<3; m++)drawRect(-10+m*3,-1,2.2f,3.5f,1);
        glColor3f(0.22f,0.22f,0.26f);
        drawRect(-3,3,4,6,1);
        if(sh.gunSmoke>0.01f)
        {
            glColor4f(1,0.82f,0.2f,sh.gunSmoke);
            drawCircle(-14,-2,4.5f,14,1);
        }
    }
    else if(type==1)
    {
        glColor3f(0.16f,0.18f,0.22f);
        glBegin(GL_POLYGON);
        glVertex2f(-34,-5);
        glVertex2f(-28,-10);
        glVertex2f(28,-10);
        glVertex2f(36,0);
        glVertex2f(28,4);
        glVertex2f(-28,4);
        glEnd();
        glColor3f(0.42f,0.07f,0.07f);
        drawRect(-28,-10,56,1.5f,1);
        glColor3f(0.24f,0.26f,0.30f);
        drawRect(-26,-10,44,14,1);
        glColor3f(0.28f,0.30f,0.36f);
        drawRect(-20,4,28,8,1);
        glColor3f(0.32f,0.34f,0.40f);
        drawRect(-11,12,16,8,1);
        glColor4f(0.52f,0.68f,0.78f,0.88f);
        for(int w2=0; w2<4; w2++)drawRect(-9+w2*3.5f,14,2.5f,3.5f,1);
        glColor3f(0.35f,0.35f,0.40f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-3,20);
        glVertex2f(-3,30);
        glEnd();
        glLineWidth(1);
        float gAng2=-0.22f+sinf(g_time*0.35f)*0.06f;
        glColor3f(0.18f,0.18f,0.22f);
        drawCircle(-22,-3,4.5f,14,1);
        glColor3f(0.20f,0.20f,0.24f);
        glLineWidth(3.5f);
        glBegin(GL_LINES);
        glVertex2f(-22,-3);
        glVertex2f(-22+cosf(gAng2-0.04f)*18,-3+sinf(gAng2-0.04f)*18);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-22,-3);
        glVertex2f(-22+cosf(gAng2+0.04f)*18,-3+sinf(gAng2+0.04f)*18);
        glEnd();
        glLineWidth(1);
        glColor3f(0.16f,0.16f,0.20f);
        for(int m=0; m<5; m++)drawRect(-18+m*3,1,2.5f,4.5f,1);
        glColor3f(0.20f,0.20f,0.24f);
        drawRect(-5,4,5,8,1);
        drawRect(3,4,5,7,1);
        glColor3f(0.22f,0.26f,0.24f);
        drawRect(14,-10,16,5,1);
        if(sh.gunSmoke>0.01f)
        {
            glColor4f(1,0.82f,0.2f,sh.gunSmoke);
            drawCircle(-22,-3,5.5f,14,1);
        }
    }
    else
    {
        float cl=45.0f;
        glColor3f(0.15f,0.17f,0.21f);
        glBegin(GL_POLYGON);
        glVertex2f(-cl,-4);
        glVertex2f(-cl+5,-10);
        glVertex2f(cl-5,-10);
        glVertex2f(cl,0);
        glVertex2f(cl-5,5);
        glVertex2f(-cl+5,5);
        glEnd();
        glColor3f(0.40f,0.06f,0.06f);
        drawRect(-cl+5,-10,(cl-5)*2,1.5f,1);
        glColor3f(0.22f,0.22f,0.26f);
        drawRect(-cl+3,5,(cl-3)*2,7,1);
        glColor4f(0.50f,0.50f,0.42f,0.5f);
        for(float rx=-cl+6; rx<cl-6; rx+=10)drawRect(rx,6.5f,6,1.2f,1);
        glColor3f(0.26f,0.28f,0.34f);
        drawRect(20,5,18,18,1);
        glColor4f(0.48f,0.62f,0.72f,0.88f);
        for(int w2=0; w2<4; w2++)drawRect(21+w2*3.5f,15,2.5f,3,1);
        glColor3f(0.35f,0.35f,0.40f);
        glLineWidth(1.8f);
        glBegin(GL_LINES);
        glVertex2f(28,23);
        glVertex2f(28,32);
        glEnd();
        glLineWidth(1);
        glColor3f(0.19f,0.19f,0.23f);
        for(int ac=0; ac<3; ac++)
        {
            float apx=-cl+8+ac*28,apy=7;
            glBegin(GL_POLYGON);
            glVertex2f(apx-5,apy);
            glVertex2f(apx+5,apy);
            glVertex2f(apx+4,apy+2);
            glVertex2f(apx-4,apy+2);
            glEnd();
        }
        glColor4f(0.52f,0.52f,0.58f,0.50f);
        drawText(-8,-7,"CVN-78",0.11f);
        if(sh.gunSmoke>0.01f)
        {
            glColor4f(1,0.82f,0.2f,sh.gunSmoke);
            drawCircle(-cl+8,7,5.0f,14,1);
        }
    }
    glPopMatrix();
}

static void drawShipMissileS4(ShipMissile&sm)
{
    float dx=sm.tx-sm.x,dy=sm.ty-sm.y,len=sqrtf(dx*dx+dy*dy);
    if(len<0.01f)return;
    float ang=atan2f(dy,dx)*57.3f;
    glPushMatrix();
    glTranslatef(sm.x,sm.y,0);
    glRotatef(ang,0,0,1);
    glBegin(GL_QUAD_STRIP);
    for(int tt=0; tt<12; tt++)
    {
        float t=(float)tt/11,w=2.0f*(1.0f-t);
        glColor4f(1.0f,0.55f+0.3f*t,0.05f,0.75f*(1.0f-t));
        glVertex2f(-tt*3.0f,w);
        glVertex2f(-tt*3.0f,-w);
    }
    glEnd();
    glColor3f(0.55f,0.55f,0.60f);
    glBegin(GL_POLYGON);
    glVertex2f(-5,-1.2f);
    glVertex2f(-5,1.2f);
    glVertex2f(3,1.0f);
    glVertex2f(6,0);
    glVertex2f(3,-1.0f);
    glEnd();
    glColor3f(1.0f,0.82f,0.2f);
    drawCircle(6,0,1.4f,8,1);
    glPopMatrix();
}

static void updateScene4(float dt)
{
    s4_seaOffset+=dt*0.5f;

    for(int i=0; i<4; i++)
    {
        s4_jets[i].x+=s4_jets[i].speed*dt;
        if(s4_jets[i].x>175)s4_jets[i].x=-165.0f;
    }

    for(int i=0; i<3; i++)
    {
        s4_shipFireTimer[i]-=dt;
        s4_ships[i].gunSmoke=fmaxf(0,s4_ships[i].gunSmoke-dt*2.5f);
        if(s4_shipFireTimer[i]<=0)
        {
            s4_shipFireTimer[i]=rf(1.2f,3.5f);
            s4_ships[i].gunSmoke=1.0f;
            for(int m=0; m<MAX_SM; m++)
            {
                if(!SM[m].active)
                {
                    SM[m].x=s4_ships[i].x;
                    SM[m].y=s4_ships[i].y-8;
                    SM[m].tx=rf(-140,140);
                    SM[m].ty=rf(S4_CITY_BOT+10,S4_CITY_BOT+60);
                    SM[m].spd=rf(50,80);
                    SM[m].active=1;
                    break;
                }
            }
        }
    }

    // Update ship missiles - hit scene 4 buildings
    for(int m=0; m<MAX_SM; m++)
    {
        if(!SM[m].active)continue;
        float dx=SM[m].tx-SM[m].x,dy=SM[m].ty-SM[m].y,dist=sqrtf(dx*dx+dy*dy);
        if(dist<6.0f)
        {
            explode(SM[m].tx,SM[m].ty,3.0f,1);
            hitS4Building(SM[m].tx,SM[m].ty);  // damage scene 4 building
            SM[m].active=0;
            for(int f=0; f<5; f++)if(s4_flashA[f]<0.1f)
                {
                    s4_flashX[f]=SM[m].tx;
                    s4_flashY[f]=SM[m].ty;
                    s4_flashA[f]=1.0f;
                    break;
                }
        }
        else
        {
            float sp=SM[m].spd*dt;
            SM[m].x+=dx/dist*sp;
            SM[m].y+=dy/dist*sp;
        }
    }

    s4_jetMissileTimer+=dt;
    if(s4_jetMissileTimer>0.8f)
    {
        s4_jetMissileTimer=0;
        for(int m=0; m<MAX_M; m++)
        {
            if(!M[m].active)
            {
                int ji=ri(0,3);
                M[m].x=s4_jets[ji].x;
                M[m].y=s4_jets[ji].y;
                M[m].tx=rf(-130,130);
                M[m].ty=rf(S4_CITY_BOT+5,S4_CITY_BOT+60);
                M[m].spd=rf(55,90);
                M[m].active=1;
                break;
            }
        }
    }
    for(int m=0; m<MAX_M; m++)
    {
        if(!M[m].active)continue;
        float dx=M[m].tx-M[m].x,dy=M[m].ty-M[m].y,dist=sqrtf(dx*dx+dy*dy);
        if(dist<5.0f)
        {
            explode(M[m].tx,M[m].ty,3.0f,1);
            hitS4Building(M[m].tx,M[m].ty);  // damage scene 4 building
            M[m].active=0;
            for(int f=0; f<5; f++)if(s4_flashA[f]<0.1f)
                {
                    s4_flashX[f]=M[m].tx;
                    s4_flashY[f]=M[m].ty;
                    s4_flashA[f]=1.0f;
                    break;
                }
        }
        else
        {
            float sp=M[m].spd*dt;
            M[m].x+=dx/dist*sp;
            M[m].y+=dy/dist*sp;
        }
    }

    for(int f=0; f<5; f++)if(s4_flashA[f]>0)s4_flashA[f]-=dt*2.8f;

    // Continuous smoke from hit buildings
    for(int i=0; i<S4_CITY_BLDS; i++)
    {
        if(s4_city[i].hitCount>0 && rf(0,1)<0.04f*(float)s4_city[i].hitCount)
        {
            int hi=ri(0,s4_city[i].hitCount-1);
            pSpawn(s4_city[i].hitX[hi]+rf(-3,3),s4_city[i].hitY[hi]+8,
                   rf(-0.1f,0.1f),rf(0.25f,1.0f),0,0,
                   rf(0.14f,0.24f),rf(0.12f,0.20f),rf(0.08f,0.16f),0.68f,rf(3,8),rf(7,15),1);
        }
    }
    if(rf(0,1)<0.05f)pSpawn(rf(-140,140),rf(S4_CITY_BOT+5,S4_CITY_BOT+50),rf(-0.1f,0.1f),rf(0.25f,1.0f),0,0,rf(0.14f,0.24f),rf(0.11f,0.20f),rf(0.08f,0.16f),0.70f,rf(4,10),rf(8,18),1);
    if(rf(0,1)<0.04f)pSpawn(rf(-120,120),rf(S4_CITY_BOT+5,S4_CITY_BOT+50),rf(-0.4f,0.4f),rf(0.3f,1.4f),0,-0.2f,rf(0.9f,1.0f),rf(0.5f,0.7f),0.05f,0.85f,rf(0.4f,1.0f),rf(1,2),2);
    pUpdate(dt);
}

static void drawScene4()
{
    // SKY ZONE (y = 55 to 100)
    glBegin(GL_QUADS);
    glColor3f(0.06f,0.03f,0.02f);
    glVertex2f(-160,100);
    glVertex2f(160,100);
    glColor3f(0.14f,0.06f,0.02f);
    glVertex2f(160,S4_SKY_BOT);
    glVertex2f(-160,S4_SKY_BOT);
    glEnd();
    drawStars(0.18f);
    glColor4f(0.68f,0.62f,0.48f,0.20f);
    drawCircle(115,88,8,20,1);
    glColor4f(0.14f,0.10f,0.07f,0.40f);
    drawCircle(-100,82,15,18,1);
    drawCircle(-45,78,12,16,1);
    drawCircle(20,85,18,20,1);
    drawCircle(75,80,14,16,1);
    drawCircle(130,84,16,18,1);

    // 4 JETS in sky
    for(int i=0; i<4; i++)
    {
        drawJet(s4_jets[i].x,s4_jets[i].y,2.2f,1);
        if(rf(0,1)<0.03f)pSpawn(s4_jets[i].x-12,s4_jets[i].y,rf(-0.3f,-0.1f),rf(-0.08f,0.08f),0,0,0.18f,0.15f,0.12f,0.38f,rf(0.8f,2.0f),rf(3,7),1);
    }
    for(int m=0; m<MAX_M; m++)if(M[m].active)drawMissileObj(M[m].x,M[m].y,M[m].tx,M[m].ty);

    // CITY ZONE (y = -35 to 55)
    // Background glow
    glColor4f(0.45f,0.12f,0.03f,0.18f);
    glBegin(GL_QUADS);
    glVertex2f(-160,S4_CITY_BOT);
    glVertex2f(160,S4_CITY_BOT);
    glVertex2f(160,S4_SKY_BOT);
    glVertex2f(-160,S4_SKY_BOT);
    glEnd();

    // City ground
    glColor3f(0.32f,0.26f,0.17f);
    drawRect(-160,S4_CITY_BOT,320,5,1);
    glColor3f(0.25f,0.20f,0.13f);
    drawRect(-160,S4_CITY_BOT+5,320,2,1);
    // Road
    glColor3f(0.18f,0.14f,0.09f);
    drawRect(-160,S4_CITY_BOT,320,3,1);
    glColor4f(0.42f,0.38f,0.28f,0.35f);
    for(int rx=-155; rx<155; rx+=20)drawRect((float)rx,S4_CITY_BOT+0.5f,11,1.2f,1);

    // Draw scene 4's own Middle-Eastern buildings (with progressive destruction)
    for(int i=0; i<S4_CITY_BLDS; i++)
    {
        drawS4Building(s4_city[i]);
    }

    // Particles
    pDraw();

    // Explosion flashes
    for(int f=0; f<5; f++)if(s4_flashA[f]>0.01f)drawExplosionFlash(s4_flashX[f],s4_flashY[f],14+f*2,s4_flashA[f]);

    // AA tracers
    srand(42);
    for(int i=0; i<3; i++)
    {
        float tx=rf(-110,110);
        float ty=rf(S4_CITY_BOT+20,S4_CITY_BOT+50);
        float alpha=0.5f+0.3f*sinf(g_time*3.0f+i*1.5f);
        if(alpha>0.15f)
        {
            glColor4f(0.88f,0.84f,0.20f,alpha*0.55f);
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            glVertex2f(tx,ty);
            glVertex2f(tx+rf(-30,30),S4_SKY_BOT+rf(5,20));
            glEnd();
        }
    }
    srand((unsigned)time(NULL));
    for(int m=0; m<MAX_SM; m++)if(SM[m].active)drawShipMissileS4(SM[m]);

    // SEA ZONE (y = -80 to -35)
    drawOcean(s4_seaOffset);
    for(int i=0; i<3; i++)
    {
        drawBattleshipS4(s4_ships[i]);
        // Wake
        glColor4f(0.65f,0.78f,0.88f,0.18f);
        for(int w=0; w<8; w++)
        {
            float wd=(float)w*4.0f;
            float wa=0.18f*(1-wd/32.0f);
            glColor4f(0.65f,0.78f,0.88f,wa);
            glBegin(GL_LINES);
            glVertex2f(s4_ships[i].x,s4_ships[i].y-1);
            glVertex2f(s4_ships[i].x-wd,s4_ships[i].y-1-wd*0.3f);
            glVertex2f(s4_ships[i].x,s4_ships[i].y-1);
            glVertex2f(s4_ships[i].x+wd,s4_ships[i].y-1-wd*0.3f);
            glEnd();
        }
        // Funnel smoke
        if(rf(0,1)<0.06f)
        {
            float fx=s4_ships[i].x+(s4_ships[i].type==2?22:-1);
            float fy=s4_ships[i].y+(s4_ships[i].type==0?10:(s4_ships[i].type==1?13:20));
            pSpawn(fx,fy,rf(-0.08f,0.08f),rf(0.2f,0.7f),0,0,0.20f,0.18f,0.15f,0.50f,rf(1.5f,4),rf(5,10),1);
        }
    }
    // Sea top overlay
    float sw=s4_seaOffset;
    for(int layer=0; layer<2; layer++)
    {
        float ly=S4_SEA_TOP-layer*1.2f;
        float speed=1.2f+layer*0.4f;
        float amp=1.8f-layer*0.4f;
        glColor4f(0.06f,0.16f,0.28f,0.14f+layer*0.04f);
        glBegin(GL_QUAD_STRIP);
        for(int x=-160; x<=160; x+=5)
        {
            float wx=(float)x;
            float wy=ly+sinf(wx*0.07f+sw*speed)*amp;
            glVertex2f(wx,wy);
            glVertex2f(wx,wy-0.8f);
        }
        glEnd();
        glColor4f(0.68f,0.78f,0.86f,0.25f-layer*0.06f);
        glLineWidth(0.7f);
        glBegin(GL_LINE_STRIP);
        for(int x=-160; x<=160; x+=2)
        {
            float wx=(float)x;
            float wy=ly+sinf(wx*0.07f+sw*speed)*amp;
            glVertex2f(wx,wy);
        }
        glEnd();
    }
    glLineWidth(1.0f);
    glColor4f(0.30f,0.22f,0.14f,0.60f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(-160,S4_SEA_TOP);
    glVertex2f(160,S4_SEA_TOP);
    glEnd();
    glLineWidth(1.0f);

    float tAlpha=0.50f+0.12f*sinf(g_time*1.2f);
    glColor4f(0.88f,0.72f,0.22f,tAlpha*0.60f);
    drawVignette(0.55f);
    glColor4f(0.25f,0.04f,0.02f,0.06f);
    drawRect(-160,-80,320,180,1);
}

// ================================================================
// SCENE 5 - CEASEFIRE (completely redesigned)
// ================================================================

// Draw an injured soldier waving a white flag on a stick
// dir: 1=facing right, -1=facing left
// wounded: adds blood/bandage details
static void drawInjuredSoldierWithFlag(float sx,float sy,float sc2,float skinR,float skinG,float skinB,int dir,float waveT)
{
    float d=(float)dir;
    // Boots (dark, scuffed)
    glColor3f(0.10f,0.08f,0.06f);
    drawRect(sx-sc2*2.0f,sy-sc2*5.3f,sc2*1.8f,sc2*0.7f,1);
    drawRect(sx+sc2*0.2f,sy-sc2*5.3f,sc2*1.8f,sc2*0.7f,1);
    // Torn trousers (dark green, some dirt)
    glColor3f(0.14f,0.20f,0.13f);
    glBegin(GL_QUADS);
    glVertex2f(sx-sc2*2,sy);
    glVertex2f(sx-sc2*0.2f,sy);
    glVertex2f(sx-sc2*0.2f,sy-sc2*5);
    glVertex2f(sx-sc2*2,sy-sc2*5);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(sx+sc2*0.2f,sy);
    glVertex2f(sx+sc2*2,sy);
    glVertex2f(sx+sc2*2,sy-sc2*5);
    glVertex2f(sx+sc2*0.2f,sy-sc2*5);
    glEnd();
    // Blood stain on leg
    glColor4f(0.55f,0.05f,0.05f,0.7f);
    drawCircle(sx-sc2*1.2f,sy-sc2*2,sc2*0.8f,8,1);
    // Torso (torn military jacket)
    glColor3f(0.15f,0.22f,0.14f);
    glBegin(GL_POLYGON);
    glVertex2f(sx-sc2*2.5f,sy+sc2*7);
    glVertex2f(sx+sc2*2.5f,sy+sc2*7);
    glVertex2f(sx+sc2*2.0f,sy);
    glVertex2f(sx-sc2*2.0f,sy);
    glEnd();
    // Torn sleeve detail
    glColor4f(0.10f,0.16f,0.09f,0.7f);
    glBegin(GL_LINES);
    glLineWidth(1.2f);
    glVertex2f(sx-sc2*1.8f,sy+sc2*3);
    glVertex2f(sx-sc2*2.2f,sy+sc2*5);
    glEnd();
    glLineWidth(1.0f);
    // Wound/bandage on torso
    glColor3f(0.85f,0.82f,0.78f);
    drawRect(sx-sc2*0.8f,sy+sc2*3,sc2*1.6f,sc2*1.2f,1);
    glColor4f(0.70f,0.05f,0.05f,0.6f);
    drawRect(sx-sc2*0.3f,sy+sc2*3.2f,sc2*0.6f,sc2*0.7f,1);
    // Left arm (hanging / injured)
    glColor3f(skinR,skinG,skinB);
    glLineWidth(sc2*1.8f);
    glBegin(GL_LINES);
    glVertex2f(sx-sc2*2.5f,sy+sc2*6);
    glVertex2f(sx-sc2*3.5f,sy+sc2*2.5f);
    glEnd();
    drawCircle(sx-sc2*3.5f,sy+sc2*2.5f,sc2*0.8f,8,1);
    // Bandage on left arm
    glColor3f(0.85f,0.82f,0.78f);
    glLineWidth(sc2*2.2f);
    glBegin(GL_LINES);
    glVertex2f(sx-sc2*3.2f,sy+sc2*3.5f);
    glVertex2f(sx-sc2*2.8f,sy+sc2*4.5f);
    glEnd();
    glLineWidth(1.0f);
    // RIGHT arm (raised, holding flag stick)
    glColor3f(0.15f,0.22f,0.14f);
    glLineWidth(sc2*1.8f);
    glBegin(GL_LINES);
    glVertex2f(sx+sc2*2.5f,sy+sc2*6);
    glVertex2f(sx+sc2*d*4.5f,sy+sc2*10.5f);
    glEnd();
    glLineWidth(1.0f);
    glColor3f(skinR,skinG,skinB);
    drawCircle(sx+sc2*d*4.5f,sy+sc2*10.5f,sc2*0.9f,8,1);
    // FLAG STICK (held in right hand, going up)
    float stickX=sx+sc2*d*4.5f,stickY=sy+sc2*10.5f;
    glColor3f(0.50f,0.42f,0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(stickX,stickY);
    glVertex2f(stickX,stickY+sc2*12);
    glEnd();
    glLineWidth(1.0f);
    // WHITE FLAG cloth (waving)
    float fw=sc2*10,fh=sc2*6;
    int segs=12;
    glBegin(GL_QUAD_STRIP);
    for(int i=0; i<=segs; i++)
    {
        float t=(float)i/segs;
        float wave=sinf(t*PI*2+waveT)*sc2*2.5f*t;
        float shade=0.88f+0.10f*(1.0f-t*0.5f);
        glColor3f(shade,shade,shade);
        glVertex2f(stickX+t*fw*d,stickY+sc2*12+wave+fh*0.5f);
        glVertex2f(stickX+t*fw*d,stickY+sc2*12+wave-fh*0.5f);
    }
    glEnd();
    // Flag edge
    glColor4f(0.78f,0.76f,0.72f,0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(stickX,stickY+sc2*12+fh*0.5f);
    glVertex2f(stickX+fw*d,stickY+sc2*12+sinf(1.0f*PI*2+waveT)*sc2*2.5f+fh*0.5f);
    glVertex2f(stickX+fw*d,stickY+sc2*12+sinf(1.0f*PI*2+waveT)*sc2*2.5f-fh*0.5f);
    glVertex2f(stickX,stickY+sc2*12-fh*0.5f);
    glEnd();
    // Neck
    glColor3f(skinR,skinG,skinB);
    drawRect(sx-sc2*0.8f,sy+sc2*7,sc2*1.6f,sc2*1.5f,1);
    // Head
    drawCircle(sx,sy+sc2*10.2f,sc2*2.5f,14,1);
    // Helmet (dented/worn)
    glColor3f(0.15f,0.22f,0.14f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=14; k++)
    {
        float a=-PI+PI*k/14;
        float rr=(a<0)?sc2*2.8f:sc2*1.9f;
        glVertex2f(sx+cosf(a)*rr,sy+sc2*10.2f+sinf(a)*rr*(a<0?0.68f:0.42f));
    }
    glEnd();
    // Eyes (weary)
    glColor3f(0.25f,0.18f,0.12f);
    drawCircle(sx-sc2*0.9f,sy+sc2*10.6f,sc2*0.4f,6,1);
    drawCircle(sx+sc2*0.9f,sy+sc2*10.6f,sc2*0.4f,6,1);
    // Sweat/dirt smear
    glColor4f(skinR*0.55f,skinG*0.45f,skinB*0.38f,0.6f);
    glLineWidth(sc2*0.4f);
    glBegin(GL_LINES);
    glVertex2f(sx-sc2*0.5f,sy+sc2*11);
    glVertex2f(sx-sc2*0.3f,sy+sc2*9.5f);
    glEnd();
    glLineWidth(1.0f);
}

// Enemy general stepping forward for ceasefire - sand/khaki uniform
static void drawEnemyGeneral(float gx,float gy,float sc2,float stepForward)
{
    float skinR=0.68f,skinG=0.54f,skinB=0.40f;
    // Step forward lean
    float lean=stepForward*sc2*2.0f;

    // Boots
    glColor3f(0.12f,0.10f,0.07f);
    drawRect(gx-sc2*2.0f+lean*0.2f,gy-sc2*5.3f,sc2*1.9f,sc2*0.8f,1);
    drawRect(gx+sc2*0.1f+lean*0.3f,gy-sc2*5.3f,sc2*1.9f,sc2*0.8f,1);

    // Trousers (tan/khaki with red stripe)
    glColor3f(0.55f,0.48f,0.30f);
    glBegin(GL_QUADS);
    glVertex2f(gx-sc2*2+lean*0.2f,gy);
    glVertex2f(gx-sc2*0.2f+lean*0.2f,gy);
    glVertex2f(gx-sc2*0.2f+lean*0.2f,gy-sc2*5);
    glVertex2f(gx-sc2*2+lean*0.2f,gy-sc2*5);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(gx+sc2*0.2f+lean*0.3f,gy);
    glVertex2f(gx+sc2*2+lean*0.3f,gy);
    glVertex2f(gx+sc2*2+lean*0.3f,gy-sc2*5);
    glVertex2f(gx+sc2*0.2f+lean*0.3f,gy-sc2*5);
    glEnd();
    // Red trouser stripe
    glColor3f(0.65f,0.08f,0.08f);
    glLineWidth(sc2*0.5f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*1.2f+lean*0.2f,gy-sc2*5);
    glVertex2f(gx-sc2*1.2f+lean*0.2f,gy);
    glVertex2f(gx+sc2*1.2f+lean*0.3f,gy-sc2*5);
    glVertex2f(gx+sc2*1.2f+lean*0.3f,gy);
    glEnd();
    glLineWidth(1.0f);

    // Jacket (tan/khaki Middle-Eastern general)
    float jlean=lean*0.25f;
    glColor3f(0.58f,0.50f,0.32f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*2.8f+jlean,gy+sc2*7.5f);
    glVertex2f(gx+sc2*2.8f+jlean,gy+sc2*7.5f);
    glVertex2f(gx+sc2*2.4f+jlean,gy);
    glVertex2f(gx-sc2*2.4f+jlean,gy);
    glEnd();
    // Lapels (darker)
    glColor3f(0.48f,0.40f,0.26f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*1.0f+jlean,gy+sc2*7.5f);
    glVertex2f(gx-sc2*2.8f+jlean,gy+sc2*7.5f);
    glVertex2f(gx-sc2*0.4f+jlean,gy+sc2*3.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(gx+sc2*1.0f+jlean,gy+sc2*7.5f);
    glVertex2f(gx+sc2*2.8f+jlean,gy+sc2*7.5f);
    glVertex2f(gx+sc2*0.4f+jlean,gy+sc2*3.5f);
    glEnd();
    // Medals (gold)
    glColor3f(0.80f,0.65f,0.14f);
    for(int m=0; m<4; m++)drawCircle(gx-sc2*1.8f+m*sc2*1.1f+jlean,gy+sc2*5.8f,sc2*0.55f,8,1);
    // Epaulettes (gold fringed)
    glColor3f(0.82f,0.68f,0.12f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*2.8f+jlean,gy+sc2*7.3f);
    glVertex2f(gx-sc2*1.4f+jlean,gy+sc2*7.3f);
    glVertex2f(gx-sc2*1.6f+jlean,gy+sc2*7.8f);
    glVertex2f(gx-sc2*2.8f+jlean,gy+sc2*7.8f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(gx+sc2*1.4f+jlean,gy+sc2*7.3f);
    glVertex2f(gx+sc2*2.8f+jlean,gy+sc2*7.3f);
    glVertex2f(gx+sc2*2.8f+jlean,gy+sc2*7.8f);
    glVertex2f(gx+sc2*1.6f+jlean,gy+sc2*7.8f);
    glEnd();
    // Right arm (extended forward for handshake/ceasefire gesture)
    glColor3f(0.58f,0.50f,0.32f);
    glLineWidth(sc2*2.0f);
    float armExtX=gx-sc2*5.5f+jlean, armExtY=gy+sc2*6.0f;
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*2.8f+jlean,gy+sc2*7.0f);
    glVertex2f(armExtX,armExtY);
    glEnd();
    glColor3f(skinR,skinG,skinB);
    drawCircle(armExtX,armExtY,sc2*0.95f,8,1);
    // Left arm (at side)
    glColor3f(0.58f,0.50f,0.32f);
    glBegin(GL_LINES);
    glVertex2f(gx+sc2*2.8f+jlean,gy+sc2*7.0f);
    glVertex2f(gx+sc2*4.5f+jlean,gy+sc2*3.5f);
    glEnd();
    glColor3f(skinR,skinG,skinB);
    drawCircle(gx+sc2*4.5f+jlean,gy+sc2*3.5f,sc2*0.9f,8,1);
    glLineWidth(1.0f);
    // Neck
    glColor3f(skinR,skinG,skinB);
    drawRect(gx-sc2*0.8f+jlean,gy+sc2*7.5f,sc2*1.6f,sc2*2.0f,1);
    // Head
    drawCircle(gx+jlean,gy+sc2*11.5f,sc2*2.8f,16,1);
    // Eyes (determined, stepping forward)
    glColor3f(0.15f,0.10f,0.08f);
    drawCircle(gx-sc2*1.0f+jlean,gy+sc2*12.0f,sc2*0.45f,8,1);
    drawCircle(gx+sc2*1.0f+jlean,gy+sc2*12.0f,sc2*0.45f,8,1);
    // Eyebrows (stern)
    glColor3f(0.12f,0.08f,0.05f);
    glLineWidth(sc2*0.7f);
    glBegin(GL_LINES);
    glVertex2f(gx-sc2*1.8f+jlean,gy+sc2*12.8f);
    glVertex2f(gx-sc2*0.4f+jlean,gy+sc2*13.0f);
    glVertex2f(gx+sc2*0.4f+jlean,gy+sc2*13.0f);
    glVertex2f(gx+sc2*1.8f+jlean,gy+sc2*12.8f);
    glEnd();
    glLineWidth(1.0f);
    // Nose
    glColor3f(skinR*0.7f,skinG*0.6f,skinB*0.55f);
    glLineWidth(sc2*0.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx+jlean,gy+sc2*11.0f);
    glVertex2f(gx+sc2*0.5f+jlean,gy+sc2*10.2f);
    glVertex2f(gx+sc2*1.0f+jlean,gy+sc2*10.4f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx+jlean,gy+sc2*11.0f);
    glVertex2f(gx-sc2*0.5f+jlean,gy+sc2*10.2f);
    glVertex2f(gx-sc2*1.0f+jlean,gy+sc2*10.4f);
    glEnd();
    glLineWidth(1.0f);
    // Mouth (firm, slightly open)
    glColor3f(skinR*0.6f,skinG*0.45f,skinB*0.40f);
    glLineWidth(sc2*0.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(gx-sc2*1.0f+jlean,gy+sc2*9.8f);
    glVertex2f(gx+jlean,gy+sc2*9.5f);
    glVertex2f(gx+sc2*1.0f+jlean,gy+sc2*9.8f);
    glEnd();
    glLineWidth(1.0f);
    // Beard (Middle-Eastern general)
    glColor3f(0.10f,0.07f,0.05f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=12; k++)
    {
        float a=PI*0.1f+PI*0.8f*k/12;
        glVertex2f(gx+jlean+cosf(a)*sc2*2.9f,gy+sc2*11.5f+sinf(a)*sc2*2.0f);
    }
    glEnd();
    // Military peaked cap (tan)
    glColor3f(0.50f,0.44f,0.28f);
    glBegin(GL_POLYGON);
    for(int k=0; k<=14; k++)
    {
        float a=-PI+PI*k/14;
        float rr=(a<0)?sc2*3.2f:sc2*2.3f;
        glVertex2f(gx+jlean+cosf(a)*rr,gy+sc2*11.5f+sinf(a)*rr*(a<0?0.68f:0.40f));
    }
    glEnd();
    // Gold band on cap
    glColor3f(0.82f,0.68f,0.12f);
    glLineWidth(sc2*0.6f);
    glBegin(GL_LINE_STRIP);
    for(int k=4; k<=10; k++)
    {
        float a=-PI+PI*k/14;
        glVertex2f(gx+jlean+cosf(a)*sc2*3.2f,gy+sc2*11.5f+sinf(a)*sc2*3.2f*0.68f);
    }
    glEnd();
    glLineWidth(1.0f);
    // Cap visor
    glColor3f(0.35f,0.30f,0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(gx-sc2*1.8f+jlean,gy+sc2*13.5f);
    glVertex2f(gx-sc2*4.8f+jlean,gy+sc2*13.0f);
    glVertex2f(gx-sc2*5.0f+jlean,gy+sc2*11.5f);
    glVertex2f(gx-sc2*1.2f+jlean,gy+sc2*11.8f);
    glEnd();
    // Gold star badge
    glColor3f(0.88f,0.78f,0.20f);
    drawCircle(gx-sc2*0.5f+jlean,gy+sc2*13.8f,sc2*0.9f,5,1);
}

// Burnt car wreck for scene 5 (same as scene 3)
static void drawBurntCarS5(float cx,float cy,float phase)
{
    glColor4f(0,0,0,0.28f);
    glBegin(GL_POLYGON);
    for(int k=0; k<8; k++)
    {
        float a=TWO_PI*k/8;
        glVertex2f(cx+cosf(a)*9,cy-1+sinf(a)*2);
    }
    glEnd();
    glColor3f(0.12f,0.08f,0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-9,cy+0.5f);
    glVertex2f(cx+9,cy+0.5f);
    glVertex2f(cx+9,cy+5);
    glVertex2f(cx-9,cy+5);
    glEnd();
    glColor3f(0.09f,0.06f,0.04f);
    glBegin(GL_POLYGON);
    glVertex2f(cx-6,cy+5);
    glVertex2f(cx+5,cy+5);
    glVertex2f(cx+3.5f,cy+8.5f);
    glVertex2f(cx-4.5f,cy+8.5f);
    glEnd();
    glColor3f(0.06f,0.05f,0.04f);
    drawCircle(cx-5.5f,cy+0.5f,2.5f,10,1);
    drawCircle(cx+5.5f,cy+0.5f,2.5f,10,1);
    float fp=phase+g_time*2.2f;
    float fhl=5+2.5f*sinf(fp);
    glColor4f(0.88f,0.26f,0.02f,0.75f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx-4,cy+5);
    glVertex2f(cx,cy+5);
    glVertex2f(cx-2+sinf(fp*1.3f)*1.5f,cy+5+fhl);
    glEnd();
    float fh2=4+2*sinf(fp+1.5f);
    glColor4f(0.88f,0.26f,0.02f,0.72f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx+1,cy+5);
    glVertex2f(cx+4,cy+5);
    glVertex2f(cx+2.5f+sinf(fp*1.6f)*1.2f,cy+5+fh2);
    glEnd();
}

// Draw a dove
static void drawDove(float dx,float dy,float wingT)
{
    glColor3f(0.97f,0.96f,0.94f);
    glBegin(GL_POLYGON);
    glVertex2f(dx-4,dy);
    glVertex2f(dx+2,dy+1);
    glVertex2f(dx+5,dy);
    glVertex2f(dx+2,dy-1);
    glEnd();
    drawCircle(dx+5,dy+0.5f,1.5f,10,1);
    glColor3f(0.9f,0.75f,0.4f);
    glBegin(GL_TRIANGLES);
    glVertex2f(dx+6.3f,dy+0.5f);
    glVertex2f(dx+8,dy+0.7f);
    glVertex2f(dx+6.3f,dy+0.2f);
    glEnd();
    glColor3f(0.95f,0.94f,0.92f);
    float wy=dy+sinf(wingT)*3;
    glBegin(GL_TRIANGLES);
    glVertex2f(dx,dy);
    glVertex2f(dx-2,wy+4);
    glVertex2f(dx+2,wy+4);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(dx-3.5f,dy);
    glVertex2f(dx-7,dy+2);
    glVertex2f(dx-7,dy-2);
    glEnd();
}

// Large central white flag pole
static void drawCentralWhiteFlag(float fx,float fy,float poleH,float waveT)
{
    // Pole
    glColor3f(0.58f,0.52f,0.38f);
    glLineWidth(3.5f);
    glBegin(GL_LINES);
    glVertex2f(fx,fy);
    glVertex2f(fx,fy+poleH);
    glEnd();
    glLineWidth(1.0f);
    // Pole knob
    glColor3f(0.78f,0.72f,0.48f);
    drawCircle(fx,fy+poleH,2.5f,12,1);
    // White flag cloth
    float fw=42.0f,fh=24.0f;
    int segs=20;
    glBegin(GL_QUAD_STRIP);
    for(int i=0; i<=segs; i++)
    {
        float t=(float)i/segs;
        float wave=sinf(t*PI*2+waveT)*5.5f*t+sinf(t*PI*1.2f+waveT*1.4f)*2.5f*t;
        float shade=0.90f+0.08f*(1.0f-t*0.6f);
        glColor3f(shade,shade,shade*0.98f);
        glVertex2f(fx+t*fw,fy+poleH+wave+fh*0.5f);
        glVertex2f(fx+t*fw,fy+poleH+wave-fh*0.5f);
    }
    glEnd();
    // Flag outline (subtle)
    glColor4f(0.72f,0.70f,0.68f,0.45f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<=segs; i++)
    {
        float t=(float)i/segs;
        float wave=sinf(t*PI*2+waveT)*5.5f*t+sinf(t*PI*1.2f+waveT*1.4f)*2.5f*t;
        glVertex2f(fx+t*fw,fy+poleH+wave+fh*0.5f);
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<=segs; i++)
    {
        float t=(float)i/segs;
        float wave=sinf(t*PI*2+waveT)*5.5f*t+sinf(t*PI*1.2f+waveT*1.4f)*2.5f*t;
        glVertex2f(fx+t*fw,fy+poleH+wave-fh*0.5f);
    }
    glEnd();
    glLineWidth(1.0f);
    // Glow halo on flag
    float glowA=0.06f+0.04f*sinf(g_time*1.5f);
    glColor4f(0.95f,0.95f,0.90f,glowA);
    drawCircle(fx+fw*0.5f,fy+poleH,fw*0.75f,32,1);
}

static void updateScene5(float dt)
{
    s5_glow+=dt;
    s5_flagWave+=dt*1.8f;
    // Doves fly gracefully
    for(int i=0; i<7; i++)
    {
        s5_doveX[i]+=cosf(s5_glow*0.4f+i*0.7f)*dt*12;
        s5_doveY[i]+=sinf(s5_glow*0.3f+i*0.9f)*dt*6;
        if(s5_doveX[i]>162)s5_doveX[i]=-155;
        if(s5_doveX[i]<-162)s5_doveX[i]=155;
        if(s5_doveY[i]>90)s5_doveY[i]=20;
        if(s5_doveY[i]<15)s5_doveY[i]=15;
        s5_doveWing[i]+=dt*5.0f;
    }
    // Background city smoke (residual)
    if(rf(0,1)<0.12f)
    {
        int bi=ri(0,S4_CITY_BLDS-1);
        pSpawn(s4_city[bi].x+s4_city[bi].w*0.5f+rf(-5,5),
               s4_city[bi].y+s4_city[bi].h*0.5f+rf(0,8),
               rf(-0.15f,0.15f),rf(0.2f,0.8f),0,0,
               rf(0.15f,0.25f),rf(0.12f,0.20f),rf(0.08f,0.15f),0.55f,
               rf(4,12),rf(8,18),1);
    }
    // Embers dying down
    if(rf(0,1)<0.04f)
        pSpawn(rf(-130,130),rf(S4_CITY_BOT+5,S4_CITY_BOT+50),rf(-0.3f,0.3f),rf(0.2f,0.9f),0,-0.1f,rf(0.8f,1.0f),rf(0.4f,0.6f),0.05f,0.6f,rf(0.3f,0.8f),rf(1,2),2);
    pUpdate(dt);
}

static void drawScene5()
{
    float dawn=fminf(s5_glow*0.05f,1.0f); // Slower dawn for ceasefire mood

    // ---- SKY: warm dawn with fire-red glow on horizon ----
    // Top sky (still dark but lightening)
    glBegin(GL_QUADS);
    glColor3f(0.04f+0.30f*dawn, 0.04f+0.22f*dawn, 0.12f+0.28f*dawn);
    glVertex2f(-160,100);
    glVertex2f(160,100);
    glColor3f(0.12f+0.35f*dawn, 0.06f+0.28f*dawn, 0.04f+0.22f*dawn);
    glVertex2f(160,55);
    glVertex2f(-160,55);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.12f+0.35f*dawn, 0.06f+0.28f*dawn, 0.04f+0.22f*dawn);
    glVertex2f(-160,55);
    glVertex2f(160,55);
    // Horizon: warm fire orange/red glow from burning city
    glColor3f(0.35f+0.20f*dawn, 0.14f+0.15f*dawn, 0.04f+0.06f*dawn);
    glVertex2f(160,5);
    glVertex2f(-160,5);
    glEnd();

    // Dim stars fading
    drawStars(fmaxf(0,0.5f-dawn));
    // Sun rising in distance (faint at first)
    if(dawn>0.2f)
    {
        float sunAlpha=(dawn-0.2f)/0.8f;
        glColor4f(1.0f,0.65f+0.25f*dawn,0.1f+0.3f*dawn,sunAlpha*0.7f);
        drawCircle(-110,5+dawn*22,12+dawn*8,24,1);
        glColor4f(1.0f,0.72f+0.2f*dawn,0.2f+0.25f*dawn,sunAlpha*0.25f);
        drawCircle(-110,5+dawn*22,28+dawn*15,20,1);
    }
    // Moon fading
    if(dawn<0.6f)drawMoon(110,78);

    // Smoke clouds from burning city (background)
    float smokeA=fmaxf(0.15f,0.55f-dawn*0.4f);
    glColor4f(0.16f,0.12f,0.08f,smokeA);
    drawCircle(-90,28,18,20,1);
    drawCircle(-40,24,15,18,1);
    drawCircle(15,30,22,20,1);
    drawCircle(60,26,17,18,1);
    drawCircle(105,28,20,18,1);
    glColor4f(0.22f,0.16f,0.10f,smokeA*0.6f);
    drawCircle(-65,36,13,16,1);
    drawCircle(38,32,18,16,1);
    drawCircle(82,34,14,16,1);

    // ---- BACKGROUND: Burning enemy city (from scene 4, already damaged) ----
    // Ground of enemy city
    glColor3f(0.28f,0.22f,0.14f);
    drawRect(-160,S4_CITY_BOT,320,5,1);

    // Draw scene4 buildings (already damaged/destroyed state)
    for(int i=0; i<S4_CITY_BLDS; i++)
    {
        drawS4Building(s4_city[i]);
    }

    // Burning cars in background city
    float bgCarX[]= {-155,-138,-120,-103,-85,-68,-50,28,48,68,90,112,132,152};
    float bgCarP[]= {0.3f,1.1f,2.0f,0.8f,1.6f,0.4f,1.2f,1.8f,0.6f,2.1f,1.0f,0.5f,1.7f,0.9f};
    for(int i=0; i<14; i++)drawBurntCarS5(bgCarX[i],S4_CITY_BOT,bgCarP[i]);

    // Persistent fires on some rubble
    for(int i=0; i<8; i++)
    {
        float fx=-140+i*40.0f;
        float fp=i*1.3f+g_time*2.5f;
        float fh=6+3*sinf(fp);
        float fw2=3+sinf(fp*1.3f);
        float fa=fmaxf(0.2f,0.70f-dawn*0.5f); // fade fires as dawn comes
        glColor4f(0.85f,0.28f,0.02f,fa);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx-fw2,S4_CITY_BOT+1);
        glVertex2f(fx+fw2,S4_CITY_BOT+1);
        glVertex2f(fx+sinf(fp*1.4f)*fw2*0.5f,S4_CITY_BOT+1+fh);
        glEnd();
        glColor4f(1.0f,0.60f,0.05f,fa*0.75f);
        glBegin(GL_TRIANGLES);
        glVertex2f(fx-fw2*0.65f,S4_CITY_BOT+1);
        glVertex2f(fx+fw2*0.65f,S4_CITY_BOT+1);
        glVertex2f(fx+sinf(fp*2.0f)*fw2*0.3f,S4_CITY_BOT+1+fh*0.72f);
        glEnd();
    }

    // Particles (smoke, embers)
    pDraw();

    // Ground in front (our side - sandy/rocky)
    glBegin(GL_QUADS);
    glColor3f(0.30f,0.25f,0.16f);
    glVertex2f(-160,S4_CITY_BOT);
    glVertex2f(160,S4_CITY_BOT);
    glColor3f(0.35f,0.28f,0.18f);
    glVertex2f(160,S4_CITY_BOT-12);
    glVertex2f(-160,S4_CITY_BOT-12);
    glEnd();
    glColor3f(0.22f,0.18f,0.11f);
    drawRect(-160,-80,320,45+S4_CITY_BOT,1);
    // Ground cracked road
    glColor3f(0.20f,0.16f,0.10f);
    drawRect(-160,S4_CITY_BOT,320,3,1);
    glColor4f(0.35f,0.30f,0.20f,0.4f);
    for(int rx=-155; rx<155; rx+=24)drawRect((float)rx,S4_CITY_BOT+0.5f,13,1.5f,1);

    // Rubble/debris on ground
    glColor3f(0.26f,0.20f,0.13f);
    for(int r=0; r<10; r++)
    {
        float rx=-145+r*30.0f;
        drawRect(rx,S4_CITY_BOT,rf(3,8),rf(1,4),1);
    }

    // ---- FOREGROUND FIGURES ----
    // OUR SIDE (LEFT): injured soldiers waving white flags
    float ourSkinPals[3][3]= {{0.75f,0.60f,0.46f},{0.68f,0.54f,0.42f},{0.80f,0.64f,0.50f}};
    float ourX[]= {-155,-130,-105,-80};
    int ourDir[]= {1,1,1,1};
    for(int i=0; i<4; i++)
    {
        int sp=i%3;
        drawInjuredSoldierWithFlag(ourX[i],S4_CITY_BOT,1.4f,
                                   ourSkinPals[sp][0],ourSkinPals[sp][1],ourSkinPals[sp][2],
                                   ourDir[i],s5_flagWave+i*0.8f);
    }

    // CENTER: Large prominent white flag pole
    drawCentralWhiteFlag(-10,S4_CITY_BOT,42,s5_flagWave);

    // ENEMY SIDE (RIGHT): Enemy general stepping forward for ceasefire
    // How far he has stepped forward (0 to 1 over time)
    float stepFwd=fminf(s5_glow*0.15f,1.0f);
    // Enemy general standing tall
    drawEnemyGeneral(110,S4_CITY_BOT,2.1f,stepFwd);

    // Our representative (second from right): stepping forward to meet general
    float ourRepX=-55+stepFwd*15;
    drawInjuredSoldierWithFlag(ourRepX,S4_CITY_BOT,1.6f,
                               0.75f,0.60f,0.48f, 1, s5_flagWave*1.2f);

    // ---- MEETING POINT: Handshake glow (when stepFwd is high enough) ----
    if(stepFwd>0.7f)
    {
        float meetX=(ourRepX+armEndXApprox(ourRepX,1.6f)+110-5.5f*1.6f)/2.0f;
        // Simple glow at meeting point
        float glowA=(stepFwd-0.7f)/0.3f*0.18f;
        glColor4f(1.0f,1.0f,0.85f,glowA);
        drawCircle(35,S4_CITY_BOT+14,20,24,1);
        glColor4f(1.0f,1.0f,0.85f,glowA*0.4f);
        drawCircle(35,S4_CITY_BOT+14,35,20,1);
    }

    // ---- DOVES OF PEACE (flying overhead) ----
    for(int d=0; d<7; d++)drawDove(s5_doveX[d],s5_doveY[d],s5_doveWing[d]);

    // Sun rays (appearing as dawn comes)
    if(dawn>0.15f)
    {
        glBegin(GL_TRIANGLES);
        for(int r=0; r<10; r++)
        {
            float rAng=-1.0f+r*0.22f;
            float rAlpha=0.04f*(dawn-0.15f)/0.85f*(0.5f+0.5f*sinf(s5_glow*1.5f+r));
            glColor4f(1.0f,0.92f,0.75f,rAlpha);
            glVertex2f(-110,5+dawn*22);
            glColor4f(1.0f,0.92f,0.75f,0);
            glVertex2f(-110+cosf(rAng)*220,5+dawn*22+sinf(rAng)*220);
            glVertex2f(-110+cosf(rAng+0.12f)*220,5+dawn*22+sinf(rAng+0.12f)*220);
        }
        glEnd();
    }

    drawVignette(fmaxf(0.1f,0.45f-dawn*0.35f));
    // Slight red/orange war tint fading
    float warTint=fmaxf(0,0.08f-dawn*0.08f);
    glColor4f(0.30f,0.06f,0.02f,warTint);
    drawRect(-160,-80,320,180,1);
}

// Helper to approximate arm end x (not a real call - just for meeting point glow)
static float armEndXApprox(float sx,float sc2)
{
    return sx + sc2*4.5f;
}


// ================================================================
// GLUT CALLBACKS
// ================================================================
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    switch(g_scene)
    {
    case 1:
        drawScene1();
        break;
    case 2:
        drawScene2();
        break;
    case 3:
        drawScene3();
        break;
    case 4:
        drawScene4();
        break;
    case 5:
        drawScene5();
        break;
    }
    glutSwapBuffers();
}
static void idle()
{
    int now=glutGet(GLUT_ELAPSED_TIME);
    g_dt=(now-g_lastMs)/1000.0f;
    if(g_dt>0.05f)g_dt=0.05f;
    g_lastMs=now;
    g_time+=g_dt;
    switch(g_scene)
    {
    case 1:
        updateScene1(g_dt);
        break;
    case 2:
        updateScene2(g_dt);
        break;
    case 3:
        updateScene3(g_dt);
        break;
    case 4:
        updateScene4(g_dt);
        break;
    case 5:
        updateScene5(g_dt);
        break;
    }
    glutPostRedisplay();
}
static void keyboard(unsigned char key,int,int)
{
    if(key==' ')
    {
        g_scene++;
        if(g_scene>5)g_scene=1;
        sceneInit(g_scene);
    }
    if(key=='r'||key=='R')
    {
        sceneInit(g_scene);
    }
    if(key==27)exit(0);
}
static void reshape(int w,int h)
{
    if(h==0)h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-160,160,-80,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
int main(int argc,char**argv)
{
    srand((unsigned)time(NULL));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(1200,700);
    glutInitWindowPosition(60,40);
    glutCreateWindow("WAR OF NATIONS");
    glClearColor(0.02f,0.02f,0.04f,1.0f);
    glLineWidth(1.0f);
    glPointSize(2.0f);
    initS4Jets();
    initS4Ships();
    initShipMissiles();
    initS4CityBuildings();
    sceneInit(1);
    initS3Buildings();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
