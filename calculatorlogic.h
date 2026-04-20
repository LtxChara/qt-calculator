#ifndef CALCULATORLOGIC_H
#define CALCULATORLOGIC_H

#include <QObject>
#include <QString>
#include <stack>

class CalculatorLogic : public QObject
{
    Q_OBJECT
public:
    explicit CalculatorLogic(QObject *parent = nullptr);

    struct Result {
        bool success;
        QString display; // 计算结果或错误信息
    };

    Result evaluate(const QString &expression) const;

private:
    int precedence(char op) const;
    bool applyTopOperator(std::stack<double> &values, std::stack<char> &ops) const;
};

#endif // CALCULATORLOGIC_H
