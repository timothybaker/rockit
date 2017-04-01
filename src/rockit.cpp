
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>

//screen size and level size
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int LEVEL_WIDTH = 3840;
const int LEVEL_HEIGHT = 2160;

//tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 1296;
const int TOTAL_TILE_SPRITES = 100;

//tile sprites
const int TILE_GRASS = 0;
const int TILE_GRASS_PLANT1 = 1;
const int TILE_PATH2 = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;
const int TILE_GRASS_TREE1 = 12;
const int TILE_GRASS_TREE2 = 13;
const int TILE_GRASS_TREE3 = 14;
const int TILE_BOAT_PART1 = 15;
const int TILE_BOAT_PART2 = 16;
const int TILE_DOCK = 17;
const int TILE_PATH = 18;

//player constants
const int GTILE_WIDTH = 32;
const int GTILE_HEIGHT = 48;
const int TOTAL_GAMBIT_SPRITES = 16;

//player sprites
const int GAMBIT_UP = 0;
const int GAMBIT_UP1 = 1;
const int GAMBIT_UP2 = 2;
const int GAMBIT_UP3 = 3;
const int GAMBIT_LEFT = 4;
const int GAMBIT_LEFT1 = 5;
const int GAMBIT_LEFT2 = 6;
const int GAMBIT_LEFT3 = 7;
const int GAMBIT_RIGHT = 8;
const int GAMBIT_RIGHT1 = 9;
const int GAMBIT_RIGHT2 = 10;
const int GAMBIT_RIGHT3 = 11;
const int GAMBIT_DOWN = 12;
const int GAMBIT_DOWN1 = 13;
const int GAMBIT_DOWN2 = 14;
const int GAMBIT_DOWN3 = 15;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The tile
class Tile
{
    public:
		//Initializes position and type
		Tile( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

    private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int mType;
};

//The dot that will move around on the screen
class player
{
    public:
	
		//The dimensions of the dot
		static const int GAMBIT_WIDTH = 25;
		static const int GAMBIT_HEIGHT = 60;

		//Maximum axis velocity of the dot
		static const int GAMBIT_VEL = 3;
		static const int GAMBIT_RUN_VEL =15;

		//Initializes the variables
		player();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and check collision against tiles
		void move( Tile *tiles[] );

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );
		
		//get direction tracker
		int get_tracker(){return direction_tracker;}
		
		//increment direction tracker
		void tracker_adder(){direction_tracker++; if(direction_tracker>3){direction_tracker=0;}}
		
		//reset tracker
		void tracker_reset(){direction_tracker=0;}
		
		//set direction heading
		void set_direction_heading(char c){direction_heading=c;}
		
		//get direction heading
		char get_direction_heading(){return direction_heading;}
		
		//set last heading
		void set_last_heading(char c){last_heading=c;}
		
		//get last heading
		char get_last_heading(){return last_heading;}
		
		//set tilestat
		bool set_tilestat();
		
		// tile stat
		int tilestat;


    private:
		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
		
		//same direction
		int direction_tracker;
		
		//direction heading
		char direction_heading;
		
		//last heading
		char last_heading;

};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia( Tile* tiles[] );

//Frees media and shuts down SDL
void close( Tile* tiles[] );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesWall( SDL_Rect box, Tile* tiles[] );

//Sets tiles from tile map
bool setTiles( Tile *tiles[] );

//set player tile
bool setGambit( Tile *player_tile, player player);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gGambitTexture;
SDL_Rect gGambitClips[ TOTAL_GAMBIT_SPRITES ];
LTexture gTileTexture;
SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision( camera, mBox ) )
    {
        //Show the tile
        gTileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips[ mType ] );
    }
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

player::player()
{
    //Initialize the collision box
    mBox.x = 0;
    mBox.y = 0;
    mBox.w = GAMBIT_WIDTH;
    mBox.h = GAMBIT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
    
    //initialize movement tracker
    direction_tracker=0;
    
    //initialize diredtion headings
    last_heading=' ';
    direction_heading=' ';
    
    //initialize tilestat
    tilestat=0;
}

bool player::set_tilestat()
{
    
    last_heading=direction_heading;
    
    if(mVelY==0 && mVelX==0)
    {
    		if(last_heading==' ')
    		{
    			tilestat=0;
    		}
    		if(last_heading=='e')
    		{
    			tilestat = 8;
    		}
    		if(last_heading=='w')
    		{
     		tilestat = 4;   		
    		}
    		if(last_heading=='s')
    		{
       		tilestat = 0;
    		}
    		if(last_heading=='n')
    		{
    		     tilestat = 12;
    		}
    		direction_heading==' ';
    }
    else if(mVelY==0 && mVelX==GAMBIT_VEL)
    {
    		direction_heading='e';
    		if(direction_heading==last_heading)
    		{
    			direction_tracker++;
    			if(direction_tracker>14)
    			{
    				direction_tracker=0;
    			}
    		}
    		else
    		{
    			direction_tracker=0;
    		}
    		
    		if(direction_tracker<8)
    		{
    			tilestat = 8;
    		} 
    		else if(direction_tracker<11)
    		{
    			tilestat = 9;
    		} 
    		else if(direction_tracker<14)
    		{
    			tilestat = 10;
    		} 
    		else if(direction_tracker==14)
    		{
    			tilestat = 11;
    		} 
    }
    
    else if(mVelY==0 && mVelX==GAMBIT_VEL*-1)
    {
    		direction_heading='w';
    		if(direction_heading==last_heading)
    		{
    			direction_tracker++;
    			if(direction_tracker>14)
    			{
    				direction_tracker=0;
    			}
    		}
    		else
    		{
    			direction_tracker=0;
    		}
    		
    		if(direction_tracker<8)
    		{
    			tilestat = 4;
    		} 
    		else if(direction_tracker<11)
    		{
    			tilestat = 5;
    		} 
    		else if(direction_tracker<14)
    		{
    			tilestat = 6;
    		} 
    		else if(direction_tracker==14)
    		{
    			tilestat = 7;
    		} 
    }
    
    else if(mVelY==GAMBIT_VEL*-1 && mVelX==0)
    {
    		direction_heading='s';
    		if(direction_heading==last_heading)
    		{
    			direction_tracker++;
    			if(direction_tracker>14)
    			{
    				direction_tracker=0;
    			}
    		}
    		else
    		{
    			direction_tracker=0;
    		}
    		
    		if(direction_tracker<8)
    		{
    			tilestat = 0;
    		} 
    		else if(direction_tracker<11)
    		{
    			tilestat = 1;
    		} 
    		else if(direction_tracker<14)
    		{
    			tilestat = 2;
    		} 
    		else if(direction_tracker==14)
    		{
    			tilestat = 3;
    		} 
    }
    
    else if(mVelY==GAMBIT_VEL && mVelX==0)
    {
    		direction_heading='n';
    		if(direction_heading==last_heading)
    		{
    			direction_tracker++;
    			if(direction_tracker>14)
    			{
    				direction_tracker=0;
    			}
    		}
    		else
    		{
    			direction_tracker=0;
    		}
    		
    		if(direction_tracker<8)
    		{
    			tilestat = 12;
    		} 
    		else if(direction_tracker<11)
    		{
    			tilestat = 13;
    		} 
    		else if(direction_tracker<14)
    		{
    			tilestat = 14;
    		} 
    		else if(direction_tracker==14)
    		{
    			tilestat = 15;
    		} 
    }
    
    return true;
}
    
void player::handleEvent( SDL_Event& e )
{

	bool turbo_mode=false;
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {

     if(turbo_mode==false)
     {
        	  switch( e.key.keysym.sym )
        	  {        
            	case SDLK_UP:    mVelY -= GAMBIT_VEL; break;
            	case SDLK_DOWN:  mVelY += GAMBIT_VEL; break;
            	case SDLK_LEFT:  mVelX -= GAMBIT_VEL; break;
            	case SDLK_RIGHT: mVelX += GAMBIT_VEL; break;
            
         		case SDLK_w: mVelY -= GAMBIT_VEL;  break;
        	     case SDLK_s: mVelY += GAMBIT_VEL;  break;
          	case SDLK_a: mVelX -= GAMBIT_VEL;  break;
        	     case SDLK_d: mVelX += GAMBIT_VEL;  break;
            }
       }
      else
      {
      	  switch( e.key.keysym.sym )
            {
               case SDLK_UP:    mVelY -= GAMBIT_RUN_VEL; break;
            	case SDLK_DOWN:  mVelY += GAMBIT_RUN_VEL; break;
            	case SDLK_LEFT:  mVelX -= GAMBIT_RUN_VEL; break;
            	case SDLK_RIGHT: mVelX += GAMBIT_RUN_VEL; break;
            
         		case SDLK_w: mVelY -= GAMBIT_RUN_VEL;  break;
        	     case SDLK_s: mVelY += GAMBIT_RUN_VEL;  break;
          	case SDLK_a: mVelX -= GAMBIT_RUN_VEL;  break;
        	     case SDLK_d: mVelX += GAMBIT_RUN_VEL;  break;
            }

        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        if(turbo_mode==false)
        {
        
        	  switch( e.key.keysym.sym )
        	  {
            	case SDLK_UP:    mVelY += GAMBIT_VEL; break;
            	case SDLK_DOWN:  mVelY -= GAMBIT_VEL; break;
            	case SDLK_LEFT:  mVelX += GAMBIT_VEL; break;
            	case SDLK_RIGHT: mVelX -= GAMBIT_VEL; break;
            
            	case SDLK_w: mVelY += GAMBIT_VEL;  break;
            	case SDLK_s: mVelY -= GAMBIT_VEL;  break;
            	case SDLK_a: mVelX += GAMBIT_VEL;  break;
            	case SDLK_d: mVelX -= GAMBIT_VEL;  break;
            }
         }
         else
         {
        	switch( e.key.keysym.sym )
        	  {
            	case SDLK_UP:    mVelY += GAMBIT_RUN_VEL; break;
            	case SDLK_DOWN:  mVelY -= GAMBIT_RUN_VEL; break;
            	case SDLK_LEFT:  mVelX += GAMBIT_RUN_VEL; break;
            	case SDLK_RIGHT: mVelX -= GAMBIT_RUN_VEL; break;
            
            	case SDLK_w: mVelY += GAMBIT_VEL;  break;
            	case SDLK_s: mVelY -= GAMBIT_VEL;  break;
            	case SDLK_a: mVelX += GAMBIT_VEL;  break;
            	case SDLK_d: mVelX -= GAMBIT_VEL;  break;
            	
            }
            
        }
    }
}

void player::move( Tile *tiles[] )
{
    //Move the dot left or right
    mBox.x += mVelX;

    //If the dot went too far to the left or right or touched a wall
    if( ( mBox.x < 0 ) || ( mBox.x + GAMBIT_WIDTH > LEVEL_WIDTH ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.x -= mVelX;
    }

    //Move the dot up or down
    mBox.y += mVelY;

    //If the dot went too far up or down or touched a wall
    if( ( mBox.y < 0 ) || ( mBox.y + GAMBIT_HEIGHT > LEVEL_HEIGHT ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.y -= mVelY;
    }
}

void player::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + GAMBIT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera.y = ( mBox.y + GAMBIT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void player::render( SDL_Rect& camera )
{
    //render player
	gGambitTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gGambitClips[tilestat] );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "red_rockit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia( Tile* tiles[])
{
	//Loading success flag
	bool success = true;

	//Load player texture
	if( !gGambitTexture.loadFromFile( "textures/player.png" ) )
	{
		printf( "Failed to load player texture!\n" );
		success = false;
	}
	
	
	//Load tile texture
	if( !gTileTexture.loadFromFile( "textures/tiles.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}
	

	//Load tile map
	if( !setTiles( tiles ) )
	{
		printf( "Failed to load tile set!\n" );
		success = false;
	}

	return success;
}

void close( Tile* tiles[], Tile* player_tile )
{
	//Deallocate map tiles
	for( int i = 0; i < TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] == NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}
	
	//deallocate player tile
	delete player_tile;

	//Free loaded images
	gGambitTexture.free();
	gTileTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setGambit( Tile* player_tile, player player )
{
	//Success flag
	bool playerLoaded = true;

    //The tile offsets
    int x = 0, y = 0;


    
    {
    		//Determines what kind of tile will be made
		int tileType = player.tilestat;

		
		//If the number is a valid tile number
		if( ( tileType >= 0 ) && ( tileType < TOTAL_GAMBIT_SPRITES ) )
		{
			player_tile = new Tile( x, y, tileType );
		}
		//If we don't recognize the tile type
		else
		{
			//Stop loading map
			printf( "Error loading player: Invalid tile type!" );
			playerLoaded = false;
		}
			
    		if(playerLoaded)
    		{
    			gGambitClips[ GAMBIT_DOWN    ].x = 0;
			gGambitClips[ GAMBIT_DOWN    ].y = 0;
			gGambitClips[ GAMBIT_DOWN    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_DOWN    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_DOWN1    ].x = GTILE_WIDTH;
			gGambitClips[ GAMBIT_DOWN1    ].y = 0;
			gGambitClips[ GAMBIT_DOWN1    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_DOWN1    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_DOWN2    ].x = GTILE_WIDTH*2;
			gGambitClips[ GAMBIT_DOWN2    ].y = 0;
			gGambitClips[ GAMBIT_DOWN2    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_DOWN2    ].h = GTILE_HEIGHT;

			gGambitClips[ GAMBIT_DOWN3    ].x = GTILE_WIDTH*3;
			gGambitClips[ GAMBIT_DOWN3    ].y = 0;
			gGambitClips[ GAMBIT_DOWN3    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_DOWN3    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_LEFT    ].x = 0;
			gGambitClips[ GAMBIT_LEFT    ].y = GTILE_HEIGHT;
			gGambitClips[ GAMBIT_LEFT    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_LEFT    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_LEFT1    ].x = GTILE_WIDTH;
			gGambitClips[ GAMBIT_LEFT1    ].y = GTILE_HEIGHT;
			gGambitClips[ GAMBIT_LEFT1    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_LEFT1    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_LEFT2    ].x = GTILE_WIDTH*2;
			gGambitClips[ GAMBIT_LEFT2    ].y = GTILE_HEIGHT;
			gGambitClips[ GAMBIT_LEFT2    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_LEFT2    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_LEFT3    ].x = GTILE_WIDTH*3;
			gGambitClips[ GAMBIT_LEFT3    ].y = GTILE_HEIGHT;
			gGambitClips[ GAMBIT_LEFT3    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_LEFT3    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_RIGHT    ].x = 0;
			gGambitClips[ GAMBIT_RIGHT    ].y = GTILE_HEIGHT*2;
			gGambitClips[ GAMBIT_RIGHT    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_RIGHT    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_RIGHT1    ].x = GTILE_WIDTH;
			gGambitClips[ GAMBIT_RIGHT1    ].y = GTILE_HEIGHT*2;
			gGambitClips[ GAMBIT_RIGHT1    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_RIGHT1    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_RIGHT2    ].x = GTILE_WIDTH*2;
			gGambitClips[ GAMBIT_RIGHT2    ].y = GTILE_HEIGHT*2;
			gGambitClips[ GAMBIT_RIGHT2    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_RIGHT2    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_RIGHT3    ].x = GTILE_WIDTH*3;
			gGambitClips[ GAMBIT_RIGHT3    ].y = GTILE_HEIGHT*2;
			gGambitClips[ GAMBIT_RIGHT3    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_RIGHT3    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_UP    ].x = 0;
			gGambitClips[ GAMBIT_UP    ].y = GTILE_HEIGHT*3;
			gGambitClips[ GAMBIT_UP    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_UP    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_UP1    ].x = GTILE_WIDTH;
			gGambitClips[ GAMBIT_UP1    ].y = GTILE_HEIGHT*3;
			gGambitClips[ GAMBIT_UP1    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_UP1    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_UP2    ].x = GTILE_WIDTH*2;
			gGambitClips[ GAMBIT_UP2    ].y = GTILE_HEIGHT*3;
			gGambitClips[ GAMBIT_UP2    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_UP2    ].h = GTILE_HEIGHT;
			
			gGambitClips[ GAMBIT_UP3    ].x = GTILE_WIDTH*3;
			gGambitClips[ GAMBIT_UP3    ].y = GTILE_HEIGHT*3;
			gGambitClips[ GAMBIT_UP3    ].w = GTILE_WIDTH;
			gGambitClips[ GAMBIT_UP3    ].h = GTILE_HEIGHT;
    		}
    }
    

    //If the map was loaded fine
    return playerLoaded;

}

bool setTiles( Tile* tiles[] )
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map( "maps/level1.map" );

    //If the map couldn't be loaded
    if( map==NULL )
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if( map.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
			{
				tiles[ i ] = new Tile( x, y, tileType );
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Clip the sprite sheet
		if( tilesLoaded )
		{
			gTileClips[ TILE_GRASS ].x = 0;
			gTileClips[ TILE_GRASS ].y = 0;
			gTileClips[ TILE_GRASS ].w = TILE_WIDTH;
			gTileClips[ TILE_GRASS ].h = TILE_HEIGHT;

			gTileClips[ TILE_GRASS_PLANT1 ].x = 0;
			gTileClips[ TILE_GRASS_PLANT1 ].y = 80;
			gTileClips[ TILE_GRASS_PLANT1 ].w = TILE_WIDTH;
			gTileClips[ TILE_GRASS_PLANT1 ].h = TILE_HEIGHT;

			gTileClips[ TILE_PATH ].x = 0;
			gTileClips[ TILE_PATH ].y = 160;
			gTileClips[ TILE_PATH ].w = TILE_WIDTH;
			gTileClips[ TILE_PATH ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_GRASS_TREE1 ].x = 0;
			gTileClips[ TILE_GRASS_TREE1 ].y = 240;
			gTileClips[ TILE_GRASS_TREE1 ].w = TILE_WIDTH;
			gTileClips[ TILE_GRASS_TREE1 ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_GRASS_TREE2 ].x = 0;
			gTileClips[ TILE_GRASS_TREE2 ].y = 320;
			gTileClips[ TILE_GRASS_TREE2 ].w = TILE_WIDTH;
			gTileClips[ TILE_GRASS_TREE2 ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_GRASS_TREE3 ].x = 0;
			gTileClips[ TILE_GRASS_TREE3 ].y = 400;
			gTileClips[ TILE_GRASS_TREE3 ].w = TILE_WIDTH;
			gTileClips[ TILE_GRASS_TREE3 ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOPLEFT ].x = 80;
			gTileClips[ TILE_TOPLEFT ].y = 0;
			gTileClips[ TILE_TOPLEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_LEFT ].x = 80;
			gTileClips[ TILE_LEFT ].y = 80;
			gTileClips[ TILE_LEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_LEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOMLEFT ].x = 80;
			gTileClips[ TILE_BOTTOMLEFT ].y = 160;
			gTileClips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOP ].x = 160;
			gTileClips[ TILE_TOP ].y = 0;
			gTileClips[ TILE_TOP ].w = TILE_WIDTH;
			gTileClips[ TILE_TOP ].h = TILE_HEIGHT;

			gTileClips[ TILE_CENTER ].x = 160;
			gTileClips[ TILE_CENTER ].y = 80;
			gTileClips[ TILE_CENTER ].w = TILE_WIDTH;
			gTileClips[ TILE_CENTER ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOM ].x = 160;
			gTileClips[ TILE_BOTTOM ].y = 160;
			gTileClips[ TILE_BOTTOM ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOM ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOPRIGHT ].x = 240;
			gTileClips[ TILE_TOPRIGHT ].y = 0;
			gTileClips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

			gTileClips[ TILE_RIGHT ].x = 240;
			gTileClips[ TILE_RIGHT ].y = 80;
			gTileClips[ TILE_RIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_RIGHT ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOMRIGHT ].x = 240;
			gTileClips[ TILE_BOTTOMRIGHT ].y = 160;
			gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_BOAT_PART1 ].x = 320;
			gTileClips[ TILE_BOAT_PART1 ].y = 0;
			gTileClips[ TILE_BOAT_PART1 ].w = TILE_WIDTH;
			gTileClips[ TILE_BOAT_PART1 ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_BOAT_PART2 ].x = 400;
			gTileClips[ TILE_BOAT_PART2 ].y = 0;
			gTileClips[ TILE_BOAT_PART2 ].w = TILE_WIDTH;
			gTileClips[ TILE_BOAT_PART2 ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_DOCK ].x = 320;
			gTileClips[ TILE_DOCK ].y = 80;
			gTileClips[ TILE_DOCK ].w = TILE_WIDTH;
			gTileClips[ TILE_DOCK ].h = TILE_HEIGHT;
			
			gTileClips[ TILE_PATH2 ].x = 320;
			gTileClips[ TILE_PATH2 ].y = 160;
			gTileClips[ TILE_PATH2 ].w = TILE_WIDTH;
			gTileClips[ TILE_PATH2 ].h = TILE_HEIGHT;
		}
	}

    //Close the file
    map.close();

    //If the map was loaded fine
    return tilesLoaded;
}

bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        if( ( tiles[ i ]->getType() >= TILE_CENTER ) && ( tiles[ i ]->getType() <= TILE_TOPLEFT ) )
        {
            //If the collision box touches the wall tile
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The level tiles
		Tile* tileSet[ TOTAL_TILES ];
		
		//player tiles
		Tile* player_tile;

		//Load media
		if( !loadMedia( tileSet ) )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//player, character under player control
			player player;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for player
					player.handleEvent( e );
				}

				//Move the character player
				player.move( tileSet );
				player.setCamera( camera );

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render level
				for( int i = 0; i < TOTAL_TILES; ++i )
				{
					tileSet[ i ]->render( camera );
				}

				//Render player
				if(!player.set_tilestat())
				{
					quit = true;
				}
				if(!setGambit(player_tile, player))
				{	
					quit = true;
				}
				player.render( camera );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
		
		//Free resources and close SDL
		close( tileSet , player_tile);
	}

	return 0;
}
