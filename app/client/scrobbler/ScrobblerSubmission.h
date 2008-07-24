/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "ScrobblerHttp.h"
#include "lib/types/Track.h"
#include <QList>


class ScrobblerSubmission : public ScrobblerPostHttp
{
    QList<Track> m_tracks;
    QList<Track> m_batch;

public:
    void setTracks( const QList<Track>& );
    /** submits a batch of 50 to the scrobbling service */
    void submitNextBatch();
    /** the current batch, depending on */
    QList<Track> batch() const { return m_batch; }
};
