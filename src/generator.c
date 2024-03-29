/* generator.c - this module generates a new password */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "generator.h"
#include "pswUtil.h"

/* Returns a string containing a new-generated password.
 * 
 * FLAGS:
 *     bits [0-7] : length
 *     bit    [8] : -d or -D | includes digits
 *     bit    [9] : -l       | includes lowercase letters
 *     bit   [10] : -L       | includes uppercase letters
 *     bit   [11] : -s       | includes special symbols !@#$%^&*()
 *     bit   [12] : -S       | includes additional symbols "'+,-./:;<=>?[\]_
 *                           |                             `{|}~ and <space>
 * ---------------------------------------------------------------------- */
char *generatePassword(int FLAGS, int seed_shift)
{
    // -l + -L + -s*2 + -S*2 + -d*2 + \0 = 139
    char *char_set = malloc(139 * sizeof(char));
    char *password = malloc((FLAGS % 0x100) * sizeof(char));
    
    _Bool base_is_set = 0;
    const char *d = "0123456789";
    const char *l = "abcdefghijklmnopqrstuvwxyz";
    const char *L = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *s = "!@#$%^&*()";
    const char *S = "\"'+,-./:;<=>\?[\\]_`{|}~ ";
    
    // setting -d or -D
    if(u_GetBit(FLAGS, 0x8)) {
        strcpy(char_set, d);
        base_is_set = 1;

        // improve frequency in case of -l
        if(u_GetBit(FLAGS, 0x9)) {
            strcat(char_set, d);
        }

        // improve frequency in case of -L
        if(u_GetBit(FLAGS, 0xA)) {
            strcat(char_set, d);
        }
    }
    
    // setting -l
    if(u_GetBit(FLAGS, 0x9)) {
        if(!base_is_set) {
            strcpy(char_set, l);
            base_is_set = 1;
        } else {
            strcat(char_set, l);
        }
    }
    
    // setting -L
    if(u_GetBit(FLAGS, 0xA)) {
        if(!base_is_set) {
            strcpy(char_set, L);
            base_is_set = 1;
        } else {
            strcat(char_set, L);
        }
    }
    
    // setting -s
    if(u_GetBit(FLAGS, 0xB)) {
        if(!base_is_set) {
            strcpy(char_set, s);
            base_is_set = 1;
        } else {
            strcat(char_set, s);
        }

        // improve frequency in case of -l
        if(u_GetBit(FLAGS, 0x9)) {
            strcat(char_set, s);
        }

        // improve frequency in case of -L
        if(u_GetBit(FLAGS, 0xA)) {
            strcat(char_set, s);
        }
    }
    
    // setting -S
    if(u_GetBit(FLAGS, 0xC)) {
        if(!base_is_set) {
            strcpy(char_set, S);
            base_is_set = 1;
        } else {
            strcat(char_set, S);
        }
    }
    
    unsigned long seed = time(NULL);
    unsigned long useless_garbage = (unsigned long) &useless_garbage;
    
    seed ^= ((useless_garbage * (seed * 0xCAFE + useless_garbage % 0xBEEF))
             >> (seed % 0x4)) * seed * /*THIS_CODE_IS*/0x50BAD + seed_shift;
    
    for(int i = 0; i < FLAGS % 0x100; i++) {
        password[i] = randomChar(char_set, seed * (seed_shift + 1) + i
                      + (useless_garbage * seed_shift + 0xF00D));
    }
    
    free(char_set);
    return password;
}

// it's obvious what it does
char randomChar(char *char_set, unsigned long seed)
{
    srand(seed);   
    return char_set[rand() % (strlen(char_set))];
}

