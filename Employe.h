#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <string>
#include <mysql.h>
#include "Employe_Basic_Info.h"
#include "D_H.h"

class Employe : public Employe_Basic_Info, public D_H
{
    public:
        Employe(const string&, const string&, const int&, const string&, const string&);
        bool Authentication(const string&, const string&) const override;
        void ConsulterAbs() override;
        void ConsulterProduc() override;
        void ConsulterInfos() const;
        void DemanderConge();
        void EnvoyerReclamation();
        void setDbConnection(MYSQL* conn);

    private:
        MYSQL* dbConnection;
        string formatRIB(const string&) const;  // New private helper method
};

#endif // EMPLOYE_H
