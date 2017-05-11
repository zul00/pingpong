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
  printf("Pingpong...\n");

  render_init(1);

  // Reset screen with ORANGE 
  fillrect(0,0,DVI_WIDTH,DVI_HEIGHT,ORANGE);

  render_destroy();

  printf("All done.\n");
  return 0;
}
