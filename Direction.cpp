#include "Direction.h"
#include <iostream>
#include <iomanip>
#include <mysql.h>
#include <cstring>

using namespace std;

Direction::Direction(const string& nom, const string& pren, const int &id, const string &email, const string &passwd)
    : Employe_Basic_Info(nom, pren, id, email, passwd), dbConnection(nullptr)
{
}

void Direction::setDbConnection(MYSQL* conn) {
    dbConnection = conn;
}

bool Direction::Authentication(const string &Email, const string &Password) const {
    return (this->Email == Email && this->Password == Password);
}

void Direction::ConsulterAbs() {
    if (!dbConnection) {
        cout << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, a.dateAbs, a.typeAbs, d.nomDir "
                  "FROM Absence a "
                  "JOIN Employe e ON a.idEmp = e.idEmp "
                  "LEFT JOIN Direction d ON e.idDir = d.idDir "
                  "ORDER BY a.dateAbs DESC";

    if (mysql_query(dbConnection, query.c_str())) {
        cout << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cout << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Liste des Absences ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prenom"
         << setw(15) << "Date" << setw(20) << "Type" << setw(20) << "Direction" << endl;
    cout << string(95, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(15) << (row[2] ? row[2] : "N/A")
             << setw(20) << (row[3] ? row[3] : "N/A")
             << setw(20) << (row[4] ? row[4] : "N/A") << endl;
    }
    mysql_free_result(result);
}

void Direction::ConsulterProduc() {
    if (!dbConnection) {
        cout << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, p.date, p.heuresTravaile, "
                  "p.tachesAccomplies, d.nomDir "
                  "FROM Productivite p "
                  "JOIN Employe e ON p.idEmp = e.idEmp "
                  "LEFT JOIN Direction d ON e.idDir = d.idDir "
                  "ORDER BY p.date DESC";

    if (mysql_query(dbConnection, query.c_str())) {
        cout << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cout << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Productivite des Employes ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prenom"
         << setw(12) << "Date" << setw(8) << "Heures"
         << setw(30) << "Taches" << setw(20) << "Direction" << endl;
    cout << string(110, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        string taches = row[4] ? string(row[4]) : "N/A";
        if (taches.length() > 25) {
            taches = taches.substr(0, 25) + "...";
        }

        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(12) << (row[2] ? row[2] : "N/A")
             << setw(8) << (row[3] ? row[3] : "N/A")
             << setw(30) << taches
             << setw(20) << (row[5] ? row[5] : "N/A") << endl;
    }
    mysql_free_result(result);
}

void Direction::ConsulterContr() {
    if (!dbConnection) {
        cout << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, c.debutCont, c.finCont, c.typeCont, c.salaire, "
                   "d.nomDir, g.nomGest "
                   "FROM Contrat c "
                   "JOIN Employe e ON c.idEmp = e.idEmp "
                   "LEFT JOIN Direction d ON e.idDir = d.idDir "
                   "LEFT JOIN Gestionnaire g ON c.idGest = g.idGest "
                   "WHERE c.finCont IS NULL OR c.finCont >= CURDATE() "
                   "ORDER BY e.nomEmp, e.prenEmp";

    if (mysql_query(dbConnection, query.c_str())) {
        cout << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cout << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Liste des contrats actifs ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prenom"
         << setw(12) << "Debut" << setw(12) << "Fin"
         << setw(20) << "Type" << setw(12) << "Salaire"
         << setw(20) << "Direction" << setw(20) << "Gestionnaire" << endl;
    cout << string(136, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        string finCont = row[3] ? row[3] : "CDI"; // Show "CDI" if no end date

        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(12) << (row[2] ? row[2] : "N/A")
             << setw(12) << finCont
             << setw(20) << (row[4] ? row[4] : "N/A")
             << setw(12) << (row[5] ? row[5] : "N/A")
             << setw(20) << (row[6] ? row[6] : "N/A")
             << setw(20) << (row[7] ? row[7] : "N/A") << endl;
    }
    mysql_free_result(result);
}
