#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_GUESTS 100
#define MAX_DISHES 100
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 256
#define PORT 2000

// Global arrays for guests and dishes
char guests[MAX_GUESTS][MAX_NAME_LEN];
char dishes[MAX_DISHES][MAX_NAME_LEN];
int num_guests = 0, num_dishes = 0;

// Mutex for thread-safe file access
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

// Load data from a file into an array
void load_file(const char *filename, char array[][MAX_NAME_LEN], int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) && *count < MAX_GUESTS) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        strcpy(array[*count], line);
        (*count)++;
    }
    fclose(file);
}

// Check if a guest has already voted by searching votes.txt
int has_voted(const char *guest) {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen("votes.txt", "r");
    if (!file) {
        pthread_mutex_unlock(&file_mutex);
        return 0; // File doesn't exist yet, no votes
    }
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, guest) == 0) {
            fclose(file);
            pthread_mutex_unlock(&file_mutex);
            return 1; // Guest found in votes
        }
    }
    fclose(file);
    pthread_mutex_unlock(&file_mutex);
    return 0;
}

// Record a vote in votes.txt
void record_vote(const char *guest, const char *dish, int preference) {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen("votes.txt", "a");
    if (!file) {
        perror("Error opening votes file");
        exit(1);
    }
    fprintf(file, "%s,%s,%d\n", guest, dish, preference);
    fclose(file);
    pthread_mutex_unlock(&file_mutex);
}

// Calculate and format the top 3 dishes based on total preference points
void calculate_results(char *result_str) {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen("votes.txt", "r");
    if (!file) {
        strcpy(result_str, "No votes yet.\n");
        pthread_mutex_unlock(&file_mutex);
        return;
    }

    // Initialize points for each dish
    int points[MAX_DISHES] = {0};
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        strtok(line, ","); // Skip guest
        char *dish = strtok(NULL, ",");
        char *pref_str = strtok(NULL, ",");
        int pref = atoi(pref_str);
        for (int i = 0; i < num_dishes; i++) {
            if (strcmp(dishes[i], dish) == 0) {
                points[i] += pref;
                break;
            }
        }
    }
    fclose(file);
    pthread_mutex_unlock(&file_mutex);

    // Structure to hold dish and points for sorting
    struct Dish {
        char name[MAX_NAME_LEN];
        int points;
    } dish_points[MAX_DISHES];

    // Populate dish_points
    for (int i = 0; i < num_dishes; i++) {
        strcpy(dish_points[i].name, dishes[i]);
        dish_points[i].points = points[i];
    }

    // Simple bubble sort to find top 3 (descending order)
    for (int i = 0; i < num_dishes - 1; i++) {
        for (int j = 0; j < num_dishes - i - 1; j++) {
            if (dish_points[j].points < dish_points[j + 1].points) {
                struct Dish temp = dish_points[j];
                dish_points[j] = dish_points[j + 1];
                dish_points[j + 1] = temp;
            }
        }
    }

    // Format top 3 results
    sprintf(result_str, "Top 3 dishes:\n1. %s (%d points)\n2. %s (%d points)\n3. %s (%d points)\n",
            dish_points[0].name, dish_points[0].points,
            dish_points[1].name, dish_points[1].points,
            dish_points[2].name, dish_points[2].points);
}

// Thread function to handle each client
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);
    char buffer[MAX_LINE_LEN];
    char client_name[MAX_NAME_LEN];

    // Receive client's name
    recv(client_sock, buffer, sizeof(buffer), 0);
    sscanf(buffer, "%s", client_name);

    // Authenticate against guest list
    int is_guest = 0;
    for (int i = 0; i < num_guests; i++) {
        if (strcmp(guests[i], client_name) == 0) {
            is_guest = 1;
            break;
        }
    }
    if (!is_guest) {
        send(client_sock, "Invalid name.\n", 14, 0);
        close(client_sock);
        return NULL;
    }

    // Check if client has already voted
    if (has_voted(client_name)) {
        send(client_sock, "You have already voted.\n1. See results\n2. Exit\n", 47, 0);
        while (1) {
            recv(client_sock, buffer, sizeof(buffer), 0);
            if (strncmp(buffer, "1", 1) == 0) { // See results
                char result_str[MAX_LINE_LEN];
                calculate_results(result_str);
                send(client_sock, result_str, strlen(result_str), 0);
            } else if (strncmp(buffer, "2", 1) == 0) { // Exit
                break;
            }
        }
    } else {
        // Welcome and send dish list
        send(client_sock, "Welcome, you can vote now.\n", 27, 0);
        char dish_list[1024] = "";
        for (int i = 0; i < num_dishes; i++) {
            char temp[MAX_NAME_LEN];
            sprintf(temp, "%d. %s\n", i + 1, dishes[i]);
            strcat(dish_list, temp);
        }
        send(client_sock, dish_list, strlen(dish_list), 0);

        // Voting loop
        char voted_dishes[MAX_DISHES][MAX_NAME_LEN];
        int voted_count = 0;
        while (1) {
            recv(client_sock, buffer, sizeof(buffer), 0);
            if (strncmp(buffer, "done", 4) == 0) {
                break;
            }
            int dish_num, preference;
            if (sscanf(buffer, "%d %d", &dish_num, &preference) != 2 ||
                dish_num < 1 || dish_num > num_dishes || preference < 1 || preference > 5) {
                send(client_sock, "Invalid vote.\n", 14, 0);
                continue;
            }
            char *dish = dishes[dish_num - 1];
            // Check for duplicate votes in session
            int already_voted = 0;
            for (int i = 0; i < voted_count; i++) {
                if (strcmp(voted_dishes[i], dish) == 0) {
                    already_voted = 1;
                    break;
                }
            }
            if (already_voted) {
                send(client_sock, "You have already voted for this dish.\n", 38, 0);
            } else {
                record_vote(client_name, dish, preference);
                strcpy(voted_dishes[voted_count], dish);
                voted_count++;
                send(client_sock, "Vote recorded.\n", 15, 0);
            }
        }

        // Post-voting menu
        send(client_sock, "Voting complete.\n1. See results\n2. Exit\n", 41, 0);
        while (1) {
            recv(client_sock, buffer, sizeof(buffer), 0);
            if (strncmp(buffer, "1", 1) == 0) { // See results
                char result_str[MAX_LINE_LEN];
                calculate_results(result_str);
                send(client_sock, result_str, strlen(result_str), 0);
            } else if (strncmp(buffer, "2", 1) == 0) { // Exit
                break;
            }
        }
    }
    close(client_sock);
    return NULL;
}

int main() {
    // Load guest and dish lists
    load_file("guests.txt", guests, &num_guests);
    load_file("dishes.txt", dishes, &num_dishes);

    // Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Could not create socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    // Bind and listen
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        return 1;
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept clients and spawn threads
    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, NULL, NULL);
        if (*client_sock < 0) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_sock) != 0) {
            perror("Thread creation failed");
            close(*client_sock);
            free(client_sock);
        } else {
            pthread_detach(thread); // Detach thread to clean up automatically
        }
    }
    close(server_sock);
    return 0;
}