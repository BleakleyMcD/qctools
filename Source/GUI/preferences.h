#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "Core/Core.h"
#include <QDialog>
#include <QList>
#include <QUrl>
#include <tuple>

namespace Ui {
class Preferences;
}

class SignalServerConnectionChecker;
class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(SignalServerConnectionChecker* connectionChecker, QWidget *parent = 0);
    ~Preferences();

    //Preferences
    activefilters ActiveFilters;
    activealltracks ActiveAllTracks;

    QList<std::tuple<int, int>> loadFilterSelectorsOrder();
    void saveFilterSelectorsOrder(const QList<std::tuple<int, int>>& order);

    bool isSignalServerEnabled() const;
    bool isSignalServerAutoUploadEnabled() const;

    QUrl signalServerUrl() const;
    QString signalServerLogin() const;
    QString signalServerPassword() const;

Q_SIGNALS:
    void saved();

private:
    Ui::Preferences *ui;
    SignalServerConnectionChecker* connectionChecker;

    void Load();
    void Save();

private Q_SLOTS:
    void on_testConnection_pushButton_clicked();
    void OnAccepted();
    void OnRejected();
};

#endif // PREFERENCES_H
