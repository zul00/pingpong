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

int main(int argc,char** argv)
{
  printf("Pingpong...\n");

  render_init(1);

  render_destroy();

  printf("All done.\n");
  return 0;
}
