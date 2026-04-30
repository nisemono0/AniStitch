#pragma once

#include "ui/ui_displayimagedialog.h"

#include <opencv2/core.hpp>

#include "app/displayimageview.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
    class DisplayImageDialog;
};

class DisplayImageDialog : public QDialog {
    Q_OBJECT;
public:
    explicit DisplayImageDialog(QWidget *parent = nullptr);
    ~DisplayImageDialog() override;

private:
    Ui::DisplayImageDialog *ui;

    cv::Mat stitched_cv_mat;

public slots:
    // Display cv_mat on DisplayImageDialog
    void receive_show_DisplayImageDialog_request(const cv::Mat &cv_mat);

private slots:
    void pushButtonSaveImage_clicked(bool checked = false);

};

