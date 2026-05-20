#include "gui/logdialog.hpp"

#include "base/settings.hpp"

#include "utils/log.hpp"
#include "utils/file.hpp"
#include "utils/str.hpp"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>


LogDialog::LogDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogDialog) {
    this->ui->setupUi(this);

    // Time to process log messages
    this->log_timer = new QTimer(this);
    this->log_timer->setInterval(1);

    // Event filters for buttosn
    this->ui->pushButtonSaveLog->installEventFilter(this);
    this->ui->pushButtonClearLog->installEventFilter(this);
    this->ui->pushButtonClose->installEventFilter(this);

    // Show default statustip
    this->showDefaultStatusTip();

    connect(this->log_timer, &QTimer::timeout, this, &LogDialog::log_timer_timeout);
    this->log_timer->start();

    connect(this->ui->pushButtonSaveLog, &QPushButton::clicked, this, &LogDialog::pushButtonSaveLog_clicked);
    connect(this->ui->pushButtonClearLog, &QPushButton::clicked, this, &LogDialog::pushButtonClearLog_clicked);

    connect(this->ui->plainTextEditLogs, &QPlainTextEdit::blockCountChanged, this, &LogDialog::plainTextEditLogs_blockcountchanged);

    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &LogDialog::hide);
}

LogDialog::~LogDialog() {
    this->log_timer->deleteLater();
    delete this->ui;
}

void LogDialog::showDefaultStatusTip() {
    this->ui->logStatusLabel->setText(QStringLiteral("Lines: %1").arg(
                QString::number(this->ui->plainTextEditLogs->blockCount())
                ));
}

bool LogDialog::eventFilter(QObject *o, QEvent *e) {
    auto widget = qobject_cast<QWidget*>(o);

    if (!widget) {
        return QDialog::eventFilter(o, e);
    }

    switch (e->type()) {
        // Mouse enter widget area
        case QEvent::Enter:
        {
            this->ui->logStatusLabel->setText(widget->statusTip());
            return true;
        }
        // Mouse leaves widget area
        case QEvent::Leave:
        {
            this->showDefaultStatusTip();
            return true;
        }
        default:
            break;
    }

    return QDialog::eventFilter(o, e);
}

void LogDialog::receive_show_LogDialog_request(bool checked) {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->show();
    }
}

void LogDialog::pushButtonSaveLog_clicked(bool checked) {
    QString save_path = Utils::FileDialog::saveLog(this);

    if (Utils::String::isNullOrEmpty(save_path)) {
        return;
    }

    QFile log_file = QFile(save_path);
    if (log_file.open(QFile::ReadWrite)) {
        QTextStream text_stream = QTextStream(&log_file);
        text_stream << this->ui->plainTextEditLogs->toPlainText();
        text_stream.flush();
        if (text_stream.status() == QTextStream::Ok) {
            QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Log written to file"));
        } else if (text_stream.status() == QTextStream::WriteFailed) {
            QMessageBox::warning(this, QStringLiteral("Warning"), QStringLiteral("Log file could not be saved"));
        }
        log_file.close();
    }
}

void LogDialog::pushButtonClearLog_clicked(bool checked) {
    this->ui->plainTextEditLogs->clear();
}

void LogDialog::plainTextEditLogs_blockcountchanged(int newBlockCount) {
    this->ui->logStatusLabel->setText(QStringLiteral("Lines: %1").arg(QString::number(newBlockCount)));
}

void LogDialog::log_timer_timeout() {
    if (!Log::isLogQueueEmpty()) {
        this->ui->plainTextEditLogs->appendHtml(Log::getQueuedMessage());
    }
}

