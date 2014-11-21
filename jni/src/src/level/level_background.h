/***************************************************************************
 * level_background.h
 *
 * Copyright (C) 2005 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SMC_LEVEL_BACKGROUND_H
#define SMC_LEVEL_BACKGROUND_H

#include "../core/global_basic.h"
#include "../video/video.h"
#include "../core/obj_manager.h"
// CEGUI
#include "CEGUIXMLSerializer.h"

namespace SMC
{

/* *** *** *** *** *** *** *** Background Image Type *** *** *** *** *** *** *** *** *** *** */

enum BackgroundType
{
	BG_NONE = 0,			// nothing
	BG_IMG_TOP = 3,			// tiles only horizontal and is on the top
	BG_IMG_BOTTOM = 1,		// tiles only horizontal and is on the bottom
	BG_IMG_ALL = 2,			// tiles into all directions
	BG_GR_VER = 103,		// vertical gradient
	BG_GR_HOR = 104			// horizontal gradient
};

/* *** *** *** *** *** *** *** Background class *** *** *** *** *** *** *** *** *** *** */

class cBackground
{
public:
	// default constructor
	cBackground( cSprite_Manager *sprite_manager );
	// create from stream
	cBackground( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager );
	// destructor
	~cBackground( void );

	// Init defaults
	void Init( void );

	// load from stream
	void Load_From_XML( CEGUI::XMLAttributes &attributes );
	// save to stream
	void Save_To_XML( CEGUI::XMLSerializer &stream );

	// Set the parent sprite manager
	void Set_Sprite_Manager( cSprite_Manager *sprite_manager );
	// Sets the type of Background
	void Set_Type( const BackgroundType type );
	void Set_Type( const std::string &type );

	// Sets the background color
	void Set_Color_1( const Color &color );
	void Set_Color_2( const Color &color );

	// Set the Position
	void Set_Start_Pos( const float x, const float y );
	// Set the Position z
	void Set_Pos_Z( const float val );
	// Set the Background image
	void Set_Image( const std::string &img_file_1 );
	// Set the Background Image scrolling speed
	void Set_Scroll_Speed( const float x = 1.0f, const float y = 1.0f );

	// Set constant x velocity
	void Set_Const_Velocity_X( const float vel );
	// Set constant y velocity
	void Set_Const_Velocity_Y( const float vel );

	// Update
	void Update( void );
	// draw
	void Draw( void );
	// draw gradient
	void Draw_Gradient( void );

	// Returns the name of the current type
	std::string Get_Type_Name( void ) const;
	static std::string Get_Type_Name( const BackgroundType type );

	// the parent sprite manager
	cSprite_Manager *m_sprite_manager;
	// type
	BackgroundType m_type;
	// position
	float m_pos_x;
	float m_pos_y;
	// start position
	float m_start_pos_x;
	float m_start_pos_y;
	float m_pos_z;

	// - background image settings
	// image
	cGL_Surface *m_image_1;
	// image filename
	std::string m_image_1_filename;
	// scrolling speed
	float m_speed_x;
	float m_speed_y;
	// constant velocity
	float m_const_vel_x;
	float m_const_vel_y;

	// - background gradient settings
	// colors
	Color m_color_1;
	Color m_color_2;
};

/* *** *** *** *** *** cBackground_Manager *** *** *** *** *** *** *** *** *** *** *** *** */

class cBackground_Manager : public cObject_Manager<cBackground>
{
public:
	cBackground_Manager( void );
	virtual ~cBackground_Manager( void );
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
