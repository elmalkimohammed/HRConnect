#include "Paiement.h"
#include <iostream>
#include <iomanip>
#include <mysql.h>
#include <vector>
#include <algorithm>

using namespace std;

Paiement::Paiement(const string& nom, const string& pren, const int &id, const string &email, const string &passwd)
    : Employe_Basic_Info(nom, pren, id, email, passwd), dbConnection(nullptr)
{
}

void Paiement::setDbConnection(MYSQL* conn) {
    dbConnection = conn;
}

bool Paiement::Authentication(const string &Email, const string &Password) const {
    return (this->Email == Email && this->Password == Password);
}

void Paiement::ConsulterAbs() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, a.dateAbs, a.typeAbs "
                   "FROM Absence a "
                   "JOIN Employe e ON a.idEmp = e.idEmp "
                   "ORDER BY a.dateAbs DESC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Liste des absences ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prenom"
         << setw(15) << "Date" << setw(20) << "Type" << endl;
    cout << string(75, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(15) << (row[2] ? row[2] : "N/A")
             << setw(20) << (row[3] ? row[3] : "N/A") << endl;
    }
    mysql_free_result(result);
}

void Paiement::ConsulterProduc() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    string query = "SELECT e.nomEmp, e.prenEmp, p.date, p.heuresTravaile, p.tachesAccomplies "
                   "FROM Productivite p "
                   "JOIN Employe e ON p.idEmp = e.idEmp "
                   "ORDER BY p.date DESC";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\n=== Productivite des employes ===\n";
    cout << left << setw(20) << "Nom" << setw(20) << "Prenom"
         << setw(12) << "Date" << setw(8) << "Heures"
         << setw(40) << "Taches" << endl;
    cout << string(100, '-') << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        string taches = row[4] ? string(row[4]) : "N/A";
        if (taches.length() > 35) {
            taches = taches.substr(0, 35) + "...";
        }

        cout << setw(20) << (row[0] ? row[0] : "N/A")
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(12) << (row[2] ? row[2] : "N/A")
             << setw(8) << (row[3] ? row[3] : "N/A")
             << setw(40) << taches << endl;
    }
    mysql_free_result(result);
}

void Paiement::CalculeSal() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    // Query to get ALL employees with their current contract
    string query = "SELECT e.idEmp, e.nomEmp, e.prenEmp, e.role, c.salaire, c.typeCont "
                   "FROM Employe e "
                   "LEFT JOIN Contrat c ON e.idEmp = c.idEmp "
                   "WHERE (c.finCont IS NULL OR c.finCont >= CURDATE()) "
                   "ORDER BY e.nomEmp, e.prenEmp";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* empResult = mysql_store_result(dbConnection);
    if (!empResult) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    // Display all employees
    cout << "\n=== Liste complete des employes ===\n";
    cout << left << setw(10) << "ID" << setw(20) << "Nom"
         << setw(20) << "Prenom" << setw(15) << "Role"
         << setw(15) << "Salaire Base" << setw(15) << "Type Contrat" << endl;
    cout << string(90, '-') << endl;

    vector<int> employeeIds;
    MYSQL_ROW empRow;
    while ((empRow = mysql_fetch_row(empResult))) {
        int empId = stoi(empRow[0]);
        employeeIds.push_back(empId);

        cout << setw(10) << empId
             << setw(20) << (empRow[1] ? empRow[1] : "N/A")
             << setw(20) << (empRow[2] ? empRow[2] : "N/A")
             << setw(15) << (empRow[3] ? empRow[3] : "N/A")
             << fixed << setprecision(2) << setw(15) << (empRow[4] ? stod(empRow[4]) : 0.0)
             << setw(15) << (empRow[5] ? empRow[5] : "N/A")
             << endl;
    }
    mysql_free_result(empResult);

    if (employeeIds.empty()) {
        cout << "Aucun employe trouve.\n";
        return;
    }

    // Employee selection
    int selectedEmpId;
    cout << "\nEntrez l'ID de l'employe a ajuster (0 pour quitter): ";
    cin >> selectedEmpId;

    if (selectedEmpId == 0) return;

    // Verify selected employee exists
    if (find(employeeIds.begin(), employeeIds.end(), selectedEmpId) == employeeIds.end()) {
        cout << "ID employe invalide.\n";
        return;
    }

    // Get current salary info
    query = "SELECT c.salaire, e.nomEmp, e.prenEmp "
            "FROM Employe e "
            "JOIN Contrat c ON e.idEmp = c.idEmp "
            "WHERE e.idEmp = " + to_string(selectedEmpId) + " "
            "AND (c.finCont IS NULL OR c.finCont >= CURDATE())";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* salaryResult = mysql_store_result(dbConnection);
    if (!salaryResult || mysql_num_rows(salaryResult) == 0) {
        cerr << "Employe non trouve ou sans contrat valide.\n";
        if (salaryResult) mysql_free_result(salaryResult);
        return;
    }

    MYSQL_ROW salaryRow = mysql_fetch_row(salaryResult);
    double currentSalary = salaryRow[0] ? stod(salaryRow[0]) : 0.0;
    string nom = salaryRow[1] ? salaryRow[1] : "N/A";
    string prenom = salaryRow[2] ? salaryRow[2] : "N/A";

    mysql_free_result(salaryResult);

    // Get productivity and absence info (for reference)
    double avgHours = 0.0;
    query = "SELECT AVG(heuresTravaile) FROM Productivite "
            "WHERE idEmp = " + to_string(selectedEmpId) + " "
            "AND MONTH(date) = MONTH(CURDATE())";
    if (!mysql_query(dbConnection, query.c_str())) {
        MYSQL_RES* prodResult = mysql_store_result(dbConnection);
        if (prodResult) {
            MYSQL_ROW prodRow = mysql_fetch_row(prodResult);
            avgHours = prodRow[0] ? stod(prodRow[0]) : 0.0;
            mysql_free_result(prodResult);
        }
    }

    int absences = 0;
    query = "SELECT COUNT(*) FROM Absence "
            "WHERE idEmp = " + to_string(selectedEmpId) + " "
            "AND MONTH(dateAbs) = MONTH(CURDATE())";
    if (!mysql_query(dbConnection, query.c_str())) {
        MYSQL_RES* absResult = mysql_store_result(dbConnection);
        if (absResult) {
            MYSQL_ROW absRow = mysql_fetch_row(absResult);
            absences = absRow[0] ? stoi(absRow[0]) : 0;
            mysql_free_result(absResult);
        }
    }

    // Display employee info
    cout << "\n=== Fiche employe ===\n";
    cout << "ID: " << selectedEmpId << endl;
    cout << "Nom: " << nom << " " << prenom << endl;
    cout << fixed << setprecision(2);
    cout << "Salaire actuel: " << currentSalary << endl;
    cout << "Productivite moyenne: " << avgHours << " heures/jour (ce mois)\n";
    cout << "Jours d'absence: " << absences << " (ce mois)\n";

    // Manual adjustment interface
    cout << "\n=== Options d'ajustement ===\n";
    cout << "1. Definir un nouveau salaire manuellement\n";
    cout << "2. Appliquer une augmentation en pourcentage\n";
    cout << "3. Appliquer une reduction en pourcentage\n";
    cout << "4. Ajouter un montant fixe\n";
    cout << "5. Deduire un montant fixe\n";
    cout << "0. Annuler\n";
    cout << "Choix: ";

    int choice;
    cin >> choice;

    double newSalary = currentSalary;
    string justification;

    switch (choice) {
        case 1: {
            cout << "Nouveau salaire: ";
            cin >> newSalary;
            cout << "Raison du changement: ";
            cin.ignore();
            getline(cin, justification);
            break;
        }
        case 2: {
            cout << "Pourcentage d'augmentation (ex: 5 pour 5%): ";
            double percent;
            cin >> percent;
            newSalary = currentSalary * (1 + percent/100);
            cout << "Raison de l'augmentation: ";
            cin.ignore();
            getline(cin, justification);
            break;
        }
        case 3: {
            cout << "Pourcentage de reduction (ex: 5 pour 5%): ";
            double percent;
            cin >> percent;
            newSalary = currentSalary * (1 - percent/100);
            cout << "Raison de la reduction: ";
            cin.ignore();
            getline(cin, justification);
            break;
        }
        case 4: {
            cout << "Montant a ajouter: ";
            double amount;
            cin >> amount;
            newSalary = currentSalary + amount;
            cout << "Raison de l'ajout: ";
            cin.ignore();
            getline(cin, justification);
            break;
        }
        case 5: {
            cout << "Montant a deduire: ";
            double amount;
            cin >> amount;
            newSalary = currentSalary - amount;
            cout << "Raison de la deduction: ";
            cin.ignore();
            getline(cin, justification);
            break;
        }
        case 0:
            cout << "Operation annulee.\n";
            return;
        default:
            cout << "Choix invalide.\n";
            return;
    }

    // Confirmation
    cout << fixed << setprecision(2);
    cout << "\nAncien salaire: " << currentSalary << endl;
    cout << "Nouveau salaire: " << newSalary << endl;
    cout << "Difference: " << (newSalary - currentSalary) << endl;
    cout << "Raison: " << justification << endl;

    cout << "\nConfirmer la modification? (o/n): ";
    char confirm;
    cin >> confirm;

    if (tolower(confirm) == 'o') {
        // Update salary in Contrat table
        query = "UPDATE Contrat SET salaire = " + to_string(newSalary) +
                " WHERE idEmp = " + to_string(selectedEmpId) +
                " AND (finCont IS NULL OR finCont >= CURDATE())";

        if (mysql_query(dbConnection, query.c_str())) {
            cerr << "Erreur lors de la mise a jour: " << mysql_error(dbConnection) << endl;
        } else {
            cout << "Salaire mis a jour avec succes.\n";

            // Store justification in Productivite table
            query = "INSERT INTO Productivite (idEmp, date, tachesAccomplies, remarques) VALUES (" +
                    to_string(selectedEmpId) + ", CURDATE(), 'Ajustement salaire', '" +
                    justification + "')";

            mysql_query(dbConnection, query.c_str());
        }
    } else {
        cout << "Modification annulee.\n";
    }
}

void Paiement::ModifierInfosBancaires() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    cout << "\n=== Modification des informations bancaires ===\n";

    // 1. List all employees
    string query = "SELECT idEmp, nomEmp, prenEmp FROM Employe ORDER BY nomEmp, prenEmp";
    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete: " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(dbConnection);
    if (!result) {
        cerr << "Erreur de resultat: " << mysql_error(dbConnection) << endl;
        return;
    }

    cout << "\nListe des employes:\n";
    cout << left << setw(10) << "ID" << setw(20) << "Nom" << setw(20) << "Prenom" << endl;
    cout << string(50, '-') << endl;

    vector<int> employeeIds;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        int empId = stoi(row[0]);
        employeeIds.push_back(empId);
        cout << setw(10) << empId
             << setw(20) << (row[1] ? row[1] : "N/A")
             << setw(20) << (row[2] ? row[2] : "N/A") << endl;
    }
    mysql_free_result(result);

    if (employeeIds.empty()) {
        cout << "Aucun employe trouve.\n";
        return;
    }

    // 2. Select employee
    int selectedEmpId;
    cout << "\nEntrez l'ID de l'employe a modifier (0 pour annuler): ";
    cin >> selectedEmpId;
    cin.ignore();

    if (selectedEmpId == 0) return;

    if (find(employeeIds.begin(), employeeIds.end(), selectedEmpId) == employeeIds.end()) {
        cout << "ID employe invalide.\n";
        return;
    }

    // 3. Check if bank info exists
    query = "SELECT nomBanque, codeBanque, numCompte, rib FROM InfosBancaires WHERE idEmp = " + to_string(selectedEmpId);
    bool exists = false;
    string currentNomBanque, currentCodeBanque, currentNumCompte, currentRib;

    if (!mysql_query(dbConnection, query.c_str())) {
        MYSQL_RES* infoResult = mysql_store_result(dbConnection);
        if (infoResult) {
            MYSQL_ROW infoRow = mysql_fetch_row(infoResult);
            if (infoRow) {
                exists = true;
                currentNomBanque = infoRow[0] ? infoRow[0] : "";
                currentCodeBanque = infoRow[1] ? infoRow[1] : "";
                currentNumCompte = infoRow[2] ? infoRow[2] : "";
                currentRib = infoRow[3] ? infoRow[3] : "";
            }
            mysql_free_result(infoResult);
        }
    }

    // 4. Display current info if exists
    if (exists) {
        cout << "\nInformations bancaires actuelles:\n";
        cout << "Nom de la banque: " << currentNomBanque << endl;
        cout << "Code banque: " << currentCodeBanque << endl;
        cout << "Numero de compte: " << currentNumCompte << endl;
        cout << "RIB: " << currentRib << endl;
    } else {
        cout << "\nAucune information bancaire enregistree pour cet employe.\n";
    }

    // 5. Get new information
    string nomBanque, codeBanque, numCompte, rib;
    cout << "\nNouvelles informations bancaires:\n";
    cout << "Nom de la banque: ";
    getline(cin, nomBanque);
    cout << "Code banque: ";
    getline(cin, codeBanque);
    cout << "Numero de compte: ";
    getline(cin, numCompte);
    cout << "RIB: ";
    getline(cin, rib);

    // 6. Validate inputs
    if (nomBanque.empty() || codeBanque.empty() || numCompte.empty() || rib.empty()) {
        cout << "Tous les champs sont obligatoires. Operation annulee.\n";
        return;
    }

    // 7. Update or insert
    if (exists) {
        query = "UPDATE InfosBancaires SET "
                "nomBanque = '" + nomBanque + "', "
                "codeBanque = '" + codeBanque + "', "
                "numCompte = '" + numCompte + "', "
                "rib = '" + rib + "' "
                "WHERE idEmp = " + to_string(selectedEmpId);
    } else {
        query = "INSERT INTO InfosBancaires (nomBanque, codeBanque, numCompte, rib, idEmp) VALUES ('" +
                nomBanque + "', '" + codeBanque + "', '" + numCompte + "', '" + rib + "', " +
                to_string(selectedEmpId) + ")";
    }

    // 8. Execute query
    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur lors de la mise a jour: " << mysql_error(dbConnection) << endl;
    } else {
        cout << "Informations bancaires mises a jour avec succes.\n";

        // Log this action in Productivite table
        string logQuery = "INSERT INTO Productivite (idEmp, date, tachesAccomplies, remarques) VALUES (" +
                         to_string(Id) + ", CURDATE(), 'Modification infos bancaires', " +
                         "'Modification infos bancaires pour employe " + to_string(selectedEmpId) + "')";
        mysql_query(dbConnection, logQuery.c_str());
    }
}


void Paiement::genererBord() {
    if (!dbConnection) {
        cerr << "Erreur: Pas de connexion a la base de donnees.\n";
        return;
    }

    // Get current month/year
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int currentMonth = 1 + ltm->tm_mon;
    int currentYear = 1900 + ltm->tm_year;
    string monthYear = to_string(currentMonth) + "/" + to_string(currentYear);

    cout << "\n=== Generation des bordereaux de paie - " << monthYear << " ===\n";

    // Get all employees with active contracts
    string query = "SELECT e.idEmp, e.nomEmp, e.prenEmp, e.role, c.salaire, c.typeCont "
                   "FROM Employe e "
                   "JOIN Contrat c ON e.idEmp = c.idEmp "
                   "WHERE (c.finCont IS NULL OR c.finCont >= CURDATE()) "
                   "ORDER BY e.nomEmp, e.prenEmp";

    if (mysql_query(dbConnection, query.c_str())) {
        cerr << "Erreur de requete (employes): " << mysql_error(dbConnection) << endl;
        return;
    }

    MYSQL_RES* empResult = mysql_store_result(dbConnection);
    if (!empResult) {
        cerr << "Erreur de resultat (employes): " << mysql_error(dbConnection) << endl;
        return;
    }

    // Process each employee
    MYSQL_ROW empRow;
    while ((empRow = mysql_fetch_row(empResult))) {
        int empId = stoi(empRow[0]);
        string nom = empRow[1] ? empRow[1] : "N/A";
        string prenom = empRow[2] ? empRow[2] : "N/A";
        string role = empRow[3] ? empRow[3] : "N/A";
        double salaireBase = empRow[4] ? stod(empRow[4]) : 0.0;
        string typeContrat = empRow[5] ? empRow[5] : "N/A";

        // Get absences for this month
        int absences = 0;
        string absQuery = "SELECT COUNT(*) FROM Absence "
                         "WHERE idEmp = " + to_string(empId) + " "
                         "AND MONTH(dateAbs) = " + to_string(currentMonth) + " "
                         "AND YEAR(dateAbs) = " + to_string(currentYear);

        if (!mysql_query(dbConnection, absQuery.c_str())) {
            MYSQL_RES* absResult = mysql_store_result(dbConnection);
            if (absResult) {
                MYSQL_ROW absRow = mysql_fetch_row(absResult);
                absences = absRow[0] ? stoi(absRow[0]) : 0;
                mysql_free_result(absResult);
            }
        }

        // Get average productivity for this month
        double productivite = 0.0;
        string prodQuery = "SELECT AVG(heuresTravaile) FROM Productivite "
                          "WHERE idEmp = " + to_string(empId) + " "
                          "AND MONTH(date) = " + to_string(currentMonth) + " "
                          "AND YEAR(date) = " + to_string(currentYear);

        if (!mysql_query(dbConnection, prodQuery.c_str())) {
            MYSQL_RES* prodResult = mysql_store_result(dbConnection);
            if (prodResult) {
                MYSQL_ROW prodRow = mysql_fetch_row(prodResult);
                productivite = prodRow[0] ? stod(prodRow[0]) : 0.0;
                mysql_free_result(prodResult);
            }
        }

        // Calculate deductions and bonuses (using existing fields)
        double deductionAbsence = min(salaireBase * 0.25, absences * (salaireBase * 0.05));
        double bonusProductivite = 0.0;

        if (productivite > 8.8) bonusProductivite = salaireBase * 0.15;
        else if (productivite > 8.0) bonusProductivite = salaireBase * 0.10;
        else if (productivite > 7.2) bonusProductivite = salaireBase * 0.05;

        double salaireNet = salaireBase - deductionAbsence + bonusProductivite;

        // Generate pay slip (display only - no database storage)
        cout << "\n========================================\n";
        cout << "BORDEREAU DE PAIE - " << monthYear << "\n";
        cout << "========================================\n";
        cout << left << setw(20) << "Employe:" << nom << " " << prenom << "\n";
        cout << setw(20) << "ID:" << empId << "\n";
        cout << setw(20) << "Role:" << role << "\n";
        cout << setw(20) << "Type Contrat:" << typeContrat << "\n";
        cout << "----------------------------------------\n";
        cout << fixed << setprecision(2);
        cout << setw(20) << "Salaire de base:" << salaireBase << " DH\n";
        cout << setw(20) << "Jours d'absence:" << absences << "\n";
        cout << setw(20) << "Retenue absences:" << -deductionAbsence << " DH\n";
        cout << setw(20) << "Productivite:" << productivite << " h/j\n";
        cout << setw(20) << "Bonus productivite:" << "+" << bonusProductivite << " DH\n";
        cout << "----------------------------------------\n";
        cout << setw(20) << "SALAIRE NET:" << salaireNet << " DH\n";
        cout << "========================================\n";

        // Optionally store the justification in Productivite table (existing field)
        string justification = "Bordereau genere " + monthYear + ": Base=" + to_string(salaireBase) +
                             ", Abs=" + to_string(absences) + ", Prod=" + to_string(productivite) +
                             ", Net=" + to_string(salaireNet);

        string saveQuery = "INSERT INTO Productivite (idEmp, date, tachesAccomplies, remarques) VALUES (" +
                         to_string(empId) + ", CURDATE(), 'Generation bordereau', '" + justification + "')";

        mysql_query(dbConnection, saveQuery.c_str());
    }

    mysql_free_result(empResult);
}
