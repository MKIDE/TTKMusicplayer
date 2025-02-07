#include "musicconsolemodule.h"
#include "musicplayer.h"
#include "musicplaylist.h"
#include "musicformats.h"
#include "musictkplconfigmanager.h"
#include "musichotkeymanager.h"
#include "musicfileutils.h"
#include "ttkversion.h"

MusicConsoleModule::MusicConsoleModule(QObject *parent)
    : QObject(parent),
      m_volume(100),
      m_playbackMode("Order"),
      m_enhanced("Off")
{
    m_playlist = new MusicPlaylist(this);
    m_playlist->setPlaybackMode(MusicObject::PlayMode::Order);
    m_player = new MusicPlayer(this);
    m_player->setPlaylist(m_playlist);

    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(m_playlist, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));

    G_HOTKEY_PTR->addHotKey("Ctrl+B");
    G_HOTKEY_PTR->addHotKey("Ctrl+Left");
    G_HOTKEY_PTR->addHotKey("Ctrl+Right");
    G_HOTKEY_PTR->addHotKey("Ctrl+Up");
    G_HOTKEY_PTR->addHotKey("Ctrl+Down");
    G_HOTKEY_PTR->addHotKey("Ctrl+1");
    G_HOTKEY_PTR->addHotKey("Ctrl+2");
    G_HOTKEY_PTR->addHotKey("Ctrl+3");
    G_HOTKEY_PTR->addHotKey("Ctrl+4");
    G_HOTKEY_PTR->addHotKey("Ctrl+5");
    G_HOTKEY_PTR->addHotKey("Alt+1");
    G_HOTKEY_PTR->addHotKey("Alt+2");
    G_HOTKEY_PTR->addHotKey("Alt+3");
    G_HOTKEY_PTR->addHotKey("Alt+4");
    G_HOTKEY_PTR->addHotKey("Alt+5");
    G_HOTKEY_PTR->addHotKey("Ctrl+Q");

    connect(G_HOTKEY_PTR->hotKey(0), SIGNAL(activated()), SLOT(musicStatePlay()));
    connect(G_HOTKEY_PTR->hotKey(1), SIGNAL(activated()), SLOT(musicPlayPrevious()));
    connect(G_HOTKEY_PTR->hotKey(2), SIGNAL(activated()), SLOT(musicPlayNext()));
    connect(G_HOTKEY_PTR->hotKey(3), SIGNAL(activated()), SLOT(musicActionVolumePlus()));
    connect(G_HOTKEY_PTR->hotKey(4), SIGNAL(activated()), SLOT(musicActionVolumeSub()));

    connect(G_HOTKEY_PTR->hotKey(5), SIGNAL(activated()), SLOT(musicPlayOrder()));
    connect(G_HOTKEY_PTR->hotKey(6), SIGNAL(activated()), SLOT(musicPlayRandom()));
    connect(G_HOTKEY_PTR->hotKey(7), SIGNAL(activated()), SLOT(musicPlaylistLoop()));
    connect(G_HOTKEY_PTR->hotKey(8), SIGNAL(activated()), SLOT(musicPlayOneLoop()));
    connect(G_HOTKEY_PTR->hotKey(9), SIGNAL(activated()), SLOT(musicPlayOnce()));

    connect(G_HOTKEY_PTR->hotKey(10), SIGNAL(activated()), SLOT(musicEnhancedOff()));
    connect(G_HOTKEY_PTR->hotKey(11), SIGNAL(activated()), SLOT(musicEnhanced3D()));
    connect(G_HOTKEY_PTR->hotKey(12), SIGNAL(activated()), SLOT(musicEnhancedNICAM()));
    connect(G_HOTKEY_PTR->hotKey(13), SIGNAL(activated()), SLOT(musicEnhancedSubwoofer()));
    connect(G_HOTKEY_PTR->hotKey(14), SIGNAL(activated()), SLOT(musicEnhancedVocal()));

    connect(G_HOTKEY_PTR->hotKey(15), SIGNAL(activated()), qApp, SLOT(quit()));
}

MusicConsoleModule::~MusicConsoleModule()
{
    delete m_player;
    delete m_playlist;
}

bool MusicConsoleModule::initialize(const QCoreApplication &app)
{
    TTK_LOG_STREAM("\n" APP_NAME << "Console Module" << TTK_VERSION_STR "\n");

    TTKCommandLineOption op1("-u", "--url", "Music play url path");
    TTKCommandLineOption op2("-d", "--dir", "Music play dir path");
    TTKCommandLineOption op3("-l", "--playlist", "Music playlist url path");

    TTKCommandLineParser parser;
    parser.addOption(op1);
    parser.addOption(op2);
    parser.addOption(op3);
    parser.process(app);

    if(parser.isEmpty())
    {
        parser.printHelp();
        return false;
    }

    if(parser.isSet(op1))
    {
        const QString &path = parser.value(op1);
        if(path.isEmpty())
        {
            TTK_LOG_STREAM("Music play url path is empty");
            return false;
        }
        else
        {
            TTK_LOG_STREAM("Add play url path: " << path);
            m_playlist->add(0, path);
            m_playlist->setCurrentIndex(0);
        }
    }
    else if(parser.isSet(op2))
    {
        const QString &path = parser.value(op2);
        if(path.isEmpty())
        {
            TTK_LOG_STREAM("Music play dir path is empty");
            return false;
        }
        else
        {
            for(const QFileInfo &fin : MusicUtils::File::fileInfoListByPath(path, MusicFormats::supportMusicInputFilterFormats()))
            {
                TTK_LOG_STREAM("Add play url path: " << fin.absoluteFilePath());
                m_playlist->append(0, fin.absoluteFilePath());
            }

            if(!m_playlist->isEmpty())
            {
                m_playlist->setCurrentIndex(0);
            }
        }
    }
    else if(parser.isSet(op3))
    {
        const QString &path = parser.value(op3);
        if(path.isEmpty())
        {
            TTK_LOG_STREAM("Music playlist path is empty");
            return false;
        }
        else
        {
            if(FILE_SUFFIX(QFileInfo(path)) != TPL_FILE_SUFFIX)
            {
                TTK_LOG_STREAM("Music playlist format not support");
                return false;
            }

            MusicTKPLConfigManager manager;
            if(!manager.fromFile(path))
            {
                TTK_LOG_STREAM("Music playlist read error");
                return false;
            }

            MusicSongItemList items;
            manager.readBuffer(items);

            for(const MusicSongItem &item : qAsConst(items))
            {
                for(const MusicSong &song : qAsConst(item.m_songs))
                {
                    TTK_LOG_STREAM("Add play url path: " << song.path());
                    m_playlist->append(0, song.path());
                }
            }

            if(!m_playlist->isEmpty())
            {
                m_playlist->setCurrentIndex(0);
            }
        }
    }
    else
    {
        TTK_LOG_STREAM("Options unknown error");
        return false;
    }

    TTK_LOG_STREAM("Music Files count: " << m_playlist->count() << "\n");

    m_player->play();
    m_player->setVolume(m_volume);
    return app.exec();
}

void MusicConsoleModule::positionChanged(qint64 position)
{
    print(position, m_player->duration());
}

void MusicConsoleModule::currentIndexChanged(int index)
{
    TTK_LOG_STREAM("Current Play Indedx: " << index);
    QTimer::singleShot(MT_S2MS, this, SLOT(resetVolume()));
    if(index == TTK_NORMAL_LEVEL)
    {
        m_player->stop();
        QTimer::singleShot(MT_S2MS, qApp, SLOT(quit()));
        TTK_LOG_STREAM("Music play end and application quit now");
    }
}

void MusicConsoleModule::musicStatePlay()
{
    if(m_playlist->isEmpty())
    {
        return;
    }

    if(m_player->isPlaying())
    {
        m_player->pause();
    }
    else
    {
        m_player->play();
        m_player->setVolume(m_volume);
    }
}

void MusicConsoleModule::musicPlayPrevious()
{
    if(m_playlist->isEmpty())
    {
        return;
    }

    if(m_playlist->playbackMode() == MusicObject::PlayMode::Random)
    {
        m_playlist->setCurrentIndex();
    }
    else
    {
        m_player->playPrevious();
    }

    m_player->play();
    m_player->setVolume(m_volume);
}

void MusicConsoleModule::musicPlayNext()
{
    if(m_playlist->isEmpty())
    {
        return;
    }

    if(m_playlist->playbackMode() == MusicObject::PlayMode::Random)
    {
        m_playlist->setCurrentIndex();
    }
    else
    {
        m_player->playNext();
    }

    m_player->play();
    m_player->setVolume(m_volume);
}

void MusicConsoleModule::resetVolume()
{
    m_player->setVolume(m_volume);
}

void MusicConsoleModule::musicActionVolumeSub()
{
    m_volume = m_player->volume();
    m_volume -= 15;
    if(m_volume < 0)
    {
        m_volume = 0;
    }

    m_player->setVolume(m_volume);
}

void MusicConsoleModule::musicActionVolumePlus()
{
    m_volume = m_player->volume();
    m_volume += 15;
    if(m_volume > 100)
    {
        m_volume = 100;
    }

    m_player->setVolume(m_volume);
}

void MusicConsoleModule::musicPlayOrder()
{
    m_playlist->setPlaybackMode(MusicObject::PlayMode::Order);
    m_playbackMode = "Order";
}

void MusicConsoleModule::musicPlayRandom()
{
    m_playlist->setPlaybackMode(MusicObject::PlayMode::Random);
    m_playbackMode = "Random";
}

void MusicConsoleModule::musicPlaylistLoop()
{
    m_playlist->setPlaybackMode(MusicObject::PlayMode::ListLoop);
    m_playbackMode = "ListLoop";
}

void MusicConsoleModule::musicPlayOneLoop()
{
    m_playlist->setPlaybackMode(MusicObject::PlayMode::OneLoop);
    m_playbackMode = "OneLoop";
}

void MusicConsoleModule::musicPlayOnce()
{
    m_playlist->setPlaybackMode(MusicObject::PlayMode::Once);
    m_playbackMode = "Once";
}

void MusicConsoleModule::musicEnhancedOff()
{
    m_player->setMusicEnhanced(MusicPlayer::Enhance::Off);
    m_enhanced = "Off";
}

void MusicConsoleModule::musicEnhanced3D()
{
    m_player->setMusicEnhanced(MusicPlayer::Enhance::M3D);
    m_enhanced = "3D";
}

void MusicConsoleModule::musicEnhancedNICAM()
{
    m_player->setMusicEnhanced(MusicPlayer::Enhance::NICAM);
    m_enhanced = "NICAM";
}

void MusicConsoleModule::musicEnhancedSubwoofer()
{
    m_player->setMusicEnhanced(MusicPlayer::Enhance::Subwoofer);
    m_enhanced = "Subwoofer";
}

void MusicConsoleModule::musicEnhancedVocal()
{
    m_player->setMusicEnhanced(MusicPlayer::Enhance::Vocal);
    m_enhanced = "Vocal";
}

void MusicConsoleModule::print(qint64 position, qint64 duration) const
{
    const MusicPlayItem &item = m_playlist->currentItem();
    TTK_LOG_STREAM(QString("Music Name: %1, Time:[%2/%3], Volume:%4, PlaybackMode:%5, Enhance:%6")
                .arg(item.m_path, TTKTime::formatDuration(position), TTKTime::formatDuration(duration))
                .arg(m_player->volume())
                .arg(m_playbackMode, m_enhanced));
}
