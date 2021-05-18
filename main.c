/*

    Author: Sean B
    Date: May 2021

    Name: alertc
    Description: First C Project

*/

#include <stdio.h>  // printf, scanf
#include <unistd.h> // sleep
#include <stdlib.h> // system

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
    _alert.duration = mins * 60;
    _alert.play_string = "play -v 0.1 ./sounds/ring.mp3";
    _alert.enabled = 'y';

    // Increment the counter variable
    alert_count = alert_count + 1;
    // Add the alert to alerts
    alerts[alert_count - 1] = _alert;
}

void display()
{
    while (1)
    {
        // If we didn't want to display
        if (_display != 'y')
        {
            return;
        }

        for (int i = 0; i < alert_count; i++)
        {
            // Check if the alert is enabled before printing anything to console
            if (alerts[i].enabled != 'y')
            {
                return;
            }

            // Calculate the percentage completion of the current alert
            double percent = (double)alerts[i].current_time / alerts[i].duration * 100;

            printf("\nProgress: %f%c\nTotal: %is / %is", percent, '%', alerts[i].current_time, alerts[i].duration);
        }
        sleep(1);
    }
}

void update()
{
    while (1)
    {
        for (int i = 0; i < alert_count; i++)
        {
            // Check if the alert is enabled before updating alerts
            if (alerts[i].enabled != 'y')
            {
                return;
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

/*
    Entry-point
*/
int main()
{
    create_alert(20);

    display();

    return 0;
}