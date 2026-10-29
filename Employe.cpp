#include "Employe.h"
#include <iostream>
#include <iomanip>
#include <mysql.h>
#include <cstring>

using namespace std;

Employe::Employe(const string& nom, const string& pren, const int &id, const string &email, const string &passwd)
    : Employe_Basic_Info(nom, pren, id, email, passwd), dbConnection(nullptr)
{
}

void Employe::setDbConnection(MYSQL* conn) {
    dbConnection = conn;
}

bool Employe::Authentication(const string &Email, const string &Password) const {
    return (this->Email == Email && this->Password == Password);
}

void Employe::ConsulterInfos() const {
    cout << "\n=== Mes Informations ===\n";
    cout << left << setw(15) << "ID:" << Id << endl;
    cout << setw(15) << "Nom:" << Nom << endl;
    cout << setw(15) << "Prenom:" << Prenom << endl;
    cout << setw(15) << "Email:" << Email << endl;

    if (dbConnection) {
        // Get role/department info
        string query = "SELECT e.role, d.nomDir, g.nomGest, p.nomPaiement "
                      "FROM Employe e "
                      "LEFT JOIN Direction d ON e.idDir = d.idDir "
                      "LEFT JOIN Gestionnaire g ON e.idGest = g.idGest "
                      "LEFT JOIN Paiement p ON e.idPaiement = p.idPaiement "
                      "WHERE e.idEmp = " + to_string(Id);

        if (mysql_query(dbConnection, query.c_str())) {
            cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
            return;
        }

        MYSQL_RES* result = mysql_store_result(dbConnection);
        if (!result) {
            cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
            return;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            cout << setw(15) << "Role:" << (row[0] ? row[0] : "Non assigne") << endl;
            cout << setw(15) << "Direction:" << (row[1] ? row[1] : "Non assigne") << endl;
            cout << setw(15) << "Gestionnaire:" << (row[2] ? row[2] : "Non assigne") << endl;
            cout << setw(15) << "Paiement:" << (row[3] ? row[3] : "Non assigne") << endl;
        }
        mysql_free_result(result);

        // Get bank information
        query = "SELECT nomBanque, codeBanque, numCompte, rib "
                "FROM InfosBancaires "
                "WHERE idEmp = " + to_string(Id);

        if (!mysql_query(dbConnection, query.c_str())) {
            MYSQL_RES* bankResult = mysql_store_result(dbConnection);
            if (bankResult) {
                MYSQL_ROW bankRow = mysql_fetch_row(bankResult);
                if (bankRow) {
                    cout << "\n=== Informations Bancaires ===\n";
                    cout << setw(15) << "Banque:" << (bankRow[0] ? bankRow[0] : "N/A") << endl;
                    cout << setw(15) << "Code Banque:" << (bankRow[1] ? bankRow[1] : "N/A") << endl;
                    cout << setw(15) << "Num Compte:" << (bankRow[2] ? bankRow[2] : "N/A") << endl;
                    cout << setw(15) << "RIB:" << (bankRow[3] ? formatRIB(string(bankRow[3])) : "N/A") << endl;
                } else {
                    cout << "\n[Info] Aucune information bancaire enregistree.\n";
                }
                mysql_free_result(bankResult);
            }
        } else {
            cerr << "Erreur requete infos bancaires: " << mysql_error(dbConnection) << endl;
        }
    }
}

string Employe::formatRIB(const string& rib) const {
    if (rib.length() != 23) return rib; // Return as-is if invalid length

    return rib.substr(0, 5) + " " +
           rib.substr(5, 5) + " " +
           rib.substr(10, 11) + " " +
           rib.substr(21, 2);
}

void Employe::ConsulterAbs() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT dateAbs, typeAbs FROM Absence WHERE idEmp = " + to_string(Id) +
                   " ORDER BY dateAbs DESC LIMIT 10";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Mes 10 dernieres absences ===\n";
    cout << left << setw(15) << "Date" << setw(20) << "Type" << endl;
    cout << string(35, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(15) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A") << endl;
    }
    mysql_free_result(result);
}

void Employe::ConsulterProduc() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT date, heuresTravaile, tachesAccomplies, remarques "
                   "FROM Productivite WHERE idEmp = " + to_string(Id) +
                   " ORDER BY date DESC LIMIT 5";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Ma productivite (5 dernieres entrees) ===\n";
    cout << left << setw(12) << "Date" << setw(10) << "Heures"
         << setw(30) << "Taches" << setw(20) << "Remarques" << endl;
    cout << string(72, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        string taches = row[2] ? string(row[2]) : "N/A";
        if (taches.length() > 25) {
            taches = taches.substr(0, 25) + "...";
        }

        string remarques = row[3] ? string(row[3]) : "N/A";
        if (remarques.length() > 15) {
            remarques = remarques.substr(0, 15) + "...";
        }

        cout << setw(12) << (row[0] ? row[0] : "N/A")
             << setw(10) << (row[1] ? row[1] : "N/A")
             << setw(30) << taches
             << setw(20) << remarques << endl;
    }
    mysql_free_result(result);
}

void Employe::DemanderConge() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string dateDebut, dateFin, description;
    cout << "\n=== Demande de conge ===\n";
    cout << "Date de debut (YYYY-MM-DD): ";
    cin >> dateDebut;
    cout << "Date de fin (YYYY-MM-DD): ";
    cin >> dateFin;
    cin.ignore();
    cout << "Description: ";
    getline(cin, description);

    string query = "INSERT INTO Conge (EtatConge, DescConge, dateDebut, dateFin, idEmp) "
                   "VALUES ('En attente', '" + description + "', '" + dateDebut + "', '" + dateFin + "', " + to_string(Id) + ")";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Demande de conge envoyee avec succes!\n";
    }
}

void Employe::EnvoyerReclamation() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string description;
    cout << "\n=== Envoyer une reclamation ===\n";
    cout << "Description: ";
    getline(cin, description);

    string query = "INSERT INTO Reclamation (dateRecl, descRecl, etatRecl, idEmp) "
                   "VALUES (CURDATE(), '" + description + "', 'Nouveau', " + to_string(Id) + ")";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Reclamation envoyee avec succes!\n";
    }
}
