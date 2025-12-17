/*
 * chest_uci -  For Copyright information read file "readme.md".
 *
 * (C) Jens Nissen 2025
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_SIZE 1024
#define CMD_FILE  "chest_input.txt"
#define OUT_FILE  "chest_output.txt"

#define VERSION "CHEST_UCI 1.0"
#define COPYRIGHT "(C) Jens Nissen 2025"
#define DCHEST "./dchest"

/* path to chest executable */
char dchest[260] = DCHEST;

static char current_fen[LINE_SIZE] = {0};
static char job_type[32] = {0};
static char depth[32] = {0};

void chomp(char *s) {
    s[strcspn(s, "\r\n")] = 0;
}

/* write FEN as EPD */
void write_chest_input(const char *fen)
{
    FILE *f = fopen(CMD_FILE, "w");
    if (!f) return;

    char board[128], side;

    sscanf(fen, "%127[^ ] %c", board, &side);

    fprintf(f, "LE\n");
    fprintf(f, "j%s\n", job_type);
    fprintf(f, "F %s\n", current_fen);
    fprintf(f, "z%s%c\n", depth, side);
    fprintf(f, "..\n");

    fclose(f);
}

/* run chest and capture output */
void run_chest(void) {
    char cmd[LINE_SIZE];
    snprintf(cmd, sizeof(cmd), "%s %s > %s", dchest, CMD_FILE, OUT_FILE);
    system(cmd);
}

int parse_mate_length(const char *line)
{
    int n;
    if (sscanf(line, "Solution (in %d moves):", &n) == 1)
        return n;
    return -1;
}

int check_no_solution(const char *line)
{
    if (line == strstr(line, "No Solution"))
        return 1;
    return 0;
}

int parse_chest_move(const char *line, char *uci)
{
	int first = 1;
	char* start = uci;
	while (*line++)
	{
		if (isalnum(*line)) 
		{
			if (first && isupper(*line))
			{
				first = 0;
				continue;
			}
			first = 0;
			*uci++ = *line;
		}
	}
	*uci = 0;
	return strlen(start);
}

int parse_time_ms(const char *line)
{
    const char *p;
    double sec;

	if (line != strstr(line, "Time")) return -1;

    /* find '=' */
    p = strchr(line, '=');
    if (!p)
        return -1;

    /* scan number after '=' */
    if (sscanf(p + 1, " %lf", &sec) == 1)
    {
        int n = (int)(sec * 1000.0 + 0.5);
        if (n==0) n = 1;
        return n;
	}
    return -1;
}

/* extract first move from chest output */
void extract_bestmove()
{
    FILE *f = fopen(OUT_FILE, "r");
    char line[LINE_SIZE];

    if (!f) return;

    int mate = -1;
    int time_ms = 0;
    char bestmove[LINE_SIZE] = "";
    int n = 1;
    fpos_t setPoint = 0;

    /* First pass: find mate length and time */
    while (fgets(line, sizeof(line), f)) {

        if (check_no_solution(line))
            break;
        
        if (mate < 0) {
            mate = parse_mate_length(line);
            fgetpos(f, &setPoint);
            continue;
        }

        int t = parse_time_ms(line);
        if (t >= 0) {
            time_ms = t;
            break;
        }
    }

    fsetpos(f, &setPoint);

    /* Second pass: parse moves */
    if (mate>0) {
    	while (fgets(line, sizeof(line), f)) {
        
       		if (line[0] != '\t')
            	break;

        	if (parse_chest_move(line, bestmove)) {
            	printf(
            	    "info depth %d nodes 1 time %d score mate %d multipv %d pv %s\n",
         	        mate,
                	time_ms,
            	    mate,
        	        n,
    	            bestmove
	            );

            	n++;
        	}
    	}
	}		
    fclose(f);

    if (bestmove[0]) printf("bestmove %s\n", bestmove);
}



int main(void) {
    char line[LINE_SIZE];
    char bestmove[32];

    setbuf(stdout, NULL);

    while (fgets(line, sizeof(line), stdin)) {
        chomp(line);

        if (!strcmp(line, "uci")) {
        	printf("id name %s\n", VERSION);
            printf("id author %s\n", COPYRIGHT);
            /* Job option */
			printf("option name job type combo default o var o var O var s var S var h var H\n");
			printf("option name backend type string default \"%s\"\n", DCHEST);
            printf("uciok\n");
        }
        else if (!strcmp(line, "isready")) {
            printf("readyok\n");
        }
        else if (!strncmp(line, "position fen ", 13)) {
            strcpy(current_fen, line + 13);
        }
         /* Job option */
    	else if (line == strstr(line, "setoption name job ")) {
        	const char *v = strstr(line, "value ");
        	if (!v) continue;
        	v += 6;
	        strncpy(job_type, v, sizeof(job_type) - 1);
            job_type[sizeof(job_type) - 1] = '\0';
    	}
    	else if (line == strstr(line, "setoption name backend ")) {
        	const char *v = strstr(line, "value ");
        	if (!v) continue;
        	v += 7; // 6+1
	        strncpy(dchest, v, sizeof(dchest) - 1);
            dchest[sizeof(dchest) - 1] = '\0';
            dchest[strlen(dchest) - 1] = '\0'; // Remove trailing '\"'
    	}
        else if (line == strstr(line, "go depth ")) {
        	
        	strncpy(depth, line+9, sizeof(depth)-1);
            depth[sizeof(depth)-1] = 0;
            write_chest_input(current_fen);
            run_chest();

            extract_bestmove();
        }
        else if (!strcmp(line, "quit")) {
            break;
        }
    }
    return 0;
}
