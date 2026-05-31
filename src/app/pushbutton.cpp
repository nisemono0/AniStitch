#include "app/pushbutton.hpp"

#include <QTimer>


PushButton::PushButton(QWidget *parent) : QPushButton(parent) {
    this->shortcuts_list = QList<QShortcut *>();
}

PushButton::~PushButton() {
    qDeleteAll(this->shortcuts_list);
}

void PushButton::addShortcut(const QKeySequence &key_seq, void (PushButton::*method)()) {
    QShortcut *shortcut = new QShortcut(key_seq, this);
    this->shortcuts_list.append(shortcut);

    connect(shortcut, &QShortcut::activated, this, method);
}

void PushButton::animateRightClick() {
    this->setDown(true);
    // animateClick also uses 100ms
    QTimer::singleShot(100, this, [this]{
                this->setDown(false);
                emit rightClicked();
            });
}

void PushButton::mouseMoveEvent(QMouseEvent *e) {
    if (this->is_pressed && e->buttons().testFlag(Qt::RightButton)) {
        this->setDown(this->hitButton(e->position().toPoint()));
    }
    QPushButton::mouseMoveEvent(e);
}

void PushButton::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton && this->hitButton(e->position().toPoint())) {
        if (!this->isDown()) {
            this->setDown(true);
        }
        this->is_pressed = true;
    }
    QPushButton::mousePressEvent(e);
}

void PushButton::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton && this->is_pressed) {
        this->is_pressed = false;
        if (this->hitButton(e->position().toPoint())) {
            emit rightClicked();
        }
        this->setDown(false);
    }
    QPushButton::mouseReleaseEvent(e);
}

