#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#define MAX_TURN 100

const enum state { ROCK = 0,
                   PAPER = 1,
                   SCISSORS = 2 };

const enum winner { PROCESS_ZERO = 0,
                    PROCESS_ONE = 1,
                    EQUALS = -1 };

int find_winner_of_turn(int process_zero, int process_one)
{
    int winner_process;

    if (process_zero == process_one)
    {
        winner_process = EQUALS;
    }
    else if (process_zero == ROCK)
    {
        switch (process_one)
        {
        case PAPER:
            winner_process = PROCESS_ONE;
            break;
        case SCISSORS:
            winner_process = PROCESS_ZERO;
            break;
        }
    }
    else if (process_zero == PAPER)
    {
        switch (process_one)
        {
        case ROCK:
            winner_process = PROCESS_ZERO;
            break;
        case SCISSORS:
            winner_process = PROCESS_ONE;
        }
    }
    else if (process_zero = SCISSORS)
    {
        switch (process_one)
        {
        case ROCK:
            winner_process = PROCESS_ONE;
            break;

        case PAPER:
            winner_process = PROCESS_ZERO;
            break;
        }
    }
    return winner_process;
}

char *string_matching(int state)
{
    char *p;
    switch (state)
    {
    case ROCK:
        p = "ROCK";
        break;
    case PAPER:
        p = "PAPER";
        break;
    case SCISSORS:
        p = "SCISSORS";
        break;
    }

    return p;
}

int main()
{
    int SHOULD_CONTINUE = 1;
    int number_of_process;
    int process_rank;
    int process_zero_score = 0;
    int process_one_score = 0;
    int turn = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    if (process_rank != 0)
    {
        srand(time(NULL) + process_rank);
        int choice;
        int winner;
        while (SHOULD_CONTINUE != 0)
        {
            choice = rand() % 3;
            MPI_Send(&choice, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&winner, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          
            switch (winner)
            {
                case PROCESS_ZERO:
                    process_zero_score += 1;
                    break;
                case PROCESS_ONE:
                    process_one_score += 1;
                    break;
            }
            turn++;
            if (process_one_score == 5 || process_zero_score == 5)
            {
                SHOULD_CONTINUE = 0;
            }
        }
    }
    else
    {
        srand(time(NULL) + process_rank);
        printf("The game starts\n");
        int process_one_choice;
        int choice;
        int result;
        while (SHOULD_CONTINUE != 0)
        {
            MPI_Recv(&process_one_choice, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            choice = rand() % 3;
            result = find_winner_of_turn(choice, process_one_choice);
            turn++;
            printf("\nTurn %d, Process-0: %s, Process-1: %s\n", turn, string_matching(choice), string_matching(process_one_choice));
            if (result == EQUALS)
            {
                printf("Draw, Score: %d-%d\n", process_zero_score, process_one_score);
            }
            else if (result == PROCESS_ONE)
            {
                process_one_score += 1;
                printf("Process-1 win, Score: %d-%d\n", process_zero_score, process_one_score);
            }
            else if (result == PROCESS_ZERO)
            {
                process_zero_score += 1;
                printf("Process-0 win, Score: %d-%d\n", process_zero_score, process_one_score);
            }
            MPI_Send(&result, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD);

            if (process_one_score == 5 || process_zero_score == 5)
            {
                SHOULD_CONTINUE = 0;
                printf("\nProcess-%d has won the game with score: %d - %d in %d Turns\nThe game ends\n",process_zero_score > process_one_score ? 0 : 1, process_zero_score, process_one_score, turn);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
