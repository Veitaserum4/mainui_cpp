/*
RayTracing.cpp -- OpenRemix renderer options
Copyright (C) 2026

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.
*/

#include "Framework.h"
#include "keydefs.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "SpinControl.h"
#include "Slider.h"
#include "Action.h"
#include "CheckBox.h"
#include "StringArrayModel.h"

#define ART_BANNER "gfx/shell/head_vidoptions"

// Settings owned by ref_openremix (r_main.c applies them live, except glass)
class CMenuRayTracing : public CMenuFramework
{
public:
	CMenuRayTracing() : CMenuFramework( "CMenuRayTracing" ) { }

	bool KeyDown( int key ) override;

private:
	void _Init() override;
	void Reload() override;
	void SaveCb();
	void RestoreCb();
	void Restore();

	CMenuSpinControl upscaler;
	CMenuCheckBox    vsync;
	CMenuCheckBox    framegen;

	CMenuAction      fogHeader;
	CMenuCheckBox    fog;
	CMenuSlider      fogDistance;
	CMenuSlider      fogScattering;

	CMenuAction      glassHeader;
	CMenuCheckBox    glass;
	CMenuCheckBox    glassThin;
	CMenuSlider      glassIOR;
	CMenuSlider      glassTexture;
	CMenuAction      glassNote;

	CMenuAction      unavailable;
};

bool CMenuRayTracing::KeyDown( int key )
{
	if( UI::Key::IsEscape( key ))
		Restore();
	return CMenuFramework::KeyDown( key );
}

void CMenuRayTracing::SaveCb()
{
	upscaler.WriteCvar();
	vsync.WriteCvar();
	framegen.WriteCvar();
	fog.WriteCvar();
	fogDistance.WriteCvar();
	fogScattering.WriteCvar();
	glass.WriteCvar();
	glassThin.WriteCvar();
	glassIOR.WriteCvar();
	glassTexture.WriteCvar();

	SaveAndPopMenu();
}

void CMenuRayTracing::Restore()
{
	upscaler.DiscardChanges();
	vsync.DiscardChanges();
	framegen.DiscardChanges();
	fog.DiscardChanges();
	fogDistance.DiscardChanges();
	fogScattering.DiscardChanges();
	glass.DiscardChanges();
	glassThin.DiscardChanges();
	glassIOR.DiscardChanges();
	glassTexture.DiscardChanges();
}

void CMenuRayTracing::RestoreCb()
{
	Restore();
	Hide();
}

static void SetupHeader( CMenuAction &item, const char *name, int x, int y )
{
	item.iFlags = QMF_INACTIVE | QMF_DROPSHADOW;
	item.szName = name;
	item.colorBase = uiColorHelp;
	item.SetCharSize( QM_BIGFONT );
	item.SetRect( x, y, 300, 32 );
}

void CMenuRayTracing::_Init( void )
{
	const int left = 360, right = 680;

	banner.SetPicture( ART_BANNER );

	// Display
	static const char *upscalerItems[] = { L( "Off" ), L( "Auto" ), L( "FSR 3.1" ), L( "DLSS" ) };
	static CStringArrayModel upscalerModel( upscalerItems, V_ARRAYSIZE( upscalerItems ));
	upscaler.Setup( &upscalerModel );
	upscaler.SetNameAndStatus( L( "Upscaler" ), L( "Upscaling and denoising used by the path tracer" ));
	upscaler.LinkCvar( "rt_upscaler", CMenuEditable::CVAR_VALUE );
	upscaler.SetRect( left, 260, 240, 32 );

	vsync.SetNameAndStatus( L( "Vertical sync" ), L( "Synchronize presentation with the display refresh rate" ));
	vsync.LinkCvar( "rt_vsync" );
	vsync.SetCoord( left, 310 );

	framegen.SetNameAndStatus( L( "Frame generation" ), L( "Generate intermediate frames (requires a supported GPU)" ));
	framegen.LinkCvar( "rt_framegen" );
	framegen.SetCoord( left, 360 );

	// Volumetric fog
	SetupHeader( fogHeader, L( "Volumetric fog" ), left, 430 );

	fog.SetNameAndStatus( L( "Enable fog" ), L( "Path-traced volumetric fog and light shafts" ));
	fog.LinkCvar( "rt_fog" );
	fog.SetCoord( left, 470 );

	fogDistance.SetNameAndStatus( L( "Fog visibility distance" ), L( "How far you can see through the fog, in meters" ));
	fogDistance.Setup( 25, 2000, 25 );
	fogDistance.LinkCvar( "rt_fog_distance" );
	fogDistance.SetCoord( left, 550 );

	fogScattering.SetNameAndStatus( L( "Fog light scattering" ), L( "How strongly the fog scatters light" ));
	fogScattering.Setup( 0.0f, 2.0f, 0.05f );
	fogScattering.LinkCvar( "rt_fog_scattering" );
	fogScattering.SetCoord( left, 620 );

	// Glass
	SetupHeader( glassHeader, L( "Glass" ), right, 230 );

	glass.SetNameAndStatus( L( "Ray-traced glass" ), L( "Refractive, reflective glass instead of flat translucency" ));
	glass.LinkCvar( "rt_glass" );
	glass.SetCoord( right, 270 );

	glassThin.SetNameAndStatus( L( "Thin glass" ), L( "Treat glass as thin sheets without refraction" ));
	glassThin.LinkCvar( "rt_glass_thin" );
	glassThin.SetCoord( right, 320 );

	glassIOR.SetNameAndStatus( L( "Index of refraction" ), L( "How strongly glass bends light (1.52 is window glass)" ));
	glassIOR.Setup( 1.0f, 2.0f, 0.01f );
	glassIOR.LinkCvar( "rt_refr_glass" );
	glassIOR.SetCoord( right, 400 );

	glassTexture.SetNameAndStatus( L( "Glass texture opacity" ), L( "0 = clear refractive glass; higher shows the glass texture with reflections" ));
	glassTexture.Setup( 0.0f, 1.0f, 0.05f );
	glassTexture.LinkCvar( "rt_glass_texture" );
	glassTexture.SetCoord( right, 470 );

	glassNote.iFlags = QMF_INACTIVE;
	glassNote.szName = L( "Glass changes apply on the next map load" );
	glassNote.colorBase = uiColorHelp;
	glassNote.SetRect( right, 510, 320, 24 );

	unavailable.iFlags = QMF_INACTIVE | QMF_DROPSHADOW;
	unavailable.szName = L( "These options require the OpenRemix renderer (-ref openremix)" );
	unavailable.colorBase = uiColorHelp;
	unavailable.SetRect( left, 690, 640, 24 );

	AddItem( banner );
	AddButton( L( "Done" ), nullptr, PC_DONE, VoidCb( &CMenuRayTracing::SaveCb ));
	AddButton( L( "GameUI_Cancel" ), nullptr, PC_CANCEL, VoidCb( &CMenuRayTracing::RestoreCb ));

	AddItem( upscaler );
	AddItem( vsync );
	AddItem( framegen );
	AddItem( fogHeader );
	AddItem( fog );
	AddItem( fogDistance );
	AddItem( fogScattering );
	AddItem( glassHeader );
	AddItem( glass );
	AddItem( glassThin );
	AddItem( glassIOR );
	AddItem( glassTexture );
	AddItem( glassNote );
	AddItem( unavailable );
}

void CMenuRayTracing::Reload()
{
	bool active = !stricmp( EngFuncs::GetCvarString( "r_refdll_loaded" ), "openremix" );

	CMenuBaseItem *items[] = { &upscaler, &vsync, &framegen, &fog, &fogDistance, &fogScattering, &glass, &glassThin, &glassIOR, &glassTexture };
	for( CMenuBaseItem *item : items )
	{
		item->SetGrayed( !active );
		item->SetInactive( !active );
	}

	if( active )
		unavailable.Hide();
	else
		unavailable.Show();

	CMenuFramework::Reload();
}

ADD_MENU( menu_raytracing, CMenuRayTracing, UI_RayTracing_Menu );
