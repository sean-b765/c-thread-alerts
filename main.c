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

/*
    Definitions
*/

#define n_alerts 20

struct alert
{
    char *play_string;
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
struct alert create_alert(int mins)
{
    struct alert _alert;
    _alert.duration = mins;
    _alert.play_string = "play -v 0.1 ./sounds/ring.mp3";
    _alert.enabled = 'y';

    // Increment the counter and then add this alert to our global array
    alert_count = alert_count + 1;

    alerts[alert_count - 1] = _alert;
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

        for (int i = 0; i < alert_count; i++)
        {
            // Check if the alert is enabled before printing anything to console
            if (alerts[i].enabled != 'y')
            {
                continue;
            }

            system("clear");

            // Calculate the percentage completion of the current alert
            double percent = (double)alerts[i].current_time / alerts[i].duration * 100;

            printf("Alarm [%i] progress: %f%c\t(Total: %is / %is)\n", i, percent, '%', alerts[i].current_time, alerts[i].duration);
        }

        printf("(Enter key --> stop displaying)\n");

        char *c;

        // Stop display thread
        if (c == "x" || c == "exit")
        {
            system("clear");
            _display = 'n';
            break;
        }

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
                system("clear");
                alerts[i].current_time = 0;
            }
        }
        sleep(1);
    }
}

void *menu()
{
    while (1)
    {
        if (_display != 'n')
        {
            continue;
        }

        printf("\t\t<| MENU |>\n");

        char choice;
        scanf("%c", &choice);
        printf("You chose: %c\n", choice);
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
    create_alert(20);

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