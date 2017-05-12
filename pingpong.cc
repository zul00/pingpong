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


struct ball_t
{
  coordinate_t pos;
  int16_t vel;
}

void draw_ball(ball_t *p, uint8_t size)
{
  // Temporary ball using rectangle
  fillrect(
      p->pos.x,      p->pos.y,
      p->pos.x+size, p->pos.y+size,
      white
      );

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

int main(int argc,char** argv)
{
  ball_t ball_par = {0,0,0,0};  // x,y position, x,y velocity
  const uint8_t size = 20;

  printf("Pingpong...\n");

  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);

  // Initialize ball parameter
  ball_par.pos = {50, 50};
  ball_par.vel = {2, 2};

  while (true)
  {
    // Draw to back buffer
    fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);
    draw_ball(&ball_par, size);

    // Flip buffer
    render_flip_buffer();

    sleep(1);
  }

  render_destroy();

  printf("All done.\n");
  return 0;
}
