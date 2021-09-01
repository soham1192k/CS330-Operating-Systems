#include "wc.h"

int test = -1;
int finalTeam1 = -1;
int finalTeam2 = -1;
struct team teams[NUM_TEAMS];



int main(int argc, char *argv[])
{
  test = atoi(argv[1]); 
  spawnTeams();
  conductGroupMatches();
  match(finalTeam1, finalTeam2); // Finals
  endTeam(finalTeam1);
  endTeam(finalTeam2);
  return 0;
}
