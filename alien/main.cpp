//
//  main.cpp
//  alien
//
//  Created by James Johnson on 11/6/18.
//  Copyright © 2018 James Johnson. All rights reserved.
//

// There is no magic here but, this is where it all starts
extern void loadAliens(void);
extern void setupGame(void);
extern void play(void);

int main() {
    
    // Setup the game
    loadAliens();
    setupGame();
    
    // and play it
    play();
}
