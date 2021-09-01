/* NOTE->REQUIRES THE PRESENCE OF AN "out" folder inside test/1 */

//header
#include "wc.h"

extern struct team teams[NUM_TEAMS];
extern int test;
extern int finalTeam1;
extern int finalTeam2;

int processType = HOST;
//team names array-groups fixed
const char *team_names[] = {
  "India", "Australia", "New Zealand", "Sri Lanka",   // Group A
  "Pakistan", "South Africa", "England", "Bangladesh" // Group B
};

//helper function to handle error
void error(int e){

    if(e==-1){
        printf("Error\n");
        exit(-1);
    }

}

void teamPlay(void)
{
    //to store the path of the input file from which we have to take the input
    char filepath[50];
    
    //store the input path in the array
    sprintf(filepath,"test/%d/inp/%s",test,teams[processType].name);
    
    //open the file in read-only mode
    int src_fd=open(filepath,O_RDONLY);
    error(src_fd);

    //while the team is not eliminated from the tournament
    while(1)
    {
        int message;
        //read the message from the commpipe and check for errors if any
        int val=read(teams[processType].commpipe[0],&message,4);
        if(val!=4) error(-1);

        //Any integer which is not 1, when read from commpipe, indicates that the teamProcess should be closed.
        if(message==1) //process should continue
        {
            char* ch= (char*)malloc(sizeof(char));

            //reading a character from the source file
            error(read(src_fd,ch,sizeof(char)));
            
            //writing the character read to the matchpipe
            int val=write(teams[processType].matchpipe[1],ch,1);
            if(val!=1) error(-1);
        }

        //process should self-terminate
        else break;
    }
}

void endTeam(int teamID)
{
    //we need to send a message to commpipe telling it to terminate the process
    
    //0 indicates-terminate the process
    int message = 0;

    //write to commpipe,asking it to terminate the process
    int val=write(teams[teamID].commpipe[1],&message,4);
    if(val!=4) error(-1);
}

int match(int team1, int team2)
{
    //initially message is 1,which is written to the commpipe, so that we can read from matchpipe to do the toss
    int message=1; 

    // toss
    char *toss1=(char*)malloc(sizeof(char));
    char *toss2=(char*)malloc(sizeof(char));

    //write message to commpipe-team 1
    int val=write(teams[team1].commpipe[1],&message,4);
    if(val!=4) error(-1);
   
    //read value from matchpipe-team 1
    val=read(teams[team1].matchpipe[0],toss1,1);
    if(val!=1) error(-1);
    
    //write message to commpipe-team 2
    val=write(teams[team2].commpipe[1],&message,4);
    if(val!=4) error(-1);

    //read value from matchpipe-team 2
    val=read(teams[team2].matchpipe[0],toss2,1);
    if(val!=1) error(-1);
    
    int team_bat,team_bowl,teama,teamb;

    teama=atoi(toss1);
    teamb=atoi(toss2);
    
    //if sum is odd
    if((teama+teamb)&1){
        //team1 bats first
        team_bat=team1;
        team_bowl=team2;
    }

    else{
        //team1 bowls first
        team_bat=team2;
        team_bowl=team1;
    }
    
    // create output file
    char *newly_created_path=(char*)malloc(100*sizeof(char));
    sprintf(newly_created_path,"test/%d/out/%sv%s",test,teams[team_bat].name,teams[team_bowl].name);
    
    //appending final to the path, if the two teams are from different groups
    if(team1<=3&&team2>=4) strcat(newly_created_path,"-Final");
    
    //open the file 
    int dest_fd=open(newly_created_path,O_CREAT|O_WRONLY,0644);
    
    char *store=(char*)malloc(100*sizeof(char));
    strcpy(store,"Innings1: ");strcat(store,teams[team_bat].name);strcat(store," bats\n");

    //write to the destination file
    val=write(dest_fd,store,strlen(store));
    if(val!=strlen(store)) error(-1);
    
    //First innings
    int runs=0,individual_runs=0,wickets=0,number=1,ballcount=120;
    
    while(ballcount){
        //Batting
        char *a=(char*)malloc(sizeof(char));

        //writing to commpipe-team 1
        val=write(teams[team_bat].commpipe[1],&message,4);
        if(val!=4) error(-1);

        //reading from matchpipe-team 1
        val=read(teams[team_bat].matchpipe[0],a,1);
        if(val!=1) error(-1);

        //converting to integer
        teama=atoi(a);

        //Bowling
        char* b=(char*)malloc(sizeof(char));

        //writing to commpipe-team 2
        val=write(teams[team_bowl].commpipe[1],&message,4);
        if(val!=4) error(-1);

        //reading from matchpipe-team 2
        val=read(teams[team_bowl].matchpipe[0],b,1);
        if(val!=1) error(-1);
        
        //converting to integer
        teamb=atoi(b);

        if(teama!=teamb){
            runs+=teama;
            individual_runs+=teama;
        }

        else{
            
            wickets+=1;
            sprintf(store,"%d:%d\n",number,individual_runs);
            
            //resetting individual runs
            individual_runs = 0;
            
            //next batsman comes in
            number += 1;

            val=write(dest_fd,store,strlen(store));
            if(val!=strlen(store)) error(-1);
        }

        if(wickets==10) break;

        ballcount--;
    }

    //if after 20 overs, the team is not all out:
    if(wickets<10){
        sprintf(store,"%d:%d*\n",number,individual_runs);
        val=write(dest_fd,store,strlen(store));
        if(val!=strlen(store)) error(-1);
    }

    //write the total runs
    sprintf(store,"%s TOTAL: %d\n",teams[team_bat].name,runs);
    val=write(dest_fd,store,strlen(store));
    if(val!=strlen(store)) error(-1);

    int to_be_chased=runs;

    //Second Innings
    //swapping the two teams
    team_bat=team_bat^team_bowl;
    team_bowl=team_bat^team_bowl;
    team_bat=team_bat^team_bowl;

    //writing to the file
    sprintf(store,"Innings2: %s bats\n",teams[team_bat].name);
    val=write(dest_fd,store,strlen(store));
    if(val!=strlen(store)) error(-1);
 
    //resetting the variables
    runs=0;individual_runs=0;number=1;wickets=0;ballcount=120;

    while(ballcount){
        // Batting
        char* a=(char*)malloc(sizeof(char));
        
        //write to commpipe
        val=write(teams[team_bat].commpipe[1],&message,4);
        if(val!=4) error(-1);

        //reading from matchpipe
        val=read(teams[team_bat].matchpipe[0],a,1);
        if(val!=1) error(-1);

        //converting to integer
        teama=atoi(a);

        // Bowling
        char* b=(char*)malloc(sizeof(char));

        //write to commpipe
        val=write(teams[team_bowl].commpipe[1],&message,4);
        if(val!=4) error(-1);

        //read from matchpipe
        val=read(teams[team_bowl].matchpipe[0],b,1);
        if(val!=1) error(-1);
        
        teamb=atoi(b);
        if(teama!=teamb){
            runs+=teama;
            individual_runs+=teama;
        }

        else{

            wickets+=1;
            sprintf(store,"%d:%d\n",number,individual_runs);
            
            individual_runs=0;
            
            number+=1;

            val=write(dest_fd,store,strlen(store));
            if(val!=strlen(store)) error(-1);
        }

        if(wickets==10) break;
        
        //if runs is already greater than target,break
        if(runs>to_be_chased) break;

        ballcount--;
    }

    if(wickets<10){
        //write to file
        sprintf(store,"%d:%d*\n",number,individual_runs);
        val=write(dest_fd,store,strlen(store));
        if(val!=strlen(store));
    }

    sprintf(store,"%s TOTAL: %d\n",teams[team_bat].name,runs);
    val=write(dest_fd,store,strlen(store));
    if(val!=strlen(store)) error(-1);

    int winner=team1;
    //team_bat wins
    if(runs>to_be_chased)
    {
        winner=team_bat;
        sprintf(store,"%s beats %s by %d wickets\n",teams[team_bat].name,teams[team_bowl].name,10-wickets);
    }

    //team_bowl wins
    else if(runs<to_be_chased){
        winner=team_bowl;
        sprintf(store,"%s beats %s by %d runs\n",teams[team_bowl].name,teams[team_bat].name,to_be_chased-runs);
    }

    //lower ranked team wins
    else sprintf(store,"TIE: %s beats %s\n",teams[team1].name,teams[team2].name);

    //writing to the destination file
    val=write(dest_fd,store,strlen(store));
    if(val!=strlen(store)) error(-1);

    //return index of the winner
	return winner;
}

void spawnTeams(void)
{
    //we have to do the same work for 8 teams, hence we use a for loop to fork() and create new team processes
    for(int i=0;i<8;i++){

        //assign names to the corresponding entry in teams array.
        strcpy(teams[i].name,team_names[i]);

        //create the matchpipe
        if(pipe(teams[i].matchpipe)<0) error(-1);
        //create the commpipe
        if(pipe(teams[i].commpipe)<0) error(-1);
        
        //forking the process to create a new team process
        pid_t pid=fork();
        if(pid<0) error(-1);

        if(!pid) // Child
        {
            //set processtype to teamindex
            processType=i;

            //call the teamplay() function
            teamPlay();

            exit(0);
        }
        //host returns to main
    }
}

void conductGroupMatches(void)
{
    //for each group, we set up a pipe to communicate between host and group process
    int grp1[2],grp2[2];
    if(pipe(grp1)<0) error(-1);
    if(pipe(grp2)<0) error(-1);

    //fork a new group process
    int grp1_pid=fork();

    if(grp1_pid<0) error(-1);

    //first group process child
    if(!grp1_pid) 
    {
        //to keep track of the number of matches each team has won in that group
        int wins[4];
        memset(wins,0,sizeof(wins));

        //conduct matches between team a and team b, and increment win count of the winner
        for(int a=0;a<3;a++){
            for(int b=a+1;b<4;b++){
                wins[match(a,b)]++;
            }
        }

        //to find the winner of the current group
        int grp1_leader = 0;

        //the winner is the team with maximum wins, incase of tie, the team with lower rank is the winner. Also end those processes who are not winners
        for(int a=1;a<4;a++){
            if(wins[grp1_leader]<wins[a]){
                endTeam(grp1_leader);
                grp1_leader=a;
            }
            else endTeam(a);
        }
        
        //write to grp1 pipe the index of the leader
        int val=write(grp1[1],&grp1_leader,4);
        if(val!=4) error(-1);

        //self terminating the group process
        exit(0);
    }

    //fork the second group process
    int grp2_pid=fork();
    if(grp2_pid<0) error(-1);

    //second group process child
    if(!grp2_pid) 
    {
        int wins[4];
        memset(wins,0,sizeof(wins));

        //conduct matches between teams of that group
        for(int a=0;a<3;a++){
            for(int b=a+1;b<4;b++){
                wins[match(a+4,b+4)-4]++;
            }
        }

        int grp2_leader=0;
        //get the winner from group 2
        for(int a=1;a<4;a++){
            if(wins[grp2_leader]<wins[a]){
                endTeam(grp2_leader);
                grp2_leader=a;
            }
            else endTeam(a);
        }

        //teams in group 2 have an offset of 4
        grp2_leader+=4;

        //writing to grp2 pipe
        int val=write(grp2[1],&grp2_leader,4);
        if(val!=4) error(-1);
        
        //self terminating group process
        exit(0);
    }

    //we need to wait for both the group processes before we can continue
    int status;
    //waiting for the first group process
    waitpid(grp1_pid,&status,0);
    if(status!=0) error(-1);
    waitpid(grp2_pid,&status,0);
    if(status!=0) error(-1);
    
    //we read from grp1 pipe into finalTeam1 to get the first finalTeam
    int val=read(grp1[0],&finalTeam1,4);
    if(val!=4) error(-1);

    //read from grp2 pipe to get the second final team
    val=read(grp2[0],&finalTeam2,4);
    if(val!=4) error(-1);
}
