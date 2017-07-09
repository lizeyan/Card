#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
class QPushButton;
class QLineEdit;
class QLabel;
class Terminal : public QWidget
{
    Q_OBJECT

public:
    Terminal(QWidget *parent = 0);
    ~Terminal();

private:
    QPushButton *mCreateBtn, *mDeleteBtn, *mUpdateBtn, *mRechargeBtn, *mConsumeBtn;
    QLabel *mAvatarLabel, *mNameLabel, *mStudentIdLabel;
    QLineEdit *mRechargeEdit, *mConsumeEdit;
};

#endif // TERMINAL_H
