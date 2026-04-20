#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class CalculatorLogic;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDigitClicked();
    void onOperatorClicked();
    void onLeftParenClicked();
    void onRightParenClicked();
    void onEqualClicked();
    void onClearClicked();
    void onDeleteClicked();

private:
    void appendToExpression(const QString &text);
    void setupUI();
    void connectButtons();

    Ui::Widget *ui;
    CalculatorLogic *m_logic;
    QString m_expression;
    bool m_expectingFreshInput;
};

#endif // WIDGET_H
