#pragma once

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>


class PushButton : public QPushButton {
    Q_OBJECT;
public:
    explicit PushButton(QWidget *parent = nullptr);
    ~PushButton();

private:
    bool is_pressed = false;

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

signals:
    void rightClicked();

};

