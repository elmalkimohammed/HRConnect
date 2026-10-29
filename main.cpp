#include <mysql.h> /*For MySQL connection*/
#include <iostream> /*Standard I/O operations*/
#include <cstdlib> /*General utilities 'EXIT_SUCCESS-FAILURE'*/
#include <string> /*string manipulation*/
#include <memory> /*smart pointers and memory management'unique_ptr'*/
#include <limits> /*numeric limits. For 'cin.ignore'*/
#include <iomanip> /*input-output display control*/
#include "Employe.h"
#include "Gestionnaire.h"
#include "Direction.h"
#include "Paiement.h"

using namespace std;

class DatabaseConnection {
public:
    DatabaseConnection(const string& server, const string& user,
                      const string& password, const string& database) {
        conn = mysql_init(nullptr);
        if (!conn) {
            throw runtime_error("MySQL initialization failed");
        }

        if (!mysql_real_connect(conn, server.c_str(), user.c_str(),
                              password.c_str(), database.c_str(),
                              0, nullptr, 0)) {
            throw runtime_error("Connection error: " + string(mysql_error(conn)));
        }
    }

    ~DatabaseConnection() {
        if (conn) {
            mysql_close(conn);
        }
    }

    MYSQL* get() const { return conn; }

private:
    MYSQL* conn;
};

unique_ptr<Employe_Basic_Info> authenticateUser(MYSQL* conn, const string& email, const string& password) {
    string query = "SELECT e.idEmp, e.nomEmp, e.prenEmp, e.emailEmp, e.passwEmp, e.role "
                  "FROM Employe e "
                  "WHERE e.emailEmp = '" + email + "'";

    if (mysql_query(conn, query.c_str())) {
        throw runtime_error("Query error: " + string(mysql_error(conn)));
    }

    MYSQL_RES* res = mysql_use_result(conn);
    if (!res) {
        throw runtime_error("Result error: " + string(mysql_error(conn)));
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return nullptr;
    }

    int id = stoi(row[0]);
    string nom = row[1];
    string prenom = row[2];
    string dbEmail = row[3];
    string dbPassword = row[4];
    string userType = row[5];

    mysql_free_result(res);

    if (dbPassword != password) {
        return nullptr;
    }

    if (userType == "Gestionnaire") {
        auto gest = make_unique<Gestionnaire>(nom, prenom, id, dbEmail, dbPassword);
        return gest;
    } else if (userType == "Direction") {
        auto dir = make_unique<Direction>(nom, prenom, id, dbEmail, dbPassword);
        return dir;
    } else if (userType == "Paiement") {
        auto paiement = make_unique<Paiement>(nom, prenom, id, dbEmail, dbPassword);
        return paiement;
    } else {
        auto emp = make_unique<Employe>(nom, prenom, id, dbEmail, dbPassword);
        return emp;
    }
}

void showEmployeeMenu(Employe* emp, MYSQL* dbConnection) {
    emp->setDbConnection(dbConnection);

    int choice;
    do {
        cout << "\nMenu Employe\n";
        cout << "-----------\n";
        cout << "1. Consulter mes informations\n";
        cout << "2. Consulter mes absences\n";
        cout << "3. Consulter ma productivite\n";
        cout << "4. Demander un conge\n";
        cout << "5. Envoyer une reclamation\n";
        cout << "0. Deconnexion\n";
        cout << "Votre choix: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                emp->ConsulterInfos();
                break;
            case 2:
                emp->ConsulterAbs();
                break;
            case 3:
                emp->ConsulterProduc();
                break;
            case 4:
                emp->DemanderConge();
                break;
            case 5:
                emp->EnvoyerReclamation();
                break;
            case 0:
                cout << "Deconnexion...\n";
                break;
            default:
                cout << "Choix invalide. Veuillez reessayer.\n";
        }
    } while (choice != 0);
}

void showManagerMenu(Gestionnaire* gest, MYSQL* dbConnection) {
    gest->setDbConnection(dbConnection);
    int choice;
    do {
        cout << "\nMenu Gestionnaire\n";
        cout << "----------------\n";
        cout << "1. Consulter les absences\n";
        cout << "2. Consulter la productivite\n";
        cout << "3. Ajouter un employe\n";
        cout << "4. Modifier un employe\n";
        cout << "5. Supprimer un employe\n";
        cout << "6. Traiter les reclamations\n";
        cout << "7. Gerer les demandes de conge\n";
        cout << "8. Creer un contrat\n";
        cout << "9. Annuler un contrat\n";
        cout << "0. Deconnexion\n";
        cout << "Votre choix: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                gest->ConsulterAbs();
                break;
            case 2:
                gest->ConsulterProduc();
                break;
            case 3:
                gest->AjouterEmp();
                break;
            case 4:
                gest->ModfierEmp();
                break;
            case 5:
                gest->SupprimerEmp();
                break;
            case 6:
                gest->TraitementRecl();
                break;
            case 7:
                cout << "1. Accepter un conge\n";
                cout << "2. Refuser un conge\n";
                cout << "Choix: ";
                int congChoice;
                cin >> congChoice;
                if (congChoice == 1) {
                    gest->AccepterCong();
                } else if (congChoice == 2) {
                    gest->RefuserCong();
                } else {
                    cout << "Choix invalide\n";
                }
                break;
            case 8:
                gest->CreerContr();
                break;
            case 9:
                gest->AnnulerContr();
                break;
            case 0:
                cout << "Deconnexion...\n";
                break;
            default:
                cout << "Choix invalide. Veuillez reessayer.\n";
        }
    } while (choice != 0);
}

void showDirectorMenu(Direction* dir, MYSQL* dbConnection) {
    dir->setDbConnection(dbConnection);

    int choice;
    do {
        cout << "\nMenu Direction\n";
        cout << "--------------\n";
        cout << "1. Consulter les absences\n";
        cout << "2. Consulter la productivite\n";
        cout << "3. Consulter les contrats\n";
        cout << "0. Deconnexion\n";
        cout << "Votre choix: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                dir->ConsulterAbs();
                break;
            case 2:
                dir->ConsulterProduc();
                break;
            case 3:
                dir->ConsulterContr();
                break;
            case 0:
                cout << "Deconnexion...\n";
                break;
            default:
                cout << "Choix invalide. Veuillez reessayer.\n";
        }
    } while (choice != 0);
}

void showPaymentMenu(Paiement* paiement, MYSQL* dbConnection) {
    paiement->setDbConnection(dbConnection);

    int choice;
    do {
        cout << "\nMenu Paiement\n";
        cout << "------------\n";
        cout << "1. Consulter les absences\n";
        cout << "2. Consulter la productivite\n";
        cout << "3. Calculer les salaires\n";
        cout << "4. Modifier informations bancaires\n";
        cout << "5. Generer bordereaux\n";
        cout << "0. Deconnexion\n";
        cout << "Votre choix: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                paiement->ConsulterAbs();
                break;
            case 2:
                paiement->ConsulterProduc();
                break;
            case 3:
                paiement->CalculeSal();
                break;
            case 4:
                paiement->ModifierInfosBancaires();
                break;
            case 5:
                paiement->genererBord();
                break;
            case 0:
                cout << "Deconnexion...\n";
                break;
            default:
                cout << "Choix invalide. Veuillez reessayer.\n";
        }
    } while (choice != 0);
}

int main() {
    try {
        const string server = "localhost";
        const string user = "root";
        const string password = "tutorial147";
        const string database = "Gestion_Entreprise";

        DatabaseConnection dbConn(server, user, password, database);
        MYSQL* connection = dbConn.get();

        while (true) {
            cout << "HRConnect - Connexion\n";
            cout << "---------------------\n";

            string email, pass;
            cout << "Email: ";
            getline(cin, email);
            cout << "Mot de passe: ";
            getline(cin, pass);

            auto user = authenticateUser(connection, email, pass);

            if (!user) {
                cout << "\nAuthentification echouee. Veuillez reessayer.\n\n";
                continue;
            }

            cout << "\nAuthentification reussie!\n";
            cout << "Bienvenue, " << user->getNom() << " " << user->getPrenom() << "!\n";

            if (auto gest = dynamic_cast<Gestionnaire*>(user.get())) {
                cout << "Type: Gestionnaire\n";
                showManagerMenu(gest, connection);
            }
            else if (auto dir = dynamic_cast<Direction*>(user.get())) {
                cout << "Type: Direction\n";
                showDirectorMenu(dir, connection);
            }
            else if (auto paiement = dynamic_cast<Paiement*>(user.get())) {
                cout << "Type: Paiement\n";
                showPaymentMenu(paiement, connection);
            }
            else if (auto emp = dynamic_cast<Employe*>(user.get())) {
                cout << "Type: Employe\n";
                showEmployeeMenu(emp, connection);
            }

            char choice;
            cout << "\nVoulez-vous vous reconnecter? (o/n): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice != 'o' && choice != 'O') {
                break;
            }
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "Merci d'avoir utilise HRConnect. Au revoir!\n";
    return EXIT_SUCCESS;
}
