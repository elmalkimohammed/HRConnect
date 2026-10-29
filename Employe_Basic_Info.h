#ifndef EMPLOYE_BASIC_INFO_H
#define EMPLOYE_BASIC_INFO_H

#include <string>
using namespace std;

class Employe_Basic_Info
{
    public:
        Employe_Basic_Info(const string&, const string&, const int &, const string &, const string &) ;
        virtual ~Employe_Basic_Info() = default;
        virtual bool Authentication(const string&, const string&) const = 0;
        string getNom() const { return Nom; }
        string getPrenom() const { return Prenom; }
        int getId() const { return Id; }
        string getEmail() const { return Email; }
    protected:
        string Nom;
        string Prenom;
        int Id;
        string Email;
        string Password;
};

#endif // EMPLOYE_BASIC_INFO_H
