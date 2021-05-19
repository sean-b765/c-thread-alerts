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
#include <signal.h>  // capture Ctrl+C via signal()
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
struct alert _alerts[n_alerts];
char _display = 'y';
int _alert_count = 0;

const int MAX_NAME = 20;

/*
    Functions
*/
int create_alert(char name[], int mins)
{
    struct alert new_alert;
    new_alert.duration = mins * 60;
    new_alert.current_time = 0;
    strcpy(new_alert.name, name);
    new_alert.play_string = "play --no-show-progress -V0 -v 0.1 ./sounds/ring.mp3";
    new_alert.enabled = 'y';

    // Increment the counter and then add this alert to our global array
    if (_alert_count + 1 >= n_alerts)
    {
        return 0;
    }

    _alert_count = _alert_count + 1;

    _alerts[_alert_count - 1] = new_alert;
    return 1;
}

void list_alerts()
{
    for (int i = 0; i < _alert_count; i++)
    {
        printf("\n%i). [%s] - %i mins", i, _alerts[i].name, (_alerts[i].duration / 60));
    }
    printf("\n\n");
}

int find_alert(char name[])
{
    for (int i = 0; i < _alert_count; i++)
    {
        // Find the substring name param. within the alert name
        if (strstr(_alerts[i].name, name) != NULL)
        {
            return i;
        }
    }
    return -1;
}
int find_alert_via_index(int idx)
{
    if (idx > _alert_count)
    {
        return -1;
    }
    return idx;
}

// Copy the array to a new array, skipping the element at the given idx
int remove_index(int idx)
{
    // Decrement global counter
    _alert_count--;

    struct alert new_alerts[n_alerts];

    int newIdx = 0;
    for (int i = 0; i < _alert_count; i++)
    {
        // Skip the idx
        if (i == idx)
        {
            continue;
        }
        new_alerts[newIdx] = _alerts[newIdx];
        newIdx++;
    }

    // Copy new array into global array
    for (int i = 0; i < n_alerts; i++)
    {
        _alerts[i] = new_alerts[i];
    }
}

// Shows the menu
void show_menu()
{
    printf("\t\t<| MENU |>\n");
    printf("'1' | 'add'     - Add alert\n");
    printf("'2' | 'remove'  - Remove alert\n");
    printf("'3' | 'list'    - List alerts\n");
    printf("'4' | 'display' - Display mode\n");

    char choice[10] = {};
    scanf(" %s", choice);
    printf("\nYou chose: %s\n", choice);

    if (!strcmp(choice, "add") || !strcmp(choice, "1"))
    {
        // Add new alarm
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
        list_alerts();
        char input[MAX_NAME];

        printf("\nEnter the index). or [name] you'd like to remove:\n");
        scanf(" %s", input);

        int idx = atoi(input);
        int alertIdx;

        if (idx != 0)
        {
            // Use index
            alertIdx = find_alert_via_index(idx);
        }
        else
        {
            // Use the name
            alertIdx = find_alert(input);
        }

        if (alertIdx == -1)
        {
            printf("\nCould not find that one, sorry.\n");
            return;
        }

        remove_index(alertIdx);
    }
    else if (!strcmp(choice, "list") || !strcmp(choice, "3"))
    {
        list_alerts();
    }
    else if (!strcmp(choice, "display") || !strcmp(choice, "4"))
    {
        _display = 'y';
    }
    printf("\n\n");
}

// Display thread
void *display()
{
    while (1)
    {
        // If we didn't want to display
        if (_display != 'y')
        {
            show_menu();
            continue;
        }

        system("clear");
        for (int i = 0; i < _alert_count; i++)
        {
            // Check if the alert is enabled before printing anything to console
            if (_alerts[i].enabled != 'y')
            {
                continue;
            }

            // Calculate the percentage completion of the current alert
            double percent = (double)_alerts[i].current_time / _alerts[i].duration * 100;

            printf("[%s] progress: %f%c\t(Total: %is / %is)\n", _alerts[i].name, percent, '%', _alerts[i].current_time, _alerts[i].duration);
        }

        printf("(Ctrl+C = stop displaying)\n");

        sleep(1);
    }
}

// Update thread
void *update()
{
    while (1)
    {
        for (int i = 0; i < _alert_count; i++)
        {
            // Check if the alert is enabled before updating alerts
            if (_alerts[i].enabled != 'y')
            {
                continue;
            }

            _alerts[i].current_time++;

            // If it's time for an alert to show:
            if (_alerts[i].current_time == _alerts[i].duration)
            {
                // Play alert and set current_time to 0
                system(_alerts[i].play_string);
                _alerts[i].current_time = 0;
            }
        }
        sleep(1);
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

    pthread_t display_thread_id, update_thread_id;

    pthread_create(&display_thread_id, NULL, display, NULL);
    pthread_create(&update_thread_id, NULL, update, NULL);

    // Join a thread to halt the execution of main() thread
    // The update thread is always executing, whereas display thread can be temporarily disabled
    pthread_join(update_thread_id, NULL);

    printf("Thank you for using this app!\n");

    return 0;
}