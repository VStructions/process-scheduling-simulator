/*
 * Process_Scheduling_Simulator.c
 *
 *  Created on: Jan 5, 2019
 *      Author: VStructions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define prnol 7
char filename[30];

typedef struct processes
{
	char prno[prnol];
	int arrivalt;
	int servicet;
	int waitt;
	int prty;
	int ran;
	int redservicet; //reducted service time (for Preemptive policies)
}Process;

int File_Checker_And_Line_counter();
void File_Loader(Process *process, int count);
int Policy_Selector();
int First_Arrival_Time(Process *process, int count);
void Non_Preemptive_Shortest_Job_First(Process *process, int count);
void Preemptive_Shortest_Job_First(Process *process, int count);
void Preemptive_Priority_Scheduling(Process *process, int count);
void Round_Robin_q(Process *process, int count);

int main()
{
	Process *process;
	int count;

	printf("Program: Process Scheduling Simulator\nAuthor: VStructions\n\n\n");

	count = File_Checker_And_Line_counter();

	process = (Process *)malloc(count * sizeof(Process));
	if (process == NULL)
	{
		printf("Memory allocation error");
		exit(-1);
	}

	File_Loader (process, count);

	switch(Policy_Selector())
	{
		case 1:
			  {
				  Non_Preemptive_Shortest_Job_First(process, count);
				  break;
			  }
		case 2:
			  {
				  Preemptive_Shortest_Job_First(process, count);
				  break;
			  }
		case 3:
			  {
				  Preemptive_Priority_Scheduling(process, count);
				  break;
			  }
		case 4:
			  {
				  Round_Robin_q(process, count);
				  break;
			  }
	}

	free(process);

	getchar();

	return 0;
}


int File_Checker_And_Line_counter()
{
	FILE *fp;
	int  blanklineready=1, count=0, ext=0;
	char ch;

	printf("Enter file name (.txt files only): ");
	scanf("%s", filename);
	printf("\n");

	fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("File unavailable.");
		getchar();
		getchar();
		exit(-1);
	}

	while ((ch = fgetc(fp)) != '\n')
	if (ch == EOF)
	{
		printf("No process data in file.");
		getchar();
		getchar();
		exit(-1);
	}

	while( ext != 1 )
	{
		blanklineready = 1;
		while ((ch = fgetc(fp)) != '\n')
		{
			if (ch == EOF)
			{
				ext = 1;
				break;
			}
			if( ch != ' ' && ch != '\n' && ch != '\r' )
				blanklineready = 0;
		}
		count++;
		if (blanklineready == 1)
			count--;
	}
	fclose(fp);

	return count;
}

void File_Loader (Process *process, int count)
{
	FILE *fp;
	int i;
	char ch;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("File unavailable.");
		exit(-1);
	}

	while ((ch = fgetc(fp)) != '\n')
		if(ch == EOF)
			break;

	for(i=0; i<count;i++)
	{
		fscanf(fp,"%s %d %d %d", process[i].prno, &(process[i].arrivalt), &(process[i].servicet), &(process[i].prty));
	}
}

int Policy_Selector()
{
	int choice=0;

	while (1)
	{
		printf("Select a process scheduling policy:\n\n");
		printf("1: Non Preemptive Shortest Job First.\n");
		printf("2: Preemptive Shortest Job First.\n");
		printf("3: Preemptive Priority Scheduling.\n");
		printf("4: Round Robin q.\n");
		scanf("%d", &choice);
		fflush(stdin);

		if( choice > 4 || choice < 1 )
		{
			printf("Invalid policy selected\n\n");
			choice = 0;
			continue;
		}
		return choice;
	}
}

int First_Arrival_Time(Process *process, int count)
{
	int i, FAt=999999999;

	for (i=0; i<count; i++)
		if (process[i].arrivalt < FAt)
			FAt = process[i].arrivalt;

	return FAt;
}

void Non_Preemptive_Shortest_Job_First(Process *process, int count)
{
	int i, j, k, time, loop_start_time, isidle, tottime=0, MTTA=0, *MTTm; //Mean Turnaround Time Accumulator/members
	Process temp;

	for(i=0; i < count-1; i++)
		for(j=0; j < count-i-1; j++)
			if( process[j].servicet > process[j+1].servicet )
			{
				temp = process[j+1];
				process[j+1] = process[j];
				process[j] = temp;
			}
	
	for(i=0; i<count; i++)
	{
		process[i].ran = process[i].waitt = 0;
		tottime += process[i].servicet;
	}

	time = First_Arrival_Time(process, count);

	MTTm = (int*)malloc(count * sizeof(int));

	printf("Gantt D: ");

	while(tottime > time)
	{
		isidle = 1;
		for(i=0; i<count; i++)
		{
			if( process[i].arrivalt <= time && process[i].ran == 0 )
			{
				loop_start_time = time;
				for(j=0; j<process[i].servicet; j++)
				{
					printf("%s,", process[i].prno);
					time++;
				}
				process[i].ran = 1;

				for( j=loop_start_time; j<time; j++ )
					for ( k=0; k<count; k++ )
						if (process[k].arrivalt <= j && process[k].ran != 1)
							process[k].waitt++;

				MTTm[i] = process[i].servicet + process[i].waitt;
				isidle = 0;
				break;
			}
		}
		if( isidle == 1 )
		{
			printf("-,");
			tottime++;
			time++;
		}
	}
	
	printf("\n\nMean Turnaround Time: ( %d ", MTTm[0]);
	MTTA += MTTm[0];
	for(i=1; i<count; i++)
	{
		printf("+ %d ", MTTm[i]);
		MTTA += MTTm[i];
	}
	printf(") / %d = %.2f", count, (float)MTTA/(float)count);

	free(MTTm);
}

void Preemptive_Shortest_Job_First(Process *process, int count)
{
	int i, j, time, isidle, only_check_for_waittime, tottime=0, MTTA=0, *MTTm; //Mean Turnaround Time Accumulator/members
	Process temp;

	for(i=0; i < count-1; i++)
		for(j=0; j < count-i-1; j++)
			if( process[j].servicet > process[j+1].servicet )
			{
				temp = process[j+1];
				process[j+1] = process[j];
				process[j] = temp;
			}
	
	for(i=0; i<count; i++)
	{
		process[i].waitt = 0;
		tottime += process[i].servicet;
		process[i].redservicet = process[i].servicet;
	}
	time = First_Arrival_Time(process, count);

	MTTm = (int*)malloc(count * sizeof(int));

	printf("Gantt D.: ");

	while(tottime > time)
	{
		only_check_for_waittime = 0;
		isidle = 1;
		for(i=0; i<count; i++)
		{
			if( process[i].arrivalt <= time && process[i].redservicet > 0 && only_check_for_waittime == 0)
			{
				printf("%s,", process[i].prno);

				process[i].redservicet--;
				if (i > 0)
				{
					j = i-1;
					while(j >= 0 && process[i].redservicet == process[j].redservicet)
						j--;
					
					if( j == 0 )
					{
						temp = process[i];
						process[i] = process[j];
						process[j] = temp;
					}
					else 
					{
						temp = process[i];
						process[i] = process[j+1];
						process[j+1] = temp;
					}
				}

				only_check_for_waittime = 1;
				isidle = 0;
			}
			else if (process[i].arrivalt <= time && process[i].redservicet > 0 )
			{
				process[i].waitt++;
			}
		}
		if( isidle == 1 )
		{
			printf("-,");
			tottime++;
		}
		time++;
	}
	for(i=0; i<count; i++)
	{
		MTTm[i] = process[i].servicet + process[i].waitt;
	}

	printf("\n\nMean Turnaround Time: ( %d ", MTTm[0]);
	MTTA += MTTm[0];
	for(i=1; i<count; i++)
	{
		printf("+ %d ", MTTm[i]);
		MTTA += MTTm[i];
	}
	printf(") / %d = %.2f", count, (float)MTTA/(float)count);

	free(MTTm);
}

void Preemptive_Priority_Scheduling(Process *process, int count)
{
	int i, j, time, isidle, only_check_for_waittime, tottime=0, MTTA=0, *MTTm; //Mean Turnaround Time Accumulator/members
	Process temp;

	for(i=0; i < count-1; i++)
		for(j=0; j < count-i-1; j++)
			if( process[j].prty < process[j+1].prty )
			{
				temp = process[j+1];
				process[j+1] = process[j];
				process[j] = temp;
			}
	
	for(i=0; i<count; i++)
	{
		process[i].waitt = 0;
		tottime += process[i].servicet;
		process[i].redservicet = process[i].servicet;
	}

	time = First_Arrival_Time(process, count);

	MTTm = (int*)malloc(count * sizeof(int));

	printf("Gantt D.: ");

	while(tottime > time)
	{
		only_check_for_waittime = 0;
		isidle = 1;
		for(i=0; i<count; i++)
		{
			if( process[i].arrivalt <= time && process[i].redservicet > 0 && only_check_for_waittime == 0)
			{
				printf("%s,", process[i].prno);

				MTTm[i] = process[i].servicet + process[i].waitt;
				process[i].redservicet--;
				only_check_for_waittime = 1;
				isidle = 0;
			}
			else if (process[i].arrivalt <= time && process[i].redservicet > 0 )
			{
				process[i].waitt++;
			}
		}
		if( isidle == 1 )
		{
			printf("-,");
			tottime++;
		}
		time++;
	}
	printf("\n\nMean Turnaround Time: ( %d ", MTTm[0]);
	MTTA += MTTm[0];
	for(i=1; i<count; i++)
	{
		printf("+ %d ", MTTm[i]);
		MTTA += MTTm[i];
	}
	printf(") / %d = %.2f", count, (float)MTTA/(float)count);

	free(MTTm);
}

void Round_Robin_q(Process *process, int count)
{
	int i, j, k, time, quantum=-1, loop_start_time, isidle, tottime=0, MTTA, *MTTm; //Mean Turnaround Time Accumulator/members
	Process temp;

	while( quantum == -1 )
	{
		printf("Choose time quantum: ");
		scanf("%d", &quantum);
		fflush(stdin);
		
		if( quantum < 1 )
		{
			printf("Invalid quantum value entered\n\n");
			quantum = -1;
			continue;
		}
		break;
	}

	for(i=0; i < count-1; i++)
		for(j=0; j < count-i-1; j++)
			if( process[j].arrivalt > process [j+1].arrivalt )
			{
				temp = process[j+1];
				process[j+1] = process[j];
				process[j] = temp;
			}

	for(i=0; i<count; i++)
	{
		process[i].waitt = 0;
		tottime += process[i].servicet;
		process[i].redservicet = process[i].servicet;
	}
	time = First_Arrival_Time(process, count);

	MTTm = (int*)malloc(count * sizeof(int));

	printf("Gantt D.: ");

	while(tottime > time)
	{
		isidle = 1;
		for(i=0; i<count; i++)
		{
			if( process[i].arrivalt <= time && process[i].redservicet > 0 )
			{
				loop_start_time = time;
				if( quantum > process[i].redservicet)
				{
					for(; process[i].redservicet > 0; process[i].redservicet--)
					{
						printf("%s,", process[i].prno);
						time++;
					}
				}
				else
				{
					for(j=quantum; j > 0; j--)
					{
						printf("%s,", process[i].prno);
						process[i].redservicet--;
						time++;
					}
				}

				for ( j=loop_start_time; j<time; j++ )
					for ( k=0; k<count; k++ )
						if (process[k].arrivalt <= j && process[k].redservicet > 0 && k != i )
							process[k].waitt++;

				isidle = 0;
			}
		}
		if( isidle == 1 )
		{
			printf("-,");
			tottime++;
			time++;
		}
	}
	for(i=0; i<count; i++)
	{
		MTTm[i] = process[i].servicet + process[i].waitt;
	}
	printf("\n\nMean Turnaround Time: ( %d ", MTTm[0]);
	MTTA += MTTm[0];
	for(i=1; i<count; i++)
	{
		printf("+ %d ", MTTm[i]);
		MTTA += MTTm[i];
	}
	printf(") / %d = %.2f", count, (float)MTTA/(float)count);

	free(MTTm);
}
