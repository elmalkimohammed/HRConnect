#ifndef GESTIONNAIRE_H
#define GESTIONNAIRE_H

#include <string>
#include <mysql.h>
#include "Employe_Basic_Info.h"
#include "D_H.h"

class Gestionnaire : public Employe_Basic_Info, public D_H
{
    public:
        Gestionnaire(const string&, const string&, const int&, const string&, const string&);
        bool Authentication(const string&, const string&) const override;
        void ConsulterAbs() override;
        void ConsulterProduc() override;
        void AjouterEmp();
        void ModfierEmp();
        void SupprimerEmp();
        void TraitementRecl();
        void AccepterCong();
        void RefuserCong();
        void CreerContr();
        void AnnulerContr();
        void setDbConnection(MYSQL* conn);
    private:
        MYSQL* dbConnection;
};

#endif // GESTIONNAIRE_H
