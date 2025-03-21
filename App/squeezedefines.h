/***************************************************************************
 *   Copyright (C) 2010 by George Galt                                     *
 *   george at galtfamily dot net                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 *  Many thanks to Richard Titmuss who developed the SlimProtoLib library  *
 *  SoftSqueeze and squeezeslave, from which much of the code here is      *
 *  stolen.  For more information on SoftSqueeze and squeezeslave, see     *
 *  Richard's site at: http://sourceforge.net/projects/softsqueeze         *
 *                                                                         *
 ***************************************************************************/

#ifndef SQUEEZEDEFINES_H
#define SQUEEZEDEFINES_H

// Qt
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QNetworkReply>
#include <QtGlobal>
#include <QtDebug>

// C++

// uncomment the following to turn on debugging for a particular file
#define SQUEEZESERVER_DEBUG
// #define SLIMCLI_DEBUG
// #define SLIMJSON_DEBUG
// #define SLIMP3PLAYER_DEBUG
// #define PLAYERMODEL_DEBUG
// #define SONGMODEL_DEBUG


// forward declaration of classes
class SlimCLI;
class SqueezeServer;
class Slimp3JSON;

#define LOC QString( "Slimp3Control: ")
#define LOC_WARN QString( "Slimp3Control, Warning: ")
#define LOC_ERR QString( "Slimp3Control, Error: ")

#define MAX_REQUEST_SIZE 1000
#define MAX_REQUEST_SIZE_TEXT "100"
#define MAX_PLAYLIST_SIZE 10000

enum playMode
{
    PLAY=0,
    PAUSE,
    STOP,
    MAX_PLAY_MODES
};
Q_DECLARE_METATYPE(playMode)


class TrackData
{
public:
    QByteArray playlist_index;
    QByteArray title;
    QByteArray genre;
    QByteArray artist;
    QByteArray album;
    QByteArray tracknum;
    QByteArray year;
    QByteArray duration;
    QByteArray coverid;
    QByteArray album_id;
    QByteArray song_id;
};

typedef QList< TrackData > CurrentPlayList;
typedef QHash< QString, QString > SlimItem;
typedef QHash< QString, QStringList > SlimItemList;

#endif // SQUEEZEDEFINES_H
