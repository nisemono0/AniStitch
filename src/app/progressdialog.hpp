#pragma once

#include <QWidget>
#include <QProgressDialog>


class ProgressDialog : public QProgressDialog {
    Q_OBJECT;
public:
    explicit ProgressDialog(QWidget *parent = nullptr, bool no_cancel_button = false);
    ~ProgressDialog();

public slots:
    void receive_progress(int progress);
};

