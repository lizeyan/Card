#include "terminal.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
Terminal::Terminal(QWidget *parent)
    : QWidget(parent)
    , mCreateBtn(new QPushButton(tr("Create"), this)), mDeleteBtn(new QPushButton(tr("Delete"), this))
    , mUpdateBtn(new QPushButton(tr("Register"), this)), mRechargeBtn(new QPushButton(tr("Recharge"), this))
    , mConsumeBtn(new QPushButton(tr("Consume"), this))
    , mAvatarLabel(new QLabel(this))
    , mNameLabel(new QLabel(this))
    , mStudentIdLabel(new QLabel(this))
    , mRechargeEdit(new QLineEdit(this))
    , mConsumeEdit(new QLineEdit(this))
{
    QVBoxLayout* operationLayout = new QVBoxLayout();
    QHBoxLayout* rechargeLayout = new QHBoxLayout();
    rechargeLayout->addWidget(mRechargeEdit);
    rechargeLayout->addWidget(mRechargeBtn);
    QHBoxLayout* consumeLayout = new QHBoxLayout();
    consumeLayout->addWidget(mConsumeEdit);
    consumeLayout->addWidget(mConsumeBtn);
    operationLayout->addWidget(mCreateBtn);
    operationLayout->addWidget(mDeleteBtn);
    operationLayout->addWidget(mUpdateBtn);
    operationLayout->addLayout(consumeLayout);
    operationLayout->addLayout(rechargeLayout);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->addWidget(mAvatarLabel);
    infoLayout->addWidget(mNameLabel);
    infoLayout->addWidget(mStudentIdLabel);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(operationLayout);
    mainLayout->addLayout(infoLayout);

}

Terminal::~Terminal()
{

}
