/***************************************************************************
 * goldpiece.h
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

#ifndef SMC_GOLDPIECE_H
#define SMC_GOLDPIECE_H

#include "../core/global_basic.h"
#include "../objects/animated_sprite.h"

namespace SMC
{

/* *** *** *** *** *** cGoldpiece *** *** *** *** *** *** *** *** *** *** *** *** */

class cGoldpiece : public cAnimated_Sprite
{
public:
	// constructor
	cGoldpiece( cSprite_Manager *sprite_manager );
	// create from stream
	cGoldpiece( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager );
	// destructor
	virtual ~cGoldpiece( void );

	// init defaults
	void Init( void );
	// load from stream
	virtual void Load_From_XML( CEGUI::XMLAttributes &attributes );
	// save to stream
	virtual void Save_To_XML( CEGUI::XMLSerializer &stream );

	// copy
	virtual cGoldpiece *Copy( void ) const;

	// load from savegame
	virtual void Load_From_Savegame( cSave_Level_Object *save_object );
	// save to savegame
	virtual cSave_Level_Object *Save_To_Savegame( void );

	// Set the gold color
	virtual void Set_Gold_Color( DefaultColor color );

	// activate the goldpiece
	void Activate( void );

	// update
	virtual void Update( void );
	// draw
	virtual void Draw( cSurface_Request *request = NULL );

	// if update is valid for the current state
	virtual bool Is_Update_Valid( void );

	// collision from player
	virtual void Handle_Collision_Player( cObjectCollision *collision );

	// gold color
	DefaultColor m_color_type;
};

/* *** *** *** *** *** cJGoldpiece *** *** *** *** *** *** *** *** *** *** *** *** */

/* Jumping Goldpiece
 * used by Goldbox
*/
class cJGoldpiece : public cGoldpiece
{
public:
	cJGoldpiece( cSprite_Manager *sprite_manager );
	virtual ~cJGoldpiece( void );
	
	// update
	virtual void Update( void );

	/* Validate the given collision object
	 * returns 0 if not valid
	 * returns 1 if an internal collision with this object is valid
	 * returns 2 if the given object collides with this object (blocking)
	*/
	virtual Col_Valid_Type Validate_Collision( cSprite *obj );
};

/* *** *** *** *** *** cFGoldpiece *** *** *** *** *** *** *** *** *** *** *** *** */

// Falling Goldpiece
class cFGoldpiece : public cGoldpiece
{
public:
	// if direction is undefined it moves into a random direction
	cFGoldpiece( cSprite_Manager *sprite_manager, ObjectDirection dir = DIR_UNDEFINED );
	virtual ~cFGoldpiece( void );

	// update
	virtual void Update( void );

	/* Validate the given collision object
	 * returns 0 if not valid
	 * returns 1 if an internal collision with this object is valid
	 * returns 2 if the given object collides with this object (blocking)
	*/
	virtual Col_Valid_Type Validate_Collision( cSprite *obj );
	// collision with massive
	virtual void Handle_Collision_Massive( cObjectCollision *collision );
	// collision from a box
	virtual void Handle_Collision_Box( ObjectDirection cdirection, GL_rect *r2 );
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
