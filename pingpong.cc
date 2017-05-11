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

void draw_ball(coordinate_t p, uint8_t size)
{
  // Temporary ball using rectangle
  fillrect(
      p.x, p.y,
      p.x+size, p.y+size,
      white
      );
}

int main(int argc,char** argv)
{
  coordinate_t pos = {0,0}; // x,y position
  const uint8_t size = 20;

  printf("Pingpong...\n");

  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);

  while (true)
  {
    // Update position
    pos = {50, 50};

    // Draw to back buffer
    fillrect(0, 0, DVI_WIDTH, DVI_HEIGHT, ORANGE);
    draw_ball(pos, size);

    // Flip buffer
    render_flip_buffer();

    sleep(1);
  }

  render_destroy();

  printf("All done.\n");
  return 0;
}
