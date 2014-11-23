/***************************************************************************
 * krush.cpp  -  The little dinosaur
 *
 * Copyright (C) 2004 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../enemies/krush.h"
#include "../core/game_core.h"
#include "../video/animation.h"
#include "../gui/hud.h"
#include "../level/level_player.h"
#include "../video/gl_surface.h"
#include "../user/savegame.h"
#include "../core/i18n.h"
#include "../core/sprite_manager.h"
// CEGUI
#include "CEGUIXMLAttributes.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUICombobox.h"
#include "elements/CEGUIListboxTextItem.h"

namespace SMC
{

/* *** *** *** *** *** cKrush *** *** *** *** *** *** *** *** *** *** *** *** */

cKrush :: cKrush( cSprite_Manager *sprite_manager )
: cEnemy( sprite_manager )
{
	cKrush::Init();
}

cKrush :: cKrush( CEGUI::XMLAttributes &attributes, cSprite_Manager *sprite_manager )
: cEnemy( sprite_manager )
{
	cKrush::Init();
	cKrush::Load_From_XML( attributes );
}

cKrush :: ~cKrush( void )
{
	//
}

void cKrush :: Init( void  )
{
	m_type = TYPE_KRUSH;
	m_pos_z = 0.093f;
	m_gravity_max = 27.0f;

	Add_Image( pVideo->Get_Surface( "enemy/krush/big_1.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/big_2.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/big_3.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/big_4.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/small_1.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/small_2.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/small_3.png" ) );
	Add_Image( pVideo->Get_Surface( "enemy/krush/small_4.png" ) );

	m_state = STA_FALL;
	Set_Moving_State( STA_WALK );
	Set_Direction( DIR_RIGHT );

	m_kill_sound = "enemy/krush/die.ogg";
}

cKrush *cKrush :: Copy( void ) const
{
	cKrush *krush = new cKrush( m_sprite_manager  );
	krush->Set_Pos( m_start_pos_x, m_start_pos_y );
	krush->Set_Direction( m_start_direction );
	return krush;
}

void cKrush :: Load_From_XML( CEGUI::XMLAttributes &attributes )
{
	// position
	Set_Pos( static_cast<float>(attributes.getValueAsInteger( "posx" )), static_cast<float>(attributes.getValueAsInteger( "posy" )), 1 );
	// direction
	Set_Direction( Get_Direction_Id( attributes.getValueAsString( "direction", Get_Direction_Name( m_start_direction ) ).c_str() ) );
}

void cKrush :: Save_To_XML( CEGUI::XMLSerializer &stream )
{
	// begin
	stream.openTag( m_type_name );

	// name
	Write_Property( stream, "type", "krush" );
	// position
	Write_Property( stream, "posx", static_cast<int>( m_start_pos_x ) );
	Write_Property( stream, "posy", static_cast<int>( m_start_pos_y ) );
	// direction
	Write_Property( stream, "direction", Get_Direction_Name( m_start_direction ) );

	// end
	stream.closeTag();
}

void cKrush :: Load_From_Savegame( cSave_Level_Object *save_object )
{
	// krush_state
	if( save_object->exists( "state" ) )
	{
		Moving_state mov_state = static_cast<Moving_state>(string_to_int( save_object->Get_Value( "state" ) ));

		if( mov_state == STA_RUN )
		{
			Set_Moving_State( mov_state );
		}
	}

	cEnemy::Load_From_Savegame( save_object );

	Update_Rotation_Hor();
}

void cKrush :: Set_Direction( const ObjectDirection dir )
{
	// already set
	if( m_start_direction == dir )
	{
		return;
	}

	cEnemy::Set_Direction( dir, 1 );

	m_name = "Krush ";
	m_name += _(Get_Direction_Name( m_start_direction ).c_str());

	Update_Rotation_Hor( 1 );
}

void cKrush :: Turn_Around( ObjectDirection col_dir /* = DIR_UNDEFINED */ )
{
	cEnemy::Turn_Around( col_dir );

	if( col_dir == DIR_LEFT || col_dir == DIR_RIGHT || col_dir == DIR_UNDEFINED )
	{
		m_velx *= 0.5f;
		Update_Rotation_Hor();
	}
}

void cKrush :: DownGrade( bool force /* = 0 */ )
{
	// default stomp downgrade
	if( !force )
	{
		// big to small walking
		if( m_state == STA_WALK )
		{
			Set_Moving_State( STA_RUN );

			Col_Move( 0.0f, m_images[3].m_image->m_col_h - m_images[4].m_image->m_col_h, 1, 1 );

			// animation
			cParticle_Emitter *anim = new cParticle_Emitter( m_sprite_manager );
			Generate_Hit_Animation( anim );
			anim->Set_Speed( 3.5f, 0.6f );
			anim->Set_Fading_Alpha( 1 );
			anim->Emit();
			pActive_Animation_Manager->Add( anim );
		}
		else if( m_state == STA_RUN )
		{
			Set_Scale_Directions( 1, 0, 1, 1 );
			Set_Dead( 1 );

			// animation
			cParticle_Emitter *anim = new cParticle_Emitter( m_sprite_manager );
			Generate_Hit_Animation( anim );
			anim->Set_Speed( 4.5f, 1.6f );
			anim->Set_Scale( 0.6f );
			anim->Emit();
			pActive_Animation_Manager->Add( anim );
		}
	}
	// falling death
	else
	{
		Set_Dead( 1 );
		Set_Rotation_Z( 180.0f );
	}

	if( m_dead )
	{
		m_massive_type = MASS_PASSIVE;
		m_counter = 0.0f;
		m_velx = 0.0f;
		m_vely = 0.0f;
	}
}

void cKrush :: Update_Dying( void )
{
	m_counter += pFramerate->m_speed_factor;

	// stomp death
	if( !Is_Float_Equal( m_rot_z, 180.0f ) )
	{
		float speed = pFramerate->m_speed_factor * 0.05f;

		Add_Scale_X( -speed * 0.5f );
		Add_Scale_Y( -speed );

		if( m_scale_y < 0.01f )
		{
			Set_Scale( 1.0f );
			Set_Active( 0 );
		}
	}
	// falling death
	else
	{
		// a little bit upwards first
		if( m_counter < 5.0f )
		{
			Move( 0.0f, -5.0f );
		}
		// if not below the ground : fall
		else if( m_col_rect.m_y < pActive_Camera->m_limit_rect.m_y )
		{
			Move( 0.0f, 20.0f );
		}
		// if below disable
		else
		{
			m_rot_z = 0.0f;
			Set_Active( 0 );
		}
	}
}

void cKrush :: Set_Moving_State( Moving_state new_state )
{
	if( new_state == m_state )
	{
		return;
	}

	if( new_state == STA_WALK )
	{
		Set_Animation( 1 );
		Set_Animation_Image_Range( 0, 3 );
		Set_Time_All( 120, 1 );
		Reset_Animation();
		Set_Image_Num( m_anim_img_start );

		m_kill_points = 20;
	}
	else if( new_state == STA_RUN )
	{
		Set_Animation( 1 );
		Set_Animation_Image_Range( 4, 7 );
		Set_Time_All( 70, 1 );
		Reset_Animation();
		Set_Image_Num( m_anim_img_start );

		m_kill_points = 40;
	}

	m_state = new_state;

	Update_Velocity_Max();
}

void cKrush :: Update( void )
{
	cEnemy::Update();

	if( !m_valid_update || !Is_In_Range() )
	{
		return;
	}

	Update_Velocity();
	Update_Animation();
	Update_Gravity();
}

void cKrush :: Update_Velocity_Max( void )
{
	if( m_state == STA_WALK )
	{
		m_velx_max = 3.0f;
		m_velx_gain = 0.2f;
	}
	else if( m_state == STA_RUN )
	{
		m_velx_max = 5.5f;
		m_velx_gain = 0.4f;
	}
}

bool cKrush :: Is_Update_Valid( void )
{
	if( m_dead || m_freeze_counter )
	{
		return 0;
	}

	return 1;
}

Col_Valid_Type cKrush :: Validate_Collision( cSprite *obj )
{
	// basic validation checking
	Col_Valid_Type basic_valid = Validate_Collision_Ghost( obj );

	// found valid collision
	if( basic_valid != COL_VTYPE_NOT_POSSIBLE )
	{
		return basic_valid;
	}

	if( obj->m_massive_type == MASS_MASSIVE )
	{
		switch( obj->m_type )
		{
			case TYPE_FLYON:
			{
				return COL_VTYPE_NOT_VALID;
			}
			case TYPE_ROKKO:
			{
				return COL_VTYPE_NOT_VALID;
			}
			case TYPE_GEE:
			{
				return COL_VTYPE_NOT_VALID;
			}
			default:
			{
				break;
			}
		}

		return COL_VTYPE_BLOCKING;
	}
	else if( obj->m_massive_type == MASS_HALFMASSIVE )
	{
		// if moving downwards and the object is on bottom
		if( m_vely >= 0.0f && Is_On_Top( obj ) )
		{
			return COL_VTYPE_BLOCKING;
		}
	}
	else if( obj->m_massive_type == MASS_PASSIVE )
	{
		switch( obj->m_type )
		{
			case TYPE_ENEMY_STOPPER:
			{
				return COL_VTYPE_BLOCKING;
			}
			default:
			{
				break;
			}
		}
	}

	return COL_VTYPE_NOT_VALID;
}

void cKrush :: Handle_Collision_Player( cObjectCollision *collision )
{
	// invalid
	if( collision->m_direction == DIR_UNDEFINED )
	{
		return;
	}

	if( collision->m_direction == DIR_TOP && pLevel_Player->m_state != STA_FLY )
	{
		pHud_Points->Add_Points( m_kill_points, m_pos_x, m_pos_y - 5.0f, "", static_cast<Uint8>(255), 1 );
		pAudio->Play_Sound( m_kill_sound );

		// big walking
		if( m_state == STA_WALK )
		{
			DownGrade();
		}
		// small walking
		else if( m_state == STA_RUN )
		{
			DownGrade();
			pLevel_Player->Add_Kill_Multiplier();
		}

		pLevel_Player->Action_Jump( 1 );
	}
	else
	{
		pLevel_Player->DownGrade_Player();
		Turn_Around( collision->m_direction );
	}
}

void cKrush :: Handle_Collision_Enemy( cObjectCollision *collision )
{
	if( collision->m_direction == DIR_RIGHT || collision->m_direction == DIR_LEFT )
	{
		Turn_Around( collision->m_direction );
	}

	Send_Collision( collision );
}

void cKrush :: Handle_Collision_Massive( cObjectCollision *collision )
{
	if( m_state == STA_OBJ_LINKED )
	{
		return;
	}

	Send_Collision( collision );

	// get colliding object
	cSprite *col_object = m_sprite_manager->Get_Pointer( collision->m_number );

	if( col_object->m_type == TYPE_BALL )
	{
		return;
	}

	if( collision->m_direction == DIR_TOP )
	{
		if( m_vely < 0.0f )
		{
			m_vely = 0.0f;
		}
	}
	else if( collision->m_direction == DIR_BOTTOM )
	{
		if( m_vely > 0.0f )
		{
			m_vely = 0.0f;
		}
	}
	else if( collision->m_direction == DIR_RIGHT || collision->m_direction == DIR_LEFT )
	{
		Turn_Around( collision->m_direction );
	}
}

void cKrush :: Editor_Activate( void )
{
	// get window manager
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	// direction
	CEGUI::Combobox *combobox = static_cast<CEGUI::Combobox *>(wmgr.createWindow( "TaharezLook/Combobox", "editor_krush_direction" ));
	Editor_Add( UTF8_("Direction"), UTF8_("Starting direction."), combobox, 100, 75 );

	combobox->addItem( new CEGUI::ListboxTextItem( "left" ) );
	combobox->addItem( new CEGUI::ListboxTextItem( "right" ) );

	combobox->setText( Get_Direction_Name( m_start_direction ) );

	combobox->subscribeEvent( CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber( &cKrush::Editor_Direction_Select, this ) );

	// init
	Editor_Init();
}

bool cKrush :: Editor_Direction_Select( const CEGUI::EventArgs &event )
{
	const CEGUI::WindowEventArgs &windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>( event );
	CEGUI::ListboxItem *item = static_cast<CEGUI::Combobox *>( windowEventArgs.window )->getSelectedItem();

	Set_Direction( Get_Direction_Id( item->getText().c_str() ) );

	return 1;
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
