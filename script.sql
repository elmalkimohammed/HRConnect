-- Create the database
CREATE DATABASE Gestion_Entreprise
USE Gestion_Entreprise;

-- Create Direction table
CREATE TABLE Direction (
    idDir INT PRIMARY KEY AUTO_INCREMENT,
    nomDir VARCHAR(50) NOT NULL,
    prenDir VARCHAR(50) NOT NULL,
    emailDir VARCHAR(100) UNIQUE NOT NULL,
    passwDir VARCHAR(255) NOT NULL
);

-- Create Gestionnaire table
CREATE TABLE Gestionnaire (
    idGest INT PRIMARY KEY AUTO_INCREMENT,
    nomGest VARCHAR(50) NOT NULL,
    prenGest VARCHAR(50) NOT NULL,
    emailGest VARCHAR(100) UNIQUE NOT NULL,
    passwGest VARCHAR(255) NOT NULL
);

-- Create Paiement table
CREATE TABLE Paiement (
    idPaiement INT PRIMARY KEY AUTO_INCREMENT,
    nomPaiement VARCHAR(50) NOT NULL,
    prenPaiement VARCHAR(50) NOT NULL,
    emailPaiement VARCHAR(100) UNIQUE NOT NULL,
    passwPaiement VARCHAR(255) NOT NULL
);

-- Create Employe table with role attribute
CREATE TABLE Employe (
    idEmp INT PRIMARY KEY AUTO_INCREMENT,
    nomEmp VARCHAR(50) NOT NULL,
    prenEmp VARCHAR(50) NOT NULL,
    emailEmp VARCHAR(100) UNIQUE NOT NULL,
    passwEmp VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL, -- Can be 'Direction', 'Gestionnaire', 'Paiement' or custom role
    idDir INT,
    idGest INT,
    idPaiement INT,
    FOREIGN KEY (idDir) REFERENCES Direction(idDir) ON DELETE CASCADE,
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest) ON DELETE CASCADE,
    FOREIGN KEY (idPaiement) REFERENCES Paiement(idPaiement) ON DELETE CASCADE,
    -- Ensure only one role reference is set when role is a predefined type
    CONSTRAINT chk_role_references CHECK (
        (role IN ('Direction', 'Gestionnaire', 'Paiement') AND 
         ((role = 'Direction' AND idDir IS NOT NULL AND idGest IS NULL AND idPaiement IS NULL) OR
          (role = 'Gestionnaire' AND idGest IS NOT NULL AND idDir IS NULL AND idPaiement IS NULL) OR
          (role = 'Paiement' AND idPaiement IS NOT NULL AND idDir IS NULL AND idGest IS NULL))) OR
        (role NOT IN ('Direction', 'Gestionnaire', 'Paiement') AND 
         idDir IS NULL AND idGest IS NULL AND idPaiement IS NULL)
    )
);

-- Create Reclamation table
CREATE TABLE Reclamation (
    idRecl INT PRIMARY KEY AUTO_INCREMENT,
    dateRecl DATE NOT NULL,
    descRecl TEXT NOT NULL,
    etatRecl VARCHAR(20) NOT NULL,
    idEmp INT,
    idGest INT,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp),
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest)
);

-- Create Conge table
CREATE TABLE Conge (
    idConge INT PRIMARY KEY AUTO_INCREMENT,
    EtatConge VARCHAR(20) NOT NULL,
    DescConge TEXT,
    dateDebut DATE NOT NULL,
    dateFin DATE NOT NULL,
    idEmp INT,
    idGest INT,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp),
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest)
);

-- Create Productivite table
CREATE TABLE Productivite (
    idProd INT PRIMARY KEY AUTO_INCREMENT,
    date DATE NOT NULL,
    heuresTravaile INT NOT NULL,
    tachesAccomplies TEXT NOT NULL,
    remarques TEXT,
    idEmp INT,
    idGest INT,
    idPaiement INT,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp),
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest),
    FOREIGN KEY (idPaiement) REFERENCES Paiement(idPaiement)
);

-- Create Contrat table
CREATE TABLE Contrat (
    idCont INT PRIMARY KEY AUTO_INCREMENT,
    debutCont DATE NOT NULL,
    finCont DATE,
    typeCont VARCHAR(50) NOT NULL,
    salaire DECIMAL(10,2) NOT NULL,
    idEmp INT,
    idGest INT,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp),
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest)
);

-- Create Absence table
CREATE TABLE Absence (
    idAbs INT PRIMARY KEY AUTO_INCREMENT,
    dateAbs DATE NOT NULL,
    typeAbs VARCHAR(50) NOT NULL,
    idEmp INT,
    idGest INT,
    idPaiement INT,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp),
    FOREIGN KEY (idGest) REFERENCES Gestionnaire(idGest),
    FOREIGN KEY (idPaiement) REFERENCES Paiement(idPaiement)
);
-- Create InfosBancaire table
CREATE TABLE InfosBancaires (
    idInfo INT PRIMARY KEY AUTO_INCREMENT,
    nomBanque VARCHAR(100) NOT NULL,
    codeBanque VARCHAR(50) NOT NULL,
    numCompte VARCHAR(50) UNIQUE NOT NULL,
    rib VARCHAR(100) UNIQUE NOT NULL,
    idEmp INT UNIQUE,
    FOREIGN KEY (idEmp) REFERENCES Employe(idEmp) ON DELETE CASCADE
);