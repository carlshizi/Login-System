/*
Kola Ladipo
*/

#include <stdio.h>
#include <string.h>
#include "hashmap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct hashmap *map;
#define LIMIT 3


// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}
  
// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
  
// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}


// Hashmap utility begins...
struct user {
    char *username;
    char *password;
};

int user_compare(const void *a, const void *b, void *udata) {
    const struct user *ua = a;
    const struct user *ub = b;
    return strcmp(ua->username, ub->username);
}

uint64_t user_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct user *user = item;
    return hashmap_sip(user->username, strlen(user->username), seed0, seed1);
}
// Hashmap utility ends...


/*
Used to extract username and password from char array passed from
LoginsAndPasswords.txt. The function takes the extracted username/password
combination, and stores it in a hashmap (Line 92)
*/
void extract(char line[]){
    char delimiter[] = " \r\n";
    char *user, *pw, *context;

    int lineLength = strlen(line);
    char *lineCopy = (char*) calloc(lineLength + 1, sizeof(char));
    strncpy(lineCopy, line, lineLength);

    user = strtok_r (lineCopy, delimiter, &context);  // username
    strtok (user,"@");
    pw = strtok_r (NULL, delimiter, &context);  // password

  hashmap_set(map, &(struct user){ .username=user, .password=pw });
  }


int main() {

    int i = 0;
    char name[32] = "";
    char separator = ' ';
    bool accountFound = false;
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
  
    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);
  
    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));
    
    // Create a new hash map where each item is a 'struct user'
    map = hashmap_new(sizeof(struct user), 0, 0, 0, 
      user_hash, user_compare, NULL, NULL);

  
    /* Reads from .txt file, extracts login/password, and
       stores login/password in a hashmap */
    char const* const fileName = "LoginsAndPasswords.txt";
    FILE* file = fopen(fileName, "r");
    char line[256];

    while (fgets(line, sizeof(line), file)) {
      extract(line);
    }
    fclose(file);


  // prompt user to sign-in
    struct user *user;

    printf("USER LOGIN - ");
    for(int i = 0; i < LIMIT; ++i){
      char username[20], pword[20];
      
      // Update log file ‘signIn.txt’
      FILE *f = fopen("signIn.txt", "a");
      if (f == NULL)
      {
        printf("Error opening file!\n");
        exit(1);
        }

      printf("You have %d attempts left\n\n", LIMIT - i);
      printf("Enter User ID: \n");
      scanf("%s", username);
    
      printf("Enter Password: \n");
      scanf("%s", pword);

      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      
      /* log to signIn.txt */
      int year = (tm.tm_year + 1900) % 100;
      fprintf(f, "%s@stevens.edu, %s, %02d/%02d/%d, %d:%d:%d(UTC), %s\n",
        username, pword, tm.tm_mon + 1,
        tm.tm_mday, year, tm.tm_hour, tm.tm_min, tm.tm_sec, IPbuffer);
      
      user = hashmap_get(map, &(struct user){ .username=username });
      if(user){    // if account exists
        hashmap_get(map, &(struct user){ .username=username });
        // compare the strings str1 with str2
        int c = strncmp(pword, user->password, sizeof(pword));
        if(c != 0){    // if no match exists
          printf("Password incorrect! ");
        } else{
          printf("\nLogin successful!\n");
          accountFound = true;
          break;
        }
      } else{
        printf("Account not found! ");
      }
      fclose(f);
    }
  
  
    // Lock account
    if(!accountFound){
    printf("Account locked for 1 hour!\n");
      }

    hashmap_free(map);
}
