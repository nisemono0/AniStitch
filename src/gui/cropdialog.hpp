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

signals:
    void send_CropDialog_crop_values(int top_px, int right_px, int bottom_px, int left_px);

public slots:
    void receive_show_CropDialog_request();

private slots:
    // Reset values
    void resetValues();
    // Crop button pressed
    void pushButtonCrop_clicked(bool checked = false);
};

