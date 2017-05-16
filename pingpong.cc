/**
 * @file: pingpong
 * @author: Zul & Okky
 * @note:
 * RTS2 assignment on uBlaze using 32 core
 */

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <helix.h>
#include <time.h>

#define V_MAX   20
#define N_BALL  1
#define SIZE    20

#define ERREXIT(str) {fprintf(stderr, "Error: " str "\n"); exit(1);}
#define ERREXIT2(str, ...) {fprintf(stderr, "Error: " str "\n", __VA_ARGS__); exit(1);}

// Head and Tail
CFifo<bool,CFifo<>::w> *wr;
CFifo<bool,CFifo<>::r> *rd;

struct vel_t
{
  int16_t x;
  int16_t y;
};

// Ball parameter
struct ball_t
{
  coordinate_t pos;
  vel_t vel;
  uint8_t size;
  color_t color;
};

/**
 * @brief Generate random ball parameter
 * @param p   ball parameter
 * @return None
 */
void generate_ball(ball_t *p)
{
  p->pos.x = rand() % DVI_WIDTH;
  p->pos.y = rand() % DVI_HEIGHT;
  p->vel.x = (rand() % (2*V_MAX) - V_MAX);
  p->vel.y = (rand() % (2*V_MAX) - V_MAX);
  p->size  = SIZE;
  p->color = PIX_CONST(rand()%255, rand()%255, rand()%255);
}

/**
 * @brief Update ball parameter
 * @param p   ball parameter
 * @return None
 */
void update_ball(ball_t *p)
{
  // Update next position
  p->pos.x += p->vel.x;
  p->pos.y += p->vel.y;

  // Limit on the edge of the screen
  if (p->pos.x + p->size > DVI_WIDTH ||
      p->pos.x < p->size)
  { 
    p->vel.x *= -1;
    p->pos.x += p->vel.x;
  }
  if (p->pos.y + p->size > DVI_HEIGHT ||
      p->pos.y < p->size)
  {
    p->vel.y *= -1;
    p->pos.y += p->vel.y;
  }
}

/**
 * @brief Draw ball
 * @param p     ball parameter
 * @param size  size of ball
 * @return None
 */
void draw_ball(ball_t *p)
{
  uint8_t size = p->size;

  // Approximate ball using 8 points
  coordinate_t points[8]={
    {(coord_t)(p->pos.x+size/2),    (coord_t)(p->pos.y)},         // top
    {(coord_t)(p->pos.x+size/6),    (coord_t)(p->pos.y+size/6)},
    {(coord_t)(p->pos.x),           (coord_t)(p->pos.y+size/2)},  // left
    {(coord_t)(p->pos.x+size/6),    (coord_t)(p->pos.y+5*size/6)},
    {(coord_t)(p->pos.x+size/2),    (coord_t)(p->pos.y+size)},    // bottom
    {(coord_t)(p->pos.x+5*size/6),  (coord_t)(p->pos.y+5*size/6)},
    {(coord_t)(p->pos.x+size),      (coord_t)(p->pos.y+size/2)},  // right
    {(coord_t)(p->pos.x+5*size/6),  (coord_t)(p->pos.y+size/6)}
  };

  poly_t<8> poly(points);

  fillpoly(poly,p->color,255);
}


/* ====Multiprocess==== */
void *ping(void *arg) 
{
  ball_t ball[N_BALL];
  uint8_t idx = 0;
  uint16_t ctr = 0;
  time_t t;

  // Init render 
  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, orange);
  render_flip_buffer();

  // Initialize random generator
  srand((unsigned) 1*time(&t));

  // Initialize ball parameter
  for (idx=0;idx<N_BALL;idx++)
  {
    generate_ball(&(ball[idx]));
  }

  // Check FIFO
  wr->validate();

  while(1) 
  {
    // Draw to back buffer
    fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, orange);
    for (idx=0;idx<N_BALL;idx++)
    {
      update_ball(&(ball[idx]));
      draw_ball(&(ball[idx]));
    }

    printf("Ping, ctr=%u, core=%u\n", ctr++, GetProcID());
    printf("pos%4u;%4u; ", ball[0].pos.x, ball[0].pos.y);
    printf("vel%4d;%4d\n", ball[0].vel.x, ball[0].vel.y);
    wr->push(true);

    sleep(1);
  }
  return NULL;
}

void *pong(void *arg) 
{
  ball_t ball2[N_BALL];
  uint8_t idx = 0;
  time_t t;

  // Initialize random generator
  srand((unsigned) 2*time(&t));

  // Initialize ball parameter
  for (idx=0;idx<N_BALL;idx++)
  {
    generate_ball(&(ball2[idx]));
  }

  // Check FIFO
  rd->validate();

  while (true)
  {
    rd->pop();

    // Draw to back buffer
    for (idx=0;idx<N_BALL;idx++)
    {
      update_ball(&(ball2[idx]));
      draw_ball(&(ball2[idx]));
    }

    // Flip buffer
    render_flipnow();

    printf("\tPong, core=%u\n", GetProcID());
    printf("pos%4u;%4u; ", ball2[0].pos.x, ball2[0].pos.y);
    printf("vel%4d;%4d\n", ball2[0].vel.x, ball2[0].vel.y);
    sleep(1);
  }

  render_destroy();

  return NULL;
}

int main(int argc,char** argv)
{
  pid_t pid0, pid1;

  printf("Pingpong...\n");

  // Prepare FIFO
  CFifoPtr<bool> fifo12 = CFifo<bool>::Create(1, wr, 2, rd, 2);
  if(!fifo12.valid()) ERREXIT("Error creating buffer");

  // Create process
  if(int e=CreateProcess(pid0, ping, NULL, PROC_DEFAULT_TIMESLICE,
        PROC_DEFAULT_STACK, 1))
    ERREXIT2("Process creation failed: %i", e);
  if(int e=CreateProcess(pid1, pong, NULL, PROC_DEFAULT_TIMESLICE,
        PROC_DEFAULT_STACK, 2))
    ERREXIT2("Process creation failed: %i", e);

  // Set process flag
  if(int e=SetProcessFlags(pid0, PROC_FLAG_JOINABLE, 1))
    ERREXIT2("While setting process flags: %i", e);
  if(int e=SetProcessFlags(pid1, PROC_FLAG_JOINABLE, 2))
    ERREXIT2("While setting process flags: %i", e);

  // Start process
  if(int e=StartProcess(pid0, 1)) ERREXIT2("Could not start ping: %i", e);
  if(int e=StartProcess(pid1, 2)) ERREXIT2("Could not start pong: %i", e);

  // FIFOs are destroyed when the pointers goes out of scope
  if(int e=WaitProcess(pid0, NULL, 1)) ERREXIT2("Waiting on ping %i@%i: %i\n", pid0, 1, e);
  if(int e=WaitProcess(pid1, NULL, 2)) ERREXIT2("Waiting on pong %i@%i: %i\n", pid1, 2, e);

  printf("All done.\n");
  return 0;
}
