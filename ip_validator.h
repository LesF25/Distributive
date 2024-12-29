#pragma once
#include <QValidator>
#include <QRegExp>
#include <QDebug>

class IPValidator : public QValidator
{
    Q_OBJECT

private:
    QRegExpValidator* ipv4_validator;
    QRegExpValidator* ipv6_validator;

public:
    explicit IPValidator(QObject *parent = nullptr) : QValidator(parent) {
        this->ipv4_validator = new QRegExpValidator(this);
        this->ipv4_validator->setRegExp(QRegExp(
                    "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));

        this->ipv6_validator = new QRegExpValidator(this);
        this->ipv6_validator->setRegExp(QRegExp(
                    "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|"
                    "([0-9a-fA-F]{1,4}:){1,7}:|"
                    "([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|"
                    "([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|"
                    "([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|"
                    "([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|"
                    "([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|"
                    "[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|"
                    ":((:[0-9a-fA-F]{1,4}){1,7}|:)|"
                    "fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|"
                    "::(ffff(:0{1,4}){0,1}:){0,1}"
                    "((25[0-5]|(2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
                    "(25[0-5]|(2[0-4][0-9]|[01]?[0-9][0-9]?)))|"
                    "([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
                    "(25[0-5]|(2[0-4][0-9]|[01]?[0-9][0-9]?)))|"
                    "([0-9a-fA-F]{1,4}:){1,3}:([0-9a-fA-F]{1,4}:){1,4}|"
                    "([0-9a-fA-F]{1,4}:){1,2}:([0-9a-fA-F]{1,4}:){1,5}|"
                    "[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|"
                    ":((:[0-9a-fA-F]{1,4}){1,7}|:))"));
    }

    State validate(QString &input, int &pos) const override {
            if(ipv4_validator->validate(input, pos) == Intermediate || ipv4_validator->validate(input, pos) == Acceptable)
                return Acceptable;
            if(ipv6_validator->validate(input, pos) == Intermediate || ipv6_validator->validate(input, pos) == Acceptable)
                return Acceptable;

            return Invalid;
        }
};
