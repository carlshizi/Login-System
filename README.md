Time format used: UTC, which is 4 hours ahead of Eastern time.

Note:
UserID is entered without the @stevens.edu.
From the LoginsAndPasswords.txt, the program extracts the userID right before the '@'and the password after the comma. After the extraction, the program stores the userID/password pair in a hashmap.
For the log entries, We assumed that the login system uses the @stevens.edu domain name. So, every userID entry is stored as userID@stevens.edu.

Hashmap source: https://github.com/tidwall/hashmap.c
