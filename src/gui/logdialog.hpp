#pragma once

#include "ui/ui_logdialog.h"

#include <QDialog>
#include <QWidget>
#include <QTimer>
#include <QEvent>


namespace Ui {
    class LogDialog;
};

class LogDialog : public QDialog {
    Q_OBJECT;
public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog() override;

private:
    Ui::LogDialog *ui;

    QTimer *log_timer;

    void showDefaultStatusTip();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

public slots:
    void receive_show_LogDialog_request(bool checked = false);

private slots:
    void pushButtonSaveLog_clicked(bool checked = false);
    void pushButtonClearLog_clicked(bool checked = false);

    void plainTextEditLogs_blockcountchanged(int newBlockCount);

    void log_timer_timeout();
};

