#include "Gestionnaire.h"
#include <iostream>
#include <iomanip>
#include <mysql.h>
#include <limits>
#include <vector>

using namespace std;

Gestionnaire::Gestionnaire(const string& nom, const string& pren, const int &id,
                         const string &email, const string &passwd)
    : Employe_Basic_Info(nom, pren, id, email, passwd), dbConnection(nullptr)
{
}

void Gestionnaire::setDbConnection(MYSQL* conn) {
    dbConnection = conn;
}

bool Gestionnaire::Authentication(const string &Email, const string &Password) const {
    return (this->Email == Email && this->Password == Password);
}

void Gestionnaire::ConsulterAbs() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, a.dateAbs, a.typeAbs "
                   "FROM Absence a "
                   "JOIN Employe e ON a.idEmp = e.idEmp "
                   "ORDER BY a.dateAbs DESC, e.nomEmp ASC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Liste des Absences ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prénom"
         << setw(15) << "Date" << setw(20) << "Type" << endl;
    cout << string(95, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(15) << (row[2] ? row[2] : "N/A")
             << setw(20) << (row[3] ? row[3] : "N/A")<< endl;
    }
    mysql_free_result(result);
}

void Gestionnaire::ConsulterProduc() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, p.date, p.heuresTravaile, "
                   "LEFT(p.tachesAccomplies, 30) as taches, d.nomDir "
                   "FROM Productivite p "
                   "JOIN Employe e ON p.idEmp = e.idEmp "
                   "LEFT JOIN Direction d ON e.idDir = d.idDir "
                   "ORDER BY p.date DESC, e.nomEmp ASC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Productivité des Employés ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prénom"
         << setw(12) << "Date" << setw(8) << "Heures"
         << setw(32) << "Tâches" << setw(20) << "Direction" << endl;
    cout << string(112, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(12) << (row[2] ? row[2] : "N/A")
             << setw(8) << (row[3] ? row[3] : "N/A")
             << setw(32) << (row[4] ? row[4] : "N/A")
             << setw(20) << (row[5] ? row[5] : "N/A") << endl;
    }
    mysql_free_result(result);
}

void Gestionnaire::AjouterEmp() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    string nom, prenom, email, password, role;
    cout << "\n=== Ajouter un employé ===\n";
    cout << "Nom: ";
    getline(cin, nom);
    cout << "Prénom: ";
    getline(cin, prenom);
    cout << "Email: ";
    getline(cin, email);
    cout << "Mot de passe: ";
    getline(cin, password);
    cout << "Rôle (Employe/Direction/Gestionnaire/Paiement): ";
    getline(cin, role);

    // Start transaction
    mysql_query(dbConnection, "START TRANSACTION");

    try {
        if (role == "Direction" || role == "Gestionnaire" || role == "Paiement") {
            // Determine the correct table and column names
            string roleTable, nameCol, firstNameCol, emailCol, passCol, idField;

            if (role == "Direction") {
                roleTable = "Direction";
                nameCol = "nomDir";
                firstNameCol = "prenDir";
                emailCol = "emailDir";
                passCol = "passwDir";
                idField = "idDir";
            }
            else if (role == "Gestionnaire") {
                roleTable = "Gestionnaire";
                nameCol = "nomGest";
                firstNameCol = "prenGest";
                emailCol = "emailGest";
                passCol = "passwGest";
                idField = "idGest";
            }
            else { // Paiement
                roleTable = "Paiement";
                nameCol = "nomPaiement";
                firstNameCol = "prenPaiement";
                emailCol = "emailPaiement";
                passCol = "passwPaiement";
                idField = "idPaiement";
            }

            // Insert into role-specific table
            string query = "INSERT INTO " + roleTable + " (" +
                          nameCol + ", " + firstNameCol + ", " +
                          emailCol + ", " + passCol + ") VALUES ('" +
                          nom + "', '" + prenom + "', '" +
                          email + "', '" + password + "')";

            if (mysql_query(dbConnection, query.c_str())) {
                throw runtime_error("Erreur d'insertion " + role + ": " + string(mysql_error(dbConnection)));
            }

            // Get the auto-generated role ID
            int roleId = mysql_insert_id(dbConnection);

            // Insert into Employe table with foreign key
            query = "INSERT INTO Employe (nomEmp, prenEmp, emailEmp, passwEmp, role, " + idField + ") " +
                    "VALUES ('" + nom + "', '" + prenom + "', '" + email + "', '" +
                    password + "', '" + role + "', " + to_string(roleId) + ")";

            if (mysql_query(dbConnection, query.c_str())) {
                throw runtime_error("Erreur d'insertion employé: " + string(mysql_error(dbConnection)));
            }
        }
        else {
            // For regular employees
            string query = "INSERT INTO Employe (nomEmp, prenEmp, emailEmp, passwEmp, role) "
                          "VALUES ('" + nom + "', '" + prenom + "', '" + email +
                          "', '" + password + "', '" + role + "')";

            if (mysql_query(dbConnection, query.c_str())) {
                throw runtime_error("Erreur d'insertion employé: " + string(mysql_error(dbConnection)));
            }
        }

        // Commit transaction
        if (mysql_query(dbConnection, "COMMIT")) {
            throw runtime_error("Erreur de commit: " + string(mysql_error(dbConnection)));
        }

        cout << "Employé ajouté avec succès!\n";
    } catch (const exception& e) {
        // Rollback on error
        mysql_query(dbConnection, "ROLLBACK");
        cerr << e.what() << endl;
    }
}

void Gestionnaire::ModfierEmp() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    // Display all employees first
    cout << "\n=== Liste des employés ===\n";
    string listQuery = "SELECT idEmp, nomEmp, prenEmp, role FROM Employe ORDER BY nomEmp, prenEmp";
    if (mysql_query(dbConnection, listQuery.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* listResult = mysql_store_result(dbConnection);
    if (!listResult) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    // Display employee table
    cout << left << setw(8) << "ID" << setw(20) << "Nom"
         << setw(20) << "Prénom" << setw(15) << "Rôle" << endl;
    cout << string(63, '-') << endl;

    MYSQL_ROW listRow;
    while ((listRow = mysql_fetch_row(listResult))) {
        cout << setw(8) << (listRow[0] ? listRow[0] : "")
             << setw(20) << (listRow[1] ? listRow[1] : "")
             << setw(20) << (listRow[2] ? listRow[2] : "")
             << setw(15) << (listRow[3] ? listRow[3] : "") << endl;
    }
    mysql_free_result(listResult);

    // Get employee ID to modify
    int id;
    cout << "\n=== Modifier un employé ===\n";
    cout << "ID de l'employé à modifier (0 pour annuler): ";
    cin >> id;
    cin.ignore();

    if (id == 0) {
        cout << "Modification annulée.\n";
        return;
    }

    // Get current employee data
    string currentRole;
    string query = "SELECT idEmp, nomEmp, prenEmp, emailEmp, role, idDir, idGest, idPaiement "
                   "FROM Employe WHERE idEmp = " + to_string(id);
    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        cout << "Employé non trouvé.\n";
        mysql_free_result(result);
        return;
    }

    currentRole = row[4] ? row[4] : "";
    string currentIdDir = row[5] ? row[5] : "";
    string currentIdGest = row[6] ? row[6] : "";
    string currentIdPaiement = row[7] ? row[7] : "";

    // Display fields for modification
    cout << "\nChamps disponibles pour modification:\n";
    cout << "1. Nom: " << (row[1] ? row[1] : "") << endl;
    cout << "2. Prénom: " << (row[2] ? row[2] : "") << endl;
    cout << "3. Email: " << (row[3] ? row[3] : "") << endl;
    cout << "4. Mot de passe" << endl;
    mysql_free_result(result);

    // Get field to modify
    int choice;
    cout << "\nQuel champ modifier? (1-4, 0 pour annuler): ";
    cin >> choice;
    cin.ignore();

    if (choice == 0) {
        cout << "Modification annulée.\n";
        return;
    }

    string newValue;
    string field, roleTable, roleField, idField;
    bool needsRoleTableUpdate = false;

    switch (choice) {
        case 1:
            field = "nomEmp";
            cout << "Nouveau nom: ";
            needsRoleTableUpdate = true;
            break;
        case 2:
            field = "prenEmp";
            cout << "Nouveau prénom: ";
            needsRoleTableUpdate = true;
            break;
        case 3:
            field = "emailEmp";
            cout << "Nouvel email: ";
            needsRoleTableUpdate = true;
            break;
        case 4:
            field = "passwEmp";
            cout << "Nouveau mot de passe: ";
            break;
        default:
            cout << "Choix invalide.\n";
            return;
    }

    getline(cin, newValue);

    // Determine role-specific table info if needed
    if (needsRoleTableUpdate) {
        if (currentRole == "Direction") {
            roleTable = "Direction";
            roleField = (field == "nomEmp") ? "nomDir" :
                       (field == "prenEmp") ? "prenDir" :
                       "emailDir";
            idField = "idDir";
        } else if (currentRole == "Gestionnaire") {
            roleTable = "Gestionnaire";
            roleField = (field == "nomEmp") ? "nomGest" :
                       (field == "prenEmp") ? "prenGest" :
                       "emailGest";
            idField = "idGest";
        } else if (currentRole == "Paiement") {
            roleTable = "Paiement";
            roleField = (field == "nomEmp") ? "nomPaiement" :
                       (field == "prenEmp") ? "prenPaiement" :
                       "emailPaiement";
            idField = "idPaiement";
        }
    }

    // Start transaction
    mysql_query(dbConnection, "START TRANSACTION");

    try {
        // 1. Update Employe table
        query = "UPDATE Employe SET " + field + " = '" + newValue +
               "' WHERE idEmp = " + to_string(id);
        if (mysql_query(dbConnection, query.c_str())) {
            throw runtime_error("Erreur de mise à jour (Employe): " + string(mysql_error(dbConnection)));
        }

        // 2. Update role-specific table if needed
        if (needsRoleTableUpdate && !roleTable.empty()) {
            string roleId = (currentRole == "Direction") ? currentIdDir :
                           (currentRole == "Gestionnaire") ? currentIdGest :
                           currentIdPaiement;

            if (!roleId.empty()) {
                query = "UPDATE " + roleTable + " SET " + roleField + " = '" + newValue +
                       "' WHERE " + idField + " = " + roleId;

                if (mysql_query(dbConnection, query.c_str())) {
                    throw runtime_error("Erreur de mise à jour (" + roleTable + "): " + string(mysql_error(dbConnection)));
                }
            }
        }

        // Commit transaction
        if (mysql_query(dbConnection, "COMMIT")) {
            throw runtime_error("Erreur de commit: " + string(mysql_error(dbConnection)));
        }

        cout << "Employé modifié avec succès!\n";
    } catch (const exception& e) {
        // Rollback on error
        mysql_query(dbConnection, "ROLLBACK");
        cerr << "Échec de la modification: " << e.what() << endl;
    }
}

void Gestionnaire::SupprimerEmp() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    // First display all employees
    cout << "\n=== Liste des employés ===\n";

    string listQuery = "SELECT idEmp, nomEmp, prenEmp, role FROM Employe ORDER BY nomEmp, prenEmp";
    if (mysql_query(dbConnection, listQuery.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* listResult = mysql_store_result(dbConnection);
    if (!listResult) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << left << setw(8) << "ID" << setw(20) << "Nom"
         << setw(20) << "Prénom" << setw(15) << "Rôle" << endl;
    cout << string(63, '-') << endl;

    MYSQL_ROW listRow;
    while ((listRow = mysql_fetch_row(listResult))) {
        cout << setw(8) << (listRow[0] ? listRow[0] : "")
             << setw(20) << (listRow[1] ? listRow[1] : "")
             << setw(20) << (listRow[2] ? listRow[2] : "")
             << setw(15) << (listRow[3] ? listRow[3] : "") << endl;
    }
    mysql_free_result(listResult);

    // Now ask for ID to delete
    int id;
    cout << "\n=== Supprimer un employé ===\n";
    cout << "ID de l'employé à supprimer (0 pour annuler): ";
    cin >> id;
    cin.ignore();  // Clear input buffer

    if (id == 0) {
        cout << "Suppression annulée.\n";
        return;
    }

    // Rest of the deletion code remains the same...
    // Start transaction
    mysql_query(dbConnection, "START TRANSACTION");

    try {
        // First check if employee exists and get their role
        string checkQuery = "SELECT role, idDir, idGest, idPaiement FROM Employe WHERE idEmp = " + to_string(id);
        if (mysql_query(dbConnection, checkQuery.c_str())) {
            throw runtime_error("Erreur de vérification employé: " + string(mysql_error(dbConnection)));
        }

        MYSQL_RES* result = mysql_store_result(dbConnection);
        if (!result || mysql_num_rows(result) == 0) {
            mysql_free_result(result);
            throw runtime_error("Employé non trouvé");
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        string role = row[0] ? row[0] : "";
        string idDir = row[1] ? row[1] : "";
        string idGest = row[2] ? row[2] : "";
        string idPaiement = row[3] ? row[3] : "";
        mysql_free_result(result);

        // Delete from all related tables
        vector<string> queries = {
            "DELETE FROM Absence WHERE idEmp = " + to_string(id),
            "DELETE FROM Productivite WHERE idEmp = " + to_string(id),
            "DELETE FROM Reclamation WHERE idEmp = " + to_string(id),
            "DELETE FROM Conge WHERE idEmp = " + to_string(id),
            "DELETE FROM Contrat WHERE idEmp = " + to_string(id),
            "DELETE FROM InfosBancaires WHERE idEmp = " + to_string(id),
            "DELETE FROM Employe WHERE idEmp = " + to_string(id)
        };

        // Add role-specific table deletion if needed
        if (role == "Direction" && !idDir.empty()) {
            queries.push_back("DELETE FROM Direction WHERE idDir = " + idDir);
        } else if (role == "Gestionnaire" && !idGest.empty()) {
            queries.push_back("DELETE FROM Gestionnaire WHERE idGest = " + idGest);
        } else if (role == "Paiement" && !idPaiement.empty()) {
            queries.push_back("DELETE FROM Paiement WHERE idPaiement = " + idPaiement);
        }

        // Execute all queries
        for (const string& q : queries) {
            if (mysql_query(dbConnection, q.c_str())) {
                throw runtime_error("Erreur de suppression: " + string(mysql_error(dbConnection)));
            }
        }

        // Commit transaction
        if (mysql_query(dbConnection, "COMMIT")) {
            throw runtime_error("Erreur de commit: " + string(mysql_error(dbConnection)));
        }

        cout << "Employé supprimé avec succès!\n";
    } catch (const exception& e) {
        // Rollback on error
        mysql_query(dbConnection, "ROLLBACK");
        cerr << e.what() << endl;
    }
}

// [Previous TraitementRecl() implementation goes here]
void Gestionnaire::TraitementRecl() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    // 1. Verify current user is a valid gestionnaire
    string checkGest = "SELECT 1 FROM Gestionnaire WHERE idGest = " + to_string(Id);
    if (mysql_query(dbConnection, checkGest.c_str())) {
        cerr << "Erreur de vérification gestionnaire: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* gestResult = mysql_store_result(dbConnection);
    if (!gestResult || mysql_num_rows(gestResult) == 0) {
        cerr << "Erreur: ID Gestionnaire invalide!\n";
        if (gestResult) mysql_free_result(gestResult);
        return;
    }
    mysql_free_result(gestResult);

    // 2. Display pending reclamations
    string query = "SELECT r.idRecl, e.nomEmp, e.prenEmp, r.dateRecl, "
                   "LEFT(r.descRecl, 50) as descRecl, r.etatRecl "
                   "FROM Reclamation r "
                   "JOIN Employe e ON r.idEmp = e.idEmp "
                   "WHERE r.etatRecl = 'Nouveau' "
                   "ORDER BY r.dateRecl DESC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    // Display header
    cout << "\n=== Réclamations en attente ===\n";
    cout << left
         << setw(8) << "ID"
         << setw(20) << "Nom"
         << setw(20) << "Prénom"
         << setw(12) << "Date"
         << setw(52) << "Description"
         << setw(15) << "État" << endl;
    cout << string(127, '-') << endl;

    // Display each reclamation
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(8) << (row[0] ? row[0] : "")
             << setw(20) << (row[1] ? row[1] : "")
             << setw(20) << (row[2] ? row[2] : "")
             << setw(12) << (row[3] ? row[3] : "")
             << setw(52) << (row[4] ? row[4] : "")
             << setw(15) << (row[5] ? row[5] : "") << endl;
    }
    mysql_free_result(result);

    // 3. Get reclamation to process
    int idRecl;
    cout << "\nID de la réclamation à traiter (0 pour annuler): ";
    cin >> idRecl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (idRecl == 0) {
        return;
    }

    // 4. Verify reclamation exists
    string checkRecl = "SELECT idRecl FROM Reclamation WHERE idRecl = " + to_string(idRecl);
    if (mysql_query(dbConnection, checkRecl.c_str())) {
        cerr << "Erreur de vérification réclamation: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* reclResult = mysql_store_result(dbConnection);
    if (!reclResult || mysql_num_rows(reclResult) == 0) {
        cerr << "Erreur: Réclamation non trouvée!\n";
        if (reclResult) mysql_free_result(reclResult);
        return;
    }
    mysql_free_result(reclResult);

    // 5. Get new status
    string newEtat;
    cout << "Nouvel état (Resolu/Rejete): ";
    getline(cin, newEtat);

    // 6. Start transaction
    if (mysql_query(dbConnection, "START TRANSACTION")) {
        cerr << "Erreur de début de transaction: " << mysql_error(dbConnection) << endl;
        return;
    }

    // 7. First update - set status only
    string updateQuery = "UPDATE Reclamation SET etatRecl = '" + newEtat + "' " +
                         "WHERE idRecl = " + to_string(idRecl);

    if (mysql_query(dbConnection, updateQuery.c_str())) {
        cerr << "Erreur de mise à jour: " << mysql_error(dbConnection) << endl;
        mysql_query(dbConnection, "ROLLBACK");
        return;
    }

    // 8. Second update - try to set gestionnaire (only if valid)
    updateQuery = "UPDATE Reclamation r "
                  "JOIN Gestionnaire g ON g.idGest = " + to_string(Id) + " " +
                  "SET r.idGest = g.idGest " +
                  "WHERE r.idRecl = " + to_string(idRecl);

    if (mysql_query(dbConnection, updateQuery.c_str())) {
        // This is non-critical, so just log a warning
        cerr << "Avertissement: Impossible d'associer le gestionnaire ("
             << mysql_error(dbConnection) << ")\n";
    }

    // 9. Commit transaction
    if (mysql_query(dbConnection, "COMMIT")) {
        cerr << "Erreur de commit: " << mysql_error(dbConnection) << endl;
        mysql_query(dbConnection, "ROLLBACK");
        return;
    }

    cout << "Réclamation mise à jour avec succès!\n";
}

void Gestionnaire::AccepterCong() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    string query = "SELECT c.idConge, e.nomEmp, e.prenEmp, c.dateDebut, c.dateFin, "
                   "LEFT(c.DescConge, 30) as description "
                   "FROM Conge c "
                   "JOIN Employe e ON c.idEmp = e.idEmp "
                   "WHERE c.EtatConge = 'En attente' "
                   "ORDER BY c.dateDebut ASC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Demandes de congé en attente ===\n";
    cout << left << setw(8) << "ID" << setw(20) << "Nom" << setw(20) << "Prénom"
         << setw(12) << "Début" << setw(12) << "Fin" << setw(32) << "Description" << endl;
    cout << string(104, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(8) << (row[0] ? row[0] : "")
             << setw(20) << (row[1] ? row[1] : "")
             << setw(20) << (row[2] ? row[2] : "")
             << setw(12) << (row[3] ? row[3] : "")
             << setw(12) << (row[4] ? row[4] : "")
             << setw(32) << (row[5] ? row[5] : "") << endl;
    }
    mysql_free_result(result);

    int idConge;
    cout << "\nID du congé à accepter (0 pour annuler): ";
    cin >> idConge;
    cin.ignore();

    if (idConge == 0) return;

    query = "UPDATE Conge SET EtatConge = 'Accepte', idGest = " + to_string(Id) +
            " WHERE idConge = " + to_string(idConge);

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Congé accepté avec succès!\n";
    }
}

void Gestionnaire::RefuserCong() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    // Same display as AccepterCong()
    string query = "SELECT c.idConge, e.nomEmp, e.prenEmp, c.dateDebut, c.dateFin, "
                   "LEFT(c.DescConge, 30) as description "
                   "FROM Conge c "
                   "JOIN Employe e ON c.idEmp = e.idEmp "
                   "WHERE c.EtatConge = 'En attente' "
                   "ORDER BY c.dateDebut ASC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de résultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Demandes de congé en attente ===\n";
    cout << left << setw(8) << "ID" << setw(20) << "Nom" << setw(20) << "Prénom"
         << setw(12) << "Début" << setw(12) << "Fin" << setw(32) << "Description" << endl;
    cout << string(104, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(8) << (row[0] ? row[0] : "")
             << setw(20) << (row[1] ? row[1] : "")
             << setw(20) << (row[2] ? row[2] : "")
             << setw(12) << (row[3] ? row[3] : "")
             << setw(12) << (row[4] ? row[4] : "")
             << setw(32) << (row[5] ? row[5] : "") << endl;
    }
    mysql_free_result(result);

    int idConge;
    cout << "\nID du congé à refuser (0 pour annuler): ";
    cin >> idConge;
    cin.ignore();

    if (idConge == 0) return;

    query = "UPDATE Conge SET EtatConge = 'Refuse', idGest = " + to_string(Id) +
            " WHERE idConge = " + to_string(idConge);

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Congé refusé avec succès!\n";
    }
}

void Gestionnaire::CreerContr() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    int idEmp;
    string debut, fin, type;
    double salaire;

    cout << "\n=== Créer un contrat ===\n";
    cout << "ID de l'employé: ";
    cin >> idEmp;
    cout << "Date de début (YYYY-MM-DD): ";
    cin >> debut;
    cout << "Date de fin (YYYY-MM-DD, laisser vide si CDI): ";
    cin.ignore();
    getline(cin, fin);
    cout << "Type de contrat: ";
    getline(cin, type);
    cout << "Salaire: ";
    cin >> salaire;

    string query;
    if (fin.empty()) {
        query = "INSERT INTO Contrat (debutCont, typeCont, salaire, idEmp, idGest) "
                "VALUES ('" + debut + "', '" + type + "', " + to_string(salaire) +
                ", " + to_string(idEmp) + ", " + to_string(Id) + ")";
    } else {
        query = "INSERT INTO Contrat (debutCont, finCont, typeCont, salaire, idEmp, idGest) "
                "VALUES ('" + debut + "', '" + fin + "', '" + type + "', " + to_string(salaire) +
                ", " + to_string(idEmp) + ", " + to_string(Id) + ")";
    }

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Contrat créé avec succès!\n";
    }
}

void Gestionnaire::AnnulerContr() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion à la base de données.\n";
        return;
    }

    int idCont;
    cout << "\n=== Annuler un contrat ===\n";
    cout << "ID du contrat: ";
    cin >> idCont;

    string query = "DELETE FROM Contrat WHERE idCont = " + to_string(idCont);
    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requête: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Contrat annulé avec succès!\n";
    }
}
