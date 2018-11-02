/*
 * Parent class for all moving objects
 */
#include <string>
using namespace std;

#ifndef _MOVEABLE_OBJECT_
#define _MOVEABLE_OBJECT_ 

class moveable_object
{
  private:
    // Stores the image
    string image;

    // Stores the position of the object
    int xPos, yPos, size;

  public:
    /*
     * Allows us to set and get the Image of the object
     * as well as move the object on the canvas.
     */

    string getImage(void) { return image; };
    void setImage(string s) { image = s; };
    void moveTo(int x, int y) { xPos = x; yPos=y; };

    /*
     * Information about the image
     */

    int getWidth(void) { return image.length(); };
    int getHeight(void) { return 1; };

    /*
     * Allows us to set where the object is on the canvas
     * as well as tell us where the object is located on the canvas
     */

    void setX(int x) { xPos = x; };
    void setY(int y) { yPos = y; };
    int getX(void) { return xPos; };
    int getY(void) { return yPos; };
};

#endif  // _MOVEABLE_OBJECT_ 

/*
 * Derive an alien from the generic moveable object class.
 * These will be what the player shoots.
 */

#ifndef _ALIEN_
#define _ALIEN_

class Alien : public moveable_object
{
  private:
    bool isAlive; // Is it alive or dead?

  public:
    Alien() {
    }
  
    /*
     * Tells us if the alien is alive.
     */
    bool getAlive(void) { return isAlive; };
    void setAlive(bool b) { isAlive = b; };
};

#endif  // _ALIEN_

/*
 * Derive a movable object from our generic object
 * This will be used when the player moves and shoots aliens
 */

#ifndef _BASE_
#define _BASE_

class Base : public moveable_object
{
  public:
    Base() {
      setImage(" -/*\\- ");
    }
};

#endif  // _BASE_

/*
 * Derive a moveable object from our generic object
 * This will be used when the player shoots
 */

#ifndef _MISSILE_
#define _MISSILE_

class Missile : public moveable_object
{
  public:
    Missile () {
    }

    Missile(int x, int y) {
      setX(x+4);
      setY(y);
      setImage("*");
    }
};

#endif // _MISSILE_

/*
 * Bombs are pretty much missiles but these are dropped
 * from the aliens.  We could have just used a missile class
 * but for sake of limiting confusion, let's just have a class
 * called Bomb.  Since these are missiles in disguise, these
 * have the same properties and methods as a missile.  In
 * essence, we are just having a missile with a different name.
 */

#ifndef _BOMB_
#define _BOMB_

class Bomb : public moveable_object
{
public:
    Bomb(){
    }
    
    Bomb(int x, int y) {
        setX(x+4);
        setY(y);
        setImage("|");
    }
};
#endif // _BOMB_
