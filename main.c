#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <stdbool.h>

//Function declarations
int get_next_nonblank_line(char *buf, int max_length, FILE *ifp);
void remove_crlf(char *s);

//Global variables
int m, n, BHR = 0;
double hit, miss;

int main(int argc, char* argv[])
{
    if(argc != 4)
        return 0;

    //initialize m and n
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    
    //open the file and make sure it opens properly
    FILE *inp = fopen(argv[3], "r");

    if(inp == NULL)
    {
        return 0;
    }

    //create the table based on m
    int table_size = pow(2, m);
    int table[table_size];

    //Initialize table entries to weakly taken (2)
    for(int i = 0; i < table_size; i++)
        table[i] = 2;

    char outcome;
    long long int PC = 0;
    char buf[4096];

    while(!feof(inp))
    {
        //grab PC and the actual outcome
        get_next_nonblank_line(buf, 4095, inp);
        sscanf(buf, "%llx %c", &PC, &outcome);

        //Do the operations to find the predicted outcome
        PC = PC >> 2;
        int m_idx = PC % table_size;
        int n_ext = BHR << (m - n);
        int index = m_idx ^ n_ext;
        int prediction = table[index];
        
        //compare predicted outcome with actual outcome and increment hit, miss, and BHR accordingly
        if(outcome == 't')
        {
            if(prediction > 1)
                hit++;
            else
                miss++;

            BHR = (BHR >> 1) + pow(2, (n-1));

            //account for saturation
            if(table[index] < 3)
                table[index]++;
        }
        else
        {
            if(prediction > 1)
                miss++;
            else
                hit++;

            BHR = (BHR >> 1);  

            //Account for saturation
            if(table[index] > 0)
                table[index]--;
        }
    }   

    printf("\nM: %d\nN: %d\nMisprediction Ratio: %.2f %%\n\n", m, n, (miss/(hit + miss))*100);
}


// Class library functions developed in CSI for FileIO

/* remove_crlf: traverse s and clobber any and all newline characters on its right
with nulls. s must be a properly allocated string. */

void remove_crlf(char *s)
{
/* Remember, in C, a string is an array of characters ending with a null - a '\
    '.  We're given the pointer to the FRONT of the string - the first printable character of
    the string. If we take n to be the length of the string, and move forward n characters from
    that, we find the null sentinel.  So let's use pointer arithmetic, combined with strlen(), to
    do exactly that.*/
    char *end = s + strlen(s);
/* end now points at the null sentinel.  But we don't actually want the null
    sentinel - we want the last substantive character of the string.  But since that lives one spot
    to the left of the null sentinel... */
    end--;
/* end now points to the last character of s.  We haven't changed the string
    yet, and we also haven't changed its pointer. */
/* Make sure end hasn't slipped off the left of the string, and check that we
    actually have a newline to remove.  Then, use the indirection operator to examine the
    actual chracter pointed to by end.  We take advantage of C's short circuit evaluation here -
    if end is to the left of s, we'll never try to (disastrously) read the end we can't
    read. */

/* Repeat all that until we either reach the left edge of the string or find
something that isn't a newline character.  The final effect is that we clobber everything
at the end of our string that's a newline character, and nothing else. */
    while((end >= s) && (*end == '\n' || *end == '\r')) {
        *end = '\0'; // clobber the character at end by overwriting it with null
        end--;
    }
}


/* get_next_nonblank_line(): reads the next line from ifp that isn't blank into
    buf, removing any newline characters.  Reads an empty string at EOF, and only then.
    Returns true if it read and false if it didn't. buf must be an allocated
    buffer large enough to hold max_length characters. ifp must be an open file that can be read. */

int get_next_nonblank_line(char *buf, int max_length, FILE *ifp)
{
    buf[0] = '\0'; // Start with an empty string
    while(!feof(ifp) && (buf[0] == '\0'))
    {
/* If we're not at the end of the file, and we haven't read a nonblank line
    yet, read a line and pull the CRLFs off the end. */
        fgets(buf, max_length, ifp);
        remove_crlf(buf);
    }
    return 1;
}