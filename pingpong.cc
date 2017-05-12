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

#define V_MAX   20

#define ERREXIT(str) {fprintf(stderr, "Error: " str "\n"); exit(1);}
#define ERREXIT2(str, ...) {fprintf(stderr, "Error: " str "\n", __VA_ARGS__); exit(1);}

// Head and Tail
CFifo<bool,CFifo<>::w> *wr;
CFifo<bool,CFifo<>::r> *rd;

// Ball parameter
struct ball_t
{
  coordinate_t pos;
  int16_t vel;
}
ball_t ball_par = {0,0,0,0};  // x,y position, x,y velocity
const uint8_t size = 20;

/**
 * @brief Generate random ball parameter
 * @param p   ball parameter
 * @return None
 */
void generate_ball(ball_t *p)
{
  p->pox.x = rand() % DVI_WIDTH;
  p->pox.y = rand() % DVI_HEIGHT;
  p->vel.x = (rand() % 2*V_MAX) - V_MAX;
  p->vel.y = (rand() % 2*V_MAX) - V_MAX;
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
  if (p->pos.x > DVI_WIDTH)   // pos is unsigned, negative position will
  {                           // be >> DVI_WIDTH
    p->vel.x *= -1;
    p->pos.x += p->vel.x;
  }
  if (p->pos.y > DVI_HEIGHT)  // pos is unsigned, negative position will
  {                           // be >> DVI_HEIGHT
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
void draw_ball(ball_t *p, uint8_t size)
{
  // Temporary ball using rectangle
  fillrect(
      p->pos.x,      p->pos.y,
      p->pos.x+size, p->pos.y+size,
      white
      );
}


/* ====Multiprocess==== */
void *update(void *arg) 
{
  // Initialize random generator
  srand((unsigned) time(&t));

  // Initialize ball parameter
  generate_ball(&ball_par);

  // Check FIFO
  wr->validate();

  while(1) 
  {
    update_ball(&ball_par);

    printf("Ping\n");
    wr->push(true);

    sleep(1);
  }
  return NULL;
}

void *render(void *arg) 
{
  // Init render
  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);
  render_flip_buffer();

  // Check FIFO
  rd->validate();

  while (true)
  {
    rd->pop();

    // Draw to back buffer
    fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);
    draw_ball(&ball_par, size);

    // Flip buffer
    render_flip_buffer();

    printf("\tPong\n");
    wr->push(true);
  }

  render_destroy();

  return NULL;
}

int main(int argc,char** argv)
{
  time_t t;
  pid_t pid0, pid1;

  printf("Pingpong...\n");

  // Prepare FIFO
  CFifoPtr<bool> fifo12 = CFifo<bool>::Create(1, wr, 2, rd, 2);
  if(!fifo12.valid()) ERREXIT("Error creating buffer");

  // Create process
  if(int e=CreateProcess(pid0, update, NULL, PROC_DEFAULT_TIMESLICE,
        PROC_DEFAULT_STACK, 1))
    ERREXIT2("Process creation failed: %i", e);
  if(int e=CreateProcess(pid1, render, NULL, PROC_DEFAULT_TIMESLICE,
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
  if(int e=WaitProcess(pid0, NULL, 1)) ERREXIT2("Waiting on update %i@%i: %i\n", pid0, 1, e);
  if(int e=WaitProcess(pid1, NULL, 2)) ERREXIT2("Waiting on render %i@%i: %i\n", pid1, 2, e);

  printf("All done.\n");
  return 0;
}
