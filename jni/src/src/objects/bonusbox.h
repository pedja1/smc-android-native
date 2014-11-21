/***************************************************************************
 * bonusbox.h
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

#ifndef SMC_BONUSBOX_H
#define SMC_BONUSBOX_H

#include "../core/global_basic.h"
#include "../objects/box.h"
#include "../objects/powerup.h"

namespace SMC
{

/* *** *** *** *** *** *** *** *** cBonusBox *** *** *** *** *** *** *** *** *** */

class cBonusBox : public cBaseBox
{
public:
	// constructor
	cBonusBox( cSprite_Manager *sprite_manager );
	// create from stream
	cBonusBox( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager );
	// destructor
	virtual ~cBonusBox( void );

	// init defaults
	void Init( void );

	// copy
	virtual cBonusBox *Copy( void ) const;

	// load from stream
	virtual void Load_From_XML( CEGUI::XMLAttributes &attributes );
	// save to stream
	virtual void Save_To_XML( CEGUI::XMLSerializer &stream );

	// sets the count this object can be activated
	virtual void Set_Useable_Count( int count, bool new_startcount = 0 );
	/* Set the bonus type
	 * TYPE_UNDEFINED			for empty
	 * TYPE_MUSHROOM_DEFAULT	for Mushroom
	 * TYPE_FIREPLANT			for Mushroom or Fireplant
	 * TYPE_MUSHROOM_BLUE		for Mushroom or Blue Mushroom
	 * TYPE_MUSHROOM_GHOST		for Mushroom or Ghost Mushroom
	 * TYPE_MUSHROOM_LIVE_1		for 1-UP Mushroom
	 * TYPE_JSTAR				for Star
	 * TYPE_GOLDPIECE			for Goldpiece
	 * TYPE_MUSHROOM_POISON		for Poison Mushroom
	*/
	void Set_Bonus_Type( SpriteType bonus_type );
	/* Set if to force the best possible item
	 * and not to use lower items automatically
	*/
	void Set_Force_Best_Item( bool enable );
	// Set the gold color
	void Set_Goldcolor( DefaultColor new_color );

	// Activates the bonus item
	virtual void Activate( void );

	// update
	virtual void Update( void );
	// draw
	virtual void Draw( cSurface_Request *request = NULL );

	// if update is valid for the current state
	virtual bool Is_Update_Valid( void );

	// editor activation
	virtual void Editor_Activate( void );
	// editor state update
	virtual void Editor_State_Update( void );
	// editor animation option selected event
	bool Editor_Animation_Select( const CEGUI::EventArgs &event );
	// editor item option selected event
	bool Editor_Item_Select( const CEGUI::EventArgs &event );
	// editor force best item option selected event
	bool Editor_Force_best_item_Select( const CEGUI::EventArgs &event );
	// editor gold color option selected event
	bool Editor_Gold_Color_Select( const CEGUI::EventArgs &event );

	// force best possible item
	bool m_force_best_item;

	/* active items
	 * list of active item is available
	 * and gets added to the level objects if finished
	*/
	typedef vector<cMovingSprite *> MovingSpriteList;
	MovingSpriteList m_active_items;
	// Goldpiece color
	DefaultColor m_gold_color;
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
