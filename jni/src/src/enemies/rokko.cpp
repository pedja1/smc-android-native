/***************************************************************************
 * rokko.cpp  -  giant, slow-moving bullet
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

#include "../enemies/rokko.h"
#include "../core/game_core.h"
#include "../level/level_player.h"
#include "../video/animation.h"
#include "../gui/hud.h"
#include "../video/gl_surface.h"
#include "../video/renderer.h"
#include "../input/mouse.h"
#include "../core/i18n.h"
// CEGUI
#include "CEGUIXMLAttributes.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIEditbox.h"
#include "elements/CEGUICombobox.h"
#include "elements/CEGUIListboxTextItem.h"

namespace SMC
{

/* *** *** *** *** *** *** cRokko *** *** *** *** *** *** *** *** *** *** *** */

cRokko :: cRokko( cSprite_Manager *sprite_manager )
: cEnemy( sprite_manager )
{
	cRokko::Init();
}

cRokko :: cRokko( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager )
: cEnemy( sprite_manager )
{
	cRokko::Init();
	cRokko::Load_From_XML( attributes );
}

cRokko :: ~cRokko( void )
{
	//
}

void cRokko :: Init( void  )
{
	m_type = TYPE_ROKKO;
	m_massive_type = MASS_PASSIVE;
	m_pos_z = 0.03f;
	m_gravity_max = 26.0f;
	m_editor_pos_z = 0.09f;
	m_can_be_on_ground = 0;
	m_camera_range = 4000;
	Set_Rotation_Affects_Rect( 1 );
	Set_Active( 0 );

	m_fire_resistant = 1;
	m_ice_resistance = 1;
	m_can_be_hit_from_shell = 0;

	Set_Direction( DIR_LEFT );
	Set_Speed( 8.5f );
	m_min_distance_front = 200;
	Set_Max_Distance_Front( 1000 );
	Set_Max_Distance_Sides( 400 );
	m_state = STA_STAY;

	m_smoke_counter = 0;

	m_kill_sound = "enemy/rokko/hit.wav";
	m_kill_points = 250;
}

cRokko *cRokko :: Copy( void ) const
{
	cRokko *rokko = new cRokko( m_sprite_manager );
	rokko->Set_Pos( m_start_pos_x, m_start_pos_y );
	rokko->Set_Direction( m_start_direction );
	rokko->Set_Speed( m_speed );
	return rokko;
}

void cRokko :: Load_From_XML( CEGUI::XMLAttributes &attributes )
{
	// position
	Set_Pos( static_cast<float>(attributes.getValueAsInteger( "posx" )), static_cast<float>(attributes.getValueAsInteger( "posy" )), 1 );
	// direction
	Set_Direction( Get_Direction_Id( attributes.getValueAsString( "direction", Get_Direction_Name( m_start_direction ) ).c_str() ) );
	// speed
	Set_Speed( attributes.getValueAsFloat( "speed", m_speed ) );
}

void cRokko :: Save_To_XML( CEGUI::XMLSerializer &stream )
{
	// begin
	stream.openTag( m_type_name );

	// name
	Write_Property( stream, "type", "rokko" );
	// position
	Write_Property( stream, "posx", static_cast<int>( m_start_pos_x ) );
	Write_Property( stream, "posy", static_cast<int>( m_start_pos_y ) );
	// direction
	Write_Property( stream, "direction", Get_Direction_Name( m_start_direction ) );
	// speed
	Write_Property( stream, "speed", m_speed );

	// end
	stream.closeTag();
}

void cRokko :: Load_From_Savegame( cSave_Level_Object *save_object )
{
	cEnemy::Load_From_Savegame( save_object );

	// Don't activate if dead
	if( m_dead )
	{
		return;
	}

	// activate
	if( m_state == STA_FLY )
	{
		Activate( 0 );
	}
}

void cRokko :: Set_Direction( const ObjectDirection dir )
{
	// already set
	if( m_start_direction == dir )
	{
		return;
	}

	// clear old images
	Clear_Images();

	cEnemy::Set_Direction( dir, 1 );
	m_name = "Rokko ";
	m_name += _(Get_Direction_Name(m_start_direction).c_str());

	Add_Image( pVideo->Get_Surface( "enemy/rokko/r.png" ) );
	if( m_direction == DIR_LEFT )
	{
		Set_Rotation( 0.0f, 180.0f, 0.0f, 1 );
	}
	else if( m_direction == DIR_RIGHT )
	{
		Set_Rotation( 0.0f, 0.0f, 0.0f, 1 );
	}
	else if( m_direction == DIR_UP )
	{
		Set_Rotation( 0.0f, 0.0f, 270.0f, 1 );
	}
	else if( m_direction == DIR_DOWN )
	{
		Set_Rotation( 0.0f, 0.0f, 90.0f, 1 );
	}
	else
	{
		printf( "Warning: Unknown Rokko direction %s\n", Get_Direction_Name( dir ).c_str() );
	}

	Update_Distance_rect();
	Set_Image_Num( 0, 1 );
}

void cRokko :: Set_Speed( float nspeed )
{
	if( nspeed < 2.0f )
	{
		nspeed = 2.0f;
	}

	m_speed = nspeed;
}

void cRokko :: Set_Max_Distance_Front( float distance )
{
	if( distance < m_min_distance_front )
	{
		distance = m_min_distance_front;
	}

	m_max_distance_front = distance;

	Update_Distance_rect();
}

void cRokko :: Set_Max_Distance_Sides( float distance )
{
	if( distance < 50.0f )
	{
		distance = 50.0f;
	}

	m_max_distance_sides = distance;

	Update_Distance_rect();
}

void cRokko :: Activate( bool with_sound /* = 1 */ )
{
	if( with_sound )
	{
		pAudio->Play_Sound( "enemy/rokko/activate.wav" );
	}

	m_state = STA_FLY;
	m_massive_type = MASS_MASSIVE;
	Set_Active( 1 );

	if( m_direction == DIR_LEFT )
	{
		Set_Velocity( -m_speed, 0.0f );
	}
	else if( m_direction == DIR_RIGHT )
	{
		Set_Velocity( m_speed, 0.0f );
	}
	else if( m_direction == DIR_UP )
	{
		Set_Velocity( 0.0f, -m_speed );
	}
	else if( m_direction == DIR_DOWN )
	{
		Set_Velocity( 0.0f, m_speed );
	}
}

void cRokko :: DownGrade( bool force /* = 0 */ )
{
	Set_Dead( 1 );
	m_massive_type = MASS_PASSIVE;
	m_vely = 0;

	if( !force )
	{
		// animation
		cParticle_Emitter *anim = new cParticle_Emitter( m_sprite_manager );
		Generate_Hit_Animation( anim );

		anim->Set_Quota( 8 );
		anim->Set_Speed( 4, 1 );
		anim->Set_Scale( 0.9f );
		anim->Emit();
		pActive_Animation_Manager->Add( anim );
	}
}

void cRokko :: Update_Dying( void )
{
	if( m_vely < m_gravity_max )
	{
		Add_Velocity_Y_Max( 1.5f, m_gravity_max );
	}

	Move( m_velx, m_vely );

	if( m_rot_z - m_start_rot_z < 90 )
	{
		Add_Rotation_Z( pFramerate->m_speed_factor );
	}

	// generate smoke
	m_smoke_counter += pFramerate->m_speed_factor * 4;
	if( m_smoke_counter >= 2.0f )
	{
		Generate_Smoke( static_cast<int>(m_smoke_counter) );
		Generate_Sparks( static_cast<int>(m_smoke_counter * 0.5f) );
		m_smoke_counter -= static_cast<int>(m_smoke_counter);
	}

	// below ground
	if( m_col_rect.m_y - 200.0f > pActive_Camera->m_limit_rect.m_y + game_res_h )
	{
		m_rot_z = 0.0f;
		m_massive_type = MASS_PASSIVE;
		Set_Active( 0 );
		m_velx = 0.0f;
	}
}

void cRokko :: Update( void )
{
	cEnemy::Update();

	if( !m_valid_update || !Is_In_Range() )
	{
		return;
	}

	// if not active
	if( m_state != STA_FLY )
	{
		GL_rect final_distance = Get_Final_Distance_Rect();

		// if player is in front then activate
		if( pLevel_Player->m_maryo_type != MARYO_GHOST && pLevel_Player->m_col_rect.Intersects( final_distance ) )
		{
			Activate();
		}
		else
		{
			return;
		}
	}

	// generate smoke
	m_smoke_counter += pFramerate->m_speed_factor * 4.0f;
	if( m_smoke_counter >= 1.0f )
	{
		Generate_Smoke( static_cast<int>(m_smoke_counter) );
		m_smoke_counter -= static_cast<int>(m_smoke_counter);
	}
}

void cRokko :: Draw( cSurface_Request *request /* = NULL */ )
{
	if( !m_valid_draw )
	{
		return;
	}

	// draw distance rect
	if( editor_level_enabled )
	{
		GL_rect final_distance = Get_Final_Distance_Rect();
		final_distance.m_x -= pActive_Camera->m_x;
		final_distance.m_y -= pActive_Camera->m_y;

		pVideo->Draw_Rect( &final_distance, m_pos_z - 0.00001f, &whitealpha128 );
	}

	bool create_request = 0;

	if( !request )
	{
		create_request = 1;
		// create request
		request = new cSurface_Request();
	}

	// Draw
	cEnemy::Draw( request );

	// alpha in debug mode
	if( editor_level_enabled )
	{
		request->m_color.alpha = 64;
	}

	if( create_request )
	{
		// add request
		pRenderer->Add( request );
	}
}

void cRokko :: Update_Distance_rect( void )
{
	if( m_start_direction == DIR_LEFT )
	{
		m_distance_rect.m_x = -m_max_distance_front;
		m_distance_rect.m_y = ((m_col_pos.m_y + m_col_rect.m_h) * 0.5f) - (m_max_distance_sides * 0.5f);
		m_distance_rect.m_w = m_max_distance_front;
		m_distance_rect.m_h = m_max_distance_sides;

		// add some space to not activate directly in front of the player
		m_distance_rect.m_w -= m_min_distance_front;
	}
	else if( m_start_direction == DIR_RIGHT )
	{
		m_distance_rect.m_x = m_rect.m_w;
		m_distance_rect.m_y = ((m_col_pos.m_y + m_col_rect.m_h) * 0.5f) - (m_max_distance_sides * 0.5f);
		m_distance_rect.m_w = m_max_distance_front;
		m_distance_rect.m_h = m_max_distance_sides;

		// add some space to not activate directly in front of the player
		m_distance_rect.m_x += m_min_distance_front;
		m_distance_rect.m_w -= m_min_distance_front;
	}
	else if( m_start_direction == DIR_UP )
	{
		m_distance_rect.m_x = ((m_col_pos.m_x + m_col_rect.m_w) * 0.5f) - (m_max_distance_sides * 0.5f);
		m_distance_rect.m_y = -m_max_distance_front;
		m_distance_rect.m_w = m_max_distance_sides;
		m_distance_rect.m_h = m_max_distance_front;

		// add some space to not activate directly in front of the player
		m_distance_rect.m_h -= m_min_distance_front;
	}
	else if( m_start_direction == DIR_DOWN )
	{
		m_distance_rect.m_x = ((m_col_pos.m_x + m_col_rect.m_w) * 0.5f) - (m_max_distance_sides * 0.5f);
		m_distance_rect.m_y = m_rect.m_h;
		m_distance_rect.m_w = m_max_distance_sides;
		m_distance_rect.m_h = m_max_distance_front;

		// add some space to not activate directly in front of the player
		m_distance_rect.m_y += m_min_distance_front;
		m_distance_rect.m_h -= m_min_distance_front;
	}
}

GL_rect cRokko :: Get_Final_Distance_Rect( void ) const
{
	GL_rect final_distance = m_distance_rect;

	final_distance.m_x += m_rect.m_x;
	final_distance.m_y += m_rect.m_y;

	return final_distance;
}

void cRokko :: Generate_Smoke( unsigned int amount /* = 10 */ ) const
{
	cParticle_Emitter *anim = NULL;

	// moving smoke particle animation
	anim = new cParticle_Emitter( m_sprite_manager );

	// not dead
	if( !m_dead )
	{
		if( m_direction == DIR_LEFT )
		{
			anim->Set_Emitter_Rect( m_pos_x + m_col_rect.m_w - 16, m_pos_y + 20, 6, m_rect.m_h - 40 );
			anim->Set_Direction_Range( 280, 100 );
		}
		else if( m_direction == DIR_RIGHT )
		{
			anim->Set_Emitter_Rect( m_pos_x + 10, m_pos_y + 20, 6, m_rect.m_h - 40 );
			anim->Set_Direction_Range( 180, 100 );
		}
		else if( m_direction == DIR_UP )
		{
			anim->Set_Emitter_Rect( m_pos_x + 20, m_pos_y + m_col_rect.m_h - 16, m_rect.m_w - 40, 6 );
			anim->Set_Direction_Range( 50, 80 );
		}
		// down
		else
		{
			anim->Set_Emitter_Rect( m_pos_x + 20, m_pos_y + 10, m_rect.m_w - 40, 6 );
			anim->Set_Direction_Range( 240, 80 );
		}

		anim->Set_Scale( 0.3f, 0.4f );
	}
	// dead
	else
	{
		anim->Set_Emitter_Rect( m_pos_x + ( m_col_rect.m_w * 0.2f ), m_pos_y + ( m_col_rect.m_h * 0.2f ), m_col_rect.m_w * 0.3f, m_col_rect.m_h * 0.3f );
		anim->Set_Direction_Range( 180, 180 );
		anim->Set_Scale( 0.3f, 0.6f );
	}

	// - 0.000001f caused a weird graphical z pos bug with an ATI card
	anim->Set_Pos_Z( m_pos_z - 0.00001f );
	anim->Set_Image( pVideo->Get_Surface( "animation/particles/smoke_grey_big.png" ) );
	anim->Set_Quota( amount );
	anim->Set_Time_to_Live( 0.8f, 0.8f );
	anim->Set_Speed( 1.0f, 0.2f );
	anim->Set_Const_Rotation_Z( -1, 2 );
	anim->Set_Color( Color( static_cast<Uint8>(155), 150, 130 ) );
	anim->Set_Fading_Alpha( 1 );
	
	anim->Emit();
	pActive_Animation_Manager->Add( anim );
}

void cRokko :: Generate_Sparks( unsigned int amount /* = 5 */ ) const
{
	// animation
	cParticle_Emitter *anim = new cParticle_Emitter( m_sprite_manager );
	anim->Set_Emitter_Rect( m_pos_x + m_col_rect.m_w * 0.2f, m_pos_y + m_rect.m_h * 0.2f, m_col_rect.m_w * 0.6f, m_rect.m_h * 0.6f );
	anim->Set_Pos_Z( m_pos_z + 0.00001f );
	anim->Set_Quota( amount );
	anim->Set_Time_to_Live( 0.2f, 0.1f );
	anim->Set_Speed( 1.2f, 1.1f );
	anim->Set_Image( pVideo->Get_Surface( "animation/particles/light.png" ) );
	anim->Set_Color( Color( static_cast<Uint8>(250), 250, 200 ), Color( static_cast<Uint8>(5), 5, 0, 0 ) );
	anim->Set_Scale( 0.3f, 0.3f );
	anim->Set_Fading_Size( 1 );
	anim->Set_Fading_Alpha( 0 );
	anim->Emit();
	pActive_Animation_Manager->Add( anim );
}

bool cRokko :: Is_Update_Valid( void )
{
	if( m_dead || m_freeze_counter )
	{
		return 0;
	}

	return 1;
}

bool cRokko :: Is_Draw_Valid( void )
{
	bool valid = cEnemy::Is_Draw_Valid();

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

Col_Valid_Type cRokko :: Validate_Collision( cSprite *obj )
{
	if( obj->m_massive_type == MASS_MASSIVE )
	{
		switch( obj->m_type )
		{
			case TYPE_PLAYER:
			{
				return COL_VTYPE_INTERNAL;
			}
			case TYPE_BALL:
			{
				return COL_VTYPE_INTERNAL;
			}
			default:
			{
				break;
			}
		}

		return COL_VTYPE_NOT_VALID;
	}

	return COL_VTYPE_NOT_VALID;
}

void cRokko :: Handle_Collision_Player( cObjectCollision *collision )
{
	// if invalid
	if( collision->m_direction == DIR_UNDEFINED )
	{
		return;
	}

	if( m_direction == DIR_LEFT || m_direction == DIR_RIGHT )
	{
		// if invincible
		if( pLevel_Player->m_invincible > 0.0f )
		{
			return;
		}

		if( collision->m_direction == DIR_TOP && pLevel_Player->m_state != STA_FLY )
		{
			pHud_Points->Add_Points( m_kill_points, m_pos_x + m_rect.m_w / 3, m_pos_y - 10.0f, "", static_cast<Uint8>(255), 1 );
			pAudio->Play_Sound( m_kill_sound );
			pLevel_Player->Action_Jump( 1 );

			pLevel_Player->Add_Kill_Multiplier();
			DownGrade();
		}
		else
		{
			pLevel_Player->DownGrade_Player();
		}
	}
	else if( m_direction == DIR_UP || m_direction == DIR_DOWN )
	{
		if( ( collision->m_direction == DIR_LEFT || collision->m_direction == DIR_LEFT ) && pLevel_Player->m_state == STA_FLY )
		{
			pHud_Points->Add_Points( m_kill_points, m_pos_x, m_pos_y - 5.0f, "", static_cast<Uint8>(255), 1 );
			pAudio->Play_Sound( m_kill_sound );

			pLevel_Player->Add_Kill_Multiplier();
			DownGrade();
		}
		else
		{
			pLevel_Player->DownGrade_Player();
		}
	}
}

void cRokko :: Handle_out_of_Level( ObjectDirection dir )
{
	// fixme : needs a Handle_out_of_Level_Complete function
	//Set_Active( 0 );
}

void cRokko :: Editor_Activate( void )
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	// direction
	CEGUI::Combobox *combobox = static_cast<CEGUI::Combobox *>(wmgr.createWindow( "TaharezLook/Combobox", "editor_rokko_direction" ));
	Editor_Add( UTF8_("Direction"), UTF8_("Direction it moves into."), combobox, 100, 110 );

	combobox->addItem( new CEGUI::ListboxTextItem( "left" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "right" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "up" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "down" ) );

	combobox->setText( Get_Direction_Name( m_start_direction ) );
	combobox->subscribeEvent( CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber( &cRokko::Editor_Direction_Select, this ) );

	// speed
	CEGUI::Editbox *editbox = static_cast<CEGUI::Editbox *>(wmgr.createWindow( "TaharezLook/Editbox", "editor_rokko_speed" ));
	Editor_Add( UTF8_("Speed"), UTF8_("Speed when activated"), editbox, 120 );

	editbox->setValidationString( "[+]?[0-9]*\\.?[0-9]*" );
	editbox->setText( float_to_string( m_speed, 6, 0 ) );
	editbox->subscribeEvent( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber( &cRokko::Editor_Speed_Text_Changed, this ) );

	// init
	Editor_Init();
}

bool cRokko :: Editor_Direction_Select( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	CEGUI::ListboxItem *item = static_cast<CEGUI::Combobox *>( windowEventArgs.window )->getSelectedItem();

	Set_Direction( Get_Direction_Id( item->getText().c_str() ) );

	return 1;
}

bool cRokko :: Editor_Speed_Text_Changed( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	std::string str_text = static_cast<CEGUI::Editbox *>( windowEventArgs.window )->getText().c_str();

	Set_Speed( string_to_float( str_text ) );

	return 1;
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
