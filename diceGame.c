//
// Created by Maxi on 7/27/20.
//

#include <signal.h>
#include <sys/time.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"

// player function, player name, playerId(?), fileDescriptor.
void player(char *name, int playerId, int fd);

// function that do noting for signal.
void action()
{
};


int main(int argc, char *argv[])
{
    // store the winning score, initial to 20
    int winningScore = 20;
    // display: "Please enter the winning score: "
    write(STDOUT_FILENO, "Please enter the winning score: \n", 33);
    // scan a integer from console and store it to winningScore.
    scanf("%d", &winningScore);
    // open or create, and clean the file sharedFile.bin.
    int fileDescriptor = open("sharedFile.bin", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    // if something wrong with open(), open() return -1
    if (fileDescriptor == -1)
    {
        // show an error message and exit.
        write(STDERR_FILENO, "open sharedFile.bin error!\n", 27);
        exit(1);
    }
    // the initial score.
    int initialScore = 0;
    // write three initial score to the file
    if (write(fileDescriptor, &initialScore, sizeof(initialScore)) == -1)
    {
        // error handle
        write(STDERR_FILENO, "write sharedFile.bin error!\n", 28);
        exit(1);
    }
    if (write(fileDescriptor, &initialScore, sizeof(initialScore)) == -1)
    {
        // error handle
        write(STDERR_FILENO, "write sharedFile.bin error!\n", 28);
        exit(1);
    }
    if (write(fileDescriptor, &initialScore, sizeof(initialScore)) == -1)
    {
        // error handle
        write(STDERR_FILENO, "write sharedFile.bin error!\n", 28);
        exit(1);
    }

    // create child process TATA
    int TATApid = fork();
    int TITIpid = 0;
    int TOTOpid = 0;
    // the child TATA will get in this condition and die in it.
    if (!TATApid)
    {
        // child TATA, player
        player("TATA", TATApid, fileDescriptor);
        exit(0);
    }
    // create child process TITI
    TITIpid = fork();
    // the child TITI will get in this condition and die in it.
    if (!TITIpid)
    {
        // chile TITI, player
        player("TITI", TITIpid, fileDescriptor);
        exit(0);
    }
    // create child process TOTO
    TOTOpid = fork();
    // the child TOTO will get in this condition and die in it.
    if (!TOTOpid)
    {
        // child TOTO, player
        player("TOTO", TOTOpid, fileDescriptor);
        exit(0);
    }
    // to wake up referee, set SIGUSR1 to run the function action() which do nothing.
    signal(SIGUSR1, action);
    // string array to store player names.
    char names[3][4] = {"TATA", "TITI", "TOTO"};
    // string to store players' total score.
    int scores[3] = {0, 0, 0};
    // index indicating which player is playing, 0 => TATA, 1 => TITI , 2 = TOTO.
    int index = 0; // start from TATA
    // referee
    while (1)
    {
        // choose the corresponding process to run according to index
        switch (index)
        {
            case 0: // when index is 0
                kill(TATApid, SIGUSR1); // It's TATA's turn
                break;
            case 1: // when index is 1
                kill(TITIpid, SIGUSR1); // It's TITI's turn
                break;
            case 2: // when index is 2
                kill(TOTOpid, SIGUSR1); // It's TOTO's turn
                break;
            default: // when there is no matching index
                // error handle.
                write(STDERR_FILENO, "switch index ERROR!\n", 13);
                exit(1);
        }
        // pause the referee.
        pause();
        // reset file stream position.
        switch (index)
        {
            case 0: // when index is 0
                lseek(fileDescriptor, 0, SEEK_SET);
                break;
            case 1: // when index is 1
                lseek(fileDescriptor, 4, SEEK_SET);
                break;
            case 2: // when index is 2
                lseek(fileDescriptor, 8, SEEK_SET);
                break;
            default:  // when there is no matching index
                // error handle.
                write(STDERR_FILENO, "switch index ERROR!\n", 13);
                exit(1);
        }
        // read score from sharedFile.bin
        if (read(fileDescriptor, &scores[index], sizeof(int)) == -1)
        {
            // error handle.
            write(STDERR_FILENO, "read sharedFile.bin error!\n", 27);
            exit(1);
        }
        // write Referee label to screen
        if (write(STDOUT_FILENO, "Referee: ", 9) == -1)
        {
            // error handle.
            write(STDERR_FILENO, "write to screen error!\n", 23);
            exit(1);
        }
        // write player name to screen
        if (write(STDOUT_FILENO, names[index], 4) == -1)
        {
            // error handle.
            write(STDERR_FILENO, "write to screen error!\n", 23);
            exit(1);
        }
        // write score to screen.
        printf(": Total so far %d\n\n", scores[index]);
        // when someone win.
        if (scores[index] >= winningScore)
        {
            // write the message "WINNER: "
            if (write(STDOUT_FILENO, "WINNER: ", 8) == -1)
            {
                // error handle.
                write(STDERR_FILENO, "write to screen error!\n", 23);
                exit(1);
            }
            // write the winner name.
            if (write(STDOUT_FILENO, names[index], 4) == -1)
            {
                // error handle.
                write(STDERR_FILENO, "write to screen error!\n", 23);
                exit(1);
            }
            // kill all the child processes and kill itself.
            kill(TATApid, SIGKILL); // kill TATA
            kill(TITIpid, SIGKILL); // kill TITI
            kill(TOTOpid, SIGKILL); // kill TOTO
            kill(getpid(), SIGKILL); // kill referee itself
        }
        // referee process sleep for 2 seconds.
        sleep(2);
        // next player
        index += 1;
        // when index is bigger or equal to 3, reset index and file position.
        if (index >= 3)
        {
            index = 0;
        }
    }
}

// player function, player name, playerId(?), fileDescriptor.
void player(char *name, int playerId, int fd)
{
    // store the total point obtained.
    int points;
    // store the number of dice.
    int dice;
    // to generate random number.
    long int ss = 0;
    while (1)
    {
        points = 0;
        dice = 0;
        // reset SIGUSR1 to function action() which do nothing but wake up child process.
        signal(SIGUSR1, action);
        // block the player.
        pause();
        // print to screen who is playing dice.
        printf("%s: playing my dice\n", name);
        // set a random seed.
        srand((unsigned) time(&ss));
        // get a random dice number in range [1,10].
        dice = rand() % 10 + 1;
        // print to screen dice point.
        printf("%s: got %d points\n", name, dice);
        // reset file stream position to the beginning of the file
        lseek(fd, 0, SEEK_SET);
        // when it's TATA playing.
        if (strcmp(name, "TATA") == 0)
        {
            // set file stream postion to the place of the beginning of TATA's score
            lseek(fd, 0, SEEK_SET);
            // read the total point from the file.
            read(fd, &points, sizeof(points));
            // add the dice number to the current point.
            points += dice;
            // set file stream postion to the place of the beginning of TATA's score
            lseek(fd, 0, SEEK_SET);
            // write the updated total point back to the file.
            write(fd, &points, sizeof(points));
        } // when it's TITI playing.
        else if (strcmp(name, "TITI") == 0)
        {
            // set file stream postion to the place of the beginning of TITI's score
            lseek(fd, 4, SEEK_SET);
            // read the total point from the file.
            read(fd, &points, sizeof(points));
            // add the dice number to the current point.
            points += dice;
            // set file stream postion to the place of the beginning of TITI's score
            lseek(fd, 4, SEEK_SET);
            // write the updated total point back to the file.
            write(fd, &points, sizeof(points));
        } // when it's TOTO playing.
        else if (strcmp(name, "TOTO") == 0)
        {
            // set file stream postion to the place of the beginning of TOTO's score
            lseek(fd, 8, SEEK_SET);
            // read the total point from the file.
            read(fd, &points, sizeof(points));
            // add the dice number to the current point.
            points += dice;
            // set file stream postion to the place of the beginning of TOTO's score
            lseek(fd, 8, SEEK_SET);
            // write the updated total point back to the file.
            write(fd, &points, sizeof(points));
        }
        else
        {
            // error handle.
            write(STDERR_FILENO, "getting player name ERROR!\n", 27);
        }
        // reset file stream position to the beginning of the file.
        lseek(fd, 0, SEEK_SET);
        // print player's total point so far.
//        printf("%s: Total so far %d\n\n", name, points);
        // sleep for 2 seconds.
        sleep(2);
        // wake up referee process.
        kill(getppid(), SIGUSR1);
    }
}