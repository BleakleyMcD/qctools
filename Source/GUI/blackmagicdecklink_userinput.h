#ifndef BLACKMAGIC_DECKLINK_USERINPUT_H
#define BLACKMAGIC_DECKLINK_USERINPUT_H

#include "Core/BlackmagicDeckLink_Glue.h"

#include <QDialog>

namespace Ui {
class BlackmagicDeckLink_UserInput;
}

class BlackmagicDeckLink_UserInput : public QDialog
{
    Q_OBJECT

public:
    explicit BlackmagicDeckLink_UserInput(QWidget *parent = 0);
    ~BlackmagicDeckLink_UserInput();

    BlackmagicDeckLink_Glue*    Card;
    int                         FrameCount;
    QString                     Encoding_FileName;

    // Callbacks
    void                        TimeCode_IsAvailable();

private:
    Ui::BlackmagicDeckLink_UserInput *ui;

private Q_SLOTS:

    void on_accepted();
    void on_Record_GroupBox_toggled(bool on);
    void on_CardsList_currentIndexChanged(int Pos);
};

#endif // BLACKMAGIC_DECKLINK_USERINPUT_H
