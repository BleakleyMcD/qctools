/*  Copyright (c) BAVC. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef GUI_TinyDisplay_H
#define GUI_TinyDisplay_H

#include <QWidget>
#include <QVector>

class FileInformation;
class Control;
class BigDisplay;

class QLabel;
class QToolButton;
class QHBoxLayout;

class TinyDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit TinyDisplay(QWidget *parent, FileInformation* FileInfoData);
    ~TinyDisplay();

    // To update
    Control                    *ControlArea;
    BigDisplay                 *BigDisplayArea;

    // Commands
    void                        Update();
    void                        Filters_Show(); //Quick hack for showing filters

private:
    static const int            TOTAL_THUMBS = 9;
    static const int            THUMB_WIDTH = 84;
    static const int            THUMB_HEIGHT = 84;

    QPixmap                     emptyPixmap;
    QPixmap                     scaledLogo;

    void                        updateThumbnails();

    int                         lastWidth;

    QHBoxLayout*                Layout;

protected:
    // File information
    FileInformation*            FileInfoData;

    bool                        needsUpdate;
    unsigned long               lastFramePos;

    // Widgets
    QVector<QToolButton*>       thumbnails;

private Q_SLOTS:
    void                        on_thumbnails_clicked(bool checked);
};

#endif
