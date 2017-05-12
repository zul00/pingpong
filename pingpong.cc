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
#define N_BALL  4


struct ball_t
{
  coordinate_t pos;
  int16_t vel;
}

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

int main(int argc,char** argv)
{
  ball_t ball_par = {0,0,0,0};  // x,y position, x,y velocity
  ball_t ball[N_BALL];
  const uint8_t size = 20;
  time_t t;
  uint8_t idx = 0;

  printf("Pingpong...\n");

  // Initialize random generator
  srand((unsigned) time(&t));

  // Init render
  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);

  // Initialize ball parameter
  for (idx=0;idx<N_BALL;idx++)
  {
    generate_ball(&(ball_par[idx]));
  }

  while (true)
  {
    // Draw to back buffer
    fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);
    for (idx=0;idx<N_BALL;idx++)
    {
      update_ball(&(ball_par[idx]));
      draw_ball(&(ball_par[idx]), size);
    }

    // Flip buffer
    render_flip_buffer();

    sleep(1);
  }

  render_destroy();

  printf("All done.\n");
  return 0;
}
