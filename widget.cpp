#include "widget.h"
#include "ui_widget.h"
#include "calculatorlogic.h"
#include <QFontDatabase>
#include <QKeyEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), m_logic(new CalculatorLogic(this)), m_expectingFreshInput(false)
{
    ui->setupUi(this);
    setupUI();
    connectButtons();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupUI()
{
    setWindowTitle(tr("计算器"));
    setMaximumSize(200, 286);
    setMinimumSize(200, 286);

    // 跨平台字体：主界面使用系统默认字体覆盖 UI 文件中的硬编码中文字体
    QFont appFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    appFont.setPointSize(11);
    setFont(appFont);

    // 输入框使用等宽字体，确保数字对齐，且只读防止键盘直接输入到 QLineEdit
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(14);
    ui->mainLineEdit->setFont(monoFont);
    ui->mainLineEdit->setAlignment(Qt::AlignRight);
    ui->mainLineEdit->setReadOnly(true);

    // 移除硬编码图片路径，改用通用文本符号（跨平台、无需外部资源）
    ui->deleteButton->setText(tr("←"));
    ui->deleteButton->setIcon(QIcon());

    // QSS 样式美化
    setStyleSheet(R"(
        QLineEdit#mainLineEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px;
            background-color: #f9f9f9;
            color: #333333;
        }
        QPushButton {
            border: 1px solid #bbbbbb;
            border-radius: 4px;
            background-color: #e8e8e8;
            color: #222222;
            font-size: 13px;
            font-weight: bold;
            min-width: 40px;
            min-height: 40px;
        }
        QPushButton:hover {
            background-color: #d8d8d8;
            border: 1px solid #999999;
        }
        QPushButton:pressed {
            background-color: #c8c8c8;
        }
        QPushButton#equalButton {
            background-color: #4a90d9;
            color: white;
            border: 1px solid #357abd;
        }
        QPushButton#equalButton:hover {
            background-color: #357abd;
        }
        QPushButton#equalButton:pressed {
            background-color: #2a5a8a;
        }
        QPushButton#clearButton {
            background-color: #e74c3c;
            color: white;
            border: 1px solid #c0392b;
        }
        QPushButton#clearButton:hover {
            background-color: #c0392b;
        }
    )");
}

void Widget::connectButtons()
{
    // 数字按钮
    connect(ui->zeroButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->oneButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->twoButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->threeButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->fourButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->fiveButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->sixButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->sevenButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->eightButton, &QPushButton::clicked, this, &Widget::onDigitClicked);
    connect(ui->nineButton, &QPushButton::clicked, this, &Widget::onDigitClicked);

    // 运算符按钮
    connect(ui->addButton, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->subButton, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->mulButton, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->divButton, &QPushButton::clicked, this, &Widget::onOperatorClicked);

    // 括号按钮
    connect(ui->leftButton, &QPushButton::clicked, this, &Widget::onLeftParenClicked);
    connect(ui->rightButton, &QPushButton::clicked, this, &Widget::onRightParenClicked);

    // 功能按钮
    connect(ui->equalButton, &QPushButton::clicked, this, &Widget::onEqualClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &Widget::onClearClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &Widget::onDeleteClicked);
}

void Widget::appendToExpression(const QString &text)
{
    if (m_expectingFreshInput)
    {
        bool isOperator = (text == "+" || text == "-" || text == "*" || text == "/");
        bool isParen = (text == "(" || text == ")");
        if (!isOperator && !isParen)
        {
            m_expression.clear();
        }
        m_expectingFreshInput = false;
    }

    m_expression += text;
    ui->mainLineEdit->setText(m_expression);
}

void Widget::onDigitClicked()
{
    if (QPushButton *btn = qobject_cast<QPushButton *>(sender()))
    {
        appendToExpression(btn->text());
    }
}

void Widget::onOperatorClicked()
{
    if (QPushButton *btn = qobject_cast<QPushButton *>(sender()))
    {
        appendToExpression(btn->text());
    }
}

void Widget::onLeftParenClicked()
{
    appendToExpression("(");
}

void Widget::onRightParenClicked()
{
    appendToExpression(")");
}

void Widget::onEqualClicked()
{
    CalculatorLogic::Result res = m_logic->evaluate(m_expression);
    if (res.success)
    {
        ui->mainLineEdit->setText(res.display);
        m_expression = res.display;
    }
    else
    {
        ui->mainLineEdit->setText(res.display);
        m_expression.clear();
    }
    m_expectingFreshInput = true;
}

void Widget::onClearClicked()
{
    m_expression.clear();
    ui->mainLineEdit->clear();
    m_expectingFreshInput = false;
}

void Widget::onDeleteClicked()
{
    if (m_expectingFreshInput)
    {
        onClearClicked();
        return;
    }
    m_expression.chop(1);
    ui->mainLineEdit->setText(m_expression);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        appendToExpression(QString::number(event->key() - Qt::Key_0));
        break;
    case Qt::Key_Plus:
        appendToExpression("+");
        break;
    case Qt::Key_Minus:
        appendToExpression("-");
        break;
    case Qt::Key_Asterisk:
        appendToExpression("*");
        break;
    case Qt::Key_Slash:
        appendToExpression("/");
        break;
    case Qt::Key_ParenLeft:
        appendToExpression("(");
        break;
    case Qt::Key_ParenRight:
        appendToExpression(")");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Equal:
        onEqualClicked();
        break;
    case Qt::Key_Backspace:
        onDeleteClicked();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Escape:
        onClearClicked();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
