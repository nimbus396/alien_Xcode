#include "gameboard.h"
#include <iostream>
#include <vector>
#include <curses.h>
using namespace std;

/*
 * We set these variables at the global scope.
 * These will be used through out the entire program
 */

vector<Alien> ALIENS(50, Alien()); // Aliens
vector<Missile> MISSILE(1, Missile()); // Missiles
Base BASE; // Shooter
WINDOW *CANVAS; // Gameboard
int MAXX, MAXY, DIRECTION, OFFSETX, OFFSETY; // Alien positions
int BASEX; // Shooter position
int DEBUGME; // Debug information
int SPEED; // Speed of the game

/*
 * Create the canvas
 */

WINDOW *create_newwin(int height, int width, int startX, int startY) {
    WINDOW *local_win;
    
    local_win = newwin(height, width, startY, startX);
    wrefresh(local_win);
    return local_win;
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
        ALIENS[i].setAlive(true);
        
        // Each alien is 7 characters but we really want them a little farther spaced
        x+=9;
    }
}

/*
 * Routine to make the aliens move
 */
void refreshGameBoard(void) {
    
    /*
     * First check the position of the Aliens.  When they hit the
     * screen border, they should drop one level and change direction
     * We also need to erase the line above when we drop down.
     */
    for(int i=0;i<ALIENS.size();i++) {
        if((ALIENS[i].getX()+OFFSETX+7>=MAXX) || (OFFSETX <= 0)) {
            OFFSETY++;
            DIRECTION *= -1;
            mvwaddstr(CANVAS, OFFSETY-1, 0, "                                                                                                   ");
            break;
        }
    }
    
    // Sets the direction -- (Left) ++ (Right)
    if(DIRECTION == 1)
        OFFSETX++;
    else
        OFFSETX--;
    
    // Draw the aliens on the canvas using the offsets for X and Y
    for(int i=0; i<ALIENS.size(); i++) {
        if(ALIENS[i].getAlive()) {
            mvwaddstr(CANVAS, ALIENS[i].getY()+OFFSETY, ALIENS[i].getX()+OFFSETX, ALIENS[i].getImage().c_str());
        }
    }
    
    mvwaddstr(CANVAS, MAXY-1, BASEX, BASE.getImage().c_str());
    
    // Fire the Missle
    for(int i=0; i<MISSILE.size(); i++) {
        
        // Check Collision
        for(int j=0; ALIENS.size(); j++)
            if(MISSILE[i].getX() >= ALIENS[j].getX() && MISSILE[i].getX() <= ALIENS[j].getX()+7) {
                ALIENS[j].setAlive(false);
                mvwaddstr(CANVAS, MISSILE[i].getY(), MISSILE[i].getX(), " ");
            }
        
        mvwaddstr(CANVAS, MISSILE[i].getY(), MISSILE[i].getX(), MISSILE[i].getImage().c_str());
        mvwaddstr(CANVAS, MISSILE[i].getY()+1, MISSILE[i].getX(), " ");
        if(MISSILE[i].getY()-1<=1)
            MISSILE.erase(MISSILE.begin()+i);
        else
            MISSILE[i].setY(MISSILE[i].getY()-1);
        
    }
    
    // Debug information
    if(DEBUG==1) {
        if(MISSILE.size() > 0) {
            mvwprintw(CANVAS, 19, 0, "%d, %d - Missile Image: %s",MISSILE[0].getX(), MISSILE[0].getY(), MISSILE[0].getImage().c_str());
            mvwprintw(CANVAS, 20, 0, "Missile Count: %d", MISSILE.size());
        }
        
        mvwprintw(CANVAS, 21, 0, "Base X: %d, Image: %s", BASEX, BASE.getImage().c_str());
        mvwprintw(CANVAS, 22, 0, "Max X: %d Max Y %d", MAXX, MAXY);
        mvwprintw(CANVAS, 23, 0, "Offset X: %d Offset Y %d", OFFSETX, OFFSETY);
        mvwprintw(CANVAS, 0, 0, "+");
        mvwprintw(CANVAS, 23, 0, "+");
        mvwprintw(CANVAS, 0, 99, "+");
        mvwprintw(CANVAS, 23, 99, "+");
        
    }
    
    // Call to refresh the canvas
    wrefresh(CANVAS);
    // dumb timer
    for(int i=0; i<SPEED; i++) { int d=0; };
}

/*
 * Setup the canvas and play the game
 */
void play(void) {
    
    // What key was pressed
    int keyPress;
    
    // Setup the terminal
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    noecho();
    refresh();
    
    // Create the gameboard
    CANVAS = newwin(24, 100, 0, 0);
    
    // Setup the initial direction and speed of the aliens and the screen borders
    getmaxyx(CANVAS, MAXY, MAXX);
    OFFSETX=1;
    OFFSETY=1;
    DIRECTION=1;
    SPEED=25000000;
    // Setup the intial position of the shooter
    BASEX = (MAXX/2) - (BASE.getWidth()/2);
    
    MISSILE.clear();
    
    // Play the game
    while (true) {
        
        refreshGameBoard();
        
        // Check for a keypress
        keyPress=getch();
        
        // Do the function associate with the key
        if(keyPress == 'q' || keyPress == 'Q') break;
        
        if(keyPress == 'd' || keyPress == 'D') {
            DEBUGME=(DEBUGME==1?DEBUGME=0:DEBUGME=1);
            erase();
        }
        
        if(keyPress == ',') BASEX--;
        if(keyPress == '.') BASEX++;
        if(keyPress == ' ') {
            if(MISSILE.size() < 3) {
                MISSILE.push_back(Missile(BASEX, MAXY-3));
            }
        }
    }
    
    // Teardown the game and return the screen to normal
    delwin(CANVAS);
    endwin();
}

int main() {
    
    // Setup the game
    loadAliens();
    
    // and play it
    play();
}
