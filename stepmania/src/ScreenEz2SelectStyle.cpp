/****************************************
ScreenEzSelectPlayer,cpp
Desc: See Header
Copyright (C):
Andrew Livy
*****************************************/

/* Includes */

#include "stdafx.h"
#include "ScreenEz2SelectStyle.h"
#include "ScreenManager.h"
#include "PrefsManager.h"
#include "RageMusic.h"
#include "ScreenTitleMenu.h"
#include "ScreenCaution.h"
#include "GameConstantsAndTypes.h"
#include "ThemeManager.h"
#include "ScreenSelectDifficulty.h"
#include "ScreenSandbox.h"
#include "GameManager.h"
#include "RageLog.h"
#include "AnnouncerManager.h"
#include "GameConstantsAndTypes.h"
#include "Background.h"
#include "ScreenSelectGroup.h"

/* Constants */

const CString BG_ANIMS_DIR = "BGAnimations\\";

const ScreenMessage SM_GoToPrevState		=	ScreenMessage(SM_User + 1);
const ScreenMessage SM_GoToNextState		=	ScreenMessage(SM_User + 2);
/*
const CString DANCE_STYLES[] = {
	"easy",
	"hard",
	"real",
	"double",
};
*/
enum DStyles {
	DS_CLUB,
	DS_EASY,
	DS_HARD,
	DS_REAL

};

const float TWEEN_TIME		= 0.35f;
const D3DXCOLOR OPT_NOT_SELECTED = D3DXCOLOR(0.3f,0.3f,0.3f,1);
const D3DXCOLOR OPT_SELECTED = D3DXCOLOR(1.0f,1.0f,1.0f,1);


const float OPT_X[NUM_EZ2STYLE_GRAPHICS] = { 
	CENTER_X-350,
	CENTER_X,
	CENTER_X+350,
	CENTER_X+700,
}; // tells us the default X position
const float OPT_Y[NUM_EZ2STYLE_GRAPHICS] = {
	CENTER_Y,
	CENTER_Y,
	CENTER_Y,
	CENTER_Y,
}; // tells us the default Y position

const float CLUB_X[NUM_EZ2STYLE_GRAPHICS] = {
	OPT_X[0]+0,
	OPT_X[0]-350,
	OPT_X[2]+0,
	OPT_X[1]+0,
};

const float EASY_X[NUM_EZ2STYLE_GRAPHICS] = {
	OPT_X[1]+0,
	OPT_X[0]+0,
	OPT_X[0]-350,
	OPT_X[2]+0,
};

const float HARD_X[NUM_EZ2STYLE_GRAPHICS] = {
	OPT_X[2]+0,
	OPT_X[1]+0,
	OPT_X[0]+0,
	OPT_X[0]-350,
};

const float REAL_X[NUM_EZ2STYLE_GRAPHICS] = {
	OPT_X[3]+0,
	OPT_X[2]+0,
	OPT_X[1]+0,
	OPT_X[0]+0,
};

const float OPT_XP[NUM_EZ2P_GRAPHICS] = { 
	CENTER_X+200, // This is the pad X
	CENTER_X-200, // This is the pad X
	CENTER_X-198, // This is the 1p X
	CENTER_X+195, // This is the 2p X
}; // tells us the default X position
const float OPT_YP[NUM_EZ2P_GRAPHICS] = {
	CENTER_Y+130,
	CENTER_Y+130,
	CENTER_Y+115,
	CENTER_Y+115,
}; // tells us the default Y position

float ez2p_lasttimercheck[2];
int ez2p_bounce=0; // used for the bouncing of the '1p' and '2p' images
int ez2p_direct=0; // direction of the bouncing of the '1p' and '2p' images

/************************************
ScreenEz2SelectStyle (Constructor)
Desc: Sets up the screen display
************************************/

ScreenEz2SelectStyle::ScreenEz2SelectStyle()
{
	LOG->WriteLine( "ScreenEz2SelectStyle::ScreenEz2SelectStyle()" );

	m_iSelectedStyle=0;
	ez2p_lasttimercheck[0] = TIMER->GetTimeSinceStart();
// Load in the sprites we will be working with.
	for( int i=0; i<NUM_EZ2STYLE_GRAPHICS; i++ )
	{
		CString sPadGraphicPath;
		switch( i )
		{
		case 0:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_STYLE_INFO_GAME_0_STYLE_3);	//HACK! Would LIKE to have own filename :)
			break;
		case 1:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_STYLE_INFO_GAME_0_STYLE_0);	
			break;
		case 2:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_STYLE_INFO_GAME_0_STYLE_1);	
			break;
		case 3:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_STYLE_INFO_GAME_0_STYLE_2);	
			break;
		}
		m_sprOpt[i].Load( sPadGraphicPath );
		m_sprOpt[i].SetXY( OPT_X[i], OPT_Y[i] );
		m_sprOpt[i].SetZoom( 1 );
		this->AddActor( &m_sprOpt[i] );
	}

	for( i=0; i<NUM_EZ2P_GRAPHICS; i++ )
	{
		CString sPadGraphicPath;
		switch( i )
		{
		case 0:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_DIFFICULTY_HARD_PICTURE);	
			break;
		case 1:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_DIFFICULTY_HARD_PICTURE);	
			break;
		case 2:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_DIFFICULTY_MEDIUM_PICTURE);	
			break;
		case 3:
			sPadGraphicPath = THEME->GetPathTo(GRAPHIC_SELECT_DIFFICULTY_EASY_PICTURE);	
			break;
		}
		m_sprPly[i].Load( sPadGraphicPath );
		m_sprPly[i].SetXY( OPT_XP[i], OPT_YP[i] );
		m_sprPly[i].SetZoom( 1 );
		this->AddActor( &m_sprPly[i] );
	}

	m_Menu.Load( 	
		THEME->GetPathTo(GRAPHIC_SELECT_STYLE_BACKGROUND), 
		THEME->GetPathTo(GRAPHIC_SELECT_STYLE_TOP_EDGE),
		ssprintf("Use %c %c to select, then press START", char(1), char(2) )
		);
	this->AddActor( &m_Menu );

	m_soundChange.Load( THEME->GetPathTo(SOUND_SELECT_STYLE_CHANGE) );
	m_soundSelect.Load( THEME->GetPathTo(SOUND_MENU_START) );


//	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_SELECT_STYLE_INTRO) );


	if( !MUSIC->IsPlaying() )
	{
		MUSIC->Load( THEME->GetPathTo(SOUND_MUSIC_SCROLL_MUSIC) );
        MUSIC->Play( true );
	}

	if ( GAMEMAN->m_sMasterPlayerNumber == PLAYER_1 && GAMEMAN->m_CurStyle == STYLE_EZ2_SINGLE) //if p1 already selected hide graphic.
	{
		m_iSelectedPlayer = 0;
		m_sprPly[1].BeginTweening( 0 );
		m_sprPly[1].SetTweenZoomY( 0 );
		m_sprPly[2].BeginTweening( 0 );
		m_sprPly[2].SetTweenZoomY( 0 );
	}
	else if ( GAMEMAN->m_sMasterPlayerNumber == PLAYER_2 && GAMEMAN->m_CurStyle == STYLE_EZ2_SINGLE) //if p2 already selected hide graphic.
	{
		m_iSelectedPlayer = 1;
		m_sprPly[3].BeginTweening( 0 );
		m_sprPly[3].SetTweenZoomY( 0 );
		m_sprPly[0].BeginTweening( 0 );
		m_sprPly[0].SetTweenZoomY( 0 );
	}	
	else // if both are already selected, hide the graphics alltogether
	{
		m_iSelectedPlayer = 2;
		m_sprPly[1].BeginTweening( 0 );
		m_sprPly[1].SetTweenZoomY( 0 );
		m_sprPly[2].BeginTweening( 0 );
		m_sprPly[2].SetTweenZoomY( 0 );
		m_sprPly[3].BeginTweening( 0 );
		m_sprPly[3].SetTweenZoomY( 0 );
		m_sprPly[0].BeginTweening( 0 );
		m_sprPly[0].SetTweenZoomY( 0 );

		// hide the CLUB graphic...
		m_sprOpt[0].BeginTweening( 0 );
		m_sprOpt[0].SetTweenZoomY( 0 );
		m_iSelectedStyle = 1; // make sure we DONT have CLUB selected
		m_sprOpt[DS_EASY].SetXY( EASY_X[m_iSelectedStyle]-350, OPT_Y[0] );
		m_sprOpt[DS_HARD].SetXY( HARD_X[m_iSelectedStyle]-350, OPT_Y[0] );
		m_sprOpt[DS_REAL].SetXY( HARD_X[m_iSelectedStyle], OPT_Y[0] );	
		MenuLeft( PLAYER_1 ); // shift left so that we're clean again.

	}
	GAMEMAN->m_CurStyle = STYLE_NONE; // why reset this? because we want player2 to be able to input at this stage.
	
	
	m_Menu.TweenOnScreenFromBlack( SM_None );
}

/************************************
~ScreenEz2SelectStyle (Destructor)
Desc: Writes line to log when screen
is terminated.
************************************/
ScreenEz2SelectStyle::~ScreenEz2SelectStyle()
{
	LOG->WriteLine( "ScreenEz2SelectStyle::~ScreenEz2SelectStyle()" );
}


/************************************
DrawPrimitives
Desc: Draws the screen =P
************************************/

void ScreenEz2SelectStyle::DrawPrimitives()
{
	if (m_iSelectedPlayer != 2) // no need to animate graphics if we have no graphics to animate ;)
	{
		AnimateGraphics();
	}

	m_Menu.DrawBottomLayer();
	Screen::DrawPrimitives();
	m_Menu.DrawTopLayer();
}

/************************************
Input
Desc: Handles player input.
************************************/
void ScreenEz2SelectStyle::Input( const DeviceInput& DeviceI, const InputEventType type, const GameInput &GameI, const MenuInput &MenuI, const StyleInput &StyleI )
{
	LOG->WriteLine( "ScreenEz2SelectStyle::Input()" );

	if( m_Menu.IsClosing() )
		return;

	Screen::Input( DeviceI, type, GameI, MenuI, StyleI );	// default input handler
}

/************************************
HandleScreenMessage
Desc: Handles Screen Messages and changes
	game states.
************************************/
void ScreenEz2SelectStyle::HandleScreenMessage( const ScreenMessage SM )
{
	Screen::HandleScreenMessage( SM );

	switch( SM )
	{
	case SM_MenuTimer:
		MenuStart(PLAYER_INVALID);
		break;
	case SM_GoToPrevState:
		MUSIC->Stop();
		SCREENMAN->SetNewScreen( new ScreenTitleMenu );
		break;
	case SM_GoToNextState:
		SCREENMAN->SetNewScreen( new ScreenSelectGroup );
		break;
	}
}


/************************************
MenuBack
Desc: Actions performed when a player 
presses the button bound to back
************************************/

void ScreenEz2SelectStyle::MenuBack( const PlayerNumber p )
{
	MUSIC->Stop();

	m_Menu.TweenOffScreenToBlack( SM_GoToPrevState, true );

//	m_Fade.CloseWipingLeft( SM_GoToPrevState );

//	TweenOffScreen();
}

void ScreenEz2SelectStyle::MenuDown( const PlayerNumber p )
{
	MenuStart(p);
}


/************************************
SetFadedStyles
Desc: Fades out non-highlighted items
depending on the users choice.
************************************/
void ScreenEz2SelectStyle::SetFadedStyles()
{
	m_sprOpt[0].SetTweenDiffuseColor( OPT_NOT_SELECTED );
	m_sprOpt[1].SetTweenDiffuseColor( OPT_NOT_SELECTED );
	m_sprOpt[2].SetTweenDiffuseColor( OPT_NOT_SELECTED );
	m_sprOpt[3].SetTweenDiffuseColor( OPT_NOT_SELECTED );
	if (m_iSelectedStyle != 3)
	{
		m_sprOpt[m_iSelectedStyle + 1].SetTweenDiffuseColor( OPT_SELECTED );
	}
	else
	{
		m_sprOpt[0].SetTweenDiffuseColor( OPT_SELECTED );
	}
	if (m_iSelectedPlayer == 2 && m_iSelectedStyle == DS_REAL)
	{
		m_sprOpt[1].SetTweenDiffuseColor( OPT_SELECTED );
	}
}

/************************************
MenuRight
Desc: Actions performed when a player 
presses the button bound to right
************************************/
void ScreenEz2SelectStyle::MenuRight( PlayerNumber p )
{
	if (m_iSelectedPlayer != 2) // Single player
	{
		if (((m_iSelectedPlayer == 0 && p == PLAYER_2) || (m_iSelectedPlayer == 1 && p == PLAYER_1)) != TRUE) // make sure players who haven't selected yet can't choose a style
		{
			if( m_iSelectedStyle == 3 )		// wrap to the first dance style
				m_iSelectedStyle = 0; // which is (club) easy (hard)
			else
				m_iSelectedStyle = m_iSelectedStyle + 1; // otherwise shuffle up a style...

			if(m_iSelectedStyle == 2) // If it's HARD and CLUB needs to appear from the other side...
			{
				m_sprOpt[0].SetXY( REAL_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
			}
			else if(m_iSelectedStyle == 3) // If it's REAL and EASY needs to appear from the other side...
			{
				m_sprOpt[1].SetXY( CLUB_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
			}
			else if(m_iSelectedStyle == 0) // If it's CLUB and HARD needs to appear from the other side...
			{
				m_sprOpt[2].SetXY( EASY_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[3].SetXY( EASY_X[m_iSelectedStyle]+1050, OPT_Y[0] ); // REAL must also move, due to the fact that it is the end item at the start, it moves differently to the rest.
			}

			m_sprOpt[0].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[0].SetTweenX( CLUB_X[m_iSelectedStyle] );
			m_sprOpt[0].SetTweenY( OPT_Y[m_iSelectedStyle] );


			m_sprOpt[1].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[1].SetTweenX( EASY_X[m_iSelectedStyle] );
			m_sprOpt[1].SetTweenY( OPT_Y[m_iSelectedStyle] );

			m_sprOpt[2].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[2].SetTweenX( HARD_X[m_iSelectedStyle] );
			m_sprOpt[2].SetTweenY( OPT_Y[m_iSelectedStyle] );

			m_sprOpt[3].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[3].SetTweenX( REAL_X[m_iSelectedStyle] );
			m_sprOpt[3].SetTweenY( OPT_Y[m_iSelectedStyle] );
			
			SetFadedStyles();
		}
	}
	else // Two Players (means NO club option...)
	{
		if (((m_iSelectedPlayer == 0 && p == PLAYER_2) || (m_iSelectedPlayer == 1 && p == PLAYER_1)) != TRUE) // make sure players who haven't selected yet can't choose a style
		{
			if( m_iSelectedStyle == DS_REAL )		// wrap to the last dance style
				m_iSelectedStyle = DS_EASY;
			else
				m_iSelectedStyle = m_iSelectedStyle + 1;

			if( m_iSelectedStyle == DS_REAL ) // (REALLY EASY)
			{
				m_sprOpt[DS_HARD].SetXY( CLUB_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_REAL].SetXY( REAL_X[m_iSelectedStyle]+350, OPT_Y[0] );	
				m_sprOpt[DS_EASY].SetXY( CLUB_X[m_iSelectedStyle]+350, OPT_Y[0] );
			}
			else if( m_iSelectedStyle == DS_HARD ) // (REALLY REAL)
			{
				m_sprOpt[DS_EASY].SetXY( REAL_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_HARD].SetXY( HARD_X[m_iSelectedStyle]+350, OPT_Y[0] );	
				m_sprOpt[DS_REAL].SetXY( REAL_X[m_iSelectedStyle]+350, OPT_Y[0] );					
			}
			else if( m_iSelectedStyle == DS_EASY ) // (REALLY HARD)
			{
				m_sprOpt[DS_REAL].SetXY( HARD_X[m_iSelectedStyle]+700, OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_EASY].SetXY( EASY_X[m_iSelectedStyle]+350, OPT_Y[0] );		
				m_sprOpt[DS_HARD].SetXY( HARD_X[m_iSelectedStyle]+350, OPT_Y[0] );					
			}


			/* NOTE: Because we're really shifting three values using a setup for four values
			   the DS_ values are shifted out of phase by 1, i.e. DS_REAL is now actually DS_EASY
			   Confused? I was =) Anyhow, this will only happen if there are two players and we don't want 
			   them playing double
			*/
			if (m_iSelectedStyle == DS_REAL)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( CLUB_X[m_iSelectedStyle] );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( CLUB_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( REAL_X[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}	
			else if (m_iSelectedStyle == DS_HARD)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( REAL_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( HARD_X[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( REAL_X[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}
			else if (m_iSelectedStyle == DS_EASY)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( EASY_X[m_iSelectedStyle] );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( HARD_X[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( HARD_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}
		}
		SetFadedStyles();
	}
}

/************************************
MenuLeft
Desc: Actions performed when a player 
presses the button bound to left
************************************/
void ScreenEz2SelectStyle::MenuLeft( PlayerNumber p )
{
	if (m_iSelectedPlayer != 2) // Single player
	{
		if (((m_iSelectedPlayer == 0 && p == PLAYER_2) || (m_iSelectedPlayer == 1 && p == PLAYER_1)) != TRUE) // make sure players who haven't selected yet can't choose a style
		{
			if( m_iSelectedStyle == 0 )		// wrap to the last dance style
				m_iSelectedStyle = 3;
			else
				m_iSelectedStyle = m_iSelectedStyle - 1;

			if( m_iSelectedStyle == 3 )
			{
				m_sprOpt[3].SetXY( CLUB_X[m_iSelectedStyle]-700, OPT_Y[0] ); // First move it over the other side off-screen...		
				m_sprOpt[2].SetXY( CLUB_X[m_iSelectedStyle]-1050, OPT_Y[0] );	
			}
			else if( m_iSelectedStyle == 2 )
			{
				m_sprOpt[1].SetXY( HARD_X[m_iSelectedStyle]-700, OPT_Y[0] ); // First move it over the other side off-screen...			
			}
			else if( m_iSelectedStyle == 1 )
			{
				m_sprOpt[0].SetXY( EASY_X[m_iSelectedStyle]-1050, OPT_Y[0] ); // First move it over the other side off-screen...			
			}

			m_sprOpt[0].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[0].SetTweenX( CLUB_X[m_iSelectedStyle] );
			m_sprOpt[0].SetTweenY( OPT_Y[m_iSelectedStyle] );	

			m_sprOpt[1].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[1].SetTweenX( EASY_X[m_iSelectedStyle] );
			m_sprOpt[1].SetTweenY( OPT_Y[m_iSelectedStyle] );

			m_sprOpt[2].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[2].SetTweenX( HARD_X[m_iSelectedStyle] );
			m_sprOpt[2].SetTweenY( OPT_Y[m_iSelectedStyle] );

			m_sprOpt[3].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
			m_sprOpt[3].SetTweenX( REAL_X[m_iSelectedStyle] );
			m_sprOpt[3].SetTweenY( OPT_Y[m_iSelectedStyle] );	

			SetFadedStyles();
		}
	}
	else // Two Players (means NO club option...)
	{
		if (((m_iSelectedPlayer == 0 && p == PLAYER_2) || (m_iSelectedPlayer == 1 && p == PLAYER_1)) != TRUE) // make sure players who haven't selected yet can't choose a style
		{
			if( m_iSelectedStyle == DS_EASY )		// wrap to the last dance style
				m_iSelectedStyle = DS_REAL;
			else
				m_iSelectedStyle = m_iSelectedStyle - 1;

			if( m_iSelectedStyle == DS_REAL ) // (REALLY EASY)
			{
				m_sprOpt[DS_HARD].SetXY( CLUB_X[m_iSelectedStyle], OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_REAL].SetXY( REAL_X[m_iSelectedStyle]-350, OPT_Y[0] );				
			}
			else if( m_iSelectedStyle == DS_HARD ) // (REALLY REAL)
			{
				m_sprOpt[DS_EASY].SetXY( REAL_X[m_iSelectedStyle], OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_HARD].SetXY( HARD_X[m_iSelectedStyle]-350, OPT_Y[0] );			
			}
			else if( m_iSelectedStyle == DS_EASY ) // (REALLY HARD)
			{
				m_sprOpt[DS_REAL].SetXY( HARD_X[m_iSelectedStyle], OPT_Y[0] ); // First move it over the other side off-screen...
				m_sprOpt[DS_EASY].SetXY( EASY_X[m_iSelectedStyle]-350, OPT_Y[0] );				
			}


			/* NOTE: Because we're really shifting three values using a setup for four values
			   the DS_ values are shifted out of phase by 1, i.e. DS_REAL is now actually DS_EASY
			   Confused? I was =) Anyhow, this will only happen if there are two players and we don't want 
			   them playing double
			*/
			if (m_iSelectedStyle == DS_REAL)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( CLUB_X[m_iSelectedStyle] );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( CLUB_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( REAL_X[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}	
			else if (m_iSelectedStyle == DS_HARD)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( REAL_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( HARD_X[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( REAL_X[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}
			else if (m_iSelectedStyle == DS_EASY)
			{
				m_sprOpt[DS_EASY].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_EASY].SetTweenX( EASY_X[m_iSelectedStyle] );
				m_sprOpt[DS_EASY].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_HARD].SetTweenX( HARD_X[m_iSelectedStyle] );
				m_sprOpt[DS_HARD].SetTweenY( OPT_Y[m_iSelectedStyle] );
				m_sprOpt[DS_REAL].BeginTweening( 0.2f, TWEEN_BIAS_BEGIN );
				m_sprOpt[DS_REAL].SetTweenX( HARD_X[m_iSelectedStyle] + 350 );
				m_sprOpt[DS_REAL].SetTweenY( OPT_Y[m_iSelectedStyle] );	
			}


			SetFadedStyles();
		}
	}
}

/************************************
MenuStart
Desc: Actions performed when a player 
presses the button bound to start
************************************/
void ScreenEz2SelectStyle::MenuStart( PlayerNumber p )
{
//	GAMEMAN->m_CurStyle = DANCE_STYLES[m_iSelectedStyle];
	
	if ((m_iSelectedPlayer == 0 && p == PLAYER_2) || (m_iSelectedPlayer == 1 && p == PLAYER_1))
	{
		m_soundSelect.PlayRandom();
		TweenPlyOffScreen();
		m_iSelectedPlayer = 2; // set to BOTH players now.

		// hide the CLUB graphic...
		m_sprOpt[0].BeginTweening( 0 );
		m_sprOpt[0].SetTweenZoomY( 0 );
		m_iSelectedStyle = 1; // make sure we DONT have CLUB selected
		m_sprOpt[DS_EASY].SetXY( EASY_X[m_iSelectedStyle]-350, OPT_Y[0] );
		m_sprOpt[DS_HARD].SetXY( HARD_X[m_iSelectedStyle]-350, OPT_Y[0] );
		m_sprOpt[DS_REAL].SetXY( HARD_X[m_iSelectedStyle], OPT_Y[0] );	
		MenuLeft( p ); // shift left so that we're clean again.
	}
	else
	{
		m_soundSelect.PlayRandom();
	
		this->ClearMessageQueue();

		GAMEMAN->m_CurStyle = STYLE_EZ2_SINGLE;		
		PREFSMAN->m_PlayMode = PLAY_MODE_ARCADE;

		m_Menu.TweenOffScreenToMenu( SM_GoToNextState );

		TweenOffScreen();
	}

}

/************************************
TweenOffScreen
Desc: Squashes graphics before the screen
changes state.
************************************/
void ScreenEz2SelectStyle::TweenOffScreen()
{

}

/************************************
AnimateGraphics
Desc: Animates the 1p/2p selection
************************************/
void ScreenEz2SelectStyle::AnimateGraphics()
{

//if (bounce < 10 && direct == 0 && wait == 2) // Bounce 1p/2p up
if (TIMER->GetTimeSinceStart() > ez2p_lasttimercheck[0] + 0.01f && ez2p_direct == 0)
	{
		ez2p_lasttimercheck[0] = TIMER->GetTimeSinceStart();
		ez2p_bounce+=1;
	
		m_sprPly[2].SetXY( OPT_XP[2], OPT_YP[2] - ez2p_bounce);
		m_sprPly[3].SetXY( OPT_XP[3], OPT_YP[3] - ez2p_bounce);


		if (ez2p_bounce == 10)
		{
			ez2p_direct = 1;
		}
	}
	else if (TIMER->GetTimeSinceStart() > ez2p_lasttimercheck[0] + 0.01f && ez2p_direct == 1) // bounce 1p/2p down
	{
		ez2p_lasttimercheck[0] = TIMER->GetTimeSinceStart();
		ez2p_bounce-=1;
	
		m_sprPly[2].SetXY( OPT_XP[2], OPT_YP[2] - ez2p_bounce);
		m_sprPly[3].SetXY( OPT_XP[3], OPT_YP[3] - ez2p_bounce);

		if (ez2p_bounce == 0)
		{
			ez2p_direct = 0;
		}
	}
}

/************************************
TweenPlyOffScreen
Desc: Squashes Player Graphics off screen
When selected.
************************************/
void ScreenEz2SelectStyle::TweenPlyOffScreen()
{
			m_sprPly[1].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_sprPly[1].SetTweenZoomY( 0 );
			m_sprPly[2].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_sprPly[2].SetTweenZoomY( 0 );
			m_sprPly[0].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_sprPly[0].SetTweenZoomY( 0 );
			m_sprPly[3].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_sprPly[3].SetTweenZoomY( 0 );
}