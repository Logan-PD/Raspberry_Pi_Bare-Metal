// This program demonstrates how to initialize a frame buffer for a
// 1024 x 768 display, and how to draw on it using a simple checker board
// pattern.

// Included header files
#include "uart.h"
#include "framebuffer.h"

#include "snes.h"

#define MAZESIZEY 768
#define MAZESIZEX 1024

// Size of pixel to be drawn
#define SQUARESIZE 1

#define NUMBUTTONS 5

// A struct to represent a button
struct Button
{
    int number;
    char* name;
};

// A struct to represent a position
struct Point
{
    int x;
    int y;
};

// (0,0) is top left corner
int masterMaze[MAZESIZEX][MAZESIZEY];

void initializeMasterMaze();

void drawSquare(int x, int y, unsigned int colour);
void drawMaze();

// pseudo constructors for the structs that we have created above
struct Button createButton(int number, char* name);
struct Point createPoint(int x, int y);



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function initializes the UART terminal and initializes
//                  a frame buffer for a 1024 x 768 display. Each pixel in the
//                  frame buffer is 32 bits in size, which encodes an RGB value
//                  (plus an 8-bit alpha channel that is not used). The program
//                  then draws and displays an 18 x 12 checker board pattern.
//
////////////////////////////////////////////////////////////////////////////////

void main()
{
    unsigned short data = 0xFFFF;

    // Initialize the UART terminal
    uart_init();

    uart_puts("Hello World!");

    initializeSNES();

    // Initialize the frame buffer
    initFrameBuffer();

    // Set coordinates
    initializeMasterMaze();


    // Create an array of size NUMBUTTONS to hold all the buttons that we are using on the SNES controller
    struct Button buttons[NUMBUTTONS];
    buttons[0] = createButton(3, "Start");
    buttons[1] = createButton(4, "Up");
    buttons[2] = createButton(5, "Down");
    buttons[3] = createButton(6, "Left");
    buttons[4] = createButton(7, "Right");

    // Create a character represented by an x, y position
    struct Point character = createPoint(0, 0);

    // Draw the initial maze
    drawMaze();

    // Draw the character
    drawSquare(character.x, character.y, BLACK);

    //
    while (1) {
    	// Read data from the SNES controller
    	data = get_SNES();

          // If no buttons have been pressed
          if (data == 0)
              continue;

          // Loop through every possible button press
          for (int i = 0; i < NUMBUTTONS; ++i) {
              if (((1 << buttons[i].number) & data) != 0) {


                  switch (buttons[i].number) {
                      // Start will clear the board leaving the character at its position
                      case 3 :
                      drawMaze();
                      break;

                      // Up will move the character up they if will still be within bounds
                      case 4 :
                      if (character.y > 0)
                          character.y -= 1;
                      break;

                      // Down will move the character down if they will still be within bounds
                      case 5 :
                      if (character.y < MAZESIZEY - 1)
                          character.y += 1;
                      break;

                      // Left will move the character left if they will still be within bounds
                      case 6 :
                      if (character.x > 0)
                          character.x -= 1;
                      break;

                      // Right will move the character right if they will still be within bounds
                      case 7 :
                      if (character.x < MAZESIZEX - 1)
                          character.x += 1;
                      break;

                      default :
                      break;
                  }
              }

            // Draw the character
            drawSquare(character.x, character.y, FUCHSIA);

        }

    	// Delay 1/30th of a second
    	microsecond_delay(33333);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       drawSquare
//
//  Arguments:      x location, y location, and colour of square
//
//  Returns:        void
//
//  Description:    Calls the drawSquareToFrameBuffer function with the given
//                  arguments
//
////////////////////////////////////////////////////////////////////////////////
void drawSquare(int x, int y, unsigned int colour)
{
    drawSquareToFrameBuffer(y * SQUARESIZE, x * SQUARESIZE, SQUARESIZE, colour);
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       drawMaze
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Draws the background by calling the drawSquare function
//                  for every pixel.
//
////////////////////////////////////////////////////////////////////////////////
void drawMaze()
{
    for (int i = 0; i < MAZESIZEX; ++i)
        for (int j = 0; j < MAZESIZEY; ++j)
            drawSquare(i,j,LIME);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       initializeMasterMaze
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Sets the coordinates of every pixel in the grid
//
////////////////////////////////////////////////////////////////////////////////
void initializeMasterMaze()
{
    for (int i = 0; i < MAZESIZEX; ++i) {
        for (int j = 0; j < MAZESIZEY; ++j) {
            masterMaze[i][j] = ((i + j) % 2);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function:       createButton
//
//  Arguments:      button number and name
//
//  Returns:        the newly created Button struct
//
//  Description:    Creates a button struct with the given number and name
//
////////////////////////////////////////////////////////////////////////////////
struct Button createButton(int number, char* name)
{
    struct Button b;
    b.number = number;
    b.name = name;
    return b;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function:       createPoint
//
//  Arguments:      x and y location
//
//  Returns:        the newly created point struct
//
//  Description:    Creates a point struct with the given x and y coordinates
//
////////////////////////////////////////////////////////////////////////////////
struct Point createPoint(int x, int y)
{
    struct Point p;
    p.x = x;
    p.y = y;
    return p;
}
