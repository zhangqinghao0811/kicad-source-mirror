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

#ifndef MIRROR_VIEW_MANAGER_H
#define MIRROR_VIEW_MANAGER_H

#include <math/vector2d.h>
#include <math/box2.h>

namespace KIGFX
{
class GAL;
class VIEW;
}

/**
 * Manages the mirror view functionality for PCBnew.
 * 
 * This class handles the coordinate transformations and viewport management
 * needed to display a mirrored view alongside the original PCB view.
 */
class MIRROR_VIEW_MANAGER
{
public:
    MIRROR_VIEW_MANAGER();
    ~MIRROR_VIEW_MANAGER();

    /**
     * Enable or disable the mirror view mode.
     * 
     * @param aEnable true to enable mirror view, false to disable
     */
    void SetMirrorViewEnabled( bool aEnable ) { m_mirrorViewEnabled = aEnable; }

    /**
     * Check if mirror view is currently enabled.
     * 
     * @return true if mirror view is enabled
     */
    bool IsMirrorViewEnabled() const { return m_mirrorViewEnabled; }

    /**
     * Set the screen size for viewport calculations.
     * 
     * @param aScreenSize the current screen size in pixels
     */
    void SetScreenSize( const VECTOR2D& aScreenSize );

    /**
     * Get the viewport for the original (right) view.
     * 
     * @param aFullViewport the full screen viewport
     * @return viewport rectangle for the original view
     */
    BOX2D GetOriginalViewport( const BOX2D& aFullViewport ) const;

    /**
     * Get the viewport for the mirror (left) view.
     * 
     * @param aFullViewport the full screen viewport
     * @return viewport rectangle for the mirror view
     */
    BOX2D GetMirrorViewport( const BOX2D& aFullViewport ) const;

    /**
     * Convert screen coordinates to determine which view they belong to.
     * 
     * @param aScreenPos screen position in pixels
     * @return true if position is in mirror view, false if in original view
     */
    bool IsPositionInMirrorView( const VECTOR2D& aScreenPos ) const;

    /**
     * Transform world coordinates for mirror view rendering.
     * 
     * @param aWorldPos position in world coordinates
     * @param aBoardCenter center point of the board for mirroring
     * @return transformed position for mirror view
     */
    VECTOR2D TransformForMirrorView( const VECTOR2D& aWorldPos, const VECTOR2D& aBoardCenter ) const;

    /**
     * Convert screen coordinates from mirror view to world coordinates.
     * 
     * @param aScreenPos screen position in mirror view
     * @param aView the VIEW object for coordinate conversion
     * @param aBoardCenter center point of the board
     * @return world coordinates
     */
    VECTOR2D ScreenToWorldMirror( const VECTOR2D& aScreenPos, KIGFX::VIEW* aView, 
                                  const VECTOR2D& aBoardCenter ) const;

    /**
     * Set up GAL transformation matrix for mirror view rendering.
     * 
     * @param aGal the GAL object to configure
     * @param aBoardCenter center point for mirroring
     */
    void SetupMirrorTransform( KIGFX::GAL* aGal, const VECTOR2D& aBoardCenter ) const;

    /**
     * Restore GAL transformation matrix after mirror view rendering.
     * 
     * @param aGal the GAL object to restore
     */
    void RestoreTransform( KIGFX::GAL* aGal ) const;

    /**
     * Get the divider line position between the two views.
     * 
     * @return X coordinate of the divider line in screen coordinates
     */
    double GetDividerPosition() const;

private:
    bool m_mirrorViewEnabled;
    VECTOR2D m_screenSize;
    
    // Cached transformation matrices
    mutable bool m_transformCacheValid;
    mutable MATRIX3x3D m_savedTransform;
};

#endif // MIRROR_VIEW_MANAGER_H
