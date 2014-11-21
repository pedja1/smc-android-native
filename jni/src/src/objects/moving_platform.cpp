/***************************************************************************
 * moving_platform.cpp  -  default moving platforms handler
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

#include "../objects/moving_platform.h"
#include "../core/camera.h"
#include "../core/game_core.h"
#include "../video/gl_surface.h"
#include "../core/framerate.h"
#include "../video/renderer.h"
#include "../user/savegame.h"
#include "../core/i18n.h"
#include "../level/level_player.h"
#include "../core/sprite_manager.h"
#include "../level/level.h"
#include "../objects/path.h"
#include "../input/mouse.h"
// CEGUI
#include "CEGUIWindowManager.h"
#include "elements/CEGUICombobox.h"
#include "elements/CEGUIListboxTextItem.h"
#include "elements/CEGUIEditbox.h"

namespace SMC
{

/* *** *** *** *** *** *** *** cMoving_Platform *** *** *** *** *** *** *** *** *** *** */

cMoving_Platform :: cMoving_Platform( cSprite_Manager *sprite_manager )
: cAnimated_Sprite( sprite_manager, "moving_platform" ), m_path_state( sprite_manager )
{
	cMoving_Platform::Init();
}

cMoving_Platform :: cMoving_Platform( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager )
: cAnimated_Sprite( sprite_manager, "moving_platform" ), m_path_state( sprite_manager )
{
	cMoving_Platform::Init();
	cMoving_Platform::Load_From_XML( attributes );
}

cMoving_Platform :: ~cMoving_Platform( void )
{
	//
}

void cMoving_Platform :: Init( void )
{
	m_sprite_array = ARRAY_ACTIVE;
	m_type = TYPE_MOVING_PLATFORM;
	m_pos_z = 0.085f;
	m_gravity_max = 25.0f;
	m_can_be_on_ground = 0;

	m_camera_range = 3000;
	m_can_be_ground = 1;

	m_move_type = MOVING_PLATFORM_TYPE_LINE;
	m_platform_state = MOVING_PLATFORM_STAY;
	m_moving_angle = 0.0f;
	m_touch_counter = 0.0f;
	m_shake_dir_counter = 0.0f;
	m_shake_dir = 0;
	m_shake_counter = 0.0f;

	m_max_distance_slow_down_pos = 0.0f;
	m_lowest_speed = 0.0f;
	m_editor_color = Color( static_cast<Uint8>(100), 150, 200, 128 );

	Set_Massive_Type( MASS_HALFMASSIVE );
	Set_Max_Distance( 150 );
	Set_Speed( 3.0f );
	Set_Touch_Time( 0.0f );
	Set_Shake_Time( 16.0f );
	Set_Touch_Move_Time( 0.0f );

	Set_Direction( DIR_RIGHT, 1 );

	Set_Middle_Count( 4 );
	// create default images
	Add_Image( pVideo->Get_Surface( "ground/green_1/slider/1/brown/left.png" ) );
	Add_Image( pVideo->Get_Surface( "ground/green_1/slider/1/brown/middle.png" ) );
	Add_Image( pVideo->Get_Surface( "ground/green_1/slider/1/brown/right.png" ) );
	Set_Image_Num( 1, 1 );

	Update_Rect();

	Create_Name();
}

void cMoving_Platform :: Init_Links( void )
{
	// link to parent path
	m_path_state.Set_Path_Identifier( m_path_state.m_path_identifier );
}

cMoving_Platform *cMoving_Platform :: Copy( void ) const
{
	cMoving_Platform *moving_platform = new cMoving_Platform( m_sprite_manager );
	moving_platform->Set_Pos( m_start_pos_x, m_start_pos_y, 1 );
	moving_platform->Set_Move_Type( m_move_type );
	moving_platform->Set_Path_Identifier( m_path_state.m_path_identifier );
	moving_platform->Set_Massive_Type( m_massive_type );
	moving_platform->Set_Direction( m_start_direction, 1 );
	moving_platform->Set_Max_Distance( m_max_distance );
	moving_platform->Set_Speed( m_speed );
	moving_platform->Set_Touch_Time( m_touch_time );
	moving_platform->Set_Shake_Time( m_shake_time );
	moving_platform->Set_Touch_Move_Time( m_touch_move_time );
	moving_platform->Set_Middle_Count( m_middle_count );
	moving_platform->Set_Image_Top_Left( m_images[0].m_image );
	moving_platform->Set_Image_Top_Middle( m_images[1].m_image );
	moving_platform->Set_Image_Top_Right( m_images[2].m_image );
	return moving_platform;
}

void cMoving_Platform :: Load_From_XML( CEGUI::XMLAttributes &attributes )
{
	// position
	Set_Pos( static_cast<float>(attributes.getValueAsInteger( "posx" )), static_cast<float>(attributes.getValueAsInteger( "posy" )), 1 );
	// move type
	Set_Move_Type( static_cast<Moving_Platform_Type>(attributes.getValueAsInteger( "move_type", m_move_type )) );
	// massive type
	Set_Massive_Type( Get_Massive_Type_Id( attributes.getValueAsString( "massive_type", Get_Massive_Type_Name( m_massive_type ) ).c_str() ) );
	// if move type is line or circle
	if( m_move_type == MOVING_PLATFORM_TYPE_LINE || m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
	{
		// direction
		Set_Direction( Get_Direction_Id( attributes.getValueAsString( "direction", Get_Direction_Name( m_start_direction ) ).c_str() ), 1 );
		// max distance
		Set_Max_Distance( attributes.getValueAsInteger( "max_distance", m_max_distance ) );
	}
	// path identifier
	if( m_move_type == MOVING_PLATFORM_TYPE_PATH || m_move_type == MOVING_PLATFORM_TYPE_PATH_BACKWARDS )
	{
		Set_Path_Identifier( attributes.getValueAsString( "path_identifier" ).c_str() );
	}
	// speed
	Set_Speed( attributes.getValueAsFloat( "speed", m_speed ) );
	// touch_time
	Set_Touch_Time( attributes.getValueAsFloat( "touch_time", m_touch_time ) );
	// shake time
	Set_Shake_Time( attributes.getValueAsFloat( "shake_time", m_shake_time ) );
	// touch move time
	Set_Touch_Move_Time( attributes.getValueAsFloat( "touch_move_time", m_touch_move_time ) );
	// middle image count
	Set_Middle_Count( attributes.getValueAsInteger( "middle_img_count", m_middle_count ) );
	// image top left
	Set_Image_Top_Left( pVideo->Get_Surface( attributes.getValueAsString( "image_top_left", m_images[0].m_image->Get_Filename() ).c_str() ) );
	// image top middle
	Set_Image_Top_Middle( pVideo->Get_Surface( attributes.getValueAsString( "image_top_middle", m_images[1].m_image->Get_Filename() ).c_str() ) );
	// image top right
	Set_Image_Top_Right( pVideo->Get_Surface( attributes.getValueAsString( "image_top_right", m_images[2].m_image->Get_Filename() ).c_str() ) );
}

void cMoving_Platform :: Save_To_XML( CEGUI::XMLSerializer &stream )
{
	// begin
	stream.openTag( m_type_name );

	// position
	Write_Property( stream, "posx", static_cast<int>( m_start_pos_x ) );
	Write_Property( stream, "posy", static_cast<int>( m_start_pos_y ) );
	// move type
	Write_Property( stream, "move_type", m_move_type );
	// massive type
	Write_Property( stream, "massive_type", Get_Massive_Type_Name( m_massive_type ) );

	// path identifier
	if( m_move_type == MOVING_PLATFORM_TYPE_PATH || m_move_type == MOVING_PLATFORM_TYPE_PATH_BACKWARDS )
	{
		if( !m_path_state.m_path_identifier.empty() )
		{
			Write_Property( stream, "path_identifier", m_path_state.m_path_identifier );
		}
	}
	// if move type is line or circle
	if( m_move_type == MOVING_PLATFORM_TYPE_LINE || m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
	{
		// direction
		Write_Property( stream, "direction", Get_Direction_Name( m_start_direction ) );
		// max distance
		Write_Property( stream, "max_distance", m_max_distance );
	}
	// speed
	Write_Property( stream, "speed", m_speed );
	// touch time
	Write_Property( stream, "touch_time", m_touch_time );
	// shake time
	Write_Property( stream, "shake_time", m_shake_time );
	// touch move time
	Write_Property( stream, "touch_move_time", m_touch_move_time );
	// middle image count
	Write_Property( stream, "middle_img_count", m_middle_count );
	// image top left
	Write_Property( stream, "image_top_left", m_images[0].m_image->Get_Filename( 1 ) );
	// image top middle
	Write_Property( stream, "image_top_middle", m_images[1].m_image->Get_Filename( 1 ) );
	// image top right
	Write_Property( stream, "image_top_right", m_images[2].m_image->Get_Filename( 1 ) );

	// end
	stream.closeTag();
}

void cMoving_Platform :: Set_Sprite_Manager( cSprite_Manager *sprite_manager )
{
	cSprite::Set_Sprite_Manager( sprite_manager );
	m_path_state.Set_Sprite_Manager( sprite_manager );
}

void cMoving_Platform :: Load_From_Savegame( cSave_Level_Object *save_object )
{
	// direction
	if( save_object->exists( "direction" ) )
	{
		m_direction = static_cast<ObjectDirection>(string_to_int( save_object->Get_Value( "direction" ) ));
	}

	// platform state
	if( save_object->exists( "platform_state" ) )
	{
		m_platform_state = static_cast<Moving_Platform_State>(string_to_int( save_object->Get_Value( "platform_state" ) ));
	}

	// new position x
	if( save_object->exists( "new_posx" ) )
	{
		Set_Pos_X( string_to_float( save_object->Get_Value( "new_posx" ) ) );
	}

	// new position y
	if( save_object->exists( "new_posy" ) )
	{
		Set_Pos_Y( string_to_float( save_object->Get_Value( "new_posy" ) ) );
	}

	// velocity x
	if( save_object->exists( "velx" ) )
	{
		m_velx = string_to_float( save_object->Get_Value( "velx" ) );
	}

	// velocity y
	if( save_object->exists( "vely" ) )
	{
		m_vely = string_to_float( save_object->Get_Value( "vely" ) );
	}

	// active
	if( save_object->exists( "active" ) )
	{
		Set_Active( string_to_int( save_object->Get_Value( "active" ) ) > 0 );
	}

	// path state
	m_path_state.Load_From_Savegame( save_object );
}

cSave_Level_Object *cMoving_Platform :: Save_To_Savegame( void )
{
	cSave_Level_Object *save_object = new cSave_Level_Object();

	// default values
	save_object->m_type = m_type;
	save_object->m_properties.push_back( cSave_Level_Object_Property( "posx", int_to_string( static_cast<int>(m_start_pos_x) ) ) );
	save_object->m_properties.push_back( cSave_Level_Object_Property( "posy", int_to_string( static_cast<int>(m_start_pos_y) ) ) );

	// direction
	save_object->m_properties.push_back( cSave_Level_Object_Property( "direction", int_to_string( m_direction ) ) );

	// platform state
	save_object->m_properties.push_back( cSave_Level_Object_Property( "platform_state", int_to_string( m_platform_state ) ) );

	// new position ( only save if needed )
	if( m_start_pos_x != m_pos_x || m_start_pos_y != m_pos_y )
	{
		save_object->m_properties.push_back( cSave_Level_Object_Property( "new_posx", int_to_string( static_cast<int>(m_pos_x) ) ) );
		save_object->m_properties.push_back( cSave_Level_Object_Property( "new_posy", int_to_string( static_cast<int>(m_pos_y) ) ) );
	}

	// velocity
	save_object->m_properties.push_back( cSave_Level_Object_Property( "velx", float_to_string( m_velx ) ) );
	save_object->m_properties.push_back( cSave_Level_Object_Property( "vely", float_to_string( m_vely ) ) );

	// active ( only save if needed )
	if( !m_active )
	{
		save_object->m_properties.push_back( cSave_Level_Object_Property( "active", int_to_string( m_active ) ) );
	}

	// path state
	m_path_state.Save_To_Savegame( save_object );

	return save_object;
}

void cMoving_Platform :: Set_Move_Type( Moving_Platform_Type move_type )
{
	m_move_type = move_type;

	if( m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
	{
		if( m_start_direction == DIR_UP )
		{
			Set_Direction( DIR_LEFT, 1 );
		}
		else if( m_start_direction == DIR_DOWN )
		{
			Set_Direction( DIR_RIGHT, 1 );
		}
	}
}

void cMoving_Platform :: Set_Path_Identifier( const std::string &identifier )
{
	m_path_state.Set_Path_Identifier( identifier );
	Set_Velocity( 0, 0 );
}

void cMoving_Platform :: Set_Massive_Type( MassiveType new_type )
{
	m_massive_type = new_type;

	if( m_massive_type == MASS_MASSIVE )
	{
		m_can_be_ground = 1;
	}
	else if( m_massive_type == MASS_PASSIVE )
	{
		m_can_be_ground = 0;
	}
	else if( m_massive_type == MASS_HALFMASSIVE )
	{
		m_can_be_ground = 1;
	}
	else if( m_massive_type == MASS_CLIMBABLE )
	{
		m_can_be_ground = 0;
	}

	Create_Name();
}

void cMoving_Platform :: Set_Direction( const ObjectDirection dir, bool new_start_direction /* = 0 */ )
{
	// save old direction
	ObjectDirection start_direction_old = m_start_direction;
	// set direction
	cAnimated_Sprite::Set_Direction( dir, new_start_direction );

	if( new_start_direction )
	{
		// changed direction
		if( start_direction_old != m_start_direction )
		{
			// set back to start position
			Set_Pos( m_start_pos_x, m_start_pos_y );
			// reset velocity
			Set_Velocity( 0.0f, 0.0f );
			// update name
			Create_Name();
		}
	}

	// Set new velocity
	Update_Velocity();
}

void cMoving_Platform :: Set_Max_Distance( int new_max_distance )
{
	if( new_max_distance < 0 )
	{
		new_max_distance = 0;
	}

	m_max_distance = new_max_distance;
	// set slow down position
	m_max_distance_slow_down_pos = m_max_distance * 0.2f;
}

void cMoving_Platform :: Set_Speed( float val )
{
	m_speed = val;
	m_lowest_speed = m_speed * 0.3f;

	// set velocity
	Update_Velocity();

	Create_Name();
}

void cMoving_Platform :: Set_Touch_Time( float val )
{
	m_touch_time = val;

	if( m_touch_time < 0 )
	{
		m_touch_time = 0;
	}

	Create_Name();
}

void cMoving_Platform :: Set_Shake_Time( float val )
{
	m_shake_time = val;

	if( m_shake_time < 0 )
	{
		m_shake_time = 0;
	}
}

void cMoving_Platform :: Set_Touch_Move_Time( float val )
{
	m_touch_move_time = val;

	if( m_touch_move_time < 0 )
	{
		m_touch_move_time = 0;
	}

	// set back start position
	Set_Pos( m_start_pos_x, m_start_pos_y );
	// reset velocity
	Set_Velocity( 0, 0 );

	Create_Name();
}

void cMoving_Platform :: Set_Middle_Count( const unsigned int val )
{
	m_middle_count = val;

	if( m_middle_count > 128 )
	{
		m_middle_count = 128;
	}
}

void cMoving_Platform :: Set_Image_Top_Left( cGL_Surface *surface )
{
	if( !surface )
	{
		return;
	}

	m_images[0].m_image = surface;

	// top left image sets main image
	m_curr_img = -1;
	Set_Image_Num( 0, 1 );

	Update_Rect();
}

void cMoving_Platform :: Set_Image_Top_Middle( cGL_Surface *surface )
{
	if( !surface )
	{
		return;
	}

	m_images[1].m_image = surface;
	Update_Rect();
}

void cMoving_Platform :: Set_Image_Top_Right( cGL_Surface *surface )
{
	if( !surface )
	{
		return;
	}

	m_images[2].m_image = surface;
	Update_Rect();
}

void cMoving_Platform :: Update( void )
{
	if( !m_valid_update || !Is_In_Range() )
	{
		return;
	}

	// move time should be smaller than touch time
	// because it should start moving (if it is moving) before shaking starts.
	// if larger than touch time then the platform state will change 
	// and touch_counter will never be more than move_time
	if( m_touch_time > 0.0f || m_touch_move_time > 0.0f )
	{
		if( m_platform_state == MOVING_PLATFORM_TOUCHED )
		{
			m_touch_counter += pFramerate->m_speed_factor;
		}
	}

	// if able to fall
	if( m_touch_time > 0.0f )
	{
		if( m_platform_state == MOVING_PLATFORM_TOUCHED && m_touch_counter > m_touch_time )
		{
			m_platform_state = MOVING_PLATFORM_SHAKE;
			m_shake_dir = 0;
		}
		else if( m_platform_state == MOVING_PLATFORM_SHAKE )
		{
			m_shake_counter += pFramerate->m_speed_factor;

			// move right
			if( m_shake_dir == 0 )
			{
				m_shake_dir_counter += 8 * pFramerate->m_speed_factor;

				if( m_shake_dir_counter > 3.0f )
				{
					m_shake_dir_counter = 3.0f;
					// reverse
					m_shake_dir = !m_shake_dir;
				}
			}
			// move left
			else
			{
				m_shake_dir_counter -= 8 * pFramerate->m_speed_factor;

				if( m_shake_dir_counter < -3.0f )
				{
					m_shake_dir_counter = -3.0f;
					// reverse
					m_shake_dir = !m_shake_dir;
				}
			}
			
			if( m_shake_counter > m_shake_time )
			{
				m_platform_state = MOVING_PLATFORM_FALL;
				// todo : change massivetype so no object can collide with it anymore but this needs a start_massivetype for saving
			}
		}
		else if( m_platform_state == MOVING_PLATFORM_FALL )
		{
			if( m_vely < m_gravity_max )
			{
				Add_Velocity_Y_Max( 2.1f, m_gravity_max );
			}

			// drop objects when falling fast
			if( m_can_be_ground && m_vely > 15.0f )
			{
				m_can_be_ground = 0;
			}

			if( m_pos_x > m_start_pos_x )
			{
				Add_Rotation_Z( 1.5f * pFramerate->m_speed_factor );
			}
			else
			{
				Add_Rotation_Z( -1.5f * pFramerate->m_speed_factor );
			}

			// below screen (+300 to be sure nothing can collide with it anymore)
			if( m_col_rect.m_y > pActive_Camera->m_limit_rect.m_y + game_res_h + 300.0f )
			{
				m_platform_state = MOVING_PLATFORM_STAY;
				m_can_be_ground = 0;
				Set_Active( 0 );
			}
		}
	}

	// if moving
	if( m_speed && m_platform_state != MOVING_PLATFORM_FALL && ( !m_touch_move_time || m_touch_counter > m_touch_move_time ) )
	{
		if( m_move_type == MOVING_PLATFORM_TYPE_LINE )
		{
			// distance to final position
			float dist_to_final_pos = 0.0f;

			if( m_start_direction == DIR_LEFT )
			{
				if( m_direction == DIR_LEFT )
				{
					dist_to_final_pos = m_max_distance - ( m_start_pos_x - m_pos_x );
				}
				else if( m_direction == DIR_RIGHT )
				{
					dist_to_final_pos = m_start_pos_x - m_pos_x;
				}
			}
			else if( m_start_direction == DIR_RIGHT )
			{
				if( m_direction == DIR_LEFT )
				{
					dist_to_final_pos = 0 - ( m_start_pos_x - m_pos_x );
				}
				else if( m_direction == DIR_RIGHT )
				{
					dist_to_final_pos = m_max_distance + ( m_start_pos_x - m_pos_x );
				}
			}
			if( m_start_direction == DIR_UP )
			{
				if( m_direction == DIR_UP )
				{
					dist_to_final_pos = m_max_distance - ( m_start_pos_y - m_pos_y );
				}
				else if( m_direction == DIR_DOWN )
				{
					dist_to_final_pos = m_start_pos_y - m_pos_y;
				}
			}
			else if( m_start_direction == DIR_DOWN )
			{
				if( m_direction == DIR_UP )
				{
					dist_to_final_pos = 0 - ( m_start_pos_y - m_pos_y );
				}
				else if( m_direction == DIR_DOWN )
				{
					dist_to_final_pos = m_max_distance + ( m_start_pos_y - m_pos_y );
				}
			}

			// reached final position
			if( dist_to_final_pos <= 0 )
			{
				// unset velocity
				Set_Velocity( 0, 0 );
				// set new direction
				Set_Direction( Get_Opposite_Direction( m_direction ) );
			}
			// slow down near final position
			else if( dist_to_final_pos < m_max_distance_slow_down_pos )
			{
				if( m_direction == DIR_DOWN )
				{
					if( m_vely > 0.0f )
					{
						// slow down
						if( m_vely > m_lowest_speed )
						{
							m_vely *= 1 - ( 0.05f * pFramerate->m_speed_factor );

							// reached lowest value
							if( m_vely <= m_lowest_speed )
							{
								m_vely = m_lowest_speed;
							}
						}
					}
					else
					{
						// set lowest value
						m_vely = m_lowest_speed;
					}
				}
				else if( m_direction == DIR_UP )
				{
					if( m_vely < 0.0f )
					{
						// slow down
						if( m_vely < -m_lowest_speed )
						{
							m_vely *= 1 - ( 0.05f * pFramerate->m_speed_factor );

							// reached lowest value
							if( m_vely >= -m_lowest_speed )
							{
								m_vely = -m_lowest_speed;
							}
						}
					}
					else
					{
						// set lowest value
						m_vely = -m_lowest_speed;
					}
				}
				else if( m_direction == DIR_LEFT )
				{
					if( m_velx < 0.0f )
					{
						// slow down
						if( m_velx < -m_lowest_speed )
						{
							m_velx *= 1 - ( 0.05f * pFramerate->m_speed_factor );

							// reached lowest value
							if( m_velx >= -m_lowest_speed )
							{
								m_velx = -m_lowest_speed;
							}
						}
					}
					else
					{
						// set lowest value
						m_velx = -m_lowest_speed;
					}
				}
				else if( m_direction == DIR_RIGHT )
				{
					if( m_velx > 0.0f )
					{
						// slow down
						if( m_velx > m_lowest_speed )
						{
							m_velx *= 1 - ( 0.05f * pFramerate->m_speed_factor );

							// reached lowest value
							if( m_velx <= m_lowest_speed )
							{
								m_velx = m_lowest_speed;
							}
						}
					}
					else
					{
						// set lowest value
						m_velx = m_lowest_speed;
					}
				}
			}
			// not near final position
			else
			{
				Update_Velocity();
			}
		}
		else if( m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
		{
			// increase angle
			if( m_direction == DIR_RIGHT )
			{
				m_moving_angle += m_speed * pFramerate->m_speed_factor;

				if( m_moving_angle > 360.0f )
				{
					m_moving_angle -= 360.0f;
				}
			}
			else
			{
				m_moving_angle -= m_speed * pFramerate->m_speed_factor;

				if( m_moving_angle < 0.0f )
				{
					m_moving_angle += 360.0f;
				}
			}

			// get difference to new position
			float diff_x = ( m_start_pos_x + ( m_max_distance * sin( m_moving_angle * deg_to_rad ) ) ) - m_pos_x;
			float diff_y = ( m_start_pos_y - m_max_distance + ( m_max_distance * cos( m_moving_angle * deg_to_rad ) ) ) - m_pos_y;

			// move to position
			Set_Velocity( diff_x, diff_y );
		}
		else if( m_move_type == MOVING_PLATFORM_TYPE_PATH || m_move_type == MOVING_PLATFORM_TYPE_PATH_BACKWARDS )
		{
			if( m_path_state.m_path )
			{
				// move along path
				if( m_path_state.Path_Move( m_speed * pFramerate->m_speed_factor ) == 0 )
				{
					if( !m_path_state.m_path->m_rewind )
					{
						// if we can not move further along the path, reverse the direction
						m_path_state.Move_Toggle();
					}
				}

				// get difference
				float diff_x = ( m_path_state.m_path->m_start_pos_x + m_path_state.m_pos_x ) - m_pos_x;
				float diff_y = ( m_path_state.m_path->m_start_pos_y + m_path_state.m_pos_y ) - m_pos_y;

				// move to position
				Set_Velocity( diff_x, diff_y );
			}
		}
	}
}

void cMoving_Platform :: Draw( cSurface_Request *request /* = NULL */ )
{
	if( !m_valid_draw )
	{
		return;
	}

	// current path state position
	if( game_debug )
	{
		m_path_state.Draw();
	}

	// platform images
	cSurface_Request *surface_request;
	float x = 0;

	if( !editor_level_enabled )
	{
		// shake
		x += m_shake_dir_counter;
	}

	// top left
	if( m_images[0].m_image )
	{
		// Start
		m_image = m_images[0].m_image;
		m_start_image = m_image;
		// create request

		surface_request = new cSurface_Request();
		// draw only first image complete
		cAnimated_Sprite::Draw( surface_request );
		surface_request->m_pos_x += x;
		x += m_images[0].m_image->m_w;
		// add request
		pRenderer->Add( surface_request );
	}

	// top middle
	if( m_images[1].m_image )
	{
		// Middle
		m_image = m_images[1].m_image;
		m_start_image = m_image;
		for( unsigned int i = 0; i < m_middle_count; i++ )
		{
			// create request
			surface_request = new cSurface_Request();
			cAnimated_Sprite::Draw_Image( surface_request);
			surface_request->m_pos_x += x;
			x += m_images[1].m_image->m_w;
			// add request
			pRenderer->Add( surface_request );
		}
	}

	// top right
	if( m_images[2].m_image )
	{
		// End
		m_image = m_images[2].m_image;
		m_start_image = m_image;
		// create request
		surface_request = new cSurface_Request();
		cAnimated_Sprite::Draw_Image( surface_request );
		surface_request->m_pos_x += x;
		//x += m_images[2]->w;
		// add request
		pRenderer->Add( surface_request );
	}

	Update_Rect();
	Update_Position_Rect();

	// draw distance rect
	if( editor_level_enabled && m_speed )
	{
		if( m_move_type == MOVING_PLATFORM_TYPE_LINE )
		{
			if( m_start_direction == DIR_RIGHT )
			{
				pVideo->Draw_Rect( m_start_pos_x - pActive_Camera->m_x, m_start_pos_y - pActive_Camera->m_y, m_max_distance + m_col_rect.m_w, 2, m_pos_z - 0.0001f, &m_editor_color );
			}
			else if( m_start_direction == DIR_DOWN )
			{
				pVideo->Draw_Rect( m_start_pos_x - pActive_Camera->m_x, m_start_pos_y - pActive_Camera->m_y, 2, m_max_distance + m_col_rect.m_h, m_pos_z - 0.0001f, &m_editor_color );
			}
			// added in version 1.7
			else if( m_start_direction == DIR_UP )
			{
				pVideo->Draw_Rect( m_start_pos_x - pActive_Camera->m_x, m_start_pos_y - pActive_Camera->m_y - m_max_distance, 2, m_max_distance + m_col_rect.m_h, m_pos_z - 0.0001f, &m_editor_color );
			}
			// added in version 1.7
			else if( m_start_direction == DIR_LEFT )
			{
				pVideo->Draw_Rect( m_start_pos_x - pActive_Camera->m_x - m_max_distance, m_start_pos_y - pActive_Camera->m_y, m_max_distance + m_col_rect.m_w, 2, m_pos_z - 0.0001f, &m_editor_color );
			}
		}
		else if( m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
		{
			// circle
			cCircle_Request *circle_request = new cCircle_Request();
			pVideo->Draw_Circle( m_start_pos_x - pActive_Camera->m_x, m_start_pos_y - m_max_distance - pActive_Camera->m_y, static_cast<float>(m_max_distance), m_pos_z - 0.0001f, &m_editor_color, circle_request );
			circle_request->m_line_width = 2;
			// add request
			pRenderer->Add( circle_request );
		}
	}
}

void cMoving_Platform :: Update_Rect( void )
{
	// clear
	m_col_rect.m_w = 0;
	m_rect.m_w = 0;

	// get width
	if( m_images[0].m_image )
	{
		m_rect.m_w += m_images[0].m_image->m_w;
	}
	if( m_images[1].m_image )
	{
		m_rect.m_w += m_images[1].m_image->m_w * m_middle_count;
	}
	if( m_images[2].m_image )
	{
		m_rect.m_w += m_images[2].m_image->m_w;
	}

	// set width
	m_col_rect.m_w = m_rect.m_w;
	m_start_rect.m_w = m_rect.m_w;
}

void cMoving_Platform :: Update_Velocity( void )
{
	if( m_move_type != MOVING_PLATFORM_TYPE_LINE )
	{
		return;
	}

	// set velocity
	if( m_speed > 0 && ( !m_touch_move_time || m_touch_counter > m_touch_move_time ) )
	{
		if( m_direction == DIR_UP )
		{
			if( m_vely > -m_speed )
			{
				m_vely += -m_speed * 0.1f * pFramerate->m_speed_factor;
			}
		}
		else if( m_direction == DIR_DOWN )
		{
			if( m_vely < m_speed )
			{
				m_vely += m_speed * 0.1f * pFramerate->m_speed_factor;
			}
		}
		else if( m_direction == DIR_LEFT )
		{
			if( m_velx > -m_speed )
			{
				m_velx += -m_speed * 0.1f * pFramerate->m_speed_factor;
			}
		}
		else if( m_direction == DIR_RIGHT )
		{
			if( m_velx < m_speed )
			{
				m_velx += m_speed * 0.1f * pFramerate->m_speed_factor;
			}
		}
	}
	else
	{
		// unset velocity
		Set_Velocity( 0, 0 );
	}
}

bool cMoving_Platform :: Is_Update_Valid( void )
{
	// if not visible
	if( !m_active )
	{
		return 0;
	}

	return 1;
}

bool cMoving_Platform :: Is_Draw_Valid( void )
{
	bool valid = cAnimated_Sprite::Is_Draw_Valid();

	// if editor enabled
	if( editor_enabled )
	{
		// if active mouse object
		if( pMouseCursor->m_active_object == this )
		{
			return 1;
		}
	}

	return valid;
}

Col_Valid_Type cMoving_Platform :: Validate_Collision( cSprite *obj )
{
	if( obj->m_type == TYPE_PLAYER || obj->m_sprite_array == ARRAY_ENEMY )
	{
		cMovingSprite *moving_sprite = static_cast<cMovingSprite *>(obj);

		Col_Valid_Type validation = Validate_Collision_Object_On_Top( moving_sprite );

		if( validation != COL_VTYPE_NOT_POSSIBLE )
		{
			return validation;
		}

		// massive
		if( m_massive_type == MASS_MASSIVE )
		{
			return COL_VTYPE_INTERNAL;
		}

		return COL_VTYPE_NOT_VALID;
	}
	if( obj->m_type == TYPE_BALL )
	{
		if( obj->Is_On_Top( this ) )
		{
			obj->Set_On_Top( this, 0 );

			return COL_VTYPE_INTERNAL;
		}

		return COL_VTYPE_NOT_VALID;
	}

	return COL_VTYPE_NOT_VALID;
}

void cMoving_Platform :: Handle_Collision_Player( cObjectCollision *collision )
{
	if( collision->m_direction == DIR_TOP )
	{
		// set to touched
		if( ( m_touch_time > 0.0f || m_touch_move_time > 0.0f ) && m_platform_state == MOVING_PLATFORM_STAY )
		{
			m_platform_state = MOVING_PLATFORM_TOUCHED;
		}

		// send collision
		Send_Collision( collision );
	}

	Handle_Move_Object_Collision( collision );
}

void cMoving_Platform :: Handle_Collision_Enemy( cObjectCollision *collision )
{
	// send collision
	Send_Collision( collision );

	Handle_Move_Object_Collision( collision );
}

void cMoving_Platform :: Editor_Activate( void )
{
	// get window manager
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	// move type
	CEGUI::Combobox *combobox = static_cast<CEGUI::Combobox *>(wmgr.createWindow( "TaharezLook/Combobox", "editor_moving_platform_move_type" ));
	Editor_Add( UTF8_("Move Type"), UTF8_("Movement type."), combobox, 100, 105 );

	combobox->addItem( new CEGUI::ListboxTextItem( "line" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "circle" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "path" ) );
	//combobox->addItem( new CEGUI::ListboxTextItem( "path backwards" ) );

	if( m_move_type == MOVING_PLATFORM_TYPE_LINE )
	{
		combobox->setText( "line" );
	}
	else if( m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
	{
		combobox->setText( "circle" );
	}
	else if( m_move_type == MOVING_PLATFORM_TYPE_PATH_BACKWARDS )
	{
		combobox->setText( "path backwards" );
	}
	else
	{
		combobox->setText( "path" );
	}

	combobox->subscribeEvent( CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Move_Type_Select, this ) );

	// path identifier
	CEGUI::Editbox *editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_path_identifier" ));
	Editor_Add( UTF8_("Path Identifier"), UTF8_("Name of the Path to move along."), editbox, 150 );

	editbox->setText( m_path_state.m_path_identifier.c_str() );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Path_Identifier_Text_Changed, this ) );

	// direction
	combobox = static_cast<CEGUI::Combobox *>(wmgr.createWindow( "TaharezLook/Combobox", "editor_moving_platform_direction" ));
	Editor_Add( UTF8_("Direction"), UTF8_("Starting direction."), combobox, 100, 120 );

	combobox->addItem( new CEGUI::ListboxTextItem( "up" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "down" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "left" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "right" ) );

	combobox->setText( Get_Direction_Name( m_start_direction ) );
	combobox->subscribeEvent( CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Direction_Select, this ) );
	
	// max distance
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_max_distance" ));
	Editor_Add( UTF8_("Distance"), UTF8_("Movable distance into its direction if type is line or radius if circle."), editbox, 120 );

	editbox->setValidationString( "^[+]?\\d*$" );
	editbox->setText( int_to_string( m_max_distance ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Max_Distance_Text_Changed, this ) );

	// speed
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_speed" ));
	Editor_Add( UTF8_("Speed"), UTF8_("Maximum speed"), editbox, 120 );

	editbox->setValidationString( "[+]?[0-9]*\\.?[0-9]*" );
	editbox->setText( float_to_string( m_speed, 6, 0 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Speed_Text_Changed, this ) );

	// touch time
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_touch_time" ));
	Editor_Add( UTF8_("Touch time"), UTF8_("Time when touched until shaking starts"), editbox, 120 );

	editbox->setValidationString( "[+]?[0-9]*\\.?[0-9]*" );
	editbox->setText( float_to_string( m_touch_time, 6, 0 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Touch_Time_Text_Changed, this ) );

	// shake time
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_shake_time" ));
	Editor_Add( UTF8_("Shake time"), UTF8_("Time it's shaking until falling"), editbox, 120 );

	editbox->setValidationString( "[+]?[0-9]*\\.?[0-9]*" );
	editbox->setText( float_to_string( m_shake_time, 6, 0 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Shake_Time_Text_Changed, this ) );

	// touch move time
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_move_time" ));
	Editor_Add( UTF8_("Touch move time"), UTF8_("If set does not move until this time has elapsed after touched"), editbox, 120 );

	editbox->setValidationString( "[+]?[0-9]*\\.?[0-9]*" );
	editbox->setText( float_to_string( m_touch_move_time, 6, 0 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Touch_Move_Time_Text_Changed, this ) );

	// horizontal middle image count
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_hor_middle_count" ));
	Editor_Add( UTF8_("Hor image count"), UTF8_("Horizontal middle image count"), editbox, 120 );

	editbox->setValidationString( "^[+]?\\d*$" );
	editbox->setText( int_to_string( m_middle_count ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Hor_Middle_Count_Text_Changed, this ) );

	// image top left
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_image_top_left" ));
	Editor_Add( UTF8_("Image top left"), UTF8_("Image top left"), editbox, 200 );

	editbox->setText( m_images[0].m_image->Get_Filename( 1 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Image_Top_Left_Text_Changed, this ) );

	// image top middle
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_image_top_middle" ));
	Editor_Add( UTF8_("Image top middle"), UTF8_("Image top middle"), editbox, 200 );

	editbox->setText( m_images[1].m_image->Get_Filename( 1 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Image_Top_Middle_Text_Changed, this ) );

	// image top right
	editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_moving_platform_image_top_right" ));
	Editor_Add( UTF8_("Image top right"), UTF8_("Image top right"), editbox, 200 );

	editbox->setText( m_images[2].m_image->Get_Filename( 1 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cMoving_Platform::Editor_Image_Top_Right_Text_Changed, this ) );

	// init
	Editor_Init();
}

void cMoving_Platform :: Editor_State_Update( void )
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	// path identifier
	CEGUI::Editbox *editbox_path_identifier = static_cast<CEGUI::Editbox *>(wmgr.getWindow( "editor_moving_platform_path_identifier" ));
	// direction
	CEGUI::Combobox *combobox_direction = static_cast<CEGUI::Combobox *>(wmgr.getWindow( "editor_moving_platform_direction" ));
	// max distance
	CEGUI::Editbox *editbox_max_distance = static_cast<CEGUI::Editbox *>(wmgr.getWindow( "editor_moving_platform_max_distance" ));

	if( m_move_type == MOVING_PLATFORM_TYPE_PATH || m_move_type == MOVING_PLATFORM_TYPE_PATH_BACKWARDS )
	{
		editbox_path_identifier->setEnabled( 1 );
		combobox_direction->setEnabled( 0 );
		editbox_max_distance->setEnabled( 0 );
	}
	else
	{
		editbox_path_identifier->setEnabled( 0 );
		combobox_direction->setEnabled( 1 );
		editbox_max_distance->setEnabled( 1 );

		// remove invalid directions
		if( m_move_type == MOVING_PLATFORM_TYPE_CIRCLE )
		{
			CEGUI::ListboxItem *item = combobox_direction->findItemWithText( "up", NULL );

			if( item )
			{
				combobox_direction->removeItem( item );
				item = NULL;
			}

			item = combobox_direction->findItemWithText( "down", NULL );

			if( item )
			{
				combobox_direction->removeItem( item );
			}
		}
		// add usable direction
		else
		{
			CEGUI::ListboxItem *item = combobox_direction->findItemWithText( "up", NULL );

			if( !item )
			{
				combobox_direction->addItem( new CEGUI::ListboxTextItem( "up" ) );
			}

			item = combobox_direction->findItemWithText( "down", NULL );

			if( !item )
			{
				combobox_direction->addItem( new CEGUI::ListboxTextItem( "down" ) );
			}
		}
	}
}

bool cMoving_Platform :: Editor_Move_Type_Select( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	CEGUI::ListboxItem *item = static_cast<CEGUI::Combobox *>( windowEventArgs.window )->getSelectedItem();
	std::string str_text = item->getText().c_str();

	if( str_text.compare( "line" ) == 0 )
	{
		Set_Move_Type( MOVING_PLATFORM_TYPE_LINE );
	}
	else if( str_text.compare( "circle" ) == 0 )
	{
		Set_Move_Type( MOVING_PLATFORM_TYPE_CIRCLE );
	}
	else if( str_text.compare( "path backwards" ) == 0 )
	{
		Set_Move_Type( MOVING_PLATFORM_TYPE_PATH_BACKWARDS );
	}
	else
	{
		Set_Move_Type( MOVING_PLATFORM_TYPE_PATH );
	}

	Editor_State_Update();

	return 1;
}

bool cMoving_Platform :: Editor_Path_Identifier_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Path_Identifier( str_text );

	return 1;
}


bool cMoving_Platform :: Editor_Direction_Select( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	CEGUI::ListboxItem *item = static_cast<CEGUI::Combobox *>( windowEventArgs.window )->getSelectedItem();

	Set_Direction( Get_Direction_Id( item->getText().c_str() ), 1 );

	return 1;
}

bool cMoving_Platform :: Editor_Max_Distance_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Max_Distance( string_to_int( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Speed_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Speed( string_to_float( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Touch_Time_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Touch_Time( string_to_float( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Shake_Time_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Shake_Time( string_to_float( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Touch_Move_Time_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Touch_Move_Time( string_to_float( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Hor_Middle_Count_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Middle_Count( string_to_int( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Image_Top_Left_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Image_Top_Left( pVideo->Get_Surface( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Image_Top_Middle_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Image_Top_Middle( pVideo->Get_Surface( str_text ) );

	return 1;
}

bool cMoving_Platform :: Editor_Image_Top_Right_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Image_Top_Right( pVideo->Get_Surface( str_text ) );

	return 1;
}

void cMoving_Platform :: Create_Name( void )
{
	m_name.clear();

	if( m_touch_time )
	{
		m_name = _("Falling ");
	}
	if( m_touch_move_time )
	{
		m_name += _("Delayed ");
	}
	if( m_speed )
	{
		m_name += _("Moving ");
	}

	m_name += _("Platform - ") + Get_Direction_Name( m_start_direction ) + " - " + Get_Massive_Type_Name( m_massive_type );
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
