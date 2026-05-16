#pragma once

#include "ui/ui_stitchedimagedialog.h"

#include <opencv2/core.hpp>

#include "app/stitchedimageview.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
    class StitchedImageDialog;
};

class StitchedImageDialog : public QDialog {
    Q_OBJECT;
public:
    explicit StitchedImageDialog(QWidget *parent = nullptr);
    ~StitchedImageDialog() override;

private:
    Ui::StitchedImageDialog *ui;

    cv::Mat stitched_normal_mat;
    cv::Mat stitched_parsed_mat;
    cv::Mat stitched_thresholded_mat;

public slots:
    // Receive norma, parsed and thresholded cv::Mat to be displayed
    void receive_show_DisplayImageDialog_request(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat);

private slots:
    void pushButtonSaveImage_clicked(bool checked = false);

};

