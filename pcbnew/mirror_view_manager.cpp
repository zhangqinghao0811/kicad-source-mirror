/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mirror_view_manager.h"
#include <gal/graphics_abstraction_layer.h>
#include <view/view.h>
#include <math/matrix3x3.h>

MIRROR_VIEW_MANAGER::MIRROR_VIEW_MANAGER() :
    m_mirrorViewEnabled( false ),
    m_screenSize( 800, 600 ),
    m_transformCacheValid( false )
{
}


MIRROR_VIEW_MANAGER::~MIRROR_VIEW_MANAGER()
{
}


void MIRROR_VIEW_MANAGER::SetScreenSize( const VECTOR2D& aScreenSize )
{
    m_screenSize = aScreenSize;
    m_transformCacheValid = false;
}


BOX2D MIRROR_VIEW_MANAGER::GetOriginalViewport( const BOX2D& aFullViewport ) const
{
    if( !m_mirrorViewEnabled )
        return aFullViewport;

    // Original view takes the right half of the screen
    BOX2D viewport = aFullViewport;
    double halfWidth = aFullViewport.GetWidth() / 2.0;
    viewport.SetOrigin( aFullViewport.GetOrigin() + VECTOR2D( halfWidth, 0 ) );
    viewport.SetWidth( halfWidth );
    
    return viewport;
}


BOX2D MIRROR_VIEW_MANAGER::GetMirrorViewport( const BOX2D& aFullViewport ) const
{
    if( !m_mirrorViewEnabled )
        return BOX2D();  // Empty viewport when mirror view is disabled

    // Mirror view takes the left half of the screen
    BOX2D viewport = aFullViewport;
    double halfWidth = aFullViewport.GetWidth() / 2.0;
    viewport.SetWidth( halfWidth );
    
    return viewport;
}


bool MIRROR_VIEW_MANAGER::IsPositionInMirrorView( const VECTOR2D& aScreenPos ) const
{
    if( !m_mirrorViewEnabled )
        return false;

    // Check if position is in the left half of the screen
    return aScreenPos.x < ( m_screenSize.x / 2.0 );
}


VECTOR2D MIRROR_VIEW_MANAGER::TransformForMirrorView( const VECTOR2D& aWorldPos, 
                                                      const VECTOR2D& aBoardCenter ) const
{
    // Mirror the X coordinate around the board center
    VECTOR2D transformed = aWorldPos;
    transformed.x = 2.0 * aBoardCenter.x - aWorldPos.x;
    return transformed;
}


VECTOR2D MIRROR_VIEW_MANAGER::ScreenToWorldMirror( const VECTOR2D& aScreenPos, 
                                                   KIGFX::VIEW* aView,
                                                   const VECTOR2D& aBoardCenter ) const
{
    if( !aView )
        return VECTOR2D();

    // Adjust screen position to account for the left viewport
    VECTOR2D adjustedScreenPos = aScreenPos;
    
    // Convert to world coordinates using the view
    VECTOR2D worldPos = aView->ToWorld( adjustedScreenPos );
    
    // Apply mirror transformation
    return TransformForMirrorView( worldPos, aBoardCenter );
}


void MIRROR_VIEW_MANAGER::SetupMirrorTransform( KIGFX::GAL* aGal, 
                                                const VECTOR2D& aBoardCenter ) const
{
    if( !aGal )
        return;

    // Save current GAL state
    aGal->Save();
    m_transformCacheValid = true;

    // Apply mirror transformation
    // Translate to board center, mirror X, then translate back
    aGal->Translate( aBoardCenter );
    aGal->Scale( VECTOR2D( -1.0, 1.0 ) );  // Mirror X axis
    aGal->Translate( VECTOR2D( -aBoardCenter.x, -aBoardCenter.y ) );
}


void MIRROR_VIEW_MANAGER::RestoreTransform( KIGFX::GAL* aGal ) const
{
    if( !aGal || !m_transformCacheValid )
        return;

    // Restore the saved GAL state
    aGal->Restore();
}


double MIRROR_VIEW_MANAGER::GetDividerPosition() const
{
    return m_screenSize.x / 2.0;
}
