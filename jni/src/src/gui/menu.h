/***************************************************************************
 * menu.h
 *
 * Copyright (C) 2003 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SMC_MENU_H
#define SMC_MENU_H

#include "../core/global_basic.h"
#include "../video/animation.h"
#include "../video/video.h"
#include "../gui/hud.h"
#include "../core/camera.h"

namespace SMC
{

/* *** *** *** *** *** *** cMenu_Item *** *** *** *** *** *** *** *** *** *** *** */

class cMenu_Item : public cHudSprite
{
public:
	cMenu_Item( cSprite_Manager *sprite_manager );
	virtual ~cMenu_Item( void );

	// Sets the Active Modifier
	void Set_Active( bool active = 0 );
	// Draws the Menu Item
	virtual void Draw( cSurface_Request *request = NULL );

	// The menu images
	cHudSprite *m_image_default;
	// The additional Menu Graphic
	cHudSprite *m_image_menu;
	
	// if this item quits the menu
	bool m_is_quit;

private:
	// Is this Item active
	bool m_active;
};

typedef vector<cMenu_Item *> MenuList;

/* *** *** *** *** *** *** cMenuHandler *** *** *** *** *** *** *** *** *** *** *** */

/*
* handle dynamic Menu-Items
*/
class cMenuHandler
{
public:
	cMenuHandler( void );
	~cMenuHandler( void );

	// Adds a Menu
	void Add_Menu_Item( cMenu_Item *item, float shadow_pos = 0, Color shadow_color = static_cast<Uint8>(0) );

	// Unloads all items
	void Reset( void );

	/* Sets the Active Menu Item
	* if set to -1 nothing is active
	*/
	void Set_Active( int num );

	// Updates the Menu Mouse Collision detection
	void Update_Mouse( void );

	// Update
	void Update( void );
	// Draw
	void Draw( bool with_background = 1 );

	// Returns the currently active Menu Item
	cMenu_Item *Get_Active_Item( void );
	// Returns the number of loaded Menus
	unsigned int Get_Size( void ) const;

	// menu camera
	cCamera *m_camera;
	// menu level
	cLevel *m_level;
	// menu player (currently only used to set the pActive_Player)
	cSprite *m_player;

	/* The currently active Menu Item
	* if set to -1 nothing is active
	*/
	int m_active;

private:
	MenuList m_items;
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

class cMenuCore
{
public:
	cMenuCore( void );
	~cMenuCore( void );

	// Handle Input event
	bool Handle_Event( SDL_Event *ev );
	/* handle key down event
	 * returns true if processed
	*/
	bool Key_Down( SDLKey key );
	/* handle key up event
	 * returns true if processed
	*/
	bool Key_Up( SDLKey key );
	/* handle mouse button down event
	 * returns true if processed
	*/
	bool Mouse_Down( Uint8 button );
	/* handle mouse button up event
	 * returns true if processed
	*/
	bool Mouse_Up( Uint8 button );
	/* handle joystick button down event
	 * returns true if processed
	*/
	bool Joy_Button_Down( Uint8 button );
	/* handle joystick button up event
	 * returns true if processed
	*/
	bool Joy_Button_Up( Uint8 button );


	// Returns a Menu with the common image filenames
	cMenu_Item *Auto_Menu( std::string imagename, std::string imagename_menu, float ypos = 0, bool is_quit = 0 );

	/* Load the given Menu
	 * exit_gamemode : return to this game mode on exit
	*/
	void Load( const MenuID menu = MENU_MAIN, const GameMode exit_gamemode = MODE_NOTHING );
	// Unload
	void Unload( void );

	// Enter game mode
	void Enter( const GameMode old_mode = MODE_NOTHING );
	// Leave game mode
	void Leave( const GameMode next_mode = MODE_NOTHING );

	// Update current Menu
	void Update( void );
	// Draw current Menu
	void Draw( void );

	// current menu id
	MenuID m_menu_id;

	// Menu class
	cMenu_Base *m_menu_data;

	// Menu handler
	cMenuHandler *m_handler;
	// Menu Animation Manager
	cAnimation_Manager *m_animation_manager;
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

// The Menu
extern cMenuCore *pMenuCore;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
