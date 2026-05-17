#pragma once

#include "ui/ui_cropdialog.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
    class CropDialog;
};

class CropDialog : public QDialog {
    Q_OBJECT;
public:
    explicit CropDialog(QWidget *parent = nullptr);
    ~CropDialog() override;

private:
    Ui::CropDialog *ui;

    // Reset values
    void resetValues();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

signals:
    void send_CropDialog_crop_value(int top_px, int right_px, int bottom_px, int left_px);
    void send_CropDialog_crop_selection();
    void send_CropDialog_reset();

public slots:
    void receive_show_CropDialog_request();

private slots:
    // Reset button
    void pushButtonReset_clicked(bool checked = false);
    // Crop button pressed
    void pushButtonCropValue_clicked(bool checked = false);
    void pushButtonCropSelection_clicked(bool checked = false);
};

