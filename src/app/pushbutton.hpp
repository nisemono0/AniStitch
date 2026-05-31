#pragma once

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QKeySequence>
#include <QShortcut>


class PushButton : public QPushButton {
    Q_OBJECT;
public:
    explicit PushButton(QWidget *parent = nullptr);
    ~PushButton();

    // Add shortcut and its corresponding trigger handler
    void addShortcut(const QKeySequence &key_seq, void (PushButton::*method)());
    // Animate right click press similar to animateClick
    void animateRightClick();

private:
    bool is_pressed = false;
    QList<QShortcut *> shortcuts_list;

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

signals:
    void rightClicked();

};

