/*

    Author: Sean B
    Date: May 2021

    Name: timrz
    Description: First C Project

    Uses a thread to update all alerts' current_time and compare this with duration,
    a thread which displays all alerts,
    and a thread to show the menu when not displaying

*/

#include <stdio.h>   // printf, scanf
#include <unistd.h>  // sleep
#include <stdlib.h>  // system
#include <pthread.h> // pthread
#include <signal.h>  // capture Ctrl+C
#include <string.h>  // strcmp

/*
    Definitions
*/

#define n_alerts 20

struct alert
{
    char *play_string;
    char name[20];
    unsigned int duration;
    unsigned int current_time;
    char enabled;
};

/*
    Global variables
*/
struct alert alerts[n_alerts];
char _display = 'y';
int alert_count = 0;

/*
    Functions
*/
int create_alert(char name[], int mins)
{
    struct alert _alert;
    _alert.duration = mins * 60;
    _alert.current_time = 0;
    strcpy(_alert.name, name);
    _alert.play_string = "play --no-show-progress -V0 -v 0.1 ./sounds/ring.mp3";
    _alert.enabled = 'y';

    // Increment the counter and then add this alert to our global array
    if (alert_count + 1 >= n_alerts)
    {
        return 0;
    }

    alert_count = alert_count + 1;

    alerts[alert_count - 1] = _alert;
    return 1;
}

void *display()
{
    while (1)
    {
        // If we didn't want to display
        if (_display != 'y')
        {
            continue;
        }

        system("clear");
        for (int i = 0; i < alert_count; i++)
        {
            // Check if the alert is enabled before printing anything to console
            if (alerts[i].enabled != 'y')
            {
                continue;
            }

            // Calculate the percentage completion of the current alert
            double percent = (double)alerts[i].current_time / alerts[i].duration * 100;

            printf("[%s] progress: %f%c\t(Total: %is / %is)\n", alerts[i].name, percent, '%', alerts[i].current_time, alerts[i].duration);
        }

        printf("(Ctrl+C = stop displaying)\n");

        sleep(1);
    }
}

void *update()
{
    while (1)
    {
        for (int i = 0; i < alert_count; i++)
        {
            // Check if the alert is enabled before updating alerts
            if (alerts[i].enabled != 'y')
            {
                continue;
            }

            alerts[i].current_time++;

            // If it's time for an alert to show:
            if (alerts[i].current_time == alerts[i].duration)
            {
                // Play alert and set current_time to 0
                system(alerts[i].play_string);
                alerts[i].current_time = 0;
            }
        }
        sleep(1);
    }
}

void list_alerts()
{
    for (int i = 0; i < alert_count; i++)
    {
        printf("\n%i). [%s] - %i mins", i, alerts[i].name, (alerts[i].duration / 60));
    }
}

void find_alert(char name[])
{
}

void *menu()
{
    while (1)
    {
        if (_display != 'n')
        {
            continue;
        }

        printf("\n\t\t<| MENU |>\n");
        printf("1 / 'add'     | Add alert\n");
        printf("2 / 'remove'  | Remove alert\n");
        printf("3 / 'list'    | List alerts\n");
        printf("4 / 'display' | Display mode\n");

        char choice[10] = {};
        scanf(" %s", choice);
        printf("\nYou chose: %s\n", choice);

        if (!strcmp(choice, "add") || !strcmp(choice, "1"))
        {
            // Add new alarm
            const int MAX_NAME = 20;
            int mins = 0;
            char name[MAX_NAME];

            // Get the name
            printf("What's the name of this alert? > ");
            scanf(" %19[^\n]s", name);

            // Get the mins duration
            printf("\n[%s] How many minutes between each alert? > ", name);
            scanf(" %i", &mins);

            printf("\nYour alert has been created!\n");

            if (create_alert(name, mins) == 1)
            {
                printf("\nAlert created!\n");
            }
            else
            {
                printf("\nError creating alert (You have reached the maximum)\n");
            }
        }
        else if (!strcmp(choice, "remove") || !strcmp(choice, "2"))
        {
        }
        else if (!strcmp(choice, "list") || !strcmp(choice, "3"))
        {
            list_alerts();
        }
        else if (!strcmp(choice, "display") || !strcmp(choice, "4"))
        {
            _display = 'y';
        }
    }
}

/*
    Capctures Ctrl+C
*/
void handle_sigint(int dummy)
{
    system("clear");

    if (_display == 'n')
    {
        exit(0);
    }
    _display = 'n';
}

/*
    Entry-point
*/
int main()
{
    signal(SIGINT, handle_sigint);

    // Initial alert(s)
    create_alert("Default Alert", 20);

    pthread_t display_thread_id, update_thread_id, menu_thread_id;

    pthread_create(&display_thread_id, NULL, display, NULL);
    pthread_create(&update_thread_id, NULL, update, NULL);
    pthread_create(&menu_thread_id, NULL, menu, NULL);

    // Join a thread to halt the execution of main() thread
    // The update thread is always executing, whereas display thread can be temporarily disabled
    pthread_join(update_thread_id, NULL);

    printf("Thank you for using this app!\n");

    return 0;
}