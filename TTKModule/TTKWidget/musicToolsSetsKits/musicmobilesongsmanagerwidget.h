#ifndef MUSICMOBILESONGSMANAGERWIDGET_H
#define MUSICMOBILESONGSMANAGERWIDGET_H

/***************************************************************************
 * This file is part of the TTK Music Player project
 * Copyright (C) 2015 - 2022 Greedysky Studio

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include "musicwidgetheaders.h"
#include "musicsearchinterface.h"
#include "musicabstractmovewidget.h"
#include "musicabstractsongslisttablewidget.h"

/*! @brief The class of the mobile songs table widget.
 * @author Greedysky <greedysky@163.com>
 */
class TTK_MODULE_EXPORT MusicMobileSongsTableWidget : public MusicAbstractSongsListTableWidget
{
    Q_OBJECT
    TTK_DECLARE_MODULE(MusicMobileSongsTableWidget)
public:
    /*!
     * Object contsructor.
     */
    explicit MusicMobileSongsTableWidget(QWidget *parent = nullptr);
    ~MusicMobileSongsTableWidget();

    /*!
     * Update songs files in table.
     */
    void updateSongsList(const QStringList &songs);

public Q_SLOTS:
    /*!
     * Remove all items.
     */
    virtual void removeItems() override final;
    /*!
     * Override the widget event.
     */
    virtual void contextMenuEvent(QContextMenuEvent *event) override final;

};

class MusicSongsManagerThread;

namespace Ui {
class MusicMobileSongsManagerWidget;
}

/*! @brief The class of the mobile songs manager widget.
 * @author Greedysky <greedysky@163.com>
 */
class TTK_MODULE_EXPORT MusicMobileSongsManagerWidget : public MusicAbstractMoveWidget, private MusicSearchInterface<QStringList>
{
    Q_OBJECT
    TTK_DECLARE_MODULE(MusicMobileSongsManagerWidget)
public:
    /*!
     * Object contsructor.
     */
    explicit MusicMobileSongsManagerWidget(QWidget *parent = nullptr);
    ~MusicMobileSongsManagerWidget();

    /*!
     * Find extra device path.
     */
    void findExtraDevicePath(const QString &dir);

Q_SIGNALS:
    /*!
     * Add current selected song to playlist.
     */
    void addSongToPlaylist(const QStringList &names);

public Q_SLOTS:
    /*!
     * Select all items.
     */
    void selectedAllItems(bool check);
    /*!
     * Music song audition play.
     */
    void auditionButtonClick();
    /*!
     * Add music song to play list.
     */
    void addButtonClick();
    /*!
     * Item cell on click by row and col.
     */
    void itemCellOnClick(int row, int column);
    /*!
     * Item cell on double click by row and col.
     */
    void itemDoubleClicked(int row, int column);
    /*!
     * Send the searched file or path.
     */
    void searchFilePathChanged(const QStringList &path);
    /*!
     * Search result from list.
     */
    void searchResultChanged(int row, int column);

private:
    /*!
     * Clear All Items.
     */
    void clearAllItems();
    /*!
     * Set current item or items selected.
     */
    void selectedItemsToPlaylist();

    Ui::MusicMobileSongsManagerWidget *m_ui;

    MusicSongsManagerThread *m_thread;

};

#endif // MUSICMOBILESONGSMANAGERWIDGET_H
