/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// Current open position for faster move() check
int openSqr[2];

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
void shiftTile(int tileCurr);
bool won(void);


int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(1500000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    //Init counter
    int curSqrVal = d * d -1;
    
    //Init values in global var "board"
    for (int i=0;i<d;i++)
    {
        for (int j=0;j<d;j++)
        {
            board[i][j] = curSqrVal;
            curSqrVal--;
        };
    };
    
    //Insert blank space & track
    board[d-1][d-1] = 0;
    openSqr[0] = d-1;     //Row
    openSqr[1] = d-1;     //Col
    
    //Swap last two tiles if board size is even
    if (d%2 == 0)
    {
        int tempVal = board[d-1][d-2];
        board[d-1][d-2] = board[d-1][d-3];
        board[d-1][d-3] = tempVal;
    };
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    //Draw board in d x d square
    for (int i=0;i<d;i++)
    {
        for (int j=0;j<d;j++)
        {
            if (board[i][j] == 0)
            {
                printf("__  ");
            }
            else if (board[i][j] < 10)
            {
                printf("%i   ",board[i][j]);
            }
            else
            {
                printf("%i  ",board[i][j]);
            };
        };
        
        //New row
        printf("\n\n");
    };
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    //Check if user input is adjacent to blank
    //  openSqr[0] = row
    //  openSqr[1] = col
    if (openSqr[0] != 0 && board[openSqr[0]-1][openSqr[1]] == tile)              //Above blank
    {
        //Move user square
        board[openSqr[0]-1][openSqr[1]] = 0;
        shiftTile(tile);
        
        //Update open square tracking
        openSqr[0]--;
    }
    else if (openSqr[0] != (d-1) && board[openSqr[0]+1][openSqr[1]] == tile)         //Below
    {
        //Move user square
        board[openSqr[0]+1][openSqr[1]] = 0;
        shiftTile(tile);
        
        //Update open square tracking
        openSqr[0]++;
    }
    else if (openSqr[1] != 0 && board[openSqr[0]][openSqr[1]-1] == tile)         //Left
    {
        //Move user square
        board[openSqr[0]][openSqr[1]-1] = 0;
        shiftTile(tile);
        
        //Update open square tracking
        openSqr[1]--;
    }
    else if (openSqr[1] != (d-1) && board[openSqr[0]][openSqr[1]+1] == tile)         //Right
    {
        //Move user square
        board[openSqr[0]][openSqr[1]+1] = 0;
        shiftTile(tile);
        
        //Update open square tracking
        openSqr[1]++;
    }
    else
    {
        return false;
    };
    
    return true;
}

void shiftTile(int tileCurr)
{
    board[openSqr[0]][openSqr[1]] = tileCurr;
}


/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    //Check correct pos for blank
    int bPos = d-1;
    
    if (board[bPos][bPos] != 0)
    {
        return false;
    };
    
    //Check first row
    for (int i=0;i<d;i++)
    {
        if (board[0][i] != (i+1))
        {
            return false;
        };
    };
    
    //First row correct, check balance of rows using multiples of d + first row with same k index
    for (int j=1;j<d;j++)
    {
        for (int k=0;k<d;k++)
        {
            if ((j != bPos && k != bPos) && board[j][k] != (d * j + board[0][k]))
            {
                return false;
            };
        };
    };
    
    return true;
}
