#include "qerrormessage.h"


QErrorMessage::QErrorMessage(Pump* pump, QWidget *parent) : QTextEdit(parent), message(""), pump(pump) {
    setReadOnly(true); // QTextEdit for error messages should be read-only
}

void QErrorMessage::showMessage(QString message) {
    this->message = message;
    setText(message);
}

void QErrorMessage::updateAlert() {
    // Placeholder for updating the alert (e.g., changing color, style, etc.)
    // This could involve changing the style sheet or appending new text
}
