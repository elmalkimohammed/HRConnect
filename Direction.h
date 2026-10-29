#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>
#include <mysql.h>
#include "Employe_Basic_Info.h"
#include "D_H.h"

class Direction : public Employe_Basic_Info, public D_H
{
    public:
        Direction(const string&, const string&, const int&, const string&, const string&);
        bool Authentication(const string&, const string&) const override;
        void ConsulterAbs() override;
        void ConsulterProduc() override;
        void ConsulterContr();
        void setDbConnection(MYSQL* conn);
    protected:
        MYSQL* dbConnection;
};

#endif // DIRECTION_H
