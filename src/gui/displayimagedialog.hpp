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

    cv::Mat stitched_normal_mat;
    cv::Mat stitched_parsed_mat;
    cv::Mat stitched_thresholded_mat;

public slots:
    // Receive norma, parsed and thresholded cv::Mat to be displayed
    void receive_show_DisplayImageDialog_request(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat);

private slots:
    void pushButtonSaveImage_clicked(bool checked = false);

};

