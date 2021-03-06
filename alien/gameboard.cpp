#include "gameboard.h"
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <chrono>
using namespace std;

/*
 * We set these variables at the global scope.
 * These will be used throughout the entire program
 */
typedef std::chrono::high_resolution_clock CLOCK;
int DEBUGME; // Debug information
WINDOW *CANVAS; // Gameboard

// Variables for Aliens
vector<Alien> ALIENS(50, Alien()); // Aliens
int MAXX, MAXY, DIRECTION, OFFSETX, OFFSETY; // Alien positions
const int ALIENSPEED = 100; // Speed of the aliens in milliseconds
bool ALIENTIMER=true; // Timer for the alien movement
auto alienTimerStart = CLOCK::now(); // Setup timer

// Variables for Missiles
vector<Missile> MISSILE(1, Missile()); // Missiles
const int MISSILESPEED = 85; // Speed of missiles in milliseconds
bool MISSILETIMER=true; // Timer for Missile movement
const int MISSILE_LIMIT=3; // Set missile limit
auto missileTimerStart = CLOCK::now(); // Setup timer

// Variables for the player
Base BASE; // Shooter
int BASEX; // Shooter position

/*
 * Create the canvas
 */

WINDOW *create_newwin(int height, int width, int startX, int startY) {
    WINDOW *local_win;
    
    local_win = newwin(height, width, startY, startX);
    wrefresh(local_win);
    return local_win;
}

/*
 * Debug information
 */
void debugMe(void) {
    if(MISSILE.size() > 0) {
        mvwprintw(CANVAS, 18, 0, "%d, %d - Missile Image: %s",MISSILE[0].getX(), MISSILE[0].getY(), MISSILE[0].getImage().c_str());
        mvwprintw(CANVAS, 19, 0, "Missile Count: %d", MISSILE.size());
    }
    
    mvwprintw(CANVAS, 21, 0, "Base X: %d, Image: %s", BASEX, BASE.getImage().c_str());
    mvwprintw(CANVAS, 22, 0, "Max X: %d Max Y %d", MAXX, MAXY);
    mvwprintw(CANVAS, 23, 0, "Offset X: %d Offset Y %d", OFFSETX, OFFSETY);
    mvwprintw(CANVAS, 0, 0, "+");
    mvwprintw(CANVAS, 23, 0, "+");
    mvwprintw(CANVAS, 0, 99, "+");
    mvwprintw(CANVAS, 23, 99, "+");
}

/* Load the vector of aliens
 * We need to do an initial load of the gameboard
 * by telling the aliens where they should appear
 * on the board and what they should look like.
 */

void loadAliens() {
    
    // Start the aliens at coordinates (0,0)
    int x = 0;
    int y = 0;
    
    // We have 5 rows of aliens and each one will have a different image
    string alien[5] = { " <-0-> ", " {-@-} ", " #-*-# ", " #[0]# ", " [-&-] " };
    
    /*
     * We passed in a vector of aliens so we can set the positions
     */
    for(int i=0; i<ALIENS.size(); i++)
    {
        /* We will have 10 aliens per line. Since we start at index 0,
         * we really need to start the next row on every time we hit 10
         * because 0-9 is really ten aliens per line
         *  0   1   2   3   4   5   6   7   8   9
         * 20  21  22  23  24  25  26  27  28  29
         * 30  31  32  33  34  35  36  37  38  39
         * 40  41  42  43  44  45  46  47  48  49
         * 50  51  52  53  54  55  56  57  58  59
         */
        if(i%10 == 0 && i != 0) {
            y++;
            x=0;
        }
        ALIENS[i].setX(x); // Set x position of alien on the gameboard
        ALIENS[i].setY(y); // Set y position of alien on the gameboard
        ALIENS[i].setImage(alien[y]); // Set the image according to the row
        ALIENS[i].setAlive(true); // Set the alien to unhide
        
        /*
         * Each alien is 7 characters but we really want them a little farther spaced
         * to allow some space for the player to miss.
         */
        x+=9;
    }
}

void setupGame(void) {
    /*
     * This isn't a "graphic" game and simply uses
     * standard american ascii charaters for the images.
     * NCurses library is used but, we have some setup to do.
     */
    initscr(); // Initialize Curses
    cbreak(); // Use unbuffered input
    keypad(stdscr, TRUE); // Allow for use of keypad
    nodelay(stdscr, TRUE); // Immediately process input so player doesn't press enter
    curs_set(0); // Turn the cursor off.
    noecho(); // Don't echo keys to the screen
    refresh(); // Apply our setup
    
    // Create the gameboard
    CANVAS = newwin(30, 150, 0, 0);
    
    // Setup the initial direction and speed of the aliens and the screen borders
    getmaxyx(CANVAS, MAXY, MAXX);
    OFFSETX=1;
    OFFSETY=1;
    DIRECTION=1;
    
    // Setup the intial position of the shooter
    BASEX = (MAXX/2) - (BASE.getWidth()/2);
    
    /*
     * A single missile was created to initialize the vector
     * so, we need to erase the vector so it has zero elements
     */
    MISSILE.clear();
    
}

/*
 * Routine to make the everything move and check collisions
 */
void refreshGameBoard(void) {
    
    /*
     * Each time we call a refresh, we will check the timers
     */
    
    // Alien timer duration
    auto alienTimerStop = CLOCK::now();
    long alienDelay = std::chrono::duration_cast<std::chrono::milliseconds>(alienTimerStop - alienTimerStart).count();
    
    // If we just hit a timer cycle, reset the timer
    if(ALIENTIMER) {
        ALIENTIMER=false;
        alienTimerStart = CLOCK::now();
    }
    
    // Missile timer duration
    auto missileTimerStop = CLOCK::now();
    long missileDelay = std::chrono::duration_cast<std::chrono::milliseconds>(missileTimerStop - missileTimerStart).count();
    
    // If we just hit the timer cycle, reset the timer
    if(MISSILETIMER) {
        MISSILETIMER=false;
        missileTimerStart = CLOCK::now();
    }
    
    // Set when we hit the right duration
    if (missileDelay >= MISSILESPEED) {
        MISSILETIMER=true;
    }
    
    // Set when we hit the right duration
    if (alienDelay >= ALIENSPEED) {
        ALIENTIMER=true;
    }
    
    /*
     * First check the position of the Aliens.  When they hit the
     * screen border, they should drop one level and change direction
     * We also need to erase the line above when we drop down.
     *
     * If any alien hits the border, immediatly drop down and stop checking
     * collisions.
     */
    if(ALIENTIMER) {
        for(int i=0;i<ALIENS.size();i++) {
            if(ALIENS[i].getAlive() && (((ALIENS[i].getX()+OFFSETX+9)>=MAXX) || (ALIENS[i].getX()+OFFSETX <= 0))) {
                OFFSETY++;
                DIRECTION *= -1;
                mvwaddstr(CANVAS, OFFSETY-1, 0, "                                                                                                                                       ");
                break;
            }
        }
    }
    // Sets the direction -- (Left) ++ (Right)
    if (ALIENTIMER) {
        if (DIRECTION == 1)
            OFFSETX++;
        else
            OFFSETX--;
    }
    
    // Draw the aliens on the canvas using the offsets for X and Y
    for(int i=0; i<ALIENS.size(); i++) {
        if(ALIENS[i].getAlive()) {
            mvwaddstr(CANVAS, ALIENS[i].getY()+OFFSETY, ALIENS[i].getX()+OFFSETX, ALIENS[i].getImage().c_str());
        }
    }
    
    // Fire the Missle
    if(MISSILETIMER) {
        bool isHit=false;
        for(int i=0; i<MISSILE.size(); i++) {
            // Check Collision
            for(int j=0; j<=ALIENS.size(); j++) {
                if((MISSILE[i].getX() >= ALIENS[j].getX()+OFFSETX && MISSILE[i].getX() <= ALIENS[j].getX()+OFFSETX+7) && (MISSILE[i].getY() == (ALIENS[j].getY()+OFFSETY)) && ALIENS[j].getAlive()) {
                    ALIENS[j].setAlive(false);
                    mvwaddstr(CANVAS, MISSILE[i].getY()-1, MISSILE[i].getX(), " ");
                    mvwaddstr(CANVAS, ALIENS[j].getY()+OFFSETY, ALIENS[j].getX()+OFFSETX, "       ");
                    isHit=true;
                    break;
                }
            }
            if(isHit) {
                MISSILE.erase(MISSILE.begin()+i);
                break;
            }
            
            if(MISSILE[i].getY()-1<=1)
                MISSILE.erase(MISSILE.begin()+i);
            else
                MISSILE[i].setY(MISSILE[i].getY()-1);
            mvwaddstr(CANVAS, MISSILE[i].getY(), MISSILE[i].getX(), MISSILE[i].getImage().c_str());
            mvwaddstr(CANVAS, MISSILE[i].getY()+1, MISSILE[i].getX(), " ");
        }
    }
    // Draw the "base" so the player has a shooter at the bottom of the screen
    mvwaddstr(CANVAS, MAXY-1, BASEX, BASE.getImage().c_str());
    
    // Debug information - If the player presses 'd' or 'D', show debug info
    if(DEBUGME==1) {
        debugMe();
    }
    
    // Call to refresh the canvas
    wrefresh(CANVAS);
}

/*
 * Setup the canvas and play the game
 */
void play(void) {
    
    // What key did the player press?
    int keyPress;

    // Play the game
    while (true) {
        
        refreshGameBoard();
        
        // Check for a keypress
        keyPress=getch();
        
        // Do the function associate with the key
        if(keyPress == 'q' || keyPress == 'Q') {
            break;
        }
        
        // Enable/Disable Debug information
        if(keyPress == 'd' || keyPress == 'D') {
            DEBUGME=(DEBUGME==1?DEBUGME=0:DEBUGME=1);
            erase();
        }
        
        // Process player movements and firing of missiles
        if(keyPress == ',' || keyPress == KEY_LEFT) BASEX--;
        if(keyPress == '.' || keyPress == KEY_RIGHT) BASEX++;
        if(keyPress == ' ' || keyPress == 'f' || keyPress == 'F') {
            /*
             * In the original game, you couldn't shoot more than
             * tree missiles.  We will check the missile vector
             * for the number of elements.
             */
            if(MISSILE.size() < MISSILE_LIMIT) {
                MISSILE.push_back(Missile(BASEX, MAXY-3));
            }
        }
    }
    
    // Teardown the game and return the screen to normal
    delwin(CANVAS);
    endwin();
}
