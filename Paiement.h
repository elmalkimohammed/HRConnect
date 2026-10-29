#ifndef PAIEMENT_H
#define PAIEMENT_H

#include <string>
#include <mysql.h>
#include "Employe_Basic_Info.h"
#include "D_H.h"

class Paiement : public Employe_Basic_Info, public D_H
{
public:
    Paiement(const string&, const string&, const int&, const string&, const string&);
    bool Authentication(const string&, const string&) const override;
    void ConsulterAbs() override;
    void ConsulterProduc() override;
    void CalculeSal();
    void ModifierInfosBancaires();
    void genererBord();
    void setDbConnection(MYSQL* conn);
private:
    MYSQL* dbConnection;
};

#endif // PAIEMENT_H
