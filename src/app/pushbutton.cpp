#include "app/pushbutton.hpp"


PushButton::PushButton(QWidget *parent) : QPushButton(parent) {

}

PushButton::~PushButton() {

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

