#include "Employe_Basic_Info.h"

Employe_Basic_Info::Employe_Basic_Info(const string&nom, const string&pren, const int &id, const string &email, const string &passwd)
{
    this->Nom = nom;
    this->Prenom = pren;
    this->Id = id;
    this->Email = email;
    this->Password = passwd;
}
