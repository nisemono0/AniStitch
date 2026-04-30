#pragma once

#include "ui/ui_dualprogressdialog.h"

#include <QWidget>
#include <QDialog>


namespace Ui {
    class DualProgressDialog;
};

class DualProgressDialog : public QDialog {
    Q_OBJECT;
public:
    explicit DualProgressDialog(QWidget *parent = nullptr);
    ~DualProgressDialog() override;

private:
    Ui::DualProgressDialog *ui;

signals:
    void cancel_progress();

public slots:
    void receive_reset_current_progress_request();
    void receive_current_progress(int current_progress);
    void receive_total_progress(int total_progress);

private slots:
    void pushButtonCancel_clicked(bool checked = false);
};

