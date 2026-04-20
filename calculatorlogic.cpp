#include "calculatorlogic.h"
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
#include <stdexcept>

CalculatorLogic::CalculatorLogic(QObject *parent)
    : QObject(parent)
{
}

int CalculatorLogic::precedence(char op) const
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

bool CalculatorLogic::applyTopOperator(std::stack<double> &values, std::stack<char> &ops) const
{
    if (ops.empty())
        return false;

    char op = ops.top();
    ops.pop();

    if (values.size() < 2)
        return false;

    double right = values.top();
    values.pop();
    double left = values.top();
    values.pop();

    double result = 0.0;
    switch (op)
    {
    case '+':
        result = left + right;
        break;
    case '-':
        result = left - right;
        break;
    case '*':
        result = left * right;
        break;
    case '/':
        if (std::fabs(right) < 1e-12)
            return false;
        result = left / right;
        break;
    default:
        return false;
    }

    values.push(result);
    return true;
}

CalculatorLogic::Result CalculatorLogic::evaluate(const QString &expression) const
{
    if (expression.isEmpty())
    {
        return {false, tr("错误：表达式为空")};
    }

    std::string expr = expression.toStdString();
    std::stack<double> values;
    std::stack<char> ops;

    for (size_t i = 0; i < expr.size(); ++i)
    {
        char ch = expr[i];

        if (std::isspace(static_cast<unsigned char>(ch)))
            continue;

        if (std::isdigit(static_cast<unsigned char>(ch)))
        {
            std::string numStr;
            numStr += ch;
            ++i;
            while (i < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i])))
            {
                numStr += expr[i];
                ++i;
            }
            --i;

            try
            {
                size_t pos = 0;
                long long val = std::stoll(numStr, &pos);
                if (pos != numStr.size())
                {
                    return {false, tr("错误：数字解析失败")};
                }
                values.push(static_cast<double>(val));
            }
            catch (const std::invalid_argument &)
            {
                return {false, tr("错误：无效的数字格式")};
            }
            catch (const std::out_of_range &)
            {
                return {false, tr("错误：数字超出范围")};
            }
        }
        else if (ch == '(')
        {
            ops.push(ch);
        }
        else if (ch == ')')
        {
            while (!ops.empty() && ops.top() != '(')
            {
                if (!applyTopOperator(values, ops))
                {
                    return {false, tr("错误：括号内表达式无效")};
                }
            }
            if (ops.empty())
            {
                return {false, tr("错误：括号不匹配")};
            }
            ops.pop(); // remove '('
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            // 一元减号检测：出现在开头、左括号后、或其他运算符后
            if (ch == '-' && (i == 0 || expr[i - 1] == '(' ||
                              expr[i - 1] == '+' || expr[i - 1] == '-' ||
                              expr[i - 1] == '*' || expr[i - 1] == '/'))
            {
                // 一元减号后必须紧跟数字
                size_t j = i + 1;
                if (j >= expr.size() ||
                    !std::isdigit(static_cast<unsigned char>(expr[j])))
                {
                    return {false, tr("错误：无效的负号用法")};
                }
                std::string numStr;
                while (j < expr.size() && std::isdigit(static_cast<unsigned char>(expr[j])))
                {
                    numStr += expr[j];
                    ++j;
                }
                if (numStr.empty())
                {
                    return {false, tr("错误：无效的数字")};
                }
                try
                {
                    size_t pos = 0;
                    long long val = std::stoll(numStr, &pos);
                    if (pos != numStr.size())
                    {
                        return {false, tr("错误：数字解析失败")};
                    }
                    values.push(-static_cast<double>(val));
                    i = j - 1;
                }
                catch (...)
                {
                    return {false, tr("错误：无效的数字格式")};
                }
            }
            else
            {
                while (!ops.empty() && precedence(ops.top()) >= precedence(ch))
                {
                    if (!applyTopOperator(values, ops))
                    {
                        return {false, tr("错误：运算符处理失败")};
                    }
                }
                ops.push(ch);
            }
        }
        else
        {
            return {false, tr("错误：包含非法字符")};
        }
    }

    while (!ops.empty())
    {
        if (ops.top() == '(' || ops.top() == ')')
        {
            return {false, tr("错误：括号不匹配")};
        }
        if (!applyTopOperator(values, ops))
        {
            return {false, tr("错误：表达式计算失败")};
        }
    }

    if (values.size() != 1)
    {
        return {false, tr("错误：无效的表达式")};
    }

    double finalResult = values.top();
    values.pop();

    QString resultStr = QString::number(finalResult, 'g', 12);
    return {true, resultStr};
}
