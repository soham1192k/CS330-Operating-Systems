#ifndef WC_H_
#define WC_H_

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTFILE "output.txt"
#define HOST -1
#define RD 0
#define WR 1
#define NUM_TEAMS 8
#define TEAM_NAME_SIZE 14


struct team {
  char name[TEAM_NAME_SIZE];
  int matchpipe[2];
  int commpipe[2];
};

void teamPlay(void);
void endTeam(int teamID);
int match(int team1, int team2);
void spawnTeams(void);
void conductGroupMatches(void);

#endif
