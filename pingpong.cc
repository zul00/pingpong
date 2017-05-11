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
  coordinate_t vel;
}

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
  const uint8_t size = 20;

  printf("Pingpong...\n");

  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);

  while (true)
  {
    // Update position
    ball_par.pos = {50, 50};

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
